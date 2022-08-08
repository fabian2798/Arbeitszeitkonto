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
QString MainWindow:: check_filetyp(QString filename){
    QFile file(filename);
    QTextStream in(&file);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = in.readAll();
    QString type1 = "Monatsliste";
    QString type2 = "Übersichtstabelle";
    file.close();
    if(content.contains("Abteilung:")){
        qDebug() << "Monatsliste";
        return type1;
    }
    else{
        qDebug() << "Übersichtstabelle";
        return type2;
    }
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

    QString ftype = check_filetyp(fileName); // Monatsliste or Übersichtstabelle

    QTextStream in(&file);
    ui->listWidget_2->clear();
    while (!in.atEnd()) {
       QString line = in.readLine();
       process_line(line,ftype);
       }
    file.close();
    ui->listWidget->insertItem(1,fileName);
}

void MainWindow::process_line(QString s, QString ftype){
    QString pattern_monthyear(R"(^(Januar|Februar|März|April|Mai|Juni|Juli|August|September|Oktober|November|Dezember)\s+(\d\d\d\d)$)");
    QString pattern_first(R"(^(Mo|Di|Mi|Do|Fr|Sa|So)\s+1.*Periode\s+([+-]\d{1,4}\.\d{2})*)"); //To DO: Skip Sa/So unnessecary for Trainees
    QString pattern_day(R"(^(\w{2}|\s+)\s+(\d\d?)\s+\d{1,4}\s+\|\s+(?:\d{1,2}:\d{2})?\s+-?\s{1,2}(?:\d{1,2}:\d{2})[+ *]{3}\s+(.*)\|(.*)\|(.*))");
    QString pattern_summe(R"(^(\s+Periodensumme\s+)(\d{1,4}\.\d{1,4})(\s+\d{1,4}\.\d{1,4})(\s+[+-]\d{1,4}\.\d{1,4})(\s+[+-]\d{1,4}\.\d{1,4}))"); // Letzte Zeile -> Summarize
    QString pattern_ArbZeit_Ergebnisse(R"(\s+(Mo|Di|Mi|Do|Fr|Sa|So)\s(\d{2}\.\d{2}\.\d{2})\s+(\d{1,4}\.\d{1,4})\s+(\d{1,4}\.\d{1,4})\s+(\d{1,4}\.\d{1,4})\s+([+-]\d{1,4}\.\d{1,4})\s+([+-]\d{1,4}\.\d{1,4}))");
    static QRegularExpression re0(pattern_monthyear);
    static QRegularExpression re1(pattern_first);
    static QRegularExpression re2(pattern_day);
    static QRegularExpression re3(pattern_summe);
    static QRegularExpression re4(pattern_ArbZeit_Ergebnisse);
    QString type2 = "Übersichtsliste";
    QString tag,datum,soll,ntarb,flexarb,fztyp28,fztyp29; // Daten für Übersichtstabelle
    QString match_periodensumme;
    QString soll_zeit,net_zeit,differentz,saldo;
    QString uebertrag;
    QString month, year, day, day_nr;
    QString mittelteil1,mittelteil2, endteil;
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
        day_nr = "1";
        uebertrag = match1.captured(2);
        line = day + " " + day_nr + " " + uebertrag;
        ui->listWidget_2->addItem(line);
        qDebug()<<day<<day_nr<<uebertrag;
    }
    // filtert die Tageszeile
    QRegularExpressionMatch match2 = re2.match(s);
    if (match2.hasMatch()) {
        day = match2.captured(1);
        day_nr = match2.captured(2);

        mittelteil1 = match2.captured(3).trimmed();
        mittelteil2 = match2.captured(4);
        if(!day.trimmed().isEmpty()){
            line = day + " " + day_nr + " " + mittelteil1;
            ui->listWidget_2->addItem(line);
            qDebug()<<day<<day_nr<<mittelteil1;
        }
        endteil = match2.captured(5);
        if(!endteil.isEmpty()){
            line = endteil;
            ui->listWidget_2->addItem(line);
           qDebug()<<endteil;
        }

    }
    // Periodensummendaten
    QRegularExpressionMatch match3 =re3.match(s);
    if (match3.hasMatch()){
        match_periodensumme = match3.captured(1).trimmed();
        soll_zeit = match3.captured(2);
        net_zeit = match3.captured(3);
        differentz = match3.captured(4).trimmed();
        saldo = match3.captured(5).trimmed();
        //line = match_periodensumme + " " + soll_zeit + " " + net_zeit + " " + differentz + " " + saldo;
        line = "Sollzeit:        " + soll_zeit;
        ui->listWidget_2->addItem(line);
        line = "Nettozeit:    " + net_zeit;
        ui->listWidget_2->addItem(line);
        line = "Differentz:  " + differentz;
        ui->listWidget_2->addItem(line);
        line = "Saldo:          " + saldo;
        ui->listWidget_2->addItem(line);
        qDebug() << match_periodensumme;
        qDebug() << "Sollzeit:    " + soll_zeit;
        qDebug() << "Nettozeit:   " + net_zeit;
        qDebug() << "Differentz:  " + differentz;
        qDebug() << "Saldo:       " + saldo;

    }
    // Übersichtstabelle
    QRegularExpressionMatch match4 = re4.match(s);
    if (match4.hasMatch() && ftype.compare(type2)){
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

}

