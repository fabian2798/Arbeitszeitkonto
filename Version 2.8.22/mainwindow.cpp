#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /**
    // sql connection
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Projects Qt/testsql/arbeitszeit.db");
    if(!db.open()){
        qWarning() << "ERROR" << db.lastError();
    }
    else{
        QSqlQuery create_query;
        create_query.exec("create table arbZeit"
                     "(id integer primary key autoincrement,"
                     "monat text not null,"
                     "jahr text not null);");
    }**/
}

MainWindow::~MainWindow()
{
    delete ui;
}
/**
// sql insert
void MainWindow::insert_data(QString month, QString year){
    QSqlQuery insert_query;
    insert_query.exec("insert into arbZeit(monat,jahr) values(?,?)");
    insert_query.bindValue(0,month);
    insert_query.bindValue(1,year);
    if(!insert_query.isActive()){
            qWarning() << "ERROR" << insert_query.lastError().text();
    }
}

// show table
void MainWindow::showdb(){
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("select * from arbZeit");
    model->setHeaderData(0,Qt::Horizontal,tr("ID"));
    model->setHeaderData(1,Qt::Horizontal,tr("Monat"));
    model->setHeaderData(2,Qt::Horizontal,tr("Jahr"));
    ui->tableView->setModel(model);
    ui->tableView->show();
}**/
/**
// calculate sum of sollzeit,netzeit,diff,saldo
void MainWindow:: calc_sums(QString soll, QString net, QString diff , QString saldo){
    QRegularExpression vorzeichen(R"([+-}])");
    int sollh, sollmin;
    sollh = soll.split('.').first().toInt();
    sollmin = soll.split('.').last().toInt();
    QString neth,netmin;
    neth = net.split('.').first();
    netmin = net.split('.').last();
    QString vorz;
    int diffh, diffmin;
    vorz = "+";
    if(!diff.contains('+')){
        vorz = "-";
    }
    diff.remove(vorzeichen);
    diffh = diff.split('.').first().toInt();
    diffmin = diff.split('.').last().toInt();
    int saldoh, saldomin;
    QString vorz_saldo;
    vorz_saldo = "+";
    if(!saldo.contains('+')){
        vorz_saldo = "-";
    }
    saldo.remove(vorzeichen);
    saldoh = saldo.split('.').first().toInt();
    saldomin = saldo.split('.').last().toInt();
    double sollz = 0.00;
    double nettz = 0.00;
    double diffz = 0.00;
    double saldoz = 0.00;
}**/
// Check Filetyp
void MainWindow:: check_filetyp(QString filename){
    QFile file(filename);
    filetyp = "Monatsliste";
    QTextStream in(&file);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = in.readAll();
    QString type2 = "Übersichtstabelle";
    file.close();
    if(!content.contains("Abteilung:")){
        qDebug() << "Übersichtstabelle";
        filetyp = type2;
    }
    else{
        qDebug() << "Monatsliste";
    }
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

    // Check Filetyp
    check_filetyp(fileName);

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
    QString pattern_shorts(R"((\sFA\s|\sURL\s|\sGLT\s|\sF\s|\sABS\s|\:\d{2}\s+\|))");
    // Erfasst erste auftretende Zeit
    QString first_time(R"((\s\d{1,2}\:\d{1,2}\+\s\*|\s\d{1,2}\:\d{1,2}\+\s|\s{3}\d{1,2}\:\d{1,2}))");
    // Erfasst letzte auftretende Zeit
    QString last_time(R"(\-\s+(\d{1,2}\:\d{1,2})\s+(FA|URL|GLT|F|ABS|\s+))");
    QString pattern_day(R"(^(\w{2}|\s+)\s+(\d\d?)\s+\d{1,4}\s+\|\s+(?:\d{1,2}:\d{2})?\s+-?\s{1,2}(?:\d{1,2}:\d{2})[+ *]{3}\s+(.*)\|(.*)\|(.*))");
    // Letzte Zeile -> Summarize
    //QString pattern_summe(R"(^(\s+Periodensumme\s+)(\d{1,4}\.\d{1,4})(\s+\d{1,4}\.\d{1,4})(\s+[+-]\d{1,4}\.\d{1,4})(\s+[+-]\d{1,4}\.\d{1,4}))");
    // Overviewtable pattern
    QString pattern_ArbZeit_Ergebnisse(R"(\s+(Mo|Di|Mi|Do|Fr|Sa|So)\s(\d{2}\.\d{2}\.\d{2})\s+(\d{1,4}\.\d{1,4})\s+(\d{1,4}\.\d{1,4})\s+(\d{1,4}\.\d{1,4})\s+([+-]\d{1,4}\.\d{1,4})\s+([+-]\d{1,4}\.\d{1,4}))");

    QString pattern_days(R"((Mo|Di|Mi|Do|Fr|Sa|So))");
    static QRegularExpression re_day(pattern_days);

    static QRegularExpression re0(pattern_monthyear);
    static QRegularExpression re1(pattern_first);
    static QRegularExpression re2(pattern_day);
    //static QRegularExpression re3(pattern_summe);

    static QRegularExpression re4(pattern_ArbZeit_Ergebnisse);
    static QRegularExpression re11(pattern_shorts);
    static QRegularExpression re5(first_time);
    static QRegularExpression re6(last_time);


    //Arbeitszeittypkürzel
    QString arb_short;
    QString shorts = " ";
    //Zeiten pro Zeile
    QString ftime, ltime;

    QString type2 = "Übersichtsliste";
    //Split Endteil
    QStringList endteil_pieces = {};
    // Daten für Übersichtstabelle
    QString tag,datum,soll,ntarb,flexarb,fztyp28,fztyp29;
    // Periodenabschluss
    //QString match_periodensumme;

    // Aufteilung des Endteils am Tag
    QString sollzeit, netzeit, diff, saldo = "";

    QString uebertrag;
    QString month, year, day, day_nr;
    QString mittelteil1,mittelteil2,endteil;
    QString line = "";
    QString line_widget4 ="";
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

        if(day.contains(re_day)){
           map["Day"] = day;
           maptemp["Day"] = map["Day"];
        }

        uebertrag = match1.captured(2);
        line = day + " " + day_nr + " " + uebertrag;
        ui->listWidget_2->addItem(line);
        qDebug()<<day<<day_nr<<uebertrag;
    }
    // Kürzel des Arbeitstypes
    QRegularExpressionMatch match11 = re11.match(s);
    if(match11.hasMatch()){
        arb_short = match11.captured(1).trimmed();
        if(arb_short.contains("FA")||arb_short.contains("F")||arb_short.contains("URL")||arb_short.contains("GLT")||arb_short.contains("ABS")){
            shorts = arb_short;
            //qDebug() << shorts;
        }
    }

    map["Shorts"] = shorts;
    maptemp["Shorts"] = map["Shorts"];

    // filtert die Tageszeile
    QRegularExpressionMatch match2 = re2.match(s);
    if (match2.hasMatch()) {
        day = match2.captured(1);
        day_nr = match2.captured(2);

        map["DayNr"] = day_nr;
        maptemp["DayNr"] = map["DayNr"];
        //if(day.contains(re_day)){
        //    map["Day"] = day;
        //}
        mittelteil1 = match2.captured(3).trimmed();
        mittelteil2 = match2.captured(4);
        if(!day.trimmed().isEmpty()){
            map["Day"] = day;
            maptemp["Day"] = map["Day"];
            line = day + " " + day_nr + " " + mittelteil1;
            ui->listWidget_2->addItem(line);
            qDebug()<<day<<day_nr<<mittelteil1;
        }
        endteil = match2.captured(5);
        if(!endteil.isEmpty()){
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
            qDebug() << "end " << endteil_pieces;
            map["Soll"] = sollzeit;
            map["Netto"] = netzeit;
            map["Diff"] = diff;
            map["Saldo"] = saldo;
            maptemp["Soll"] = map["Soll"];
            maptemp["Netto"] = map["Netto"];
            maptemp["Diff"] = map["Diff"];
            maptemp["Saldo"] = map["Saldo"];



            ui->listWidget_2->addItem(line);
            qDebug()<< endteil;
            //qDebug() << sollzeit+','+netzeit+','+diff+','+saldo;
        }
        line_widget4 = maptemp.value("Day") + " " + maptemp.value("DayNr") + " " + maptemp.value("Shorts") + " : " + maptemp.value("Soll") +" "+ maptemp.value("Netto") +" "+ maptemp.value("Diff") +" "+ maptemp.value("Saldo");
        qDebug() << "1: " << line_widget4;
        // Tag zusammengefasst
        if(!map.value("Day").isEmpty() && !map.value("DayNr").isEmpty() && !map.value("Soll").isEmpty()
                && !map.value("Netto").isEmpty() && !map.value("Diff").isEmpty() && !map.value("Saldo").isEmpty()){
            //it_exists = true;
            line = map.value("Day") + " " + map.value("DayNr") + " " + map.value("Shorts") + " : " + map.value("Soll") +" "+ map.value("Netto") +" "+ map.value("Diff") +" "+ map.value("Saldo");
            qDebug() <<"2: " << line;
            ui->listWidget_4->addItem(line);
        }
    }


    /**
    // Periodensummendaten
    QRegularExpressionMatch match3 =re3.match(s);
    if (match3.hasMatch()){
        match_periodensumme = match3.captured(1).trimmed();
        sum_sollzeit = match3.captured(2);
        sum_netzeit = match3.captured(3);
        sum_differentz = match3.captured(4).trimmed();
        sum_saldo = match3.captured(5).trimmed();
        //line = match_periodensumme + " " + soll_zeit + " " + net_zeit + " " + differentz + " " + saldo;
        line = "Sollzeit:        " + sum_sollzeit;
        ui->listWidget_2->addItem(line);
        line = "Nettozeit:    " + sum_netzeit;
        ui->listWidget_2->addItem(line);
        line = "Differentz:  " + sum_differentz;
        ui->listWidget_2->addItem(line);
        line = "Saldo:          " + sum_saldo;
        ui->listWidget_2->addItem(line);
        qDebug() << match_periodensumme;
        qDebug() << "Sollzeit:    " + sum_sollzeit;
        qDebug() << "Nettozeit:   " + sum_netzeit;
        qDebug() << "Differentz:  " + sum_differentz;
        qDebug() << "Saldo:       " + sum_saldo;
    }**/
    // Übersichtstabelle
    QRegularExpressionMatch match4 = re4.match(s);
    if (match4.hasMatch() && filetyp.compare(type2)){
        tag = match4.captured(1);
        datum = match4.captured(2);
        soll = match4.captured(3);
        ntarb = match4.captured(4);
        flexarb = match4.captured(5);
        fztyp28 = match4.captured(6);
        fztyp29 = match4.captured(7);
        line = tag + " " + datum;
        ui->listWidget_2->addItem(line);
        line = soll + " " + ntarb + " " + flexarb + " " + fztyp28 + " " + fztyp29;
        ui->listWidget_2->addItem(line);
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
    if(!day_nr.isEmpty() && !shorts.isEmpty()){
        line = day + " " + day_nr + " " + shorts + " " + ftime + " - " + ltime;
        ui->listWidget_3->addItem(line);
    }
    //weitere Zeile des Tages
    if(!day_nr.isEmpty() && shorts.isEmpty()){
        line = day + " " + day_nr + " " + ftime + " - " + ltime;
        ui->listWidget_3->addItem(line);
    }
}


