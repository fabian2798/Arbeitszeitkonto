#include "monat.h"

qint32 monat::getGes_Nettozeit() const
{
    return ges_Nettozeit;
}

void monat::setGes_Nettozeit(qint32 newGes_Nettozeit)
{
    ges_Nettozeit = ges_Nettozeit + newGes_Nettozeit;
}

qint32 monat::getGes_Flexnettozeit() const
{
    return ges_Flexnettozeit;
}

void monat::setGes_Flexnettozeit(qint32 newGes_Flexnettozeit)
{
    ges_Flexnettozeit = ges_Flexnettozeit + newGes_Flexnettozeit;
}

const QString &monat::getMonth() const
{
    return month;
}

void monat::setMonth(const QString &newMonth)
{
    month = newMonth;
}

const QString &monat::getYear() const
{
    return year;
}

void monat::setYear(const QString &newYear)
{
    year = newYear;
}

const QString &monat::getGes_nt() const
{
    return ges_nt;
}

void monat::setGes_nt(const QString &newGes_nt)
{
    ges_nt = newGes_nt;
}

const QString &monat::getGes_fnt() const
{
    return ges_fnt;
}

void monat::setGes_fnt(const QString &newGes_fnt)
{
    ges_fnt = newGes_fnt;
}

const QString &monat::getGesamtzeit() const
{
    return gesamtzeit;
}

void monat::setGesamtzeit(const QString &newGesamtzeit)
{
    gesamtzeit = newGesamtzeit;
}

qint32 monat::getGesamt() const
{
    return gesamt;
}

void monat::setGesamt(qint32 tages_gesamt)
{
    gesamt = gesamt + tages_gesamt;
}

const QString &monat::getFaProzent() const
{
    return faProzent;
}

void monat::setFaProzent(qint32 flex, qint32 gesamt)
{
    double flnt = flex;
    double gesamtzeit = gesamt;

    double prozente = (flnt / gesamtzeit) * 100;
    faProzent = QString::number(prozente);
}


