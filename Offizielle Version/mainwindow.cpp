#include "mainwindow.h"
#include "ui_mainwindow.h"


//TO DO: Testen mit Homeoffice und Standortarbeit am selben Tag
//TO DO: Skip Sa/So unnessecary for Trainees
//TO DO: UI-Design verbessern
//TO DO: Zeitberechnung mit Berücksichtigung der Pausenzeiten
//TO DO: Wochenende übernimmt manchmal die Arbeitskürzel vom vor Tag

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tag_view->setText("Tagesübersicht");
    ui->monat_view->setText("Monatsübersicht");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Öffne Zeus Textdatei"), "C:/Projects Qt/arbeitszeitkonto", tr("Zeus Datei (*.csv *.txt)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
       if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
           return;}

    QTextStream in(&file);
    QString Zeile;
    QString Kommt;
    QString Geht;
    QString flexKommt;
    QString flexGeht;

    qint32 flexgeht_int;
    qint32 flexkommt_int;
    qint32 kommt_int;
    qint32 geht_int;
    QString flexMin;
    QString ntMin;
    QString mView;

    //Darstellung der Daten des akutell verwendeten Files
    ui->listWidget_2->clear();
    ui->listWidget_3->clear();
    ui->listWidget->clear();

    Tagesdaten day_data = Tagesdaten();
    monat monats_data = monat();

    while (!in.atEnd()) {
       QString line = in.readLine();
       day_data = process_line(line,&day_data,&monats_data);
       //Ende jedes Tages
       if(day_data.getEnd_line() == true){
           //Zur Ausgabe der Daten
           if(day_data.getKommt().size() > 0){
               QList <QString> temp_geht = day_data.getGeht();
               QList <QString> temp_kommt = day_data.getKommt();
               for(int i = 0;i<day_data.getGeht().size();i++){
                   kommt_int = day_data.just_Minutes(temp_kommt[i]);
                   geht_int = day_data.just_Minutes(temp_geht[i]);
                   end_calc(kommt_int,geht_int,&day_data);

               }
           }
           if(day_data.getFlexArbkommt().size() > 0){
               QList <QString> temp_flexgeht = day_data.getFlexArbgeht();
               QList <QString> temp_flexkommt = day_data.getFlexArbkommt();
               for(int j = 0;j<day_data.getFlexArbgeht().size();j++){
                   flexkommt_int = day_data.just_Minutes(temp_flexkommt[j]);
                   flexgeht_int = day_data.just_Minutes(temp_flexgeht[j]);
                   end_flexcalc(flexkommt_int,flexgeht_int,&day_data);
               }
           }
           //Berechnung der Gesamtarbeitszeit
           day_data.calc_breaktime();//Berechne Pausenzeiten
           day_data.setGesamte_tageszeit(day_data.getFlexNetto_int(),day_data.getNetto_int(),day_data.getPausenzeit());
           Zeile = "<"+day_data.getTag()+">" + "  " + day_data.getTages_nr() + " " + day_data.getArb_art() + " " + "Soll: " + day_data.getSoll_zeit() + " " +
                       "NT: " + day_data.getNetto_zeit() + " " + "Diff: " + day_data.getZeit_diff() + " " + "Saldo: " + day_data.getZeit_saldo();
           ui->listWidget_3->addItem(Zeile);
           if(day_data.getNetto_int() > 0 && day_data.getFlexNetto_int() == 0){
               //qDebug() << "büro";
           monats_data.setGes_Nettozeit(day_data.getNetto_int()-day_data.getPausenzeit());
           }
           if(day_data.getFlexNetto_int() > 0 && day_data.getNetto_int() == 0){
               //qDebug() << "flex";
           monats_data.setGes_Flexnettozeit(day_data.getFlexNetto_int()-day_data.getPausenzeit());
           }
           if(day_data.getNetto_int() > 0 && day_data.getFlexNetto_int() > 0){
               //qDebug() << "both";
               monats_data.setGes_Nettozeit(day_data.getNetto_int());
               monats_data.setGes_Flexnettozeit(day_data.getFlexNetto_int()-day_data.getPausenzeit());
           }
           monats_data.setGesamt(day_data.getGesamte_tageszeit());
           day_data.clearAllTimes(); //QList.clear()
       }
       }
    //Monatsübersicht
    toMinutesandHours(&monats_data);
    mView = get_monatsView(&monats_data);
    ui->listWidget_2->addItem(mView);


    file.close();
    ui->listWidget->insertItem(1,fileName);

}

Tagesdaten MainWindow::process_line(QString s, Tagesdaten *data, monat *m_data){
    QString pattern_monthyear(R"(^(Januar|Februar|März|April|Mai|Juni|Juli|August|September|Oktober|November|Dezember)\s+(\d\d\d\d)$)");
    QString pattern_first(R"(^(Mo|Di|Mi|Do|Fr|Sa|So)\s+1.*Periode\s+([+-]\d{1,4}\.\d{2})*)");
    QString pattern_day(R"(^(\w{2}|\s+)\s+(\d\d?)\s+\d{1,4}\s+\|\s+(\d{1,2}:\d{2})?\s+-?\s{1,2}(\d{1,2}:\d{2})[+ *]{3}\s+(.*)\|(.*)\|(.*))");
    QString pattern_shorts(R"((\sFA\s|\sURL\s|\sGLT\s|\sF\s|\sABS\s))"); // Für Arbeitskürzel in anderen Zeile

    static QRegularExpression re0(pattern_monthyear);
    static QRegularExpression re1(pattern_first);
    static QRegularExpression re11(pattern_shorts);
    static QRegularExpression re2(pattern_day);

    QString anfang,ende;

    QStringList endteil_pieces;
    QString arb_short;

    data->setEnd_line(false);

    QString uebertrag;
    QString month, year, day, day_nr;
    QString mittelteil1,mittelteil2;
    QString endteil = "";
    QString line = "";

    // filtert die Monats und Jahrzeile
    QRegularExpressionMatch match0 = re0.match(s);
    if (match0.hasMatch()) {
        month = match0.captured(1);
        year = match0.captured(2);
        m_data->setMonth(month);
        m_data->setYear(year);
        qDebug()<<month<<"  "<<year;
    }
    // sucht erste Zeile mit Periodenübertrag
    QRegularExpressionMatch match1 = re1.match(s);
    if(match1.hasMatch()){
        day = match1.captured(1);

        if(!day.trimmed().isEmpty()){
        data->setTag(day);
        }

        day_nr = "1";
        data->setTages_nr(day_nr);
        uebertrag = match1.captured(2);
        qDebug()<<day<<day_nr<<uebertrag;
    }
    // filtert die Tageszeile
    QRegularExpressionMatch match2 = re2.match(s);
    if (match2.hasMatch()) {
        day = match2.captured(1);

        if(!day.trimmed().isEmpty()){
        data->setTag(day);
        }

        day_nr = match2.captured(2);
        data->setTages_nr(day_nr);

        anfang = match2.captured(3);
        ende = match2.captured(4);
        //qDebug() << anfang<<"anfangszeit";
        //qDebug() << ende << "endzeit";

        // Zeiterfassung
        data->add_toarbeitszeit(anfang, ende);


        mittelteil1 = match2.captured(5).trimmed();

        //Gesetzt wenn Arbeitskürzel nicht in einer anderen Zeile gefunden wurde
        if(data->getIn_otherline() == false){
            data->setArb_art(mittelteil1);
        }

        mittelteil2 = match2.captured(6);
        if(!day.trimmed().isEmpty()){
            qDebug()<<day<<day_nr<<mittelteil1;
        }

        endteil = match2.captured(7);

        //Ende des Tageserreicht
        if(!endteil.isEmpty()){
            line = endteil;
            data->setEnd_line(true);//Flag für Tagesende
            data->setIn_otherline(false);//reset: Flag für Kürzel unterhalb der ersten Zeile
            endteil_pieces = endteil.split(' ');

            data->setSoll_zeit(endteil_pieces.value(1));
            data->setNetto_zeit(endteil_pieces.value(4));
            data->setZeit_diff(endteil_pieces.value(7));

            if(endteil_pieces.length() > 12){
                data->setZeit_saldo(endteil_pieces.value(12));
            }
            else{
                data->setZeit_saldo(endteil_pieces.value(11));
            }
        }
    }

    // Kürzel des Arbeitstypes in andere Zeile als Erster
    QRegularExpressionMatch match11 = re11.match(s);
    if(match11.hasMatch()){
        arb_short = match11.captured(1).trimmed();
        data->setIn_otherline(true);//Kürzel wurde unterhalb der ersten Zeile aber noch im gleichen Tag gefunden
        data->setArb_art(arb_short);
    }


    return *data;
}

Tagesdaten MainWindow::end_calc(qint32 begin_inMin, qint32 end_inMin, Tagesdaten *data){
    data->setRemember_timekommt(begin_inMin);
    data->setRemember_timegeht(end_inMin);
    qint32 diff_inMin = end_inMin - begin_inMin;
    data->add_toNetto_int(diff_inMin); //gearbeitete Zeit im Büro
    return *data;
}

Tagesdaten MainWindow::end_flexcalc(qint32 begin_inMin, qint32 end_inMin, Tagesdaten *data){
    data->setRemember_timeflexkommt(begin_inMin);
    data->setRemember_timeflexgeht(end_inMin);
    qint32 diff_inMin = end_inMin - begin_inMin;
    data->add_toFlexNetto_int(diff_inMin); //gearbeitete Zeit im Homeoffice
    return *data;
}

QString MainWindow::Minutes_toString(qint32 zeit_Min){
    QString str_minutes = QString::number(zeit_Min);

    return str_minutes;
}

QString MainWindow::get_monatsView(monat *m_data){
    QString Zeile;
    Zeile = m_data->getMonth() + " " + m_data->getYear() + "        " + "NT: " + m_data->getGes_nt() + "    "
            + "FA: " + m_data->getGes_fnt() + "    " + "NT+FA: " + m_data->getGesamtzeit() + "    "+ "FA(in %): " + m_data->getFaProzent();

    return Zeile;
}

void MainWindow::toMinutesandHours(monat *m_data){
    qDebug() << m_data->getGes_Nettozeit();
    qint32 minutes = m_data->getGes_Nettozeit() % 60;
    qint32 stunden = m_data->getGes_Nettozeit() / 60;
    qDebug() << m_data->getGes_Flexnettozeit();
    qint32 flminutes = m_data->getGes_Flexnettozeit() % 60;
    qint32 flstunden = m_data->getGes_Flexnettozeit() / 60;
    qint32 ges_minutes = m_data->getGesamt() % 60;
    qint32 ges_stunden = m_data->getGesamt() / 60;
    double faProzent = (m_data->getGes_Flexnettozeit()/m_data->getGesamt())*100;
    QString ges_zeit = Minutes_toString(ges_stunden) + "." + Minutes_toString(ges_minutes);
    QString ges_nt = Minutes_toString(stunden) + "." + Minutes_toString(minutes);
    QString ges_fnt= Minutes_toString(flstunden) + "." + Minutes_toString(flminutes);
    m_data->setFaProzent(faProzent);
    m_data->setGesamtzeit(ges_zeit);
    m_data->setGes_nt(ges_nt);
    m_data->setGes_fnt(ges_fnt);
}
