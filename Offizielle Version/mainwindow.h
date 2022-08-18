#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QTextStream>
#include "Tagesdaten.h"
#include "monat.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString fileName;   
    Tagesdaten process_line(QString s,Tagesdaten *data, monat *m_data);
    Tagesdaten end_calc(qint32 begin_inMin, qint32 end_inMin, Tagesdaten *data);
    Tagesdaten end_flexcalc(qint32 begin_inMin, qint32 end_inMin, Tagesdaten *data);
    QString Minutes_toString(qint32 zeit_Min);
    void toMinutesandHours(monat *m_data);
    QString get_monatsView(monat *m_data);
    QString monthtoInt(monat *m_data);
    void dateString(Tagesdaten *data, monat *m_data);

    void create_table();
    void insert_table(Tagesdaten *data);
    void show_table();
    void delete_month(Tagesdaten *data);

    void updateones();


    QSqlDatabase db;

private slots:
    void loadFile();
    void drop_table();

};

#endif // MAINWINDOW_H
