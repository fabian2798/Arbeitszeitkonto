#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    // Open File
    fileName = QFileDialog::getOpenFileName(this,
        tr("Öffne Zeus Textdatei"), "C:/Projects Qt/arbeitszeitkonto - Kopie", tr("Zeus Datei (*.csv *.txt)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
       if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
           return;}

    // Read File
    QTextStream in(&file);
    // nur daten vom aktuellen monat anzeigen
    ui->listWidget_2->clear();
    ui->listWidget_3->clear();
    ui->listWidget_4->clear();
    while (!in.atEnd()) {
       QString line = in.readLine();
       //qDebug() << zaehler++ << line;
       process_line(line);
       }
    file.close();
    ui->listWidget->insertItem(1,fileName);
}

void MainWindow::process_line(QString s){
    QString pattern_monthyear(R"(^(Januar|Februar|März|April|Mai|Juni|Juli|August|September|Oktober|November|Dezember)\s+(\d\d\d\d)$)");
    QString pattern_first(R"(^(Mo|Di|Mi|Do|Fr|Sa|So)\s+1.*Periode\s+([+-]\d{1,4}\.\d{2})*)"); //To DO: Skip Sa/So unnessecary for Trainees
    // Nimmt Arbeitszeitkürzel auf von allen
    // Speziell für Abschnitte mit versetztem Kürzel
    QString pattern_shorts(R"((\sFA\s|\sURL\s|\sGLT\s|\sF\s|\sABS\s))");
    // Erfasst erste auftretende Zeit
    QString first_time(R"((\s\d{1,2}\:\d{1,2}\+\s\*|\s\d{1,2}\:\d{1,2}\+\s|\s{3}\d{1,2}\:\d{1,2}))");
    // Erfasst letzte auftretende Zeit
    QString last_time(R"(\-\s+(\d{1,2}\:\d{1,2})\s+(FA|URL|GLT|F|ABS|\s+))");
    QString pattern_day(R"(^(\w{2}|\s+)\s+(\d\d?)\s+\d{1,4}\s+\|\s+(?:\d{1,2}:\d{2})?\s+-?\s{1,2}(?:\d{1,2}:\d{2})[+ *]{3}\s+(.*)\|(.*)\|(.*))");

    static QRegularExpression re0(pattern_monthyear);
    static QRegularExpression re1(pattern_first);
    static QRegularExpression re2(pattern_day);
    static QRegularExpression re11(pattern_shorts);
    static QRegularExpression re5(first_time);
    static QRegularExpression re6(last_time);

    QString arb_short;
    //Zeiten pro Zeile
    QString ftime, ltime;
    bool end_exists = false;
    //Split Endteil
    QStringList endteil_pieces = {};
    // Aufteilung des Endteils am Tag
    QString sollzeit, netzeit , diff, saldo = "";


    QString uebertrag;
    QString month, year, day, day_nr;
    QString mittelteil1,mittelteil2,endteil;
    QString line = "";

    // filtert die Monats und Jahrzeile
    QRegularExpressionMatch match0 = re0.match(s);
    if (match0.hasMatch()) {
        month = match0.captured(1);
        year = match0.captured(2);
        line = month + " " + year;
        //insert_data(month,year);
        ui->listWidget_2->addItem(line);
        qDebug()<<month<<"  "<<year;
    }
    // sucht erste Zeile mit Periodenübertrag
    QRegularExpressionMatch match1 = re1.match(s);
    if(match1.hasMatch()){
        day = match1.captured(1);
        day_nr = "1";

        map["Day"] = day;

        uebertrag = match1.captured(2);
        line = day + " " + day_nr + " " + uebertrag;
        ui->listWidget_2->addItem(line);
        qDebug()<<day<<day_nr<<uebertrag;
    }
    // Kürzel des Arbeitstypes
    QRegularExpressionMatch match11 = re11.match(s);
    if(match11.hasMatch()){
        arb_short = match11.captured(1);
        qDebug() << arb_short << "hier";
        if(!arb_short.isEmpty() && !mittelteil1.isEmpty()){
            map["Shorts"] = arb_short;
        }
    }
    // filtert die Tageszeile
    QRegularExpressionMatch match2 = re2.match(s);
    if(match2.hasMatch()) {


        day = match2.captured(1);
        day_nr = match2.captured(2);

        map["DayNr"] = day_nr;

        mittelteil1 = match2.captured(3).trimmed();
        mittelteil2 = match2.captured(4);
        if(!day.trimmed().isEmpty()){
            map["Day"] = day;
            line = day + " " + day_nr + " " + mittelteil1;
            ui->listWidget_2->addItem(line);
            qDebug()<<day<<day_nr<<mittelteil1;
        }
        endteil = match2.captured(5);
        if(!endteil.trimmed().isEmpty()){
            end_exists = true;
            line = endteil;
            endteil_pieces = endteil.split(' ');
            //qDebug() << endteil_pieces.length();
            sollzeit = endteil_pieces.value(1);
            netzeit = endteil_pieces.value(4);
            diff = endteil_pieces.value(7);

            if(endteil_pieces.length() > 12){
                saldo = endteil_pieces.value(12);
            }
            else{
                saldo = endteil_pieces.value(11);
            }


            map["Soll"] = sollzeit;
            map["Netto"] = netzeit;
            map["Diff"] = diff;
            map["Saldo"] = saldo;

            ui->listWidget_2->addItem(line);
            qDebug()<< endteil;
            //qDebug() << sollzeit+','+netzeit+','+diff+','+saldo;
        }

        // Tag zusammengefasst
        if(!map.value("Day").isEmpty() && !map.value("DayNr").isEmpty() && !map.value("Soll").isEmpty()
                && !map.value("Netto").isEmpty() && !map.value("Diff").isEmpty() && !map.value("Saldo").isEmpty() && end_exists == true){
            line = map.value("Day") + " " + map.value("DayNr") + " " + map.value("Shorts") + " : " + map.value("Soll") +" "+ map.value("Netto") +" "+ map.value("Diff") +" "+ map.value("Saldo");
            ui->listWidget_4->addItem(line);
        }
    }

    // Detaiierte Zeitangabe pro Tag -> listwidget3

    // greift zweite Arbeitszeit in der Zeile auf
    QRegularExpressionMatch match5 = re5.match(s);
    if(match5.hasMatch()){
        ftime = match5.captured(1);
    }
    //greift erste Arbeitszeit in der Zeile auf
    QRegularExpressionMatch match6 = re6.match(s);
    if(match6.hasMatch()){
        ltime = match6.captured(1);
    }
    //Tagesanfang
    if(!day_nr.isEmpty() && !arb_short.isEmpty()){
        line = day + " " + day_nr + " " +arb_short + " " + ftime + " - " + ltime;
        ui->listWidget_3->addItem(line);
    }
    //weitere Zeile des Tages
    if(!day_nr.isEmpty() && arb_short.isEmpty()){
        line = day + " " + day_nr + " " + ftime + " - " + ltime;
        ui->listWidget_3->addItem(line);
    }
    if(!arb_short.compare(mittelteil1)){
        map["Shorts"] = arb_short;
    }
    if(end_exists == true){
    qDebug() << "----------------------";
    qDebug() << map.value("Day") << day;
    qDebug() << map.value("DayNr") << day_nr;
    qDebug() << map.value("Shorts") << arb_short << mittelteil1;
    qDebug() << map.value("Soll") << sollzeit;
    qDebug() << map.value("Netto") << netzeit;
    qDebug() << map.value("Diff") << diff;
    qDebug() << map.value("Saldo") << saldo;
    qDebug() << "----------------------";
    }
}

