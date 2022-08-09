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


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Tagesdaten
{
public:

    const QString &getTag() const;
    void setTag(const QString &newTag);

    const QString &getTages_nr() const;
    void setTages_nr(const QString &newTages_nr);

    const QString &getArb_art() const;
    void setArb_art(const QString &newArb_art);

    const QString &getSoll_zeit() const;
    void setSoll_zeit(const QString &newSoll_zeit);

    const QString &getNetto_zeit() const;
    void setNetto_zeit(const QString &newNetto_zeit);

    const QString &getZeit_diff() const;
    void setZeit_diff(const QString &newZeit_diff);

    const QString &getZeit_saldo() const;
    void setZeit_saldo(const QString &newZeit_saldo);

    bool getEnd_line() const;
    void setEnd_line(bool newEnd_line);

    bool getIn_otherline() const;
    void setIn_otherline(bool newIn_otherline);

    const QList<QString> &getArbeitszeit() const;
    void add_toarbeitszeit(QString anfang, QString ende);

    void removeAllTimes();

    void add_tagesende(QString soll, QString netto, QString diff, QString saldo);


    const QList<QString> &getTagesende() const;

private:
    QString tag;
    QString tages_nr;
    QString arb_art;
    QString soll_zeit;
    QString netto_zeit;
    QString zeit_diff;
    QString zeit_saldo;
    QList <QString> arbeitszeit;
    bool end_line = false;
    bool in_otherline = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString fileName;   
    Tagesdaten process_line(QString s,Tagesdaten *data);

private slots:
    void loadFile();

};

#endif // MAINWINDOW_H
