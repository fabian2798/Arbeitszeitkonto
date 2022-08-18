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
#include "chartbuilder.h"
#include "optionswindow.h"
#include "helpwindow.h"
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
    int valuesOfChart[2]; // [0] = Office, [1] = HomeOffice

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
    void getDistribution(int * test1, int * test2, QString bm, QString by, QString em, QString ey);

    void create_table();
    void insert_table(Tagesdaten *data);
    void show_table();
<<<<<<< HEAD
    void update_day(Tagesdaten *data);

=======
    void fillComboBoxesFromDB();
    void delete_month(Tagesdaten *data);
>>>>>>> d6fb62e601054c8ed6a7af69866b31fff25e3df9

    QSqlDatabase db;

private slots:
    void drop_table();
    void on_loadFile_clicked();
    void on_pushButton_options_clicked();
    void on_pushButton_help_clicked();
    void on_pushButton_clicked();
    void on_pushButton_stats_update_clicked();
};

#endif // MAINWINDOW_H
