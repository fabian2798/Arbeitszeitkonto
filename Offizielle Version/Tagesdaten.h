#ifndef TAGESDATEN_H
#define TAGESDATEN_H

#include <QDebug>
#include <QTime>

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

    void clearAllTimes();

    void add_tagesende(QString soll, QString netto, QString diff, QString saldo);

    void add_toarbeitszeit(QString anfang, QString ende);

    const QList<QString> &getTagesende() const;

    const QList<QString> &getKommt() const;
    void setKommt(QString newKommt);

    const QList<QString> &getGeht() const;
    void setGeht(QString newGeht);

    bool already_inKommt(QString anfang);

    bool already_inGeht(QString ende);

    const QList<QString> &getFlexArbkommt() const;
    void setFlexArbkommt(QString newFlexArb);

    bool already_inflexArb(QString anfang);

    const QList<QString> &getFlexArbgeht() const;
    void setFlexArbgeht(QString newFlexArbgeht);

    qint32 getGesamte_tageszeit() const;
    void setGesamte_tageszeit(qint32 flex, qint32 netto,qint32 pause);

    void calc_breaktime();

    qint32 getPausenzeit() const;

    void setPausenzeit(qint32 newPausenzeit);

    qint32 getNetto_int() const;
    void add_toNetto_int(qint32 newNetto_int);
    void setNetto_int(qint32 newNetto_int);

    qint32 getFlexNetto_int() const;
    void add_toFlexNetto_int(qint32 newFlexNetto_int);
    void setFlexNetto_int(qint32 newFlexNetto_int);

    const QList<qint32> &getRemember_timekommt() const;
    void setRemember_timekommt(qint32 newRemember_timekommt);

    const QList<qint32> &getRemember_timegeht() const;
    void setRemember_timegeht(qint32 newRemember_timegeht);

    const QList<qint32> &getRemember_timeflexkommt() const;
    void setRemember_timeflexkommt(qint32 newRemember_timeflexkommt);

    const QList<qint32> &getRemember_timeflexgeht() const;
    void setRemember_timeflexgeht(qint32 newRemember_timeflexgeht);

    qint32 just_Minutes(QString zeit);

    const QString &getOffice_time() const;
    void setOffice_time_pause();

    const QString &getFlexible_time() const;
    void setFlexible_time_pause();


    const QString &getDate() const;
    void setDate(const QString &newDate);

private:
    qint32 gesamte_tageszeit = 0;
    qint32 pausenzeit = 0;
    qint32 netto_int = 0;
    qint32 flexNetto_int = 0;

    QString tag;
    QString tages_nr;
    QString arb_art;

    //endteil
    QString soll_zeit;
    QString netto_zeit;
    QString zeit_diff;
    QString zeit_saldo;

    QString office_time;
    QString flexible_time;

    QList <QString> Kommt;
    QList <QString> Geht;
    QList <QString> flexArbkommt;
    QList <QString> flexArbgeht;

    QList <qint32> kommt;
    QList <qint32> geht;
    QList <qint32> flexkommt;
    QList <qint32> flexgeht;

    QString date;
    bool end_line = false;
    bool in_otherline = false;
};



#endif // TAGESDATEN_H
