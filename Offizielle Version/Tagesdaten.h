#ifndef TAGESDATEN_H
#define TAGESDATEN_H

#include <QDebug>

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

    void removeAllTimes();

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

private:
    QString tag;
    QString tages_nr;
    QString arb_art;
    QString soll_zeit;
    QString netto_zeit;
    QString zeit_diff;
    QString zeit_saldo;
    QList <QString> Kommt;
    QList <QString> Geht;
    QList <QString> flexArbkommt;
    QList <QString> flexArbgeht;
    bool end_line = false;
    bool in_otherline = false;
};



#endif // TAGESDATEN_H
