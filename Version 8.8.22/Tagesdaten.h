#ifndef TAGESDATEN_H
#define TAGESDATEN_H

#include "mainwindow.h"


const QString &Tagesdaten::getTag() const
{
    return this->tag;
}

void Tagesdaten::setTag(const QString &newTag)
{
    this->tag = newTag;
}

const QString &Tagesdaten::getTages_nr() const
{
    return this->tages_nr;
}

void Tagesdaten::setTages_nr(const QString &newTages_nr)
{
    this->tages_nr = newTages_nr;
}

const QString &Tagesdaten::getArb_art() const
{
    return this->arb_art;
}

void Tagesdaten::setArb_art(const QString &newArb_art)
{
    this->arb_art = newArb_art;
}

const QString &Tagesdaten::getSoll_zeit() const
{
    return this->soll_zeit;
}

void Tagesdaten::setSoll_zeit(const QString &newSoll_zeit)
{
    this->soll_zeit = newSoll_zeit;
}

const QString &Tagesdaten::getNetto_zeit() const
{
    return this->netto_zeit;
}

void Tagesdaten::setNetto_zeit(const QString &newNetto_zeit)
{
    this->netto_zeit = newNetto_zeit;
}

const QString &Tagesdaten::getZeit_diff() const
{
    return this->zeit_diff;
}

void Tagesdaten::setZeit_diff(const QString &newZeit_diff)
{
    this->zeit_diff = newZeit_diff;
}

const QString &Tagesdaten::getZeit_saldo() const
{
    return this->zeit_saldo;
}

void Tagesdaten::setZeit_saldo(const QString &newZeit_saldo)
{
    this->zeit_saldo = newZeit_saldo;
}

bool Tagesdaten::getEnd_line() const
{
    return this->end_line;
}

void Tagesdaten::setEnd_line(bool newEnd_line)
{
    this->end_line = newEnd_line;
}

bool Tagesdaten::getIn_otherline() const
{
    return this->in_otherline;
}

void Tagesdaten::setIn_otherline(bool newIn_otherline)
{
    this->in_otherline = newIn_otherline;
}

const QList<QString> &Tagesdaten::getArbeitszeit() const
{
    return this->arbeitszeit;
}

void Tagesdaten::add_toarbeitszeit(QString anfang, QString ende){
    if(!anfang.isEmpty() && !ende.isEmpty() && anfang.compare(ende) != 0){
        this->arbeitszeit.append(anfang);
        this->arbeitszeit.append(ende);
    }
    //mögliche Falschbuchung
    //wird vermutlich doppelt vorkommen
    else{
        if(!anfang.isEmpty() && ende.isEmpty()){
            this->arbeitszeit.append(anfang);
        }
        if(!ende.isEmpty() && anfang.isEmpty()){
            this->arbeitszeit.append(ende);
        }
        if(anfang.compare(ende) == 0){
            this->arbeitszeit.append(anfang);
        }
    }
}

void Tagesdaten::removeAllTimes(){
   this->arbeitszeit.clear();
}


#endif // TAGESDATEN_H
