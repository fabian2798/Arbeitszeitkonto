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
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
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
    bool it_exists = false;
    QMap<QString, QString> map;
    QMap<QString, QString> maptemp;
    Ui::MainWindow *ui;
    QString fileName;
    QString filetyp;
    //QString sumsoll, sumnet, sumdiff, sumsaldo;
    //QSqlDatabase db;
    void process_line(QString s);
    void check_filetyp(QString filename);
    //void insert_data(QString month, QString year);
    void calc_sums(QString soll, QString net, QString diff , QString saldo);
private slots:
    void loadFile();
    //void showdb();
};

#endif // MAINWINDOW_H
