#ifndef TAGESDATEN_H
#define TAGESDATEN_H

#include <QString>
#include <QTime>
#include <algorithm>

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

    const QVarLengthArray<QString> &getKommt() const;
    void setKommt(QString newKommt);

    const QVarLengthArray<QString> &getGeht() const;
    void setGeht(QString newGeht);

    bool already_inKommt(QString anfang);

    bool already_inGeht(QString ende);

    const QVarLengthArray<QString> &getFlexArbkommt() const;
    void setFlexArbkommt(QString newFlexArb);

    bool already_inflexArbkommt(QString ende);

    bool already_inflexgeht(QString ende);

    const QVarLengthArray<QString> &getFlexArbgeht() const;
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

    const QVarLengthArray<qint32> &getRemember_timekommt() const;
    void setRemember_timekommt(qint32 newRemember_timekommt);

    const QVarLengthArray<qint32> &getRemember_timegeht() const;
    void setRemember_timegeht(qint32 newRemember_timegeht);

    const QVarLengthArray<qint32> &getRemember_timeflexkommt() const;
    void setRemember_timeflexkommt(qint32 newRemember_timeflexkommt);

    const QVarLengthArray<qint32> &getRemember_timeflexgeht() const;
    void setRemember_timeflexgeht(qint32 newRemember_timeflexgeht);

    qint32 just_Minutes(QString zeit);

    const QString &getOffice_time() const;
    void setOffice_time_pause();

    const QString &getFlexible_time() const;
    void setFlexible_time_pause();


    const QString &getDate() const;
    void setDate(const QString &newDate);

    const QString &getMonthInt() const;
    void setMonthInt(const QString &newMonthInt);

    void setDaytimesZero();

    const QVarLengthArray<qint32> &getKommt_zeiten() const;
    void setKommt_zeiten(qint32 &newKommt_zeiten);

    const QVarLengthArray<qint32> &getGeht_zeiten() const;
    void setGeht_zeiten(qint32 &newGeht_zeiten);


private:
    qint32 gesamte_tageszeit = 0;
    qint32 pausenzeit = 0;
    qint32 netto_int = 0;
    qint32 flexNetto_int = 0;

    QString tag;
    QString tages_nr = "0";
    QString arb_art;

    //endteil
    QString soll_zeit;
    QString netto_zeit;
    QString zeit_diff;
    QString zeit_saldo;

    QString office_time = "0.00";
    QString flexible_time = "0.00";
    //Listen der Zeitstrings (9:32)
    QVarLengthArray <QString> Kommt;
    QVarLengthArray <QString> Geht;
    QVarLengthArray <QString> flexArbkommt;
    QVarLengthArray <QString> flexArbgeht;
    //Listen der Zeiten in Minuten (572)
    QVarLengthArray <qint32> kommt;
    QVarLengthArray <qint32> geht;
    QVarLengthArray <qint32> flexkommt;
    QVarLengthArray <qint32> flexgeht;

    //Test für Pausenzeit Berechnungen
    QVarLengthArray<qint32> kommt_zeiten;
    QVarLengthArray<qint32> geht_zeiten;

    QString monthInt;
    QString date;
    bool end_line = false;
    bool in_otherline = false;
};



#endif // TAGESDATEN_H
