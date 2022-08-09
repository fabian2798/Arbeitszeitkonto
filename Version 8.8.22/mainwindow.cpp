#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Tagesdaten.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    //Darstellung der Daten des akutell verwendeten Files
    ui->listWidget_2->clear();
    ui->listWidget_3->clear();

    Tagesdaten day_data = Tagesdaten();

    while (!in.atEnd()) {
       QString line = in.readLine();
       day_data = process_line(line,&day_data);
       //Ende jedes Tages
       if(day_data.getEnd_line() == true){
           Zeile = "Tagesdaten Objekt: " + day_data.getTag() + " " + day_data.getTages_nr() + " " + day_data.getArb_art() + " " + day_data.getSoll_zeit() + " " +
                       day_data.getNetto_zeit() + " " + day_data.getZeit_diff() + " " + day_data.getZeit_saldo();
           ui->listWidget_3->addItem(Zeile);
           ui->listWidget_3->addItem("Arbeitszeit:");
           //qDebug() << day_data.getArbeitszeit();
           ui->listWidget_3->addItems(day_data.getArbeitszeit()); // jeder Eintrag eine Zeile im Widget
           day_data.removeAllTimes(); //QList.clear()

           //zusätzliche Tätigkeiten
           //Berechnung der Nettozeit anhand der Arbeitszeit->List
           //mit Berücksichtigung der Pausen
           //optionale Berücksichtigung der arbeitsortbezogenen Arbeitszeiten
       }
       }

    file.close();
    ui->listWidget->insertItem(1,fileName);
}

Tagesdaten MainWindow::process_line(QString s, Tagesdaten *data){
    QString pattern_monthyear(R"(^(Januar|Februar|März|April|Mai|Juni|Juli|August|September|Oktober|November|Dezember)\s+(\d\d\d\d)$)"); //Kennung: <Monat> <Jahr>
    QString pattern_first(R"(^(Mo|Di|Mi|Do|Fr|Sa|So)\s+1.*Periode\s+([+-]\d{1,4}\.\d{2})*)"); //To DO: Skip Sa/So unnessecary for Trainees
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
        line = month + " " + year;
        ui->listWidget_2->addItem(line);
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
        line = day + " " + day_nr + " " + uebertrag;
        ui->listWidget_2->addItem(line);
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

        data->add_toarbeitszeit(anfang, ende);

        mittelteil1 = match2.captured(5).trimmed();

        //Gesetzt wenn Arbeitskürzel nicht in einer anderen Zeile gefunden wurde
        if(data->getIn_otherline() == false){
            data->setArb_art(mittelteil1);
        }

        mittelteil2 = match2.captured(6);
        if(!day.trimmed().isEmpty()){
            line = day + " " + day_nr + " " + mittelteil1;
            ui->listWidget_2->addItem(line);
            qDebug()<<day<<day_nr<<mittelteil1;
        }
        qDebug() << anfang<<"anfangszeit";
        qDebug() << ende << "endzeit";
        endteil = match2.captured(7);

        //Ende des Tageserreicht
        if(!endteil.isEmpty()){
            line = endteil;
            data->setEnd_line(true);//Flag für Tagesende
            data->setIn_otherline(false);//reset: Flag für Kürzel unterhalb der ersten Zeile
            endteil_pieces = endteil.split(' ');
            //qDebug() << endteil_pieces;

            data->setSoll_zeit(endteil_pieces.value(1));
            data->setNetto_zeit(endteil_pieces.value(4));
            data->setZeit_diff(endteil_pieces.value(7));

            if(endteil_pieces.length() > 12){
                data->setZeit_saldo(endteil_pieces.value(12));
            }
            else{
                data->setZeit_saldo(endteil_pieces.value(11));
            }
            ui->listWidget_2->addItem(line);
           qDebug() << endteil;
        }
    }

    // Kürzel des Arbeitstypes in andere Zeile als Erster
    QRegularExpressionMatch match11 = re11.match(s);
    if(match11.hasMatch()){
        arb_short = match11.captured(1).trimmed();
        data->setIn_otherline(true);//Kürzel wurde unterhalb der ersten Zeile aber noch im gleichen Tag gefunden
        data->setArb_art(arb_short);

        //qDebug() << arb_short << "<- arb_short";
    }


    return *data;
}

