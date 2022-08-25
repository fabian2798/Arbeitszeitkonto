#ifndef MONAT_H
#define MONAT_H

#include <QDebug>

class monat
{
public:

    qint32 getGes_Nettozeit() const;
    void setGes_Nettozeit(qint32 newGes_Nettozeit);

    qint32 getGes_Flexnettozeit() const;
    void setGes_Flexnettozeit(qint32 newGes_Flexnettozeit);

    const QString &getMonth() const;
    void setMonth(const QString &newMonth);

    const QString &getYear() const;
    void setYear(const QString &newYear);

    const QString &getGes_nt() const;
    void setGes_nt(const QString &newGes_nt);

    const QString &getGes_fnt() const;
    void setGes_fnt(const QString &newGes_fnt);

    const QString &getGesamtzeit() const;
    void setGesamtzeit(const QString &newGesamtzeit);

    qint32 getGesamt() const;
    void setGesamt(qint32 tages_gesamt);

    const QString &getFaProzent() const;
    void setFaProzent(qint32 flex, qint32 gesamt);

private:
    //for calculations
    qint32 ges_Nettozeit;
    qint32 ges_Flexnettozeit;
    qint32 gesamt;
    //for output
    QString month;
    QString year;
    QString ges_nt;
    QString ges_fnt;
    QString gesamtzeit;
    QString faProzent;

};

#endif // MONAT_H
