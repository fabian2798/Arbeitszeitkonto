#include "Tagesdaten.h"

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

const QList<QString> &Tagesdaten::getKommt() const
{
    return this->Kommt;
}

void Tagesdaten::setKommt(QString newKommt)
{
    if(!newKommt.isEmpty()){
    this->Kommt.append(newKommt);
    }
}

const QList<QString> &Tagesdaten::getGeht() const
{
    return this->Geht;
}

void Tagesdaten::setGeht(QString newGeht)
{
    if(!newGeht.isEmpty()){
    this->Geht.append(newGeht);
    }
}

bool Tagesdaten::already_inKommt(QString anfang){
    return Kommt.contains(anfang);
}

bool Tagesdaten::already_inGeht(QString ende){
    return Kommt.contains(ende);
}

const QList<QString> &Tagesdaten::getFlexArbkommt() const
{
    return this->flexArbkommt;
}

void Tagesdaten::setFlexArbkommt(QString newFlexArb)
{
    this->flexArbkommt.append(newFlexArb);
}

bool Tagesdaten::already_inflexArb(QString anfang){// Für Fehlbuchung bei FlexArb, da Regex nicht die letzte Buchung nimmt (z.B 15:30 -)
    return flexArbkommt.contains(anfang);
}

const QList<QString> &Tagesdaten::getFlexArbgeht() const
{
    return this->flexArbgeht;
}

void Tagesdaten::setFlexArbgeht(QString newFlexArbgeht)
{
    this->flexArbgeht.append(newFlexArbgeht);
}

void Tagesdaten::add_toarbeitszeit(QString anfang, QString ende){
    //TO DO: Bei Doppelbuchung z.B 15:30-15:30 wird akutell beim zweiten Post es in Kommt und Geht eingetragen -- 9.8.22 10:02
    //Spezial TO DO: Wenn Fehlbuchung aufkommt und die Endzeit und beim zweiten Post die Anfangszeit nicht übereintreffen wird Endzeit nicht eingetragen siehe Journal_Juni Fr 24 -- 9.8.22 10:02
    if(!anfang.isEmpty() && !ende.isEmpty() && anfang.compare(ende) != 0){
        //Fehlbuchung FLexible Arbeit
        //z.B - 7:04
        //7:04 - 15:30
        if(already_inflexArb(anfang) == true){
            //qDebug() << "0.5";
            flexArbgeht.append(ende);
        }
        if(flexArbkommt.size() == 0){
            //qDebug() << "1";
            this->Kommt.append(anfang);
            this->Geht.append(ende);
        }
    }
    //mögliche Falschbuchung
    //wird vermutlich doppelt vorkommen
    else{
        if(Kommt.contains(anfang) == true && !ende.isEmpty() && anfang.compare(ende) != 0){
            //qDebug() << "2";
            this->Kommt.append(ende);
        }
        //z.B 6:20 - 14:41
        //   14:41 -
        if(!anfang.isEmpty() && ende.isEmpty()){//Wird bisher nicht vom Regex aufgenommen, siehe übergeordnetes Beispiel
            //qDebug() << "3";
            if(already_inGeht(anfang) == false){
                this->Kommt.append(anfang);
                }
        }
        if(!ende.isEmpty() && anfang.isEmpty()){ // Homeoffice Einträge
            if(ende.compare("0:00") != 0 && already_inflexArb(ende) == false){ // Leerbuchung ausgeschlossen
                //qDebug() << "4";
                if(flexArbkommt.size() > flexArbgeht.size()){
//                    qDebug() << "4.5";
//                    qDebug() << flexArbkommt.size() << "flex kommt";
//                    qDebug() << flexArbgeht.size() << "flex geht";
                    this->flexArbgeht.append(ende);
                }
                if(flexArbkommt.size() < flexArbgeht.size() || flexArbkommt.size() == 0){
                    //qDebug() << "4.75";
                    this->flexArbkommt.append(ende);
                }
            }
        }
        if(anfang.compare(ende) == 0 && Kommt.size() > Geht.size()){ //Falschbuchung nur einmal aufnehmen , wenn eine bereits in Kommt enthalten
              //qDebug() << "5";
            this->Geht.append(anfang);
        }

        if(anfang.compare(ende) == 0 && (Kommt.size() == Geht.size() || Kommt.size() == 0) && already_inGeht(anfang) == false){ //Falschbuchung nur einmal aufnehmen , erstes Auftreten
              //qDebug() << "6";
            this->Kommt.append(anfang);
        }
    }
}

void Tagesdaten::removeAllTimes(){
   this->Kommt.clear();
   this->Geht.clear();
   this->flexArbkommt.clear();
   this->flexArbgeht.clear();
;}
