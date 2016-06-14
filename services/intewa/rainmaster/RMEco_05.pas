program RMEco_05;

//version rmf_1
//für atmega8 und freilaufendem Osz. mit externem R
//22.12.08
//13.2.09 besprochen mit Michael Wurzer von Intewa :
//Stellung I a : Led1 an,
//Schwimmschalter nicht aktiv, Kugelhahn bekommt 24V
//Stellung I b : Led1 an,
//Schwimmschalter aktiv, Kugelhahn bekommt keine 24V
//Stellung II :  Led2 an,
//Kugelhahn bekommt keine 24V, egal ob Schwimmschalter aktiv oder nicht
//wenn Pumpenkontakt nach 2 h wieder offen, Pumpe trotzdem ausgeschaltet
//lassen und Leds weiter blinken lassen, solang wie Gerät aus- und wieder
//eingeschaltet wird
//
//29.3.09, version rmf_2:
//blinktimer auf 500ms
//500ms-blinken, wenn pumpe >2h läuft
//watchdog auf 2s
//1 Mhz-Oszillator ohne ext. Bauteile
//
//23.6.09, version rmf_3:
//eigentlich wie rmf_2 aber
//es gibt einen testmodus, wenn pin4 mit pin6 auf dem
//programmierstecker verbunden wird, blinken die leds und
//das relais wird dauerhaft ein- und wiederausgeschaltet sodass
//man anhand des blinkens und des klackern sehen und hören kann
//ob die leds und das relais ok sind
//
//30.7.10 , heißt jetzt RMEco_04
//2.leitung als ersatzleitung um den leistungsschalter zu schalten,
//heißt PumpeOnSpare
//ca. 2ms Entprellung der Vorderflanke des Schalters in der Pumpe weil
//der 2-3 mal prellt ca. 1-2ms lang
//die anzahl wie oft der leistungsschalter eingeschaltet wird
//wird im eeprom als longword gespeichert
//
//8.12.2011 , heißt immer noch RMEco_04
//Besprechung mit Michael Wurzer : die speicherung der anzahl der
//Leistungsschalterbetätigung wird wieder rausgenommen weil nicht
//klar ist, was passiert, wenn die 100000 schreib-lösch-zyklen
//des atmega erreicht sind
//
//7.1.12 , heißt jetzt RMEco_05
//für Platinenversion RMEco_5 mit Ladepumpe auf PD3
//Ladepumpe an im Regenwasserbetrieb (Stellg. 1 obere grüne Led an)
//Ladpumpe immer aus im Trinkwasserbetrieb (St. 2 untere grüne Led an)
//

{ $BOOTRST $01C00}         {Reset Jump to $01C00}
{ $W+ Warnings}            {Warnings off}

Device = mega8, VCC = 5;

Import SysTick, WatchDog;

Define
  ProcClock      = 1000000;             //1 Mhz, int. Osc.
  SysTick        = 10, Timer0;          //10msec
  StackSize      = $0064, iData;
  FrameSize      = $0064, iData;
  WatchDog       = msec2000;            {presc=6}

Implementation

{$IDATA}
Const

var
  OffCounter     : word;
  x              : integer;
  PumpenFlag     : boolean;
  blink          : boolean;
  Blinktimer     : Systimer8;

  {$EEprom}
 var

  {$IDATA}

  //Port B
  Rel1[@PortB, 0]        : bit;         //Kugelhahnrelais
  Testmode[@PinB, 3]     : bit;         //Testmodus

  //Port C
  //Port D
  PumpeOn[@PinD, 0]      : bit;
  Schwimmer[@PinD, 1]    : bit;
  Pos1[@PinD, 2]         : bit;
  Ladepumpe[@PortD, 3]   : bit;
  Pumpe[@PortD, 4]       : bit;
  Led1[@PortD, 5]        : bit;
  Led2[@PortD, 6]        : bit;
  Led3[@PortD, 7]        : bit;

procedure InitPorts;
begin
  PortB:= %00001000;                    //internen Pullup an PB3 aktivieren
  DDRB:=  %00000011;                    //0=input, 1=output
  DDRC:=  %00000000;
  DDRD:=  %11111000;
end;

procedure Test;
begin
  Led1:= blink;
  Led2:= blink;
  Led3:= blink;
  Rel1:= blink;
end;

procedure CheckSchalterundSchwimmer;
begin
  if not Pos1 and Schwimmer then        //Regenwasserbetrieb
     Rel1:= true;
     Led1:= true;
     Led2:= false;
     Led3:= false;
  endif;

  if not Pos1 and not Schwimmer then    //Regenwasserbetrieb
     Rel1:= false;
     Led1:= true;
     Led2:= false;
     Led3:= true;
  endif;

  if Pos1 and Schwimmer then            //Klarwasserbetrieb
     Rel1:= false;
     Led1:= false;
     Led2:= true;
     Led3:= false;
  endif;

  if Pos1 and not Schwimmer then        //Klarwasserbetrieb
     Rel1:= false;
     Led1:= false;
     Led2:= true;
     Led3:= true;
  endif;
end;

//start hauptprogramm
//----------------------------------------------------------
// main program
// $idata

begin
  InitPorts;
  EnableInts;
  WatchDogStart;
  SetSysTimer(Blinktimer, 50);          //blinktimer = 500ms
  OffCounter:= 0;
  PumpenFlag:= false;

  loop

    WatchDogTrig;

    if not Testmode then                //Testmodus aktivieren
       Test;
    else

       CheckSchalterundSchwimmer;

       if not PumpeOn then              //Optokopper Pumpe abfragen
          mdelay(2);                    //Entprellen 2ms für die Vorderflanke
          if not PumpeOn then
             Pumpe:= true;              //Pumpe einschalten
             PumpenFlag:= true;         //Flag setzen
             if not Pos1 then           //Stellung I = Trinkwasser
                Ladepumpe:= true;       //Ladepumpe einschalten
             else
                Ladepumpe:= false;      //sonst immer aus
             endif;
          endif;
       else
          Pumpe:= false;
          Ladepumpe:= false;
          PumpenFlag:= false;
          OffCounter:= 0;
       endif;

    if PumpenFlag then
       if OffCounter > 14400 then       //14400x500ms=7200s/60=120min/60=2h
         //if OffCounter > 3600 then    //3600x200ms=720s/60=12min zum test
          repeat
           Pumpe:= false;               //Pumpe zwangsabschalten, wichtig!!!
           Ladepumpe:= false;           //Ladepumpe zwangsabschalten
           Led1:= true;                 //die Schleife wird solange durchlaufen
           Led2:= true;                 //bis das Gerät aus- und wieder
           Led3:= true;                 //eingeschaltet wird
           mdelay(500);
           Led1:= false;
           Led2:= false;
           Led3:= false;
           mdelay(500);
           WatchDogTrig;
           CheckSchalterundSchwimmer;
          until (false);
       endif;
    else
       OffCounter:= 0;
    endif;
    endif;

    if IsSysTimerZero(Blinktimer) then  //500ms abgelaufen
       SetSysTimer(Blinktimer, 50);
       blink:= not blink;
       if PumpenFlag then
          inc (OffCounter);
       else
          OffCounter:= 0;
       endif;
    endif;
  endloop;
end rmeco_05.

