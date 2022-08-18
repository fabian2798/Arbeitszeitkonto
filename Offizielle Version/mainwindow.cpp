#include "mainwindow.h"
#include "ui_mainwindow.h"

//TO DO: UI-Design verbessern (z.B Statusbar einfügen -> File öffnen,Datenbank öffnen)
//TO DO: Arbeitszeiten können nur zwischen und 6 und 19 Uhr eingelesen werden -> Benötige Absegnung über Vorgang
//TO DO: Mögliche Umstrukturierung der Pausenzeit - & Arbeitszeitalgortihmen -> geringe Priorität

//TO DO: Überstunden werden noch nicht anders gewertet, zählen bisher einfach in die normale Arbeitszeit hinein -> Frage: Ob Überstunden mit in %-Anzahl einbezogen werden soll
//TO DO: Tagesübersichtszeile anpassen (klare Darstellung)

MainWindow::MainWindow(QWidget *parent) // Konstructor
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    const QString DRIVER ("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER)){
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("zeitdb.db");
        if(!db.open()){
            qWarning() << "ERROR: Connection " << db.lastError();
        }
    }

    show_table();
    fillComboBoxesFromDB();
    ui->frame_stats_distribution->setLayout(ChartBuilder::createStatWidget());
    // geht noch nicht mit verschiedenen Startjahren
    QString currentMonth = QString::number(QDate::currentDate().month());
    QString beginMonth = QString::number(QDate::currentDate().month() - 2);
    QString currentYear = QString::number(QDate::currentDate().year());
    getDistribution(&valuesOfChart[0], &valuesOfChart[1], beginMonth , currentYear, currentMonth, currentYear);
       // neues Pie Chart wird erstellt, altes gelöscht
       QChartView * deleted = ui->frame_stats_distribution->findChild<QChartView *>();
       delete deleted;
       ui->frame_stats_distribution->layout()->addWidget(ChartBuilder::createDistributionChart(valuesOfChart[0], valuesOfChart[1]));
    QMainWindow::showMaximized();
}

MainWindow::~MainWindow() // Destructor
{
    db.close();
    delete ui;
}


Tagesdaten MainWindow::process_line(QString s, Tagesdaten *data, monat *m_data){
    QString pattern_monthyear(R"(^(Januar|Februar|März|April|Mai|Juni|Juli|August|September|Oktober|November|Dezember)\s+(\d\d\d\d)$)");
    QString pattern_first(R"(^(Mo|Di|Mi|Do|Fr|Sa|So)\s+1.*Periode\s+([+-]\d{1,4}\.\d{2})*)");
    QString pattern_day(R"(^(\w{2}|\s+)\s+(\d\d?)\s+\d{1,4}\s+\|\s+(\d{1,2}:\d{2})?\s+-?\s{1,2}(\d{1,2}:\d{2})[+ *]{3}\s+(.*)\|(.*)\|(.*))");

    QString pattern_shorts(R"((\sFA\s|\sURL\s|\sGLT\s|\sF\s|\sABS\s|\sKRK\s))"); // Für Arbeitskürzel in anderen Zeile

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

        day_nr = "01";
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
        //qDebug() << anfang<<"anfangszeit";
        //qDebug() << ende << "endzeit";
        // Zeiterfassung
        data->add_toarbeitszeit(anfang, ende);
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
//Büroarbeitszeit berechnen
Tagesdaten MainWindow::end_calc(qint32 begin_inMin, qint32 end_inMin, Tagesdaten *data){
    data->setRemember_timekommt(begin_inMin);
    data->setRemember_timegeht(end_inMin);
    qint32 diff_inMin = end_inMin - begin_inMin;
    data->add_toNetto_int(diff_inMin); //gearbeitete Zeit im Büro
    //qDebug() << "Zeit im Büro" << data->getNetto_int();
    return *data;
}
//Homeofficearbeitszeit berechnen
Tagesdaten MainWindow::end_flexcalc(qint32 begin_inMin, qint32 end_inMin, Tagesdaten *data){
    data->setRemember_timeflexkommt(begin_inMin);
    data->setRemember_timeflexgeht(end_inMin);
    qint32 diff_inMin = end_inMin - begin_inMin;
    data->add_toFlexNetto_int(diff_inMin); //gearbeitete Zeit im Homeoffice
    //qDebug() << "Zeit im HO" << data->getFlexNetto_int();
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
    qint32 minutes = m_data->getGes_Nettozeit() % 60;
    qint32 stunden = m_data->getGes_Nettozeit() / 60;
    qint32 flminutes = m_data->getGes_Flexnettozeit() % 60;
    qint32 flstunden = m_data->getGes_Flexnettozeit() / 60;

    qint32 ges_minutes = m_data->getGesamt() % 60;
    qint32 ges_stunden = m_data->getGesamt() / 60;

    double flnt = m_data->getGes_Flexnettozeit();
    double gesamtzeit = m_data->getGesamt();
    double faProzent = (flnt/gesamtzeit)*100;

    QString ges_zeit = Minutes_toString(ges_stunden) + "." + Minutes_toString(ges_minutes);
    QString ges_nt = Minutes_toString(stunden) + "." + Minutes_toString(minutes);
    QString ges_fnt= Minutes_toString(flstunden) + "." + Minutes_toString(flminutes);

    m_data->setFaProzent(faProzent);
    m_data->setGesamtzeit(ges_zeit);
    m_data->setGes_nt(ges_nt);
    m_data->setGes_fnt(ges_fnt);
}

QString MainWindow::monthtoInt(monat *m_data){
    QString monthstr = "0";
    if(m_data->getMonth().compare("Januar") == 0){
        monthstr = "01";
    }
    if(m_data->getMonth().compare("Februar") == 0){
        monthstr = "02";
    }
    if(m_data->getMonth().compare("März") == 0){
        monthstr = "03";
    }
    if(m_data->getMonth().compare("April") == 0){
        monthstr = "04";
    }
    if(m_data->getMonth().compare("Mai") == 0){
        monthstr = "05";
    }
    if(m_data->getMonth().compare("Juni") == 0){
        monthstr = "06";
    }
    if(m_data->getMonth().compare("Juli") == 0){
        monthstr = "07";
    }
    if(m_data->getMonth().compare("August") == 0){
        monthstr= "08";
    }
    if(m_data->getMonth().compare("September") == 0){
        monthstr = "09";
    }
    if(m_data->getMonth().compare("Oktober") == 0){
        monthstr = "10";
    }
    if(m_data->getMonth().compare("November") == 0){
        monthstr = "11";
    }
    if(m_data->getMonth().compare("Dezember") == 0){
        monthstr = "12";
    }
    return monthstr;
}

QString IntToMonth(int month){
    QString monthstr = "0";
    if(month == 1){
        monthstr = "Januar";
    }
    if(month == 2){
        monthstr = "Februar";
    }
    if(month == 3){
        monthstr = "März";
    }
    if(month == 4){
        monthstr = "April";
    }
    if(month == 5){
        monthstr = "Mai";
    }
    if(month == 6){
        monthstr = "Juni";
    }
    if(month == 7){
        monthstr = "Juli";
    }
    if(month == 8){
        monthstr= "August";
    }
    if(month == 9){
        monthstr = "September";
    }
    if(month == 10){
        monthstr = "Oktober";
    }
    if(month == 11){
        monthstr = "November";
    }
    if(month == 12){
        monthstr = "Dezember";
    }
    return monthstr;
}


void MainWindow::dateString(Tagesdaten *data, monat *m_data){
    QString monthInt = monthtoInt(m_data);
    QString date =  m_data->getYear() + "-" + monthInt + + "-" + data->getTages_nr();
    data->setDate(date);
    //qDebug() << "datestring" << date;

}

void MainWindow::drop_table(){
    QSqlQuery query("DROP TABLE zeitkonto");
    if(!query.exec()){
        qWarning() << "ERROR: Drop Table" << query.lastError();
    }
}

void MainWindow::create_table(){
    QSqlQuery query("CREATE TABLE IF NOT EXISTS zeitkonto("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "day TEXT NOT NULL,"
                    "date TEXT NOT NULL,"
                    "type TEXT NULL,"
                    "office_time TEXT NULL,"
                    "flexible_time TEXT NULL,"
                    "summary TEXT NOT NULL,"
                    "unique(date));");
    if(!query.isActive()){
        qWarning() << "ERROR: Create Table " << query.lastError();
    }
}

void MainWindow::insert_table(Tagesdaten *data){
    // TO DO: Kürzel soll angepasst werden ("" = Office, "FA" = Flexible, "GLT" = Gleittag, "KRK" = Krank, "ABS" = Berufsschule, "F" = Feiertag)
    // Kann auch als Legende eingefügt werden oder durch Hover-Beschreibung
    QSqlQuery query;
        query.prepare("INSERT INTO [zeitkonto](day, date, type, office_time, flexible_time, summary) VALUES(:day, :date, :type, :office_time, :flexible_time, :summary);");
        query.bindValue(":day", data->getTag());
        query.bindValue(":date", data->getDate());
        query.bindValue(":type", data->getArb_art());
        query.bindValue(":office_time", data->getOffice_time());
        query.bindValue(":flexible_time", data->getFlexible_time());
        query.bindValue(":summary", data->getNetto_zeit());//Netto Arbeitszeit am Tag
        db.commit();
        if(!query.exec()){
            qWarning() << "ERROR: Insert Table " << query.lastError();
        }

}

void MainWindow::show_table(){
    // TO DO: Table ausgabe im richtigen WIdget, momentan nur durch Konsole
    QString row ="";
    qint32 tablerow = 0;
    ui->datalist->setHorizontalHeaderLabels({"Day","Date","Type","Büro","Home","Sum"});//coloum names
    ui->datalist->setRowCount(50);//Maximal angezeigte Zeilen
    ui->datalist->setColumnCount(6);
    QSqlQuery query("SELECT day,date,type,office_time,flexible_time,summary FROM zeitkonto;");
    if(!query.exec()){
        qWarning() << "ERROR: Show Table" << query.lastError();
    }
    while(query.next()){
        QString day = query.value(0).toString();
        QString date = query.value(1).toString();
        QString type = query.value(2).toString();
        QString of = query.value(3).toString();
        QString fl = query.value(4).toString();
        QString sum = query.value(5).toString();
        row=  day + " " +  date + " " + type + " " + of + " " + fl + " " + sum;
        qDebug() << row;

        ui->datalist->setItem(tablerow,0, new QTableWidgetItem(day));
        ui->datalist->setItem(tablerow,1, new QTableWidgetItem(date));
        ui->datalist->setItem(tablerow,2, new QTableWidgetItem(type));
        ui->datalist->setItem(tablerow,3, new QTableWidgetItem(of));
        ui->datalist->setItem(tablerow,4, new QTableWidgetItem(fl));
        ui->datalist->setItem(tablerow,5, new QTableWidgetItem(sum));
        tablerow += 1;
    }
}


void MainWindow::on_pushButton_options_clicked()
{
    OptionsWindow * OW = new OptionsWindow();
    OW->setModal(true);
    OW->exec();
}


void MainWindow::on_pushButton_help_clicked()
{
    HelpWindow * HW = new HelpWindow();
    HW->setModal(true);
    HW->exec();
}

void MainWindow::on_loadFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Öffne Zeus Textdatei"), "C:/Projects Qt/arbeitszeitkonto", tr("Zeus Datei (*.csv *.txt)")); // "/home" -> Startverzeichnis ändern
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

    ui->lastFileLoaded->clear();

    //Darstellung der Daten des akutell verwendeten Files

    Tagesdaten day_data = Tagesdaten(); //Konstructor
    monat monats_data = monat();

    //drop_table();
    create_table();

    while (!in.atEnd()) {
       QString line = in.readLine();
       day_data = process_line(line,&day_data,&monats_data);
       //Ende jedes Tages
       if(day_data.getEnd_line() == true){
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
           //Berechne Pausenzeiten
           day_data.calc_breaktime();
           //Berechnung der Gesamtarbeitszeit
           day_data.setGesamte_tageszeit(day_data.getFlexNetto_int(),day_data.getNetto_int(),day_data.getPausenzeit()); // (Flex+NT) - Pause

           //Soll doppelte Buchung der Pausenzeit verhindern
           //Monatsdaten werden jeden Tag um die rohe Arbeitszeit(mit Pausenabzug) addiert
           if(day_data.getNetto_int() > 0 && day_data.getFlexNetto_int() == 0){
           monats_data.setGes_Nettozeit(day_data.getNetto_int()-day_data.getPausenzeit());
           day_data.setOffice_time_pause();//officetime = netto - pause, flexible_time = flex
           }
           if(day_data.getFlexNetto_int() > 0 && day_data.getNetto_int() == 0){
           monats_data.setGes_Flexnettozeit(day_data.getFlexNetto_int()-day_data.getPausenzeit());
           day_data.setFlexible_time_pause(); // flexible_time = flex - pause, office_time = netto
           }
           //Soll Minuszeiten verhindern falls Arbeitszeit zu kurz zum Pausenabzug war
           if(day_data.getNetto_int() > 0 && day_data.getFlexNetto_int() > 0){
               if((day_data.getFlexNetto_int() > 45 && day_data.getNetto_int() < 45) || (day_data.getNetto_int() > 45 && day_data.getFlexNetto_int() > 45)){
                    monats_data.setGes_Nettozeit(day_data.getNetto_int());
                    monats_data.setGes_Flexnettozeit(day_data.getFlexNetto_int()-day_data.getPausenzeit());
                    day_data.setFlexible_time_pause();
               }
               if(day_data.getNetto_int() > 45 && day_data.getFlexNetto_int() < 45){
                   monats_data.setGes_Nettozeit(day_data.getNetto_int()-day_data.getPausenzeit());
                   monats_data.setGes_Flexnettozeit(day_data.getFlexNetto_int());
                   day_data.setOffice_time_pause();
               }
           }
           //Datumszeile
           dateString(&day_data, &monats_data);

           qDebug() << "Tagesarbeitszeit: " << day_data.getGesamte_tageszeit();
           Zeile = "<"+day_data.getTag()+">" + "  " + day_data.getTages_nr() + " " + day_data.getArb_art() + " " + "Büro: " + day_data.getOffice_time() + " " +
                   "FA: " + day_data.getFlexible_time() + " " + "Soll: " + day_data.getSoll_zeit() + " " +
                       "NT: " + day_data.getNetto_zeit() + " " + "Diff: " + day_data.getZeit_diff() + " " + "Saldo: " + day_data.getZeit_saldo();
           //ui->listWidget_3->addItem(Zeile);

           //Datenbank
           insert_table(&day_data);


           monats_data.setGesamt(day_data.getGesamte_tageszeit());// Add Tagesgesamt

           //Abschluss
           day_data.clearAllTimes(); //QList.clear()
       }
       }
    //Show DB
    qDebug() << "Order: ID, Day, Date, Kürzel, Office_time, Flexible_time, summary";
    show_table();


    //Monatsübersicht
    toMinutesandHours(&monats_data); // (Int) Minuten to (String) Hour.Minuten
    mView = get_monatsView(&monats_data); // Return Übersichtsstring des Monats
    QListWidget * month_list = ui->frame_stats_distribution->findChild<QListWidget *>();

    month_list->addItem(mView);


    file.close();
    ui->lastFileLoaded->insertItem(1,fileName);
}

void MainWindow::fillComboBoxesFromDB(){
    QString queryString = "SELECT DISTINCT strftime('%Y', date) from zeitkonto;";
    QSqlQuery distinctYearSearch(queryString);
    if(!distinctYearSearch.exec()){
        qWarning() << "ERROR: Show Table" << distinctYearSearch.lastError();
    }
    while(distinctYearSearch.next()){
        qDebug() << distinctYearSearch.value(0).toString();
        qDebug() << "Hallo";
        ui->comboBox_begin_year->addItem(distinctYearSearch.value(0).toString());
        ui->comboBox_end_year->addItem(distinctYearSearch.value(0).toString());
    }

    ui->comboBox_end_month->setCurrentIndex(QDate::currentDate().month()-1);
    if(QDate::currentDate().month()-1 < 2){
            ui->comboBox_begin_month->setCurrentIndex(12 - QDate::currentDate().month()-1);
    }
    else{
        ui->comboBox_begin_month->setCurrentIndex(QDate::currentDate().month()-3);

    }
}

// Button zum Testen von Funktionen
void MainWindow::on_pushButton_clicked()
{
    //fillComboBoxesFromDB();
}


int db_timeToInt(QString string){
    int minutes = 0;
    QString pattern_int(R"(^([0-9]+)[.]([0-9]+)$)");
    static QRegularExpression re0(pattern_int);
    QRegularExpressionMatch match0 = re0.match(string);
    QString hours_s, minutes_s;
    if (match0.hasMatch()) {
        hours_s = match0.captured(1);
        minutes_s = match0.captured(2);
    }
    minutes = hours_s.toInt() * 60 + minutes_s.toInt();
    //qDebug() << hours_s;
    //qDebug() << minutes_s;


    return minutes;
}

void MainWindow::getDistribution(int * test1, int * test2, QString bm, QString by, QString em, QString ey){

     QListWidget * p = ui->frame_stats_distribution->findChild<QListWidget*>();
     p->clear();
    QString begin_month = bm;
    QString end_month = em;
    QString begin_year = by;
    QString end_year = ey;
    int summen[2];

    // falls im gleichem Jahr
    if (begin_year == end_year){
        if (begin_month == end_month){
            if(begin_month.toInt() < 10){
                begin_month = "0" + begin_month;
                }

            QString selectString = "SELECT date, office_time, flexible_time from zeitkonto WHERE strftime('%m', date) = '" + begin_month + "';";
            QSqlQuery q(selectString);
            qDebug() << begin_month << " " << end_month << " " << selectString;
            int summen[2];
            summen[0] = 0; // Office Summe
            summen[1] = 0; // Home Office Summe
            if(!q.isActive()){
                qWarning() << "ERROR: Create Table " << q.lastError();
            }
            while(q.next()){
                qDebug() << q.value(0).toString() << " " << q.value(1).toString() << " " << q.value(2).toString();
                summen[0] += db_timeToInt(q.value(1).toString());
                summen[1] += db_timeToInt(q.value(2).toString());
            }
            *test1 = summen[0];
            *test2 = summen[1];
        }
        else{
        // Zahlenreihe der Monate bilden
        QStringList * test = new QStringList();
        int i;
        for(i = begin_month.toInt(); i < end_month.toInt() + 1 ; i++ ){
            if(i<10){
                test->append("0" + QString::number(i));
            }
            else{
                test->append(QString::number(i));
            }
            QListWidget * p = ui->frame_stats_distribution->findChild<QListWidget*>();

            p->addItem(IntToMonth(i));
        }

        QString selectString = "SELECT date, office_time, flexible_time from zeitkonto ";


        for ( QString x : *test){
            if(x == test->last()){
            selectString += "strftime('%m', date) ='" + x + "' OR strftime('%y', date) = '" + begin_year + "'";
            }

            else if(x == test->first()){
                selectString += "WHERE strftime('%m', date) ='" + x + "' OR ";
            }

            else{
                selectString += " strftime('%m', date) ='" + x + "' OR ";
            }
        }
       selectString += ";";

       qDebug() << selectString;
       QSqlQuery q(selectString);

       summen[0] = 0; // Office Summe
       summen[1] = 0; // Home Office Summe
       if(!q.isActive()){
           qWarning() << "ERROR: Create Table " << q.lastError();
       }
       while(q.next()){
           qDebug() << q.value(0).toString() << " " << q.value(1).toString() << " " << q.value(2).toString();
           summen[0] += db_timeToInt(q.value(1).toString());
           summen[1] += db_timeToInt(q.value(2).toString());
       }
       *test1 = summen[0];
       *test2 = summen[1];
       }

}
}

void MainWindow::on_pushButton_stats_update_clicked(){

    // wenn der gleiche Start und Endmonat eingegeben wird stürzt das Programm ab --> fixen

    QString begin_month = QString::number(ui->comboBox_begin_month->currentIndex() + 1);
    QString end_month = QString::number(ui->comboBox_end_month->currentIndex() + 1);
    QString begin_year = ui->comboBox_begin_year->currentText();
    QString end_year = ui->comboBox_end_year->currentText();

    getDistribution(&valuesOfChart[0], &valuesOfChart[1], begin_month, begin_year, end_month, end_year);
       // neues Pie Chart wird erstellt, altes gelöscht
       QChartView * deleted = ui->frame_stats_distribution->findChild<QChartView *>();
       delete deleted;
       ui->frame_stats_distribution->layout()->addWidget(ChartBuilder::createDistributionChart(valuesOfChart[0], valuesOfChart[1]));
    }

