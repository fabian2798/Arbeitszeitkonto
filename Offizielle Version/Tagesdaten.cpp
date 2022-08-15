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

qint32 Tagesdaten::getGesamte_tageszeit() const
{
    return gesamte_tageszeit;
}

void Tagesdaten::setGesamte_tageszeit(qint32 flex, qint32 netto,qint32 pause)
{
    gesamte_tageszeit = (flex + netto) - pause;
}

void Tagesdaten::add_toarbeitszeit(QString anfang, QString ende){
    //qDebug() << "Anfang";

    if(!anfang.isEmpty() && !ende.isEmpty() && anfang.compare(ende) != 0){
        //Fehlbuchung FLexible Arbeit
        //z.B - 7:04
        //7:04 - 15:30
        if(already_inflexArb(anfang) == true){//Spezialfall TO DO: Juni 24 - FlexArbgeht
            //qDebug() << "0.5";
            flexArbgeht.append(ende);
        }
        if(flexArbkommt.size() == 0){
            //qDebug() << "1";
            this->Kommt.append(anfang);
            this->Geht.append(ende);
        }
        if(arb_art == "FA" && flexArbgeht.size() == flexArbkommt.size() && already_inflexArb(anfang) == false){//Fehlbuchung bei Homeoffice -> wird angezeigt wie Bürotag
            //qDebug() << "1.25";
            flexArbkommt.append(anfang);
            flexArbgeht.append(ende);
        }
        if(arb_art == "FA" && flexArbkommt.size() > flexArbgeht.size()){//Spezialfall Juni 24, zwei ungleiche Arbeitsanfänge mit nur einem Ende
            //qDebug() << "1.5";
            flexArbgeht.append(ende);
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
                //qDebug() << "3.3";
                this->Kommt.append(anfang);
                }
        }
        if(!ende.isEmpty() && anfang.isEmpty()){ // Homeoffice Einträge
            if(ende.compare("0:00") != 0 && already_inflexArb(ende) == false){ // Leerbuchung ausgeschlossen
                //qDebug() << "4.0";
                if(flexArbkommt.size() > flexArbgeht.size()){
                    //qDebug() << "4.5";
                    this->flexArbgeht.append(ende);
                }
                if(flexArbkommt.size() < flexArbgeht.size() || flexArbkommt.size() == 0){
                    //qDebug() << "4.75";
                    this->flexArbkommt.append(ende);
                }
                if(flexArbkommt.size() == flexArbgeht.size() && already_inflexArb(ende) == false && flexArbgeht.contains(ende) == false){//erneute Buchung im Homeoffice
                    //qDebug() << "4.8";
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

void Tagesdaten::clearAllTimes(){
   Kommt.clear();
   Geht.clear();
   flexArbkommt.clear();
   flexArbgeht.clear();
   arb_art = "";
   flexgeht.clear();
   flexkommt.clear();
   geht.clear();
   kommt.clear();
   pausenzeit = 0;
   netto_int = 0;
   flexNetto_int = 0;
}

qint32 Tagesdaten::getPausenzeit() const
{
    return pausenzeit;
}

void Tagesdaten::setPausenzeit(qint32 newPausenzeit)
{
    pausenzeit = newPausenzeit;
}

void Tagesdaten::calc_breaktime(){
    bool is_gebucht = false;
    //qDebug() << "Pausenzeit berechnen";
    //Nur Büro mit mehreren Buchungen
    if(kommt.size() == geht.size() && kommt.size() > 1 && flexkommt.size() == 0 && flexgeht.size() == 0){
        if(kommt.size() > 1 && geht.size() > 1){//Mitarbeiter liebt die Natur und macht seine Pause draußen
            //qDebug() << "time0.5";
            for(int i = 0;i<geht.size();i++){
               //qDebug() <<"i.1"<<i;
               //qDebug() << kommt[i+1];
                if(kommt[i+1] == 0){//out of range -- keine weitere buchung
                    //qDebug() << "time0.5 break";
                    break;
                }
                if(geht[i] < kommt[i+1] && kommt[i+1] - geht[i] >= 15){//Pausenzeit wird anerkannt
                    //qDebug() << "time0.75";
                    pausenzeit = pausenzeit + (kommt[i+1] - geht[i]);
                }
                if(kommt[i+1] == kommt.last()){//kann keine weitere Buchung kommen
                    //qDebug() << "time0.6 break";
                    break;
                }
            }
            if(pausenzeit < 30 && netto_int+flexNetto_int >= 360 && netto_int+flexNetto_int < 540){//War draußen hat aber keine 30 Min voll gehabt
                //qDebug() << "time1";
                pausenzeit = 30 - pausenzeit;
            }
            if(netto_int+flexNetto_int >= 540 && pausenzeit < 45){
                //qDebug() << "time2";
                pausenzeit = 45 - pausenzeit;
            }
            if(pausenzeit >= 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540){
               //qDebug() << "p2.2" << pausenzeit;
               //qDebug() << "time2.2";
                pausenzeit = 0; // Pause wurde genommen, Abzug am Schluss nötig
            }
            if(flexNetto_int+netto_int >= 540 && pausenzeit >= 45){
                //qDebug() << "p2.3" << pausenzeit;
                //qDebug() << "time2.3";
                pausenzeit = 0;
            }

        }
    }
    //Bürotag mit nur einer Buchung
    if(kommt.size() == 1 && geht.size() == 1){//Mitarbeiter ist ein Stubenhocker und macht keine Pause
        if(netto_int+flexNetto_int >= 360 && netto_int+flexNetto_int < 540){//über 6h
            //qDebug() << "time3";
            pausenzeit = 30;
        }
        if(netto_int+flexNetto_int >=540){
            //qDebug() << "time4";
        pausenzeit = 45;//ab 9h
        }
    }
    //Nur Homeoffice mit mehreren Buchungen
    if(flexkommt.size() == flexgeht.size() && flexkommt.size() > 1 && kommt.size() == 0 && geht.size() == 0){
        if(flexkommt.size() > 1 && flexgeht.size() > 1){//Naturliebhaber im Homeoffice
            //qDebug() << "time4.5";
            for(int j = 0;j<flexgeht.size();j++){
                //qDebug() << "j" << j;
                if(flexkommt[j+1] == 0){//keine zweite Buchung
                    //qDebug() << "time4.5 break";
                    break;
                }
                if(flexgeht[j] < flexkommt[j+1] && flexkommt[j+1] - flexgeht[j] >= 15){
                    //qDebug() << "time4.75";
                    pausenzeit = pausenzeit + (flexkommt[j+1] - flexgeht[j]);
                }
                if(flexkommt[j+1] == flexkommt.last()){//Nachdem kann kein weiterer Eintrag kommen
                    //qDebug() << "time4.8 break";
                    break;
                }
            }
            if(pausenzeit == 0  && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540){//keine pause bei über 6h
                //qDebug() << "time5";
                //qDebug() << "p5.1" << pausenzeit;
                is_gebucht = true;
                pausenzeit = 30;
            }
            if(flexNetto_int+netto_int >= 540 && pausenzeit == 0){//keine pause bei über 9h
                //qDebug() << "time6";
                is_gebucht = true;
                pausenzeit = 45;
            }
            if(pausenzeit >= 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540 && is_gebucht == false){//genug pause gemacht
                //qDebug() << "p6.1" << pausenzeit;
                //qDebug() << "time6.1";
                pausenzeit = 0; // Pause wurde genommen, keine Abzug am Schluss nötig
            }
            if(flexNetto_int+netto_int >= 540 && pausenzeit >= 45 && is_gebucht == false){//genug pause gemacht
                //qDebug() << "p6.2" << pausenzeit;
                //qDebug() << "time6.2";
                pausenzeit = 0;
            }
            if(pausenzeit >= 15 && pausenzeit < 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540){//kleine pause gemacht aber noch nicht genug
                //qDebug() << "time6.3";
                //qDebug() << "p6.3" << pausenzeit;
                pausenzeit = 30 - pausenzeit;
            }
            if(pausenzeit >= 15 && pausenzeit < 45 && flexNetto_int+netto_int >= 540){//kleine pause gemacht aber noch nicht genug bei über 9h
                //qDebug() << "time6.33";
                //qDebug() << "p6.33" << pausenzeit;
                pausenzeit = 45 - pausenzeit;
            }
        }
    }
    //Nur Homeoffice mit einer Buchung
    if(flexkommt.size() == 1 && flexgeht.size() == 1){//Stubenhocker im Homeoffice -- keine Pause
        if(flexNetto_int >= 360 && flexNetto_int < 540){//über 6h
            //qDebug() << "time7";
            pausenzeit = 30;
        }
        if(flexNetto_int+netto_int >= 540){
            //qDebug() << "time8";
        pausenzeit = 45;//ab 9h
        }
    }
    //Mal Homeoffice und mal Büro
    if(kommt.size() > 0 && geht.size() > 0 && flexkommt.size() > 0 && flexgeht.size() > 0){//mal homeoffice und mal büro
        if(kommt[0] < flexkommt[0]){//erste Buchung = Büro
            for(int i = 0;i<geht.size();i++){
                //qDebug() << "i.05" << i;
                if(geht.size() == 1){
                    //qDebug() << "time8.6";
                    break;
                }
                if(geht[i] < kommt[i+1] && kommt[i+1] - geht[i] >= 15 && geht.size()>1){//Pausenzeit wird anerkannt
                    //qDebug() << "time8.5";
                    pausenzeit = pausenzeit + (kommt[i+1] - geht[i]);
                }
            }
            if(geht.last() < flexkommt.first()){
                //qDebug() << "time8.55";
                pausenzeit = pausenzeit + (flexkommt.first() - geht.last());
                //qDebug() << pausenzeit << "breaktime";
            }
        }
        if(flexkommt[0] < kommt[0]){//erste Buchung = Homeoffice
            for(int i = 0;i<flexgeht.size();i++){
                //qDebug() << "i.2" << i;
                if(flexgeht.size() == 1){
                    //qDebug() << "time8.7";
                    break;
                }
                if(flexgeht[i] < flexkommt[i+1] && flexkommt[i+1] - flexgeht[i] >= 15){//Pausenzeit wird anerkannt
                    //qDebug() << "time8.75";
                    pausenzeit = pausenzeit + (flexkommt[i+1] - flexgeht[i]);
                }
            }
            if(flexgeht.last() < kommt.first()){
                //qDebug() << "time8.8";
                pausenzeit = pausenzeit + (flexgeht.last() - kommt.first());
            }
        }
        if(pausenzeit < 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540){
            //qDebug() << "time9";
            pausenzeit = 30 - pausenzeit;//Die bisherige Pause ist unter 30Min also wird die Diffenrenz zur vollen Pausenzeit noch abgezogen
        }
        if(flexNetto_int+netto_int >= 540 && pausenzeit < 45){
            //qDebug() << "time9.1";
            pausenzeit = 45 - pausenzeit;
        }
        if(pausenzeit >= 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540){
            //qDebug() << "p9.2" << pausenzeit;
            //qDebug() << "time9.2";
            pausenzeit = 0; // Pause wurde genommen, Abzug am Schluss nötig
        }
        if(flexNetto_int+netto_int >= 540 && pausenzeit >= 45){
            //qDebug() << "p9.3" << pausenzeit;
            //qDebug() << "time9.3";
            pausenzeit = 0;
        }
    }
}

const QList<qint32> &Tagesdaten::getRemember_timekommt() const
{
    return kommt;
}

void Tagesdaten::setRemember_timekommt(qint32 newRemember_timekommt)
{
    kommt.append(newRemember_timekommt);
}

const QList<qint32> &Tagesdaten::getRemember_timegeht() const
{
    return geht;
}

void Tagesdaten::setRemember_timegeht(qint32 newRemember_timegeht)
{
    geht.append(newRemember_timegeht);
}

const QList<qint32> &Tagesdaten::getRemember_timeflexkommt() const
{
    return flexkommt;
}

void Tagesdaten::setRemember_timeflexkommt(qint32 newRemember_timeflexkommt)
{
    flexkommt.append(newRemember_timeflexkommt);
}

const QList<qint32> &Tagesdaten::getRemember_timeflexgeht() const
{
    return flexgeht;
}

void Tagesdaten::setRemember_timeflexgeht(qint32 newRemember_timeflexgeht)
{
    flexgeht.append(newRemember_timeflexgeht);
}

qint32 Tagesdaten::getNetto_int() const
{
    return this->netto_int;
}

void Tagesdaten::add_toNetto_int(qint32 newNetto_int)
{
    this->netto_int = this->netto_int + newNetto_int;
}

void Tagesdaten::setNetto_int(qint32 newNetto_int)
{
    this->netto_int = newNetto_int;
}

qint32 Tagesdaten::getFlexNetto_int() const
{
    return this->flexNetto_int;
}

void Tagesdaten::add_toFlexNetto_int(qint32 newFlexNetto_int)
{
    this->flexNetto_int = this->flexNetto_int + newFlexNetto_int;
}

void Tagesdaten::setFlexNetto_int(qint32 newFlexNetto_int)
{
    this->flexNetto_int = newFlexNetto_int;
}

qint32 Tagesdaten::just_Minutes(QString zeit){
    QTime time = QTime::fromString(zeit, "H:mm");
    qint32 minutes = (time.hour() * 60) + time.minute();
    return minutes;
}
