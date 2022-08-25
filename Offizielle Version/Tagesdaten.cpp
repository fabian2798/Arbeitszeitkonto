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
    if(newTages_nr.toInt() < 10){
    this->tages_nr = "0" + newTages_nr;
    }
    else{
        this->tages_nr = newTages_nr;
    }
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

const QVarLengthArray<QString> &Tagesdaten::getKommt() const
{
    return this->Kommt;
}

void Tagesdaten::setKommt(QString newKommt)
{
    if(!newKommt.isEmpty()){
    this->Kommt.append(newKommt);
    }
}

const QVarLengthArray<QString> &Tagesdaten::getGeht() const
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

const QVarLengthArray<QString> &Tagesdaten::getFlexArbkommt() const
{
    return this->flexArbkommt;
}

void Tagesdaten::setFlexArbkommt(QString newFlexArb)
{
    this->flexArbkommt.append(newFlexArb);
}

bool Tagesdaten::already_inflexArbkommt(QString anfang){// Für Fehlbuchung bei FlexArb, da Regex nicht die letzte Buchung nimmt (z.B 15:30 -)
    return flexArbkommt.contains(anfang);
}

const QVarLengthArray<QString> &Tagesdaten::getFlexArbgeht() const
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
//insert times(string) into arrays
void Tagesdaten::add_toarbeitszeit(QString anfang, QString ende){
    //Gleitzeit beachten
    if(!anfang.isEmpty()){
    qint32 anfangint = just_Minutes(anfang, "H:mm");
    if(anfangint < 360){
        //qDebug() << "anfang unter 6uhr";
        anfang = "6:00";
    }
    if(anfangint > 1140){
        //qDebug() << "anfang über 19uhr";
        anfang = "19:00";
    }
    }
    if(!ende.isEmpty()){
    qint32 endeint = just_Minutes(ende, "H:mm");
    //qDebug() << "endeint: " << endeint;
    if(endeint < 360){
        //qDebug() << "ende unter 6uhr";
        ende = "6:00";
    }
    if(endeint > 1140){
        //qDebug() << "ende über 19uhr";
        ende = "19:00";
    }
    }
    //Bürozeit eintragen
    if(!anfang.isEmpty() && !ende.isEmpty()){
        //qDebug() << "Bürozeit";
        Kommt.append(anfang);
        kommt_zeiten.append(just_Minutes(anfang, "H:mm"));
        Geht.append(ende);
        geht_zeiten.append(just_Minutes(ende, "H:mm"));
    }
    //Homeoffice eintragen
    if(anfang.isEmpty() && !ende.isEmpty()){
        if(flexArbkommt.size() == flexArbgeht.size()){
        //qDebug() << "Homeoffice Begin";
        flexArbkommt.append(ende);
        kommt_zeiten.append(just_Minutes(ende, "H:mm"));
        }
        if(flexArbgeht.size() < flexArbkommt.size() && already_inflexArbkommt(ende) == false){
            //qDebug() << "Homeoffice Ende";
            flexArbgeht.append(ende);
            geht_zeiten.append(just_Minutes(ende, "H:mm"));
        }
    }
}
//reset all data collected from a day
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
   soll_zeit = "";
   netto_zeit = "";
   zeit_diff = "";
   zeit_saldo = "";
   office_time = "0.00";
   flexible_time = "0.00";
   kommt_zeiten.clear();
   geht_zeiten.clear();
}

qint32 Tagesdaten::getPausenzeit() const
{
    return pausenzeit;
}

void Tagesdaten::setPausenzeit(qint32 newPausenzeit)
{
    pausenzeit = newPausenzeit;
}
//calc breaktime for a day
void Tagesdaten::calc_breaktime(){
    bool is_gebucht =false;
    if(kommt_zeiten.size() > 1 && geht_zeiten.size() > 1){
        //qDebug() << "in schleife";
    for(int i = 0;i<geht_zeiten.size();i++){
        if(kommt_zeiten[i+1] - geht_zeiten[i] >= 15){
            //qDebug() << "berechen";
            pausenzeit = pausenzeit + (kommt_zeiten[i+1] - geht_zeiten[i]);
            //qDebug() << "geht" << geht_zeiten[i] << "kommt" << kommt_zeiten[i+1];
        }
        if(kommt_zeiten[i+1] == kommt_zeiten.last()){
            //qDebug() << "Break beim letzten Kommt";
            break;
        }
    }
    }
    //Nicht vollgenutzte Pausenzeit bei 6h, Rest abziehen
    if(pausenzeit < 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540){
        is_gebucht = true;
        //qDebug() << "bei 6h" << pausenzeit;
        pausenzeit = 30 - pausenzeit;
    }
    //Vollgenutzte Pausenzeit bei 6h
    if(pausenzeit >= 30 && flexNetto_int+netto_int >= 360 && flexNetto_int+netto_int < 540 && is_gebucht == false){
        //qDebug() << "voll genommen bei 6h" << pausenzeit;
        pausenzeit = 0;
    }
    //Nicht vollgenutzte Pausenzeit bei 9h, Rest abziehen
    if(pausenzeit < 45 && flexNetto_int+netto_int >= 540){
        is_gebucht = true;
        //qDebug() << "bei 9h" << pausenzeit;
        pausenzeit = 45 - pausenzeit;
    }
    //Vollgenutzte Pausenzeit bei 9h
    if(pausenzeit >= 45 && flexNetto_int+netto_int >= 540 && is_gebucht == false){
        //qDebug() << "voll genommen bei 9h" << pausenzeit;
        pausenzeit = 0;
    }
}

const QVarLengthArray<qint32> &Tagesdaten::getRemember_timekommt() const
{
    return kommt;
}

void Tagesdaten::setRemember_timekommt(qint32 newRemember_timekommt)
{
    kommt.append(newRemember_timekommt);
}

const QVarLengthArray<qint32> &Tagesdaten::getRemember_timegeht() const
{
    return geht;
}

void Tagesdaten::setRemember_timegeht(qint32 newRemember_timegeht)
{
    geht.append(newRemember_timegeht);
}

const QVarLengthArray<qint32> &Tagesdaten::getRemember_timeflexkommt() const
{
    return flexkommt;
}

void Tagesdaten::setRemember_timeflexkommt(qint32 newRemember_timeflexkommt)
{
    flexkommt.append(newRemember_timeflexkommt);
}

const QVarLengthArray<qint32> &Tagesdaten::getRemember_timeflexgeht() const
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

qint32 Tagesdaten::just_Minutes(QString zeit, QString pattern){
    QTime time = QTime::fromString(zeit, pattern);
    qint32 minutes = (time.hour() * 60) + time.minute();
    return minutes;
}

const QString &Tagesdaten::getOffice_time() const
{
    return office_time;
}
//pause wird von der bürozeit abgezogen -- Tagesweise
void Tagesdaten::setOffice_time_pause()
{
    netto_int = netto_int - pausenzeit;

    qint32 officetime_min = netto_int % 60;
    qint32 officetime_h = netto_int / 60;
    QString of_min = QString::number(officetime_min);
    QString of_h = QString::number(officetime_h);
    qint32 flextime_min = flexNetto_int % 60;
    qint32 flextime_h = flexNetto_int / 60;
    QString f_min = QString::number(flextime_min);
    QString f_h = QString::number(flextime_h);
    // for better readability -- under 10min -> 09,08,07...
    if(officetime_min < 10){
        of_min = "0"+QString::number(officetime_min);
    }
    if(flextime_min < 10){
        f_min = "0"+QString::number(flextime_min);
    }

    office_time = of_h + "." + of_min;
    flexible_time = f_h + "." + f_min;
}

const QString &Tagesdaten::getFlexible_time() const
{
    return flexible_time;
}
//Pause wird von der Homeofficezeit abgezogen -- Tagesweise
void Tagesdaten::setFlexible_time_pause()
{
    flexNetto_int = flexNetto_int - pausenzeit;

    qint32 flextime_min = flexNetto_int % 60;
    qint32 flextime_h = flexNetto_int / 60;
    qint32 officetime_min = netto_int % 60;
    qint32 officetime_h = netto_int / 60;
    QString f_h = QString::number(flextime_h);
    QString of_h = QString::number(officetime_h);
    QString f_min = QString::number(flextime_min);
    QString of_min = QString::number(officetime_min);

    // for better readability -- under 10min -> 09,08,07...
    if(officetime_min < 10){
        of_min = "0"+QString::number(officetime_min);
    }
    if(flextime_min < 10){
        f_min = "0"+QString::number(flextime_min);
    }
    office_time = of_h + "." + of_min;
    flexible_time = f_h + "." + f_min;
}

const QString &Tagesdaten::getDate() const
{
    return date;
}

void Tagesdaten::setDate(const QString &newDate)
{
    date = newDate;
}

const QString &Tagesdaten::getMonthInt() const
{
    return monthInt;
}

void Tagesdaten::setMonthInt(const QString &newMonthInt)
{
    monthInt = newMonthInt;
}

void Tagesdaten::setDaytimesZero(){
    office_time = "0.00";
    flexible_time = "0.00";
}

const QVarLengthArray<qint32> &Tagesdaten::getKommt_zeiten() const
{
    return kommt_zeiten;
}

void Tagesdaten::setKommt_zeiten(qint32 &newKommt_zeiten)
{
    kommt_zeiten.append(newKommt_zeiten);
}

const QVarLengthArray<qint32> &Tagesdaten::getGeht_zeiten() const
{
    return geht_zeiten;
}

void Tagesdaten::setGeht_zeiten(qint32 &newGeht_zeiten)
{
    geht_zeiten.append(newGeht_zeiten);
}
