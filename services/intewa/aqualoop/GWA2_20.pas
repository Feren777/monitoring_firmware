program gwa2_20;

//GWA2_6 ist f�r Atmega 128 und ist echtzeitbezogen
//von W.Enz f�r Intewa mbH Aachen
//4.9.12 : noch in Entwicklung
//Led1 (links) blinkt im Sekundenrhytmus
//Led2 Saugpumpe
//Led3 R�cksp�lpumpe
//Led4 (rechts) Ventilation
//Ermittlung von Mem (Anzahl Membrane 1-6) via Men�, m�glich M=1,2,3,4,5,6
//Saugen und R�cksp�len erfolgt in Abh�ngigkeit von zwei Zeiten
//es l�uft eine Echtzeituhr, bei 2 vom Kunden einstellbaren Zeiten wird
//gesaugt und r�ckgesp�lt
//10 Minuten vor dem Saugen wird die L�ftung abgestellt
//dann kommt der Saug-R�cksp�lblock mit 15 Minuten Saugen, 2s Pause und
//15s r�cksp�len, dieser Block erfolgt 4x, beim letzten R�cksp�len startet
//die L�ftung f�r 10 Minuten.
//Der Block erfolgt solange bis entweder SSBrmin oder SSKlarmax anspricht,
//aber maximal 3 x
//die Bel�ftung kann per Men� zeitlich eingestellt werden von 1 - 15 min,
//d.h. z.B. 5min Bel�ftung, 15 Minuten aus, oder 15min Bel�ftung, 1min aus
//Mode 1 : gibt es ab Vers. GWA2_3 nicht mehr
//immer wenn der Opmode ge�ndert wird, startet die Bel�ftung von vorn
//und garnicht bel�ften in mode=2
//in option soll ab woche 1,2,3 oder 4 (per men� w�hlbar) das saugen
//und R�cksp�len f�r 24h eingestellt werden, in option=0 wird das nicht gemacht
//Im Testmodus kann jetzt auch R�cksp�len gew�hlt werden

//12.6.12 Anpassungen an den Atmega128, die Saugpumpe wird jetzt von PWMPort1
//gesteuert, die R�cksp�lpumpe von PWMPort2 und das Gebl�se von PWMPort3,
//restliche Ports angepasst entspr. Schaltplan
//neuer Men�punkt "Chemische Reinigung", genaue Definition erfolgt noch

//21.8.12 : nach Besprechung mit Michael : Pumpen wird abgebrochen wenn
//man T1 oder T2 �ndert
//Druckanzeige Adr (Uin2) f�r Sp�len und R�cksp�len und Adg (Uin3) f�r Bel�ftung
//werden in der ersten Zeile beim Testbetrieb angezeigt

//GWA2_5 , 5.12.12 : auf der Platine ist jetzt ein 9V-Akku, der bei Ausfall des
//Netzteils die Versorgung der Platine �bernimmt wenn z.B. wenn eine der
//Pumpen einen Kurzschluss hat
//Laden des Akkus erfolgt bis zu einer Spannung von 9,6V, danach 20s Pause
//und die Schaltung geht wieder zum Laden
//wenn auf Akku umgeschaltet wird erfolgt ein Warnung auf dem Display und
//eine akustische Warnung, Piepser alle 10s

//GWA2_6, 30.12.12 : drei Spannungsausfallz�hler eingebaut f�r allgemeinen
//ausfall, einer wenn flush l�uft und einer wenn air l�uft, alle drei werden
//in status angezeigt
//nach stromausfall und r�ckkehr der 24v wird dem anwender angezeigt,
//bei welchem ereignis der strom ausgefallen ist, z.b. bei flush oder air
//laufende prozesse werden abgebrochen, auch automatikbetrieb sodass der
//anwender in ruhe checken kann, warum der stromausfall stattgefunden hat
//per menue kann zwischen akku und batterie gew�hlt werden, bei
//batterie-betrieb ist die ladeschaltung ausser funktion
//wenn flush/reflush l�uft kann A1 oder A2 nicht mehr ge�ndert werdeb
//
//12.1.13 nach Besprechung mit Michael Wurzer : bei R�ckkehr aus dem Akkubetrieb
//bleibt die akustische Warnung mit dem Piezo solange noch an bis die
//Best�tigung de Stromausfalls erfolgt ist, die Bel�ftung wird aber sofort
//nach R�ckkehr aus dem Akkubetrieb weiterlaufen
//
//GWA2_7, 14.2.13 : bei stromausfall oder kurzschluss l�uft das ger�t auf 9v
//weiter: bei r�ckkehr der 24V bleiben alle verbraucher abgeschaltet bis auf
//die bel�ftung und nach 30s kehrt das Ger�t in den modus zur�ck bei dem
//der stromausfall stattgefunden hat, also z.b. automatikbetrieb oder bel�ftung
//nicht aber in den testbetrieb, wenn der stromausfall w�hrend des testbetriebs
//stattgefunden hat
//
//GWA2_8, 29.3.13 : kleine �nderungen im Men� z.B. = statt : u.�.
//maximale Einstellung f�r A1/A2 von 15 auf 25
//nur noch f�r Akkubetrieb
//die anf�ngliche Bel�ftungszeit von 10min im Pumpzyklus auf 5 min=300s verk�rzen
//in der Hauptanzeige beim pumpen A1=xx oder A2=xx anzeigen, sp�ter Zeit raus
//wenn man in den Testmodus geht, erst mal alles abschalten, wird nichts gew�hlt,
//alten Zustand wiederherstellen wenn man automatisch aus dem Men� kommt, bzw.
//auch wenn der Test abgelaufen ist
//SLP Wochenzeit kann von 0 - 8 laufen, 0=SLP schaltet nie ein, 8 = nach 8 Wochen
//l�uft die Schlammpumpe mit der vorgegeben Zeit, das pumpen wird gestoppt,
//wenn BR leer, h�rt die Schlammpumpe auf zu laufen und die Zeit wird gestoppt,
//wenn der BR wieder voll ist und die Schlammpumpe wieder l�uft, l�uft auch die
//Zeit weiter, d.h. die eingestellte Zeit von 1-60min wird immer komplett ausgef�hrt
//Tests sind sowohl w�hrend SO (Suction overflow) als auch w�hrend der Zeit erlaubt
//in der die SLP l�uft
//Men�reihenfolge jetzt : 1 mem, 2 SO, 3 SLP, 4 Testmode, 5 Overview, 6 Uhrzeit
//7 T1, 8 T2, 9 A1/A2, 10 Air 11 Reinigung
//12 englisch/deutsch
//
//GWA2_9, 20.4.13 : wenn SO oder SLP nach 1-x Wochen, hat SLP Priorit�t
//w�hrend SO oder SLP kann jeder Test laufen
//
//GWA2_10, 23.5.13 : mit Reinigungsmen� aber ohne Zugangsfreigabe
//
//GWA2_11, 27.4.13 : Reinigungsmen�
//
//GWA2_12, 1.6.13 :  Servicemode gibt es nicht mehr, Testbetrieb
//men�bezogen neu programmiert
//
//GWA2_13, 24.6.13 : Reinigungsgmen� komplettiert, sp�len, r�cksp�len
//jetzt ohne Schwimmerabfrage
//
//GWA2_14, 28.8.13 : Men� f�r den dritten, nicht getakteten 24V-Ausgang,
//frei programmierbar, ob 24V f�r Bel�ftung parallel zur Bel�ftung via 230V
//oder Filter, das nach 1-30d f�r 1-120s eingeschaltet wird durch 24V auf den Port
//
//GWA2_15, 31.12.13 : Anzeige aktuelle Dr�cke Adr und Adg immer auch im Testmodus.
//Einbau Stagnationsanzeige, jedesmal wenn die Sp�l-oder R�cksp�lpumpe l�uft,
//wird der Stagnationsz�hler zur�ckgesetzt, wenn nicht, l�uft er bis 21 Tage, dann
//wird eine Men� ausgel�st, in dem der Anwender die Stagnation quittiert und das
//"alte" Wasser abl��t (z.B. nach dem Urlaub), au�erdem piepst der Summer
//Reset nullt den Stag-Z�hler, Testbetrieb der Sp�l- und R�cksp�lpumpe auch
//
//GWA2_15x mit verk�rzter Stagnationszeit und Anzeige derselben im Display
//
//GWA2_16, 1.6.14 : wenn das Ger�t l�nger als 2h auf Akku l�uft wird nach
//R�ckkehr der Netzspannung eine Clearwateranzeige ausgegeben, nach Best�tigung
//wird das ClearwaterFlag wieder gel�scht, ob der Kunde das faule Wasser
//allerdings durch frisches Wasser ersetzt hat bleibt das grosse Geheimnis
//
//GWA2_16x mit verk�rzten Zeiten zum Testen
//
//GWA2_17, 18.9.14 : mit T1, T2 und neu: T3 sowie A1, A2 und neu A3
//defaults von T1-3: 03:00/12:00/17:00,
//defaults von A: 6/0/9, beides erhalten von Michael durch email am 19.9.14
//
//GWA2_18, 4.1.15 : Sprungfehler im Men� chemische Reinigung behoben, neue
//Parameter f�r chemische Reinigung eingebaut
//
//GWA2_19, 10.1.15 : zur�ck zu 2 Pumpzeiten : T1 und T2 sowie A1 und A2
//Druckabfrage f�r Bel�fter ADg Air, wenn Druck bei eingeschalteter Bel�ftung
//unter 0,02 bar f�llt, soll Fehlalarm ert�nen und der Pumpzyklus unterbrochen
//werden
//
//GWA2_20, 13.1.15 : neuer Sensor A-10 an ADg Air, 0-0,6bar = 4-20mA,
//wenn Air=ON soll das Ger�t warnen, wenn ADg Air < 0,02 bar ist
//dabei Summer und Displaybeleuchtung im 1s Rhytmus zyklisch
//ein- und wiederausschalten und zus�tzlich Warnmeldung ausgeben
//22.1.15 : A1/A2 von 25 auf max. 45 �ndern, Sludgepump Zeit von 1d-60d,
//Pumpzeit bleibt bei 1-60min
//Warnung ausgeben mit blinkendem Display und Piezo im Rhytmus von 1s bei
//allgemeinem Stromausfall, Stromausfall w�hrend des Pumpens, Storage time
//und bei Bel�ftung, jeder dieser Fehler wird mit
// blinkendem Display und Piezoangezeigt und mu� vom Anwender quittiert werden
//

{$NOSHADOW}
{ $WG}                     {global Warnings off}

Device = mega128, VCC=5;
{ $BOOTRST $01C00}         {Reset Jump to $01C00}

Import SysTick, WatchDog, ADCPort, LCDport, RTclock, PWMport1, PWMport2;

From System Import Longword, Float;

define
  ProcClock      = 6144000;         {Hertz 6144}
  SysTick        = 10, Timer2;      {msec}
  WatchDog       = msec1000;        {presc = 6}
  StackSize      = $0032, iData;
  FrameSize      = $0032, iData;
  ADCchans       = 5, iData, Int2;  //Kanal1, 2 , 3 und 4, 2fach integrieren
  ADCpresc       = 128;
  LCDport        = PortC;
  LCDtype        = 44780;
  LCDrows        = 4;
  LCDcolumns     = 20;
  PWMpresc       = 256;             //Vorteiler
  PWMres         = 8;               //resolution
  RTclock        = iData, Time;     //RTC, nur Zeit

Implementation

{$IDATA}

{--------------------------------------------------------------}
{ Type Declarations }

type

{--------------------------------------------------------------}
{ Const Declarations }
const

ae :char=#1; //�
oe :char=#2; //�
ue :char=#3; //�

c_SekundenTimer :byte=100;          //100*10ms=1s, auch Blinktimer

Testdauer       :word=360;          //Dauer eines Tests 360s=6min

//c_24h         :longword=  1800;   //zum Test 1800s=30minuten

c_24h           :longword=  86400;  //24h in s, zum Test 4h=14400

//c_eineWoche     :longword= 7200;    //zum Test 7200s=120minuten=2h

c_eineWoche     :longword= 604800;  //eine Woche in s, zum Test 1d=86400

c_zweiWochen    :longword=1209600;  //zwei Wochen in s, zum Test 2d=172800

c_dreiWochen    :longword=1814400;  //drei Wochen in s, zum Test 3d=259200

c_vierWochen    :longword=2419200;  //vier Wochen in s

c_fuenfWochen   :longword=3024000;  //f�nf Wochen in s

c_sechsWochen   :longword=3628800;  //sechs Wochen in s

c_siebenWochen  :longword=4233600;  //sieben Wochen in s

c_achtWochen    :longword=4838400;  //acht Wochen in s

{--------------------------------------------------------------}
{ Var Declarations }

{$IDATA}
var
//PortA
SSKlarmin            [@PinA,0]   :Bit;
SSKlarmax            [@PinA,1]   :Bit;
SSBRmin              [@PinA,2]   :Bit;
S1_1                 [@PinA,4]   :Bit;
S1_2                 [@PinA,5]   :Bit;
Bat                  [@PinA,6]   :Bit;
S1                               :byte;

//PortB
Strobe               [@PortB,1]  :Bit;
Saugpumpe            [@PortB,5]  :Bit;
Rueckspuelpumpe      [@PortB,6]  :Bit;
FreeOut              [@PortB,7]  :Bit;

// Port C = LCD-Display

//PortD
//nur RX/TX, nur SCL, PD0 zum Abschalten der Backlight des LCD-Displays
//ansonsten nichts weiter belegt
Backlight            [@PortD,0]  :Bit;

//PortE
Led1                 [@PortE,2]  :Bit;
Led2                 [@PortE,3]  :Bit;
Led3                 [@PortE,4]  :Bit;
Led4                 [@PortE,5]  :Bit;
Laden                [@PortE,6]  :Bit;

//PortF
Next                 [@PinF,5]   :Bit;  //Nexttaste
Enter                [@PinF,6]   :Bit;  //Entertaste
IN1                  [@PinF,7]   :Bit;  //noch keine Funktion, nur vorgesehen

//PortG
SLP                  [@PortG,0]  :Bit;  //Rel1=Schlammpumpe
GeblaeseNetz         [@PortG,1]  :Bit;  //Rel2=Gebl�se Netz
Piezo                [@PortG,2]  :Bit;

Sekundentimer        :systimer8;  //timer 1s
Flag                 :boolean;    //algemeines flag
Blink                :boolean;    //toggelt mit blinktimer
Serviceflag          :boolean;    //ServiceCounterflag f�r service mode
Testflag             :boolean;    //Testflag=true, test l�uft
MenueFlag            :boolean;    //wird gesetzt bei Aufruf von Men� und im Men�
MemEnter             :boolean;    //f�r booleans f�r Tasteneingabe
TickEnter            :boolean;
MemNext              :boolean;
TickNext             :boolean;
NextFlag             :boolean;
CleanFlag            :boolean;    //true, wenn Reinigung l�uft
SpeicherFlag         :boolean;    //true, wenn daten ins eeprom gespeichert wurden
Language             :boolean;    //true=englisch, false=deutsch
TickSecond           :boolean;
TickMinute           :boolean;
TickHour             :boolean;
HourIsSet            :boolean;
AkkuFlag             :boolean;    //true, wenn akkubetrieb bei fehlendem netz
LastAction           :byte;       //0=es war nichts an bei spannungsausfall
                                  //1=flush/reflush war an bei spannungsausfall
                                  //2=air war an bei spannungsausfall
Piezocounter         :byte;       //z�hler um piezo piepen zu lassen
RTCHour              :byte;       //real time stunde
RTCMinute            :byte;       //real time minute
RTCSecond            :byte;       //real time sekunde
PT1Hour              :byte;       //1.pumpzeit stunde
PT1Minute            :byte;       //1.pumpzeit minute
PT2Hour              :byte;       //2.pumpzeit stunde
PT2Minute            :byte;       //2.pumpzeit minute
PT3Hour              :byte;       //3.pumpzeit stunde
PT3Minute            :byte;       //3.pumpzeit minute
PT1HourCopy          :byte;
PT1MinuteCopy        :byte;
PT2HourCopy          :byte;
PT2MinuteCopy        :byte;
PT3HourCopy          :byte;
PT3MinuteCopy        :byte;

PTFlag               :boolean;    //true, wenn gepumpt werden soll
PT1                  :boolean;    //wir sind im pt1-zyklus
PT2                  :boolean;    //wir sind im pt2-zyklus
PT3                  :boolean;    //wir sind im pt3-zyklus
AirOn                :word;       //Zeit in min wie lang bel�ftet wird
AirOff               :word;       //Zeit in min wie lang der L�fter aus ist
AirFlag              :boolean;    //true, wenn Air on
SLPday               :word;       //nach 1,2,3,4, od. 5 Wo f�r 24h kein pumpen
SLPdayCopy           :word;       //nach diesen 24h l�uft die schlammpumpe
                                  //entweder mit t im minuten oder bis der BR
                                  //leer ist, parallel dazu l�uft die Bel�ftung
                                  //nach t/minuten wieder normalbetrieb
SLPminute            :word;       //Zeit in Minuten wielange die Schlammpumpe l�uft
SLPminuteCopy        :word;       //Kopie davon
SLPTimer             :word;       //Timer der raufz�hlt bis er gleich SLPMinute ist
SLPFlag              :boolean;    //true, wenn Schlammpumpe laufen soll
TC                   :word;       //TastenCounter wird mit jedem TickNext oder
                                  //TickEnter zur�ckgesetzt
A                    :byte;       //Anzahl innerer Pumpvorg�nge
A1                   :byte;
A1Copy               :byte;       //Hilfs-A
A2                   :byte;
A2Copy               :byte;       //Hilfs-A
A3                   :byte;
A3Copy               :byte;       //Hilfs-A
Ax                   :byte;
B                    :byte;       //Anzahl �usserer Pumpvorg�nge
Bx                   :byte;       //Hilfsvariable beim Pumpen, nur f�r Anzeige
Lade_Sekunde         :byte;       //zum Akkuladen
ADg_Sekunde          :byte;       //zum Pr�fen des Bel�fterdrucks
ADg_Counter          :byte;
G_Sekunde            :word;       //hochlaufende Sekunde f�r Gebl�se
G_Zeit1              :word;
G_Zeit2              :word;
Akkuzeit             :word;       //hochlaufende Sekunde f�r die Zeit, die das
                                  //Ger�t auf Akku l�uft, max. ca. 5h = 18.000s
P_Sekunde            :word;       //hochlaufende Sekunde f�r pumpen und r�cksp�len
SO_Sekunde           :longword;   //hochlaufende Sekunde f�r wochenweises aussetzen
S_Sekunde            :longword;   //hochlaufende Sekunde f�r wochenweises aktivieren der SLP
Stag_Sekunde         :longword;   //hochlaufende Sekunde f�r Stagnationsanzeige
StagFlag             :boolean;    //wird true, wenn Stag_Sekunde 21d erreicht hat
ClearwaterFlag       :boolean;    //bei Akkubetrieb von mehr als 2h wird dieses
                                  //Flag gesetzt, nach R�ckkehr zum Netzbetrieb
                                  //bekommt der Kunde eine Warnung, dass gebrauchtes
                                  //durch frisches Wasser ersetzt werden soll
ADgFlag              :boolean;    //true: wenn ADg Bel�fterdruck unter 0,02bar
                                  //false:wenn ADg Bel�fterdruck unter 0,02bar
Out                  :boolean;    //true: als Gebl�se parallel air, false: filter
OutOld               :boolean;    //OutOld hat den alten Zustand von Out
OutCopy              :boolean;    //Kopie von Out
OutFlag              :boolean;    //true: 0-30d abgelaufen, 24V f�r filter aktivieren
O_Sekunde            :longword;   //hochlaufende Sekunde f�r Filter am 24V-Ausgang
OutTag               :word;       //0-30d, nach der Zeit geht das Filter an 24V
OutTagCopy           :word;
OutSekunde           :word;       //0-120s, Zeit in der das Filter an 24V liegt
OutSekundeCopy       :word;
OutTimer             :word;       //l�uft von 0-OutSekunde im Sekundentakt

SOzeit               :longword;
SLPzeit              :longword;
Minute               :word;       //f�r den 6-minuten service counter
Sekunde              :word;
T1                   :word;       //Zeitbereich beim pumpen
T2                   :word;
T3                   :word;
T4                   :word;
T5                   :word;
T6                   :word;
T7                   :word;
T8                   :word;
MemCount             :word;
ResAkkuflag_Sekunde  :word;
DutyFlush            :word;       //Tastverh�ltnis f�r Saugpumpe
DutyReflush          :word;       //Tastverh�ltnis f�r R�cksp�lpumpe
mode                 :word;       //mode ist die nummer der einzelnen men�s
Uin1                 :float;      //Spannung Kanal1
Uin2                 :float;      //Spannung Kanal2
Uin3                 :float;      //Spannung Kanal3
Uin4                 :float;      //Spannung Kanal4
Netzaus              :word;       //Anzahl Netzausf�lle gesamt
Netzaus1             :word;       //Anzahl netzausf�lle bei flush
Netzaus2             :word;       //Anzahl netzausf�lle bei air
Tastenzeit           :byte;       // ++ wenn taste gedr�ckt
i                    :word;       //Laufindex
j                    :word;       //Laufindex
x                    :word;       //variable
Mem                  :byte;       //Membrananzahl von 1 - 6,
SOweek               :byte;       //nach 1,2 od. 3 Wo f�r 24h kein saugen/r�cksp�len
SOweekCopy           :byte;       //kopie von option
SOTimer              :longword;   //l�uft hoch bis c_24h wenn SO
SOFlag               :boolean;    //true, wenn SO
Testmode             :byte;       //0=test aufgerufen aber noch nichts gew�hlt
                                  //1=normalbetrieb, 2=bel�ftung, 3=saugen
                                  //4=R�cksp�len, 5=Schlammpumpe
Servicecounter       :word;       //l�uft bei service jeweils 6 minuten
Counter              :word;       //Hilfsz�hler
CountEnd             :word;       //Hilfsgr��e: Ende des Z�hlens

text1                :string[20];
text2                :string[20];
text3                :string[20];
text4                :string[20];
text5                :string[20];

{$EEprom}
var
EP_StartFlag         :byte;       //55 bei stromausfall, ff bei systemreset
EP_A                 :byte;
EP_A1                :byte;
EP_A2                :byte;
EP_A3                :byte;
EP_B                 :byte;
EP_Mem               :byte;
EP_SOweek            :byte;
EP_SOweekCopy        :byte;
EP_Netzaus           :word;
EP_Netzaus1          :word;
EP_Netzaus2          :word;
EP_Language          :boolean;
EP_GSekunde          :word;
EP_PSekunde          :word;
EP_SOSekunde         :longword;
EP_SSekunde          :longword;
EP_OSekunde          :longword;
EP_OutTag            :word;
EP_OutSekunde        :word;
EP_Out               :boolean;
EP_SOzeit            :longword;
EP_SLPzeit           :longword;
EP_Hour              :byte;
EP_Minute            :byte;
EP_Second            :byte;
EP_AirOn             :word;
EP_AirOff            :word;
EP_PT1Hour           :byte;
EP_PT1Minute         :byte;
EP_PT2Hour           :byte;
EP_PT2Minute         :byte;
EP_PT3Hour           :byte;
EP_PT3Minute         :byte;
EP_PTFlag            :boolean;
EP_PT1               :boolean;
EP_PT2               :boolean;
EP_PT3               :boolean;
EP_SLPday            :word;
EP_SLPdayCopy        :word;
EP_SLPminute         :word;
EP_SLPminuteCopy     :word;
EP_StagSekunde       :longword;
EP_StagFlag          :boolean;
EP_ClearwaterFlag    :boolean;

{$IDATA}

{--------------------------------------------------------------}
{ functions }

procedure InitPorts;
begin
  PortA:= %11111111;     //interne pullups am port mit einer 1 aktivieren
  DDRA := %00000000;     //dann direction definieren, 0=input, 1=output
  DDRB := %11111111;     //nur outputs
  //PortD:= %11110111;
  DDRD := %00000001;     //nur PD0 output
  PortE:= %00000010;
  DDRE := %11111101;
  PortF:= %11100000;
  DDRF := %00000000;     //nur inputs
  DDRG := %11111111;     //nur outputs
  LCDCharSet(ae, $0A, $00, $0E, $01, $0F, $11, $0F, $00); //�
  LCDCharSet(oe, $0A, $00, $0E, $11, $11, $11, $0E, $00); //�
  LCDCharSet(ue, $0A, $00, $11, $11, $11, $13, $0D, $00); //�
end InitPorts;

{--------------------------------------------------------------}

procedure Click;
begin
  Piezo:=true;
  mdelay(1);
  Piezo:=false;
end;

{--------------------------------------------------------------}

procedure NextKey;
begin
  if (Next=false) then
     mdelay(25);
     if (Next=false) then
        NextFlag:=true;
     endif;
  endif;
end;

{--------------------------------------------------------------}

procedure EnterTaste;
begin
  if (Enter=false) and (MemEnter=false) then //Flanke Entertaste
     mdelay(25);
     if Enter=false then
        MemEnter:=true;                 //Tick wird true und muss von
        TickEnter:=true;                //procedure zur�ckgesetzt werden
        Click;                          //Piezo einmal kurz klicken lassen
        TC:=0;                          //Keycounter auf 0 zur�cksetzen
     else
        MemEnter:=false;
        TickEnter:=false;
     endif;
  endif;
  if (Enter=true) then
     MemEnter:=false;
  endif;
end;

{--------------------------------------------------------------}

procedure NextTaste;
begin
  if (Next=false) and (MemNext=false) then
     mdelay(25);
     if (Next=false) then
        MemNext:=true;
        TickNext:=true;
        Click;
        TC:=0;
     else
        MemNext:=false;
        TickNext:=false;
     endif;
  endif;
  if (Next=true) then
     MemNext:=false;
  endif;
end;

{--------------------------------------------------------------}

procedure RTCtickSecond;                //wird von RTC bei jedem Sekunden�bertrag
begin                                   //aufgerufen
  TickSecond:= true;
end;

{--------------------------------------------------------------}

procedure RTCtickMinute;                //wird von RTC bei jedem Minuten�bertrag
begin                                   //aufgerufen
  TickMinute:= true;
end;

{--------------------------------------------------------------}

procedure RTCtickHour;                  //wird von RTC bei jedem Stunden�bertrag
begin                                   //aufgerufen
  TickHour:= true;
end;

{--------------------------------------------------------------}

procedure Zeit (ServiceCounter:word);   //Umrechnung von Servicecounter (in s)
begin                                   //in Minuten und Sekunden
  Minute:=(ServiceCounter div 60);
  Sekunde:=ServiceCounter-(Minute*60);

 { case Minute of
   5 : Sekunde:=(ServiceCounter-300); |
   4 : Sekunde:=(ServiceCounter-240); |
   3 : Sekunde:=(ServiceCounter-180); |
   2 : Sekunde:=(ServiceCounter-120); |
   1 : Sekunde:=(ServiceCounter-60);  |
   0 : Sekunde:=ServiceCounter;       |
  endcase; }
end;

{--------------------------------------------------------------}

procedure Get24V;
begin
  Uin1:=float(GetADC(1));               //Berechnung der Versorgungsspannung
  Uin1:=(Uin1*5.0)/1024;                //auf 5V beziehen
  Uin1:=(Uin1*10.1);                    //Teilungsfaktor von 24V auf 5V
end;

{--------------------------------------------------------------}

procedure GetAdr;
begin
  Uin2:=float(GetADC(2));               //Spannungswert Kanal2 holen
  Uin2:=(Uin2*5.0)/1024;                //in float wandeln, auf 5V beziehen
  Uin2:=(Uin2*1000.0)/200;
  Uin2:=((Uin2-4.0)/16)*1.6;
  Uin2:=Uin2 - 0.8;
end;

{--------------------------------------------------------------}

procedure GetAdg;                       //Druck Air
begin
  Uin3:=float(GetADC(3));
  Uin3:=(Uin3*5.0)/1024;
  Uin3:=(Uin3*1000.0)/200;
  Uin3:=((Uin3-4.0)/16)*0.6;
  //Uin3:=Uin3 - 0.8;
end;

{--------------------------------------------------------------}

procedure Get9V;
begin
  Uin4:=float(GetADC(4));
  Uin4:=(Uin4*5.0)/1024;                //auf 5V beziehen
  Uin4:=(Uin4*2.96);                    //2,96 ist Teilungsfaktor von 9V auf 5V
end;

{--------------------------------------------------------------}

procedure ShowText;
begin
  LCDcursor(false, false);
  for x:=0 to 19 do
      if (Ord (text1[x])=228) then      //Umlaute �, �, � finden und ersetzen
         text1[x]:=ae; //ae=�
      endif;
      if (Ord (text1[x])=252) then
         text1[x]:=ue; //ue=�
      endif;
      if (Ord (text1[x])=246) then
         text1[x]:=oe; //oe=�
      endif;
  endfor;
  LCDxy(0,0);
  Write(LCDout, text1);

  for x:=0 to 19 do
      if (Ord (text2[x])=228) then
         text2[x]:=ae; //ae=�
      endif;
      if (Ord (text2[x])=252) then
         text2[x]:=ue; //ue=�
      endif;
      if (Ord (text2[x])=246) then
         text2[x]:=oe; //oe=�
      endif;
  endfor;
  LCDxy(0,1);
  Write(LCDout, text2);

  for x:=0 to 19 do
      if (Ord (text3[x])=228) then
         text3[x]:=ae; //ae=�
      endif;
      if (Ord (text3[x])=252) then
         text3[x]:=ue; //ue=�
      endif;
      if (Ord (text3[x])=246) then
         text3[x]:=oe; //oe=�
      endif;
  endfor;
  LCDxy(0,2);
  Write(LCDout, text3);

  for x:=0 to 19 do
      if (Ord (text4[x])=228) then
         text4[x]:=ae; //ae=�
      endif;
      if (Ord (text4[x])=252) then
         text4[x]:=ue; //ue=�
      endif;
      if (Ord (text4[x])=246) then
         text4[x]:=oe; //oe=�
      endif;
  endfor;
  LCDxy(0,3);
  Write(LCDout, text4);
end;

{--------------------------------------------------------------}

procedure MembranText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu1:              ';
     text2:= 'Number of MEM='+ByteToStr((mem):2:0)+'    ';
     text3:= 'NEXT: to next menu  ';
     text4:= 'ENTER: to change MEM';
  else
     text1:= 'Men�1:              ';
     text2:= 'Anzahl MEM='+ByteToStr((mem):2:0)+'       ';
     text3:= 'NEXT: n�chstes Men� ';
     text4:= 'ENTER: MEM �ndern   ';
  endif;
  mode:=1;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Mem1Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu1:              ';
     text2:= 'Change number of MEM';
     text3:= 'NEXT: MEM= '+ByteToStr(mem:2:0)+'       ';
     text4:= 'ENTER: Set          ';
  else
     text1:= 'Men�1:              ';
     text2:= 'Anzahl �ndern       ';
     text3:= 'NEXT: '+ByteToStr(mem:2:0)+'            ';
     text4:= 'ENTER: Set          ';
  endif;
  mode:=11;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SOText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu2:              ';
     text2:= 'Suction Overfl SO='+ByteToStr((SOweek):2:0);
     text3:= 'NEXT: to next menu  ';
     text4:= 'ENTER: change SO    ';
  else
     text1:= 'Men�2:';
     text2:= 'Saug�berlauf S�='+ByteToStr((SOweek):2:0)+'  ';
     text3:= 'NEXT: n�chstes Men� ';
     text4:= 'ENTER: S� �ndern    ';
  endif;
  mode:=20;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SO1Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu2:              ';
     text2:= 'Change SO='+ ByteToStr(SOweekCopy:2:0)+'        ';
     text3:= 'NEXT: next week     ';
     text4:= 'ENTER: Set          ';
  else
     text1:= 'Men�2:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: Woche =    ' + ByteToStr(SOweekCopy:2:0);
     text4:= 'ENTER: Woche=1      ';
  endif;
  mode:=21;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SO2Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu2:              ';
     text2:= 'SO old='+ByteToStr(SOweek:2:0)+' new=' +ByteToStr(SOweekCopy:2:0)+'    ';
     text3:= 'NEXT: exit,no change';
     text4:= 'ENTER: confirm      ';
  else
     text1:= 'Men�2:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: keine �nderung';
     text4:= 'ENTER: kein �berlauf';
  endif;
  mode:=22;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SLPText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu3:              ';
     text2:= 'Sludgepump='+IntToStr(SLPday:2:0)+' /'
                          +IntToStr(SLPminute:2:0)+'   ';
     text3:= 'NEXT: to next menu  ';
     text4:= 'ENTER: change SLP   ';
  else
     text1:= 'Men�3:';
     text2:= 'Saug�berlauf S�='+ByteToStr((SOweek):2:0)+'  ';
     text3:= 'NEXT: n�chstes Men� ';
     text4:= 'ENTER: S� �ndern    ';
  endif;
  mode:=30;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SLP1Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu3:              ';
     text2:= 'Change SLP          ';
     text3:= 'NEXT:>' + IntToStr((SLPdayCopy):2:'0') + '</ '
                      + IntToStr((SLPminuteCopy):2:'0') +'       ';
     text4:= 'ENTER: Set day 0-60 ';
  else
     text1:= 'Men�2:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: Woche =    ' + ByteToStr(SOweekCopy:2:0);
     text4:= 'ENTER: Woche=1      ';
  endif;
  mode:=31;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SLP2Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu3:              ';
     text2:= 'Change SLP          ';
     text3:= 'NEXT: ' + IntToStr((SLPdayCopy):2:'0') + ' />'
                      + IntToStr((SLPminuteCopy):2:'0') + '<      ';
     text4:= 'ENTER: set min 0-60 ';
  else
     text1:= 'Men�2:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: Woche =    ' + ByteToStr(SOweekCopy:2:0);
     text4:= 'ENTER: Woche=1      ';
  endif;
  mode:=32;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SLP3Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu3:              ';
     text2:= 'SLP old='+IntToStr(SLPday:2:0)+' new='
              +IntToStr(SLPdayCopy:2:0)+'   ';
     text3:= 'NEXT: exit,no change';
     text4:= 'ENTER: confirm      ';
  else
     text1:= 'Men�2:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: keine �nderung';
     text4:= 'ENTER: kein �berlauf';
  endif;
  mode:=33;
  ShowText;
end;

{--------------------------------------------------------------}

procedure ServText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu4:              ';
     text2:= 'Testmode            ';
     text3:= 'NEXT: to next menu  ';
     text4:= 'ENTER: for Testmode ';
  else
     text1:= 'Men�4:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chstes Men� ';
     text4:= 'ENTER: Testbetrieb  ';
  endif;
  mode:=40;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Serv1Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu4:              ';
     text2:= 'Testmode            ';
     text3:= 'NEXT: to next test  ';
     text4:= 'ENTER: for Air ON   ';
  else
     text1:= 'Men�4:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chster Test ';
     text4:= 'ENTER: Bel�ftung an ';
  endif;
  mode:=41;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Serv2Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu4:              ';
     text2:= 'Testmode            ';
     text3:= 'NEXT: to next test  ';
     text4:= 'ENTER:for Suction ON';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chster Test ';
     text4:= 'ENTER: Saugen an    ';
  endif;
  mode:=42;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Serv3Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu4:              ';
     text2:= 'Testmode            ';
     text3:= 'NEXT: to next test  ';
     text4:= 'ENTER: Backflush ON ';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: Men� verlassen';
     text4:= 'ENTER: R�cksp�len an';
  endif;
  mode:=43;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Serv4Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu4:              ';
     text2:= 'Testmode            ';
     text3:= 'NEXT: to exit menu  ';
     text4:= 'ENTER: Sludgepump ON';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: Men� verlassen';
     text4:= 'ENTER: R�cksp�len an';
  endif;
  mode:=44;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Test1Text;
begin
  GetAdg;
  Zeit(ServiceCounter);
  if Language then
     //       12345678901234567890
     text1:= 'Menu4: Test Air ON  ';
     text2:= 'P(ADg):'+ FloatToStr(Uin3:5:3:' ')+'bar   ';
     text3:= 'Autostop:'+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min  ';
     text4:= 'Stop with ENTER/NEXT';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chster Test ';
     text4:= 'ENTER: Saugen an    ';
  endif;
  mode:=45;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Test2Text;
begin
  GetAdr;
  Zeit(ServiceCounter);
  if Language then
     //       12345678901234567890
     text1:= 'Menu4: Test Suction ';

     //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
     //SS sieht Wasser = 1 (z.B. Beh�lter voll)

     if SSBRmin and not SSKlarmax then      //BR muss voll und Klarmax nicht voll sein
        text2:= 'P(ADr):'+ FloatToStr(Uin2:5:3:' ')+'bar     ';
     else
        text2:= 'Suction not possible';
     endif;

     text3:= 'Autostop:'+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min  ';
     text4:= 'Stop with ENTER/NEXT';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chster Test ';
     text4:= 'ENTER: Saugen an    ';
  endif;
  mode:=46;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Test3Text;
begin
  GetAdr;
  Zeit(ServiceCounter);
  if Language then
     //       12345678901234567890
     text1:= 'Menu4:Test Backflush';

     //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
     //SS sieht Wasser = 1 (z.B. Beh�lter voll)

     if SSBRmin and not SSKlarmax then      //BR muss voll und Klarmax nicht voll sein
        text2:= 'P(ADr):'+ FloatToStr(Uin2:5:3:' ')+'bar     ';
     else
        text2:= 'Backfl. not possible';
     endif;

     text3:= 'Autostop:'+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min  ';
     text4:= 'Stop with ENTER/NEXT';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chster Test ';
     text4:= 'ENTER: Saugen an    ';
  endif;
  mode:=47;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Test4Text;
begin
  Zeit(ServiceCounter);
  if Language then
     //       12345678901234567890
     text1:= 'Menu4: Test SLP     ';

     //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
     //SS sieht Wasser = 1 (z.B. Beh�lter voll)

     if SSBRmin then                    //BR muss voll sein
        text2:= 'Test Sludgepump     ';
     else
        text2:= 'Test not possible   ';
     endif;

     text3:= 'Autostop:'+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min  ';
     text4:= 'Stop with ENTER/NEXT';
  else
     text1:= 'Men�3:              ';
     text2:= 'Testbetrieb         ';
     text3:= 'NEXT: n�chster Test ';
     text4:= 'ENTER: Saugen an    ';
  endif;
  mode:=48;
  ShowText;
end;

{--------------------------------------------------------------}

procedure StatusText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu5:              ';
     text2:= 'Status overview     ';
     text3:= 'NEXT: to next menu  ';
     text4:= 'ENTER: for status   ';
  else
     text1:= 'Men�5:              ';
     text2:= 'Status              ';
     text3:= 'NEXT: n�chstes Men� ';
     text4:= 'ENTER: Status zeigen';
  endif;
  mode:=50;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Status1Text;
begin
  //       12345678901234567890
  text1:= '------Status 1------';
  text2:= 'MEM       :'+ByteToStr((mem):2:0)+'       ';
  text3:= 'AirON/OFF :'+IntToStr(AirOn:2:0)+'/'+IntToStr(AirOff:2:0)+' min';

  if Out then
  text4:= '24V-Output: Air     ';
  else
  text4:= 'Filter: '+IntToStr((OutSekunde) :3:0)+' s / '
                    +IntToStr((OutTag) :2:0)+'d ';
  endif;

  mode:=51;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Status2Text;
begin
  //       12345678901234567890
  text1:= '------Status 2------';
  text2:= 'Suc.Overflow:'+ByteToStr((SOweek):2:0)+' week' ;
  text3:= 'Time in sec :'+LongToStr((SOzeit):7:0);
  text4:= 'Execution in:'+LongToStr((SO_Sekunde):7:0);
  mode:=52;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Status3Text;
begin
  //       12345678901234567890
  text1:= '------Status 3------';
  text2:= 'SLP : '+IntToStr((SLPday):2:0)+' d/ '
                  +IntToStr((SLPminute):2:0)+ ' min  ';
  text3:= 'Time in sec :'+LongToStr((SLPZeit):7:0);
  text4:= 'Execution in:'+LongToStr((S_Sekunde):7:0);
  mode:=53;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Status4Text;
begin
  GetAdr;
  GetAdg;
  //       12345678901234567890
  text1:= '------Status 4------';
  text2:= 'P (ADr): '+FloatToStr(Uin2:5:3:' ')+' bar  ';
  text3:= 'P (ADg): '+FloatToStr(Uin3:5:3:' ')+' bar  ';
  text4:= '                    ';
  mode:=54;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Status5Text;
begin
  Get24V;
  Get9V;
  //       12345678901234567890
  text1:= '------Status 5------';
  text2:= 'Supply : '+FloatToStr(Uin1:5:1:' ')+' V    ';
  text3:= '9V-Accu: '+FloatToStr(Uin4:5:1:' ')+' V    ';

  if Laden then
  text4:= 'Accu is charged     ';
  else
  text4:= 'Accu is full        ';
  endif;

  mode:=55;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Status6Text;
begin
  //       12345678901234567890
  text1:= '------Status 6------';
  text2:= 'Supply failures: '+IntToStr((Netzaus) :2:0)+' ';
  text3:= 'During pumping : '+IntToStr((Netzaus1):2:0)+' ';
  text4:= 'Firmware GWA2.20   ';
  mode:=56;
  ShowText;
end;

{--------------------------------------------------------------}

procedure ZeitText;
begin
  //       12345678901234567890
  text1:= 'Menu6:              ';
  text2:= 'Set actual time     ';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: change time  ';
  mode:=60;
  ShowText;
end;

{--------------------------------------------------------------}

procedure StundenText;
begin
  //       12345678901234567890
  text1:= 'Menu6:              ';
  text2:= 'Change hour         ';
  text3:= 'NEXT: >' + ByteToStr(RTCHour:2:'0') +'<:' +ByteToStr(RTCMinute:2:'0')
                    + ' :'+ ByteToStr(RTCSecond:2:'0')+'h';
  text4:= 'ENTER: set hour     ';
  mode:=61;
  ShowText;
end;

{--------------------------------------------------------------}

procedure MinutenText;
begin
  //       12345678901234567890
  text1:= 'Menu6:              ';
  text2:= 'Change minute       ';
  text3:= 'NEXT:  ' + ByteToStr(RTCHour:2:'0') +' :>' + ByteToStr(RTCMinute:2:'0')
                    + '<:'+ ByteToStr(RTCSecond:2:'0')+'h';
  text4:= 'ENTER: set minute   ';
  mode:=62;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SekundenText;
begin
  //       12345678901234567890
  text1:= 'Menu6:              ';
  text2:= 'Change second       ';
  text3:= 'NEXT: ' + ByteToStr(RTCSecond:2:0) + '    ';
  text4:= 'ENTER: set second   ';
  mode:=63;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT1Text;
begin
  //       12345678901234567890
  text1:= 'Menu7:              ';
  text2:= 'Change start time T1';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: set time     ';
  mode:=70;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT1StundenText;
begin
  //       12345678901234567890
  text1:= 'Menu7:              ';
  text2:= 'Change hour for T1  ';
  text3:= 'NEXT:>' + ByteToStr(PT1HourCopy:2:'0')+ '<:'+ ByteToStr(PT1MinuteCopy:2:'0')
                   + '         ';
  text4:= 'ENTER: set hour T1  ';
  mode:=71;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT1MinutenText;
begin
  //       12345678901234567890
  text1:= 'Menu7:              ';
  text2:= 'Change minute for T1';
  text3:= 'NEXT: ' + ByteToStr(PT1HourCopy:2:'0')+ ' :>'+ ByteToStr(PT1MinuteCopy:2:'0')
                   + '<        ';
  text4:= 'ENTER: set minute T1';
  mode:=72;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT2Text;
begin
  //       12345678901234567890
  text1:= 'Menu8:              ';
  text2:= 'Change start time T2';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: set time     ';
  mode:=80;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT2StundenText;
begin
  //       12345678901234567890
  text1:= 'Menu8:              ';
  text2:= 'Change hour for T2  ';
  text3:= 'NEXT:>' + ByteToStr(PT2HourCopy:2:'0')+ '<:'+ ByteToStr(PT2MinuteCopy:2:'0')
                    + '         ';
  text4:= 'ENTER: set hour T2  ';
  mode:=81;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT2MinutenText;
begin
  //       12345678901234567890
  text1:= 'Menu8:              ';
  text2:= 'Change minute for T2';
  text3:= 'NEXT: ' + ByteToStr(PT2HourCopy:2:'0')+ ' :>'+ ByteToStr(PT2MinuteCopy:2:'0')
                   + '<        ';
  text4:= 'ENTER: set minute T2';
  mode:=82;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT3Text;
begin
  //       12345678901234567890
  text1:= 'Menu9:              ';
  text2:= 'Change start time T3';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: set time     ';
  mode:=90;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT3StundenText;
begin
  //       12345678901234567890
  text1:= 'Menu7:              ';
  text2:= 'Change hour for T3  ';
  text3:= 'NEXT:>' + ByteToStr(PT3HourCopy:2:'0')+ '<:'+ ByteToStr(PT3MinuteCopy:2:'0')
                   + '         ';
  text4:= 'ENTER: set hour T3  ';
  mode:=91;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PT3MinutenText;
begin
  //       12345678901234567890
  text1:= 'Menu7:              ';
  text2:= 'Change minute for T3';
  text3:= 'NEXT: ' + ByteToStr(PT3HourCopy:2:'0')+ ' :>'+ ByteToStr(PT3MinuteCopy:2:'0')
                   + '<        ';
  text4:= 'ENTER: set minute T3';
  mode:=92;
  ShowText;
end;

{--------------------------------------------------------------}

procedure AText;
begin
  //       12345678901234567890
  text1:= 'Menu10:             ';
  text2:= 'Change A1/A2 suction';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: change A1/A2 ';
  mode:=100;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SetA1Text;
begin
  //       12345678901234567890
  text1:= 'Menu10:             ';
  text2:= 'Change A1 (0-45)    ';
  text3:= 'NEXT:>' + ByteToStr(A1Copy:2:0) + '< '
                   + ByteToStr(A2Copy:2:0) + '        ';
                   //+ ByteToStr(A3Copy:2:0) + '     ';
  text4:= 'ENTER: set A1       ';
  mode:=101;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SetA2Text;
begin
  //       12345678901234567890
  text1:= 'Menu10:             ';
  text2:= 'Change A2 (0-45)    ';
  text3:= 'NEXT: ' + ByteToStr(A1Copy:2:0) + ' >'
                   + ByteToStr(A2Copy:2:0) + '<       ';
                   //+ ByteToStr(A3Copy:2:0) + '    ';
  text4:= 'ENTER: set A2       ';
  mode:=102;
  ShowText;
end;

{--------------------------------------------------------------}

procedure SetA3Text;
begin
  //       12345678901234567890
  text1:= 'Menu10:             ';
  text2:= 'Change A3 (0-45)    ';
  text3:= 'NEXT: ' + ByteToStr(A1Copy:2:0) + ' '
                   + ByteToStr(A2Copy:2:0) + ' >'
                   + ByteToStr(A3Copy:2:0) + '<    ';
  text4:= 'ENTER: set A3       ';
  mode:=103;
  ShowText;
end;

{--------------------------------------------------------------}

procedure NoSetAText;
begin
  //       12345678901234567890
  text1:= 'Change of A1 or A2  ';
  text2:= 'during operation is ';
  text3:= 'not possible !      ';
  text4:= 'exit with NEXT/ENTER';
  mode:=104;
  ShowText;
end;

{--------------------------------------------------------------}

procedure AirText;
begin
  //       12345678901234567890
  text1:= 'Menu11:             ';
  text2:= 'Set time for Air    ';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: change time  ';
  mode:=110;
  ShowText;
end;

{--------------------------------------------------------------}

procedure AirOnText;
begin
  //       12345678901234567890
  text1:= 'Menu11:             ';
  text2:= 'Change Air ON/OFF   ';
  text3:= 'NEXT:>' + IntToStr(AirOn:2:'0') + '</ '
                   + IntToStr(AirOff:2:'0') +'       ';
  text4:= 'ENTER: set minute   ';
  mode:=111;
  ShowText;
end;

{--------------------------------------------------------------}

procedure AirOffText;
begin
  //       12345678901234567890
  text1:= 'Menu11:             ';
  text2:= 'Change Air ON/OFF   ';
  text3:= 'NEXT: ' + IntToStr(AirOn:2:'0') + ' />'
                   + IntToStr(AirOff:2:'0') + '<      ';
  text4:= 'ENTER: set minute   ';
  mode:=112;
  ShowText;
end;

{--------------------------------------------------------------}

procedure CleanText;
begin
  //       12345678901234567890
  text1:= 'Menu12:             ';
  text2:= 'Chemical cleaning   ';
  text3:= 'NEXT: exit menu     ';
  text4:= 'ENTER:start cleaning';
  mode:=120;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean1Text;
begin
  //       12345678901234567890
  text1:= 'Menu12: Step1       ';
  text2:= 'Chem. cleaning start';
  text3:= 'NEXT: exit menu     ';
  text4:= 'ENTER: confirm      ';
  mode:=121;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean2Text;
begin
  //       12345678901234567890
  text1:= 'Menu12: Step2       ';
  text2:= 'Sludgepump working  ';

  //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
  //SS sieht Wasser = 1 (z.B. Beh�lter voll)

  if SSBRmin then                         //der Bioreaktor hat genug Wasser
     text3:= 'Status: BRmin +     ';
  else
     text3:= 'Status: BRmin -     ';
  endif;
  text4:= 'ENTER/NEXT: exit    ';
  mode:=122;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean3Text;
begin
  Zeit(ServiceCounter); //Sekundenz�hler in min u. sek umrechnen
  //       12345678901234567890
  text1:= 'Menu12: Step3       ';
  //text2:= 'Backflush ' +IntToStr(Servicecounter:4:0)+' s     ';
  text2:= 'Backfl.Stop'+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min';
  text3:= 'NEXT:  exit         ';
  text4:= 'ENTER: to next step ';
  mode:=123;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean4Text;
begin
  //       12345678901234567890
  text1:= 'Menu12: Step4       ';
  text2:= 'Chem.liquid filled ?';
  text3:= 'NEXT: exit          ';
  text4:= 'ENTER:start cleaning';
  mode:=124;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean5Text;
begin
  Zeit(ServiceCounter); //Sekundenz�hler in min u. sek umrechnen
  //       12345678901234567890
  text1:= 'Menu12: Step5       ';
  text2:= 'Cleaning working    ';
  text3:= 'Autostop '+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min  ';
  //text3:= 'Autostop: '+IntToStr(Counter:4:0)+'      ';
  text4:= 'ENTER/NEXT: exit    ';
  mode:=125;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean6Text;
begin
  //       12345678901234567890
  text1:= 'Menu12: Step6       ';
  text2:= 'Clearwater filled in';
  text3:= 'NEXT: exit          ';
  text4:= 'ENTER:start flushing';
  mode:=126;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Clean7Text;
begin
  Zeit(ServiceCounter); //Sekundenz�hler in min u. sek umrechnen
  //       12345678901234567890
  text1:= 'Menu12: Step7       ';
  text2:= 'Clearwater flushing ';
  text3:= 'Autostop:'+IntToStr(Minute:2:0)+':'+IntToStr(Sekunde:2:0)+' min  ';
  text4:= 'ENTER/NEXT: exit    ';
  mode:=127;
  ShowText;
end;

{--------------------------------------------------------------}

procedure OutText;
begin
  //       12345678901234567890
  text1:= 'Menu13:             ';
  text2:= '24VOut= Air / Filter';
  text3:= 'NEXT: exit menu     ';
  text4:= 'ENTER: change output';
  mode:=130;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out1Text;
begin
  //       12345678901234567890
  text1:= 'Menu13:             ';
  text2:= 'Output=Air          ';
  text3:= 'NEXT: next selection';
  text4:= 'ENTER: set Air      ';
  mode:=131;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out2Text;
begin
  //       12345678901234567890
  text1:= 'Menu13:             ';
  text2:= 'Output=Filter       ';
  text3:= 'NEXT: next selection';
  text4:= 'ENTER: set Filter   ';
  mode:=132;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out3Text;
begin
  //       12345678901234567890
  text1:= 'Menu13:             ';
  text2:= 'New Output=Air      ';
  text3:= 'NEXT: exit,no change';
  text4:= 'ENTER: confirm      ';
  mode:=133;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out4Text;
begin
  //       12345678901234567890
  text1:= 'Menu13:             ';
  text2:= 'New Output=Filter   ';
  text3:= 'NEXT: exit,no change';
  text4:= 'ENTER: confirm      ';
  mode:=134;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out5Text;
begin
  //       12345678901234567890
  text1:= 'Menu13:             ';
  text2:= 'Set: ' + IntToStr((OutSekunde):3:'0') + ' s / '
             + IntToStr((OutTag):2:'0') +' d    ';
  text3:= 'NEXT: exit,no change';
  text4:= 'ENTER: change timing';
  mode:=135;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out6Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu13:              ';
     text2:= 'set time Filter      ';
     text3:= 'NEXT:>' + IntToStr((OutSekundeCopy):3:'0') + ' s< /'
                      + IntToStr((OutTagCopy):2:'0') +' d    ';
     text4:= 'ENTER: Set second    ';
  else
     text1:= 'Men�13:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: Woche =    ' + ByteToStr(SOweekCopy:2:0);
     text4:= 'ENTER: Woche=1      ';
  endif;
  mode:=136;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Out7Text;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Menu13:              ';
     text2:= 'set time Filter      ';
     text3:= 'NEXT: ' + IntToStr((OutSekundeCopy):3:'0') + ' s / >'
                      + IntToStr((OutTagCopy):2:'0') +' d<    ';
     text4:= 'ENTER: Set day       ';
  else
     text1:= 'Men�13:              ';
     text2:= 'Saug�berlauf �ndern ';
     text3:= 'NEXT: Woche =    ' + ByteToStr(SOweekCopy:2:0);
     text4:= 'ENTER: Woche=1      ';
  endif;
  mode:=137;
  ShowText;
end;

{--------------------------------------------------------------}

procedure LanguageText;
begin
  //       12345678901234567890
  text1:= 'Menu14:              ';
  text2:= 'Language/Sprache    ';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: change/�ndern';
  mode:=140;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Lang1Text;
begin
  //       12345678901234567890
  text1:= 'Menu14:              ';
  text2:= 'Language/Sprache    ';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: English      ';
  mode:=141;
  ShowText;
end;

{--------------------------------------------------------------}

procedure Lang2Text;
begin
  //       12345678901234567890
  text1:= 'Menu14:              ';
  text2:= 'Language/Sprache    ';
  text3:= 'NEXT: to next menu  ';
  text4:= 'ENTER: Deutsch      ';
  mode:=142;
  ShowText;
end;

{--------------------------------------------------------------}

procedure ResetText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'System was reset    ';
     text3:= 'Press ENTER for >2s ';
     text4:= 'To restart system   ';
  else
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'Das System wurde    ';
     text3:= 'zur�ckgesetzt       ';
     text4:= 'Neustart mit ENTER  ';
  endif;
  mode:=199;
  ShowText;
end;

{--------------------------------------------------------------}

procedure AkkuText;
begin
  if Language then
     //       12345678901234567890
     text1:= '---- Attention ---- ';
     text2:= 'System runs on Accu ';
     text3:= 'all actions disabled';
     text4:= '9V-Accu: '+FloatToStr(Uin4:5:2:' ')+' V    ';
  else
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'Das System wurde    ';
     text3:= 'zur�ckgesetzt       ';
     text4:= 'Neustart mit ENTER  ';
  endif;
  mode:=200;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PowerFailureText;             //allgemeiner spannungsausfall
begin
  if Language then
     //       12345678901234567890
     text1:= 'A Power failure has ';
     text2:= 'occured,            ';
     text3:= 'confirm with        ';
     text4:= 'ENTER or NEXT       ';
  else
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'Das System wurde    ';
     text3:= 'zur�ckgesetzt       ';
     text4:= 'Neustart mit ENTER  ';
  endif;
  mode:=210;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PowerFailureText1;            //spannungsausfall beim pumpen
begin
  if Language then
     //       12345678901234567890
     text1:= 'Power failure       ';
     text2:= 'during flushing     ';
     text3:= 'confirm with        ';
     text4:= 'ENTER or NEXT       ';
  else
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'Das System wurde    ';
     text3:= 'zur�ckgesetzt       ';
     text4:= 'Neustart mit ENTER  ';
  endif;
  mode:=211;
  ShowText;
end;

{--------------------------------------------------------------}

procedure PowerFailureText2;            //spannungsausfall bei bel�ftung
begin
  if Language then
     //       12345678901234567890
     text1:= 'Power failure       ';
     text2:= 'during air          ';
     text3:= 'confirm with        ';
     text4:= 'press ENTER or NEXT ';
  else
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'Das System wurde    ';
     text3:= 'zur�ckgesetzt       ';
     text4:= 'Neustart mit ENTER  ';
  endif;
  mode:=212;
  ShowText;
end;

{--------------------------------------------------------------}

procedure StagText;                     //Stagnationstext
begin
  if Language then
     //       12345678901234567890
     text1:= 'Storagetime exceeded';
     text2:= 'Discharge clearwater';
     text3:= 'tank ! Confirm with ';
     text4:= 'ENTER or NEXT       ';
  else
     text1:= 'Intewa mbH Aachen   ';
     text2:= 'Das System wurde    ';
     text3:= 'zur�ckgesetzt       ';
     text4:= 'Neustart mit ENTER  ';
  endif;
  mode:=220;
  ShowText;
end;

{--------------------------------------------------------------}

procedure ClearwaterText;               //Clearwatertext
begin
  if Language then
     //       12345678901234567890
     text1:= 'Power failure> 21d ?';
     text2:= 'Check storage time  ';
     text3:= 'clear water! Confirm';
     text4:= 'with ENTER or NEXT  ';
  else
     text1:= 'Power failure> 21d ?';
     text2:= 'Check storage time  ';
     text3:= 'clear water! Confirm';
     text4:= 'with ENTER or NEXT  ';
  endif;
  mode:=230;
  ShowText;
end;

{--------------------------------------------------------------}

procedure ADgText;
begin
  if Language then
     //       12345678901234567890
     text1:= 'Failure Aeration    ';
     text2:= 'Check air and       ';
     text3:= 'confirm with        ';
     text4:= 'ENTER or NEXT       ';
  else
     text1:= 'Power failure> 21d ?';
     text2:= 'Check storage time  ';
     text3:= 'clear water! Confirm';
     text4:= 'with ENTER or NEXT  ';
  endif;
  mode:=240;
  ShowText;
end;

{--------------------------------------------------------------}

procedure CalcBX;
begin
  if B=0 then
     case P_Sekunde of
          0..1219 : BX:=1; |
       1220..2138 : if Ax=1 then BX:=Ax; else BX:=2; endif; |
       2139..3057 : if Ax=2 then Bx:=Ax; else BX:=3; endif; |
       3058..3976 : if Ax=3 then Bx:=Ax; else BX:=4; endif; |
       3977..5000 : if Ax=4 then Bx:=Ax; else BX:=4; endif; |
     endcase;
  endif;

  if B=1 then
     case P_Sekunde of
          0..1219 : BX:=5; |
       1220..2138 : if Ax=5 then Bx:=Ax; else BX:=6; endif; |
       2139..3057 : if Ax=6 then Bx:=Ax; else BX:=7; endif; |
       3058..3976 : if Ax=7 then Bx:=Ax; else BX:=8; endif; |
       3977..5000 : if Ax=8 then Bx:=Ax; else BX:=8; endif; |
     endcase;
  endif;

  if B=2 then
     case P_Sekunde of
          0..1219 : BX:=9; |
       1220..2138 : if Ax=9 then Bx:=Ax;  else BX:=10; endif; |
       2139..3057 : if Ax=10 then Bx:=Ax; else BX:=11; endif; |
       3058..3976 : if Ax=11 then Bx:=Ax; else BX:=12; endif; |
       3977..5000 : if Ax=12 then Bx:=Ax; else BX:=12; endif; |
     endcase;
  endif;

  if B=3 then
     case P_Sekunde of
          0..1219 : BX:=13;|
       1220..2138 : if Ax=13 then Bx:=Ax; else BX:=14; endif; |
       2139..3057 : if Ax=14 then Bx:=Ax; else BX:=15; endif; |
       3058..3976 : if Ax=15 then Bx:=Ax; else BX:=16; endif; |
       3977..5000 : if Ax=16 then Bx:=Ax; else BX:=16; endif; |
     endcase;
  endif;

  if B=4 then
     case P_Sekunde of
          0..1219 : BX:=17;|
       1220..2138 : if Ax=17 then Bx:=Ax; else BX:=18; endif; |
       2139..3057 : if Ax=18 then Bx:=Ax; else BX:=19; endif; |
       3058..3976 : if Ax=19 then Bx:=Ax; else BX:=20; endif; |
       3977..5000 : if Ax=20 then Bx:=Ax; else BX:=20; endif; |
     endcase;
  endif;

  if B=5 then
     case P_Sekunde of
          0..1219 : BX:=21;|
       1220..2138 : if Ax=21 then Bx:=Ax; else BX:=22; endif; |
       2139..3057 : if Ax=22 then Bx:=Ax; else BX:=23; endif; |
       3058..3976 : if Ax=23 then Bx:=Ax; else BX:=24; endif; |
       3977..5000 : if Ax=24 then Bx:=Ax; else BX:=24; endif; |
     endcase;
  endif;

  if B=6 then
     case P_Sekunde of
          0..1219 : BX:=25;|
       1220..2138 : if Ax=25 then Bx:=Ax; else BX:=26; endif; |
       2139..3057 : if Ax=26 then Bx:=Ax; else BX:=27; endif; |
       3058..3976 : if Ax=27 then Bx:=Ax; else BX:=28; endif; |
       3977..5000 : if Ax=28 then Bx:=Ax; else BX:=28; endif; |
     endcase;
  endif;

  if B=7 then
     case P_Sekunde of
          0..1219 : BX:=29;|
       1220..2138 : if Ax=29 then Bx:=Ax; else BX:=30; endif; |
       2139..3057 : if Ax=30 then Bx:=Ax; else BX:=31; endif; |
       3058..3976 : if Ax=31 then Bx:=Ax; else BX:=32; endif; |
       3977..5000 : if Ax=32 then Bx:=Ax; else BX:=32; endif; |
     endcase;
  endif;

  if B=8 then
     case P_Sekunde of
          0..1219 : BX:=33;|
       1220..2138 : if Ax=33 then Bx:=Ax; else BX:=34; endif; |
       2139..3057 : if Ax=34 then Bx:=Ax; else BX:=35; endif; |
       3058..3976 : if Ax=35 then Bx:=Ax; else BX:=36; endif; |
       3977..5000 : if Ax=36 then Bx:=Ax; else BX:=36; endif; |
     endcase;
  endif;

  if B=9 then
     case P_Sekunde of
          0..1219 : BX:=37;|
       1220..2138 : if Ax=37 then Bx:=Ax; else BX:=38; endif; |
       2139..3057 : if Ax=38 then Bx:=Ax; else BX:=39; endif; |
       3058..3976 : if Ax=39 then Bx:=Ax; else BX:=40; endif; |
       3977..5000 : if Ax=40 then Bx:=Ax; else BX:=40; endif; |
     endcase;
  endif;

  if B=10 then
     case P_Sekunde of
          0..1219 : BX:=41;|
       1220..2138 : if Ax=41 then Bx:=Ax; else BX:=42; endif; |
       2139..3057 : if Ax=42 then Bx:=Ax; else BX:=43; endif; |
       3058..3976 : if Ax=43 then Bx:=Ax; else BX:=44; endif; |
       3977..5000 : if Ax=44 then Bx:=Ax; else BX:=44; endif; |
     endcase;
  endif;

  if B=11 then
     BX:=45;
  endif;

end;

{--------------------------------------------------------------}

procedure UpdateDisplay;                //Prozessanzeige
begin
  if (not MenueFlag) then               //wenn Men�flag gesetzt menu,
     if TickSecond then                 //sonst normales display
        TickSecond:=false;              //display update jede sekunde

        Backlight:=true;                //bei der Prozessanzeige backlight immer an

        text1:=ByteToStr(RTCgetHour   : 2 : '0') + ':'
               +ByteToStr(RTCgetMinute : 2 : '0') + ':'
               +ByteToStr(RTCgetSecond : 2 : '0') + 'h/ A= '
               +ByteToStr(A1:2:0)+' /'+ByteToStr(A2:2:0);
               //+ByteToStr(A3:2:0);

        text2:= 'T1=' + ByteToStr(PT1Hour:2:'0')+':' +
                        ByteToStr(PT1Minute:2:'0') + 'h/ ' +
                'T2=' + ByteToStr(PT2Hour:2:'0')+':' +
                        ByteToStr(PT2Minute:2:'0') + 'h';

        if PTFlag then                  //pumpen l�uft
           CalcBx;                      //aktuelles A berechnen
           if Led2 then
              if PT1 then
                 text3:= 'Flush   '+ IntToStr((P_Sekunde):5:0)+'s '
                                   + 'A1=' + ByteToStr(BX:2:0);
              endif;
              if PT2 then
                 text3:= 'Flush   '+ IntToStr((P_Sekunde):5:0)+'s '
                                   + 'A2=' + ByteToStr(BX:2:0);
              endif;
           endif;

           if Led3 then
              if PT1 then
                 text3:= 'Reflush '+ IntToStr((P_Sekunde):5:0)+'s '
                                   + 'A1=' + ByteToStr(BX:2:0);
              endif;
              if PT2 then
                 text3:= 'Reflush '+ IntToStr((P_Sekunde):5:0)+'s '
                                   + 'A2=' + ByteToStr(BX:2:0);
              endif;
           endif;

           if (not Led2) and (not Led3) then
              if PT1 then
                 text3:= 'Break   '+ IntToStr((P_Sekunde):5:0)+'s '
                                   + 'A1=' + ByteToStr(BX:2:0);
              endif;
              if PT2 then
                 text3:= 'Break   '+ IntToStr((P_Sekunde):5:0)+'s '
                                   + 'A2=' + ByteToStr(BX:2:0);
              endif;
           endif;

        else

           if AirFlag then
              GetADg;
              text3:= 'Air ON / P: '+FloatToStr(Uin3:5:3:' ')+'bar';
           else
              text3:= 'Air OFF             ';
           endif;
           
           if SLPFlag and SSBRmin then
              text3:= 'SLP ON           SLP';
           endif;

           if SLPFlag and not SSBRmin then
              text3:= 'SLP OFF          SLP';
           endif;

           if SOFlag and AirFlag then
              text3:= 'Air ON            SO';
           endif;

           if SOFlag and not AirFlag then
              text3:= 'Air OFF           SO';
           endif;

        endif;

        //und noch Abfrage der Schwimmschalter
        //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
        //SS sieht Wasser = 1 (z.B. Beh�lter voll)

        //text4:='Stag_Sekunde: '+ LongToStr((Stag_Sekunde):6:0);

        if SSBRmin then                 //Schwimmer BR abfragen
           text4:= 'BRmin +  / ';
        else
           text4:= 'BRmin -  / ';
        endif;

        if SSKlarmax then               //Schwimmer Klarwasser abfragen
           text4:= text4 + 'CLmax +   ';
        else
           text4:= text4 + 'CLmax -   ';
        endif;

        ShowText;

     endif;
  endif;
end;

{--------------------------------------------------------------}

{procedure GetDutyviaDilSwitch;
begin
  S1:=(not HiNibble(pinA)) and %00000111;
  case S1 of
   0: Duty:=102; mem:=6; | //Tastverh�ltnis = 40%
   1: Duty:=127; mem:=1; | //50%
   2: Duty:=152; mem:=2; | //60%
   3: Duty:=178; mem:=3; | //70%
   4: Duty:=203; mem:=4; | //80%
   5: Duty:=228; mem:=5; | //90%
   6: Duty:=250; mem:=6; | //100%
   7: Duty:=250; mem:=6; | //100%
   else Duty:=250; mem:=6;
  endcase;
end; }

{--------------------------------------------------------------}

procedure GetDuty (Mem:byte);
begin
  case Mem of
   0: DutyFlush:=127;     //50%
      DutyReflush:=250; |
   1: DutyFlush:=102;     //40 und 100%
      DutyReflush:=250; |
   2: DutyFlush:=127;     //50 und 100%
      DutyReflush:=250; |
   3: DutyFlush:=152;     //60 und 100%
      DutyReflush:=250; |
   4: DutyFlush:=203;     //80 und 100%
      DutyReflush:=250; |
   5: DutyFlush:=225;     //90 und 100%
      DutyReflush:=250; |
   6: DutyFlush:=250;     //100 und 100%
      DutyReflush:=250; |
   else DutyFlush:=102; DutyReflush:=250; mem:=1;
  endcase;
end;

{--------------------------------------------------------------}

procedure SLPOn;
begin
  SLP:=true;
  PWMport1:=0;                          //pumpen abstellen, weil die schlammpumpe nicht
  PWMport2:=0;                          //gleichzeitig mit den pwm pumpen l�uft
  Led2:=true;
  Led3:=true;
end;

{--------------------------------------------------------------}

procedure SLPOff;
begin
  SLP:=false;
  Led2:=false;
  Led3:=false;
end;

{--------------------------------------------------------------}

procedure FanOn;
begin
  GeblaeseNetz:=true;
  if Out then                           //true: FreeOut=24V parallel air
     FreeOut:=true;
  else
     if OutFlag then                    //wenn OutFlag=true bekommt das Filter +24V
        FreeOut:=true;
     else
        FreeOut:=false;
     endif;
  endif;
  AirFlag:=true;
  Led4:=true;
end;

{--------------------------------------------------------------}

procedure FanOff;
begin
  GeblaeseNetz:=false;
  if Out then
     FreeOut:=false;
  endif;
  AirFlag:=false;
  Led4:=false;
end;

{--------------------------------------------------------------}

procedure Flush;
begin

  //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
  //SS sieht Wasser = 1 (z.B. Beh�lter voll)

  if SSBRmin and not SSKlarmax then     //BR muss voll und Klarmax nicht voll sein
     PWMport1:=DutyFlush;               //PWMport1 = Saugpumpe
     Led2:=true;
     PWMPort2:=0;                       //PWMport2 = Rueckspuelpumpe
     Led3:=false;
     SLP:=false;                        //Schlammpumpe l�uft nicht
     Stag_Sekunde:=0;                   //Stagnationsz�hler immer dann zur�cksetzen
                                        //wenn gesp�lt wird
  else                                  //entweder ist BR leer oder Klarmax voll
     PWMport1:=0;
     Led2:=false;
     PWMPort2:=0;
     Led3:=false;
     SLP:=false;
  endif;
end;

{--------------------------------------------------------------}

procedure Reflush;
begin
  if SSBRmin and not SSKlarmax then     //BR muss voll und Klarmax nicht voll sein
     PWMport1:=0;
     Led2:=false;
     PWMport2:=DutyReflush;
     Led3:=true;
     SLP:=false;                        //Schlammpumpe l�uft nicht
     Stag_Sekunde:=0;                   //Stagnationsz�hler immer dann zur�cksetzen
                                        //wenn r�ckgesp�lt wird
  else                                  //entweder ist BR leer oder Klarmax voll
     PWMport1:=0;
     Led3:=false;
     PWMport2:=0;
     Led2:=false;
     SLP:=false;
  endif;
end;

{--------------------------------------------------------------}

procedure Pause;                        //pumpen abstellen
begin
  PWMport1:=0;
  Led3:=false;
  PWMport2:=0;
  Led2:=false;
end;

{---------------------------------------------------------------}

procedure CalcTiming (A:word);   //Berechnung der Zeitsegmente beim pumpen
begin

  if (P_Sekunde>=0) and (P_Sekunde<=300) then    //5 min nichts machen
     Pause;
     FanOff;
  endif;

  for i:=0 to (A-1) do                  //A ist 1,2,3 oder 4x
      T1:=300+(i*919);                  //900s flush, 2s Pause mit 600s Offset
      //T1:=60+(i*109);
      T2:=T1+900;                       //15s reflush, 2s Pause
      //T2:=T1+90;
      T3:=T2+2;                         //insgesamt 919s
      T4:=(T3+15);                      //und das Ganze Ax
      T5:=(T4+2);

      {
      T1= 300 - T2=1500   //900s pumpen =15min   1.Durchlauf
      T2=1500 - T3=1502   //2s Pause
      T3=1502 - T4=1517   //15s r�cksp�len
      T4=1517 - T5=1519   //2s Pause
      ---------------------------------------
      T1=1519 - T2=2419   //900s pumpen =15min  2.Durchlauf
      T2=2419 - T3=2421   //2s Pause
      T3=2421 - T4=2436   //15s r�cksp�len
      T4=2436 - T5=2438   //2s Pause
      ---------------------------------------
      T1=2438 - T2=3338   //900s pumpen =15min  3.Durchlauf
      T2=3338 - T3=3340   //2s Pause
      T3=3340 - T4=3355   //15s r�cksp�len
      T4=3355 - T5=3357   //2s Pause
      ---------------------------------------
      T1=3357 - T2=4257   //900s pumpen =15min  4.Durchlauf
      T2=4257 - T3=4259   //2s Pause
      T3=4259 - T4=4274   //15s r�cksp�len
      T4=4274 - T5=4276   //2s Pause
      }

      if (P_Sekunde>T1) and (P_Sekunde<=T2) then
         Flush;
         FanOff;
      endif;

      if (P_Sekunde>T2) and (P_Sekunde<=T3) then
         Pause;
         FanOff;
      endif;

      if (P_Sekunde>T3) and (P_Sekunde<=T4) then
         Reflush;
         if (i=(A-1)) then              //beim letzten A zyklus mit bel�ften
            FanOn;                      //schon w�hrend des R�cksp�lens beginnen
         else
            FanOff;
         endif;
      endif;

      if (P_Sekunde>T4) and (P_Sekunde<=T5) then
         Pause;
         if (i=(A-1)) then
            FanOn;
         else
            FanOff;
         endif;
      endif;

  endfor;

  T6:=300+(A*919);                      //nach Abschluss des pumpens noch 600s bel�ften
  //T6:=60+(A*109);
  T7:=T6+600;                           //mit a=4 ist T6=4276, T7=4876
  //T7:=T6+60;

  if (P_Sekunde>T6) and (P_Sekunde<=T7) then
     Pause;
     FanOn;                             //noch 600s bel�ften
  endif;

  if (P_Sekunde > T7) then              //Ende des Pumpens
     Pause;
     FanOff;
     G_Sekunde:=0;                      //damit wird bel�ftet wenn das Pumpen zu Ende ist
     P_Sekunde:=0;                      //hier auch Sekunde zur�cksetzen, und
     inc(B);                            //wichtig ! B um 1 erh�hen, damit die Routine
  endif;                                //mehrfach benutzt werden kann

end;

{--------------------------------------------------------------}

procedure Timing;
begin
  if PT1 then
     Ax:=A1;
  endif;
  if PT2 then
     Ax:=A2;
  endif;
  if PT3 then
     Ax:=A3;
  endif;

  if PTFlag then                        //nur pumpen wenn ptflag=true
     case Ax of
     1..4: CalcTiming(word(Ax));
            if B>0 then
               PTFlag:=false;
            endif;
         |
       5 :  if (B=0)  then  CalcTiming(4);  endif;
            if (B=1)  then  CalcTiming(1);  endif;
            if (B>1)  then  PTFlag:=false;  endif;
         |
       6 :  if (B=0)  then  CalcTiming(4);  endif;
            if (B=1)  then  CalcTiming(2);  endif;
            if (B>1)  then  PTFlag:=false;  endif;
         |
       7 :  if (B=0)  then  CalcTiming(4);  endif;
            if (B=1)  then  CalcTiming(3);  endif;
            if (B>1)  then  PTFlag:=false;  endif;
         |
       8 :  if B<=1   then  CalcTiming(4);  endif;
            if (B>1)  then  PTFlag:=false;  endif;
         |
       9 :  if B<=1   then  CalcTiming(4);  endif;
            if (B=2)  then  CalcTiming(1);  endif;
            if (B>2)  then  PTFlag:=false;  endif;
         |
      10 :  if B<=1   then  CalcTiming(4);  endif;
            if (B=2)  then  CalcTiming(2);  endif;
            if (B>2)  then  PTFlag:=false;  endif;
         |
      11 :  if B<=1   then  CalcTiming(4);  endif;
            if (B=2)  then  CalcTiming(3);  endif;
            if (B>2)  then  PTFlag:=false;  endif;
         |
      12 :  if B<=2   then  CalcTiming(4);  endif;
            if (B>2)  then  PTFlag:=false;  endif;
         |
      13 :  if B<=2   then  CalcTiming(4);  endif;
            if (B=3)  then  CalcTiming(1);  endif;
            if (B>3)  then  PTFlag:=false;  endif;
         |
      14 :  if B<=2   then  CalcTiming(4);  endif;
            if (B=3)  then  CalcTiming(2);  endif;
            if (B>3)  then  PTFlag:=false;  endif;
         |
      15 :  if B<=2   then  CalcTiming(4);  endif;
            if (B=3)  then  CalcTiming(3);  endif;
            if (B>3)  then  PTFlag:=false;  endif;
         |
      16 :  if B<=3   then  CalcTiming(4);  endif;
            if (B>3)  then  PTFlag:=false;  endif;
         |
      17 :  if B<=3   then  CalcTiming(4);  endif;
            if (B=4)  then  CalcTiming(1);  endif;
            if (B>4)  then  PTFlag:=false;  endif;
         |
      18 :  if B<=3   then  CalcTiming(4);  endif;
            if (B=4)  then  CalcTiming(2);  endif;
            if (B>4)  then  PTFlag:=false;  endif;
         |
      19 :  if B<=3   then  CalcTiming(4);  endif;
            if (B=4)  then  CalcTiming(3);  endif;
            if (B>4)  then  PTFlag:=false;  endif;
         |
      20 :  if B<=4   then  CalcTiming(4);  endif;
            if (B>4)  then  PTFlag:=false;  endif;
         |
      21 :  if B<=4   then  CalcTiming(4);  endif;
            if (B=5)  then  CalcTiming(1);  endif;
            if (B>5)  then  PTFlag:=false;  endif;
         |
      22 :  if B<=4   then  CalcTiming(4);  endif;
            if (B=5)  then  CalcTiming(2);  endif;
            if (B>5)  then  PTFlag:=false;  endif;
         |
      23 :  if B<=4   then  CalcTiming(4);  endif;
            if (B=5)  then  CalcTiming(3);  endif;
            if (B>5)  then  PTFlag:=false;  endif;
         |
      24 :  if B<=5   then  CalcTiming(4);  endif;
            if (B>5)  then  PTFlag:=false;  endif;
         |
      25 :  if B<=5   then  CalcTiming(4);  endif;
            if (B=6)  then  CalcTiming(1);  endif;
            if (B>6)  then  PTFlag:=false;  endif;
         |
      26 :  if B<=5   then  CalcTiming(4);  endif;
            if (B=6)  then  CalcTiming(2);  endif;
            if (B>6)  then  PTFlag:=false;  endif;
         |
      27 :  if B<=5   then  CalcTiming(4);  endif;
            if (B=6)  then  CalcTiming(3);  endif;
            if (B>6)  then  PTFlag:=false;  endif;
         |
      28 :  if B<=6   then  CalcTiming(4);  endif;
            if (B>6)  then  PTFlag:=false;  endif;
         |
      29 :  if B<=6   then  CalcTiming(4);  endif;
            if (B=7)  then  CalcTiming(1);  endif;
            if (B>7)  then  PTFlag:=false;  endif;
         |
      30 :  if B<=6   then  CalcTiming(4);  endif;
            if (B=7)  then  CalcTiming(2);  endif;
            if (B>7)  then  PTFlag:=false;  endif;
         |
      31 :  if B<=6   then  CalcTiming(4);  endif;
            if (B=7)  then  CalcTiming(3);  endif;
            if (B>7)  then  PTFlag:=false;  endif;
         |
      32 :  if B<=7   then  CalcTiming(4);  endif;
            if (B>7)  then  PTFlag:=false;  endif;
         |
      33 :  if B<=7   then  CalcTiming(4);  endif;
            if (B=8)  then  CalcTiming(1);  endif;
            if (B>8)  then  PTFlag:=false;  endif;
         |
      34 :  if B<=7   then  CalcTiming(4);  endif;
            if (B=8)  then  CalcTiming(2);  endif;
            if (B>8)  then  PTFlag:=false;  endif;
         |
      35 :  if B<=7   then  CalcTiming(4);  endif;
            if (B=8)  then  CalcTiming(3);  endif;
            if (B>8)  then  PTFlag:=false;  endif;
         |
      36 :  if B<=8   then  CalcTiming(4);  endif;
            if (B>8)  then  PTFlag:=false;  endif;
         |
      37 :  if B<=8   then  CalcTiming(4);  endif;
            if (B=9)  then  CalcTiming(1);  endif;
            if (B>9)  then  PTFlag:=false;  endif;
         |
      38 :  if B<=8   then  CalcTiming(4);  endif;
            if (B=9)  then  CalcTiming(2);  endif;
            if (B>9)  then  PTFlag:=false;  endif;
         |
      39 :  if B<=8   then  CalcTiming(4);  endif;
            if (B=9)  then  CalcTiming(3);  endif;
            if (B>9)  then  PTFlag:=false;  endif;
         |
      40 :  if B<=9   then  CalcTiming(4);  endif;
            if (B>9)  then  PTFlag:=false;  endif;
         |
      41 :  if B<=9   then  CalcTiming(4);  endif;
            if (B=10) then  CalcTiming(1);  endif;
            if (B>10) then  PTFlag:=false;  endif;
         |
      42 :  if B<=9   then  CalcTiming(4);  endif;
            if (B=10) then  CalcTiming(2);  endif;
            if (B>10) then  PTFlag:=false;  endif;
         |
      43 :  if B<=9   then  CalcTiming(4);  endif;
            if (B=10) then  CalcTiming(3);  endif;
            if (B>10) then  PTFlag:=false;  endif;
         |
      44 :  if B<=10  then  CalcTiming(4);  endif;
            if (B>10) then  PTFlag:=false;  endif;
         |
      45 :  if B<=10  then  CalcTiming(4);  endif;
            if (B=11) then  CalcTiming(1);  endif;
            if (B>11) then  PTFlag:=false;  endif;
         |
     endcase;
  endif;
end;

{--------------------------------------------------------------}

procedure Pumpen;
begin
  GetDuty(Mem);                         //Tastverh�ltnis ermitteln

  if not Testflag then                  //wenn kein test weiter mit pumpen
     Timing;
  endif;

end;

{--------------------------------------------------------------}

procedure SoSlp;
begin

  if SOFlag then                        //SO, pumpen aussetzen
     PTFlag:=false;
     Pause;                             //Pumpen aus
     SLP:=false;                        //SLP aus
     if InctolimWrap(SOTimer, c_24h, 0) then
        SOFlag:=false;                  //SO-Flag zur�cksetzen
     endif;
  endif;

  //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
  //SS sieht Wasser = 1 (z.B. Beh�lter voll)

  if SLPFlag then                       //wenn Schlammpumpe arbeiten soll
     PTFlag:=false;                     //pumpen abstellen
     if SSBRmin then                    //der Bioreaktor hat genug Wasser
        SLPOn;                          //Schlammpumpe ein
        FanOn;                          //L�ftung ein
        if InctolimWrap(SLPTimer, (SLPminute*60), 0) then
           SLPOff;                      //Schlammpumpe ausstellen
           SLPFlag:=false;              //Schlammpumpenflag zur�ckstellen
        endif;
     else
        SLPOff;                         //Schlammpumpe aus, da im Bioreaktor kein
     endif;                             //Wasser mehr ist, Bel�ftung l�uft jetzt
  endif;                                //normal weiter

end;

{--------------------------------------------------------------}

procedure PumpControl;                  //pump-zyklus
begin

  if (SOweek=0) then
     SOFlag:=false;
  else
     SOzeit:=longword(SOweek)*604800;   //1d*7=1wo
  endif;

  //case SOweek of
  // 0 : Optionzeit:=c_achtWochen;        //im mode 0 wird immer gepumpt, es gibt
  //     SOFlag:=false;            |      //keine pause nach 1,2 oder 3 Wo
  // 1 : Optionzeit:=c_eineWoche;  |      //nach einer Woche f�r 24h pumpen aussetzen
  // 2 : Optionzeit:=c_zweiWochen; |      //nach 2 Wochen f�r 24h pumpen aussetzen
  // 3 : Optionzeit:=c_dreiWochen; |      //nach 3 Wochen f�r 24h pumpen aussetzen
  // 4 : Optionzeit:=c_vierWochen; |      //nach 4 Wochen f�r 24h pumpen aussetzen
  // else Pumpen;
  //endcase;

  if (SLPday=0) then
     SLPFlag:=false;
  else
     SLPZeit:=longword(SLPday)*86400;   //86400=1d * 60min * 60s
  endif;

  //case SLPweek of
  // 0 : SLPzeit:=c_achtWochen;
  //     SLPFlag:=false;           |
  // 1 : SLPzeit:=c_eineWoche;     |
  // 2 : SLPzeit:=c_zweiWochen;    |
  // 3 : SLPzeit:=c_dreiWochen;    |
  // 4 : SLPzeit:=c_vierWochen;    |
  // 5 : SLPzeit:=c_fuenfWochen;   |
  // 6 : SLPzeit:=c_sechsWochen;   |
  // 7 : SLPzeit:=c_siebenWochen;  |
  // 8 : SLPzeit:=c_achtWochen;    |
  // else Pumpen;
  //endcase;

  if SLPFlag then                       //Schlammpumpe hat priorit�t
     SOFlag:=false;                     //daher SO ausschalten
  endif;

  if Testflag then                      //Test gew�hlt
     Pumpen;                            //zum pumpen
     return;                            //und raus
  endif;

  if SOFlag or SLPFlag then             //wenn kein test oder normalbetrieb
     SoSLP;                             //und wenn SO oder SLP laufen sollen
  else
     Pumpen;                            //ansonsten zum pumpen
  endif;

end;

{--------------------------------------------------------------}

procedure FanControl;                   //L�ftersteuerung
begin

  G_Zeit1:=AirOn*60;
  G_Zeit2:=(AirOn*60)+(AirOff*60);      //z.B. 15 * 60 = 300

  if PTFlag or CleanFlag or Testflag then //beim pumpen, reinigen, oder test
     nop;                                 //�berspringen weil da bel�ftung
  else                                    //individuell gesteuert wird
     if (G_Sekunde>=0) and (G_Sekunde < G_Zeit1) then
        FanOn;
     endif;

     if (G_Sekunde>=G_Zeit1) and (G_Sekunde < G_Zeit2) then
        if SLPFlag and SSBRmin then     //wenn Schlammpumpe l�uft und BR voll
           FanOn;                       //mu� auch die Bel�ftung laufen
        else
           FanOff;                      //wenn BR leer mu� normal bel�ftet werden
        endif;
     endif;
  endif;

end;

{--------------------------------------------------------------}

procedure CleanControl;                 //Reinigungszyklus
begin
  case mode of

   123 : if (Counter=0) then            //Step3 - Automatisches R�cksp�len
            case mem of
               1 : Servicecounter:=120; Memcount:=120; |
               2 : Servicecounter:=60;  Memcount:=60;  |
               3 : Servicecounter:=40;  Memcount:=40;  |
               4 : Servicecounter:=30;  Memcount:=30;  |
               5 : Servicecounter:=24;  Memcount:=24;  |
               6 : Servicecounter:=20;  Memcount:=20;  |
            endcase;
         endif;

         inc (Counter);
         dec (Servicecounter);          //Achtung Servicecounter z�hlt runter !

         if (Counter>=0) and (Counter<=MemCount) then
            PWMport1:=0;
            Led2:=false;
            PWMport2:=DutyReflush;      //R�cksp�lpumpe starten ohne
            Led3:=true;                 //Schwimmschalterabfrage
            SLP:=false;
            FanOff;
         endif;

         if (Counter>Memcount) then
            Pause;                      //Pumpen abstellen
            mode:=124;                  //reflush ende und zum n�chsten men�
         endif;
       |
   125 : inc (Counter);                 //Step5 - Reinigungszyklus
         dec (Servicecounter);          //Achtung Servicecounter z�hlt runter !

         if (Counter>=0) and (Counter<=10) then //1 Zyklus 10s R�cksp�len
            PWMport1:=0;                //Flush aus
            Led2:=false;
            PWMport2:=DutyReflush;      //Reflush ohne Schwimmerabfrage
            Led3:=true;                 //also praktisch zwangsr�cksp�len
            SLP:=false;                 //Schlammpumpe aus
            FanOff;                     //Bel�ftung aus
         endif;

         for i:=0 to 4 do               //5 Zyklen mit 10min=600s warten
             T1:=10+(i*603);            //mit 10min=600s warten und 3s R�cksp�len
                                        //insgesamt 603
             T2:=T1+600;                //600s=10min warten
             T3:=T2+3;                  //3s R�cksp�len

             if (Counter>T1) and (Counter<=T2) then
                Pause;
                SLP:=false;
                FanOff;
             endif;

             if (Counter>T2) and (Counter<=T3) then
                PWMport1:=0;            //Reflush ohne Schwimmerabfrage
                Led2:=false;            //also praktisch zwangsr�cksp�len
                PWMport2:=DutyReflush;
                Led3:=true;
                SLP:=false;
                FanOff;
             endif;

         endfor;

         if (Counter>3025) and (Counter<=3325) then    //300s=5min warten
            Pause;
            SLP:=false;
            FanOff;
         endif;

         if (Counter>3325) and (Counter<=3335) then    //1 Zyklus mit 10s R�cksp�len
            PWMport1:=0;
            Led2:=false;
            PWMport2:=DutyReflush;
            Led3:=true;
            SLP:=false;
            FanOff;
         endif;

         for j:=0 to 5 do               //6 Zyklen mit 60s Saugen und 10s R�cksp�len
             T4:=3335+(j*70);           //60s+10s=70s
             T5:=T4+60;                 //60s flush
             T6:=T5+10;                 //10s reflush

             if (Counter>T4) and (Counter<=T5) then
                PWMport1:=DutyFlush;    //flush ohne Schwimmerabfrage
                Led2:=true;             //also praktisch zwangssp�len
                PWMPort2:=0;
                Led3:=false;
                SLP:=false;
                FanOff;
             endif;

             if (Counter>T5) and (Counter<=T6) then
                PWMport1:=0;            //Reflush ohne Schwimmerabfrage
                Led2:=false;            //also praktisch zwangsr�cksp�len
                PWMport2:=DutyReflush;
                Led3:=true;
                FanOff;
             endif;
         endfor;
                                        //nach 3755s ist der
         if (Counter>3755) then         //Reinigungszyklus beendet
            FanOff;
            Pause;
            SLPOff;
            FanOff;
            mode:=126;                  //und es geht zum Klarwassersp�len
         endif;
       |
   127 : if (Counter=0) then            //Step7 - Klarwassersp�lung
            case mem of
              1 : Servicecounter:=410; MemCount:=60; |  //60+350
              2 : Servicecounter:=380; MemCount:=30; |  //30+350
              3 : Servicecounter:=370; MemCount:=20; |  //20+350
              4 : Servicecounter:=365; MemCount:=15; |  //15+350
              5 : Servicecounter:=362; MemCount:=12; |  //12+350
              6 : Servicecounter:=360; MemCount:=10; |  //10+350
            endcase;
         endif;

         inc (Counter);
         dec (Servicecounter);

         if (Counter>=0) and (Counter<=MemCount) then
            PWMport1:=0;                //R�cksp�lpumpe starten ohne
            Led2:=false;                //Schwimmschalterabfrage
            PWMport2:=DutyReflush;
            Led3:=true;
            SLP:=false;
            FanOn;                      //Bel�ftung ein
         endif;

         for i:=0 to 4 do               //5 Zyklen mit 60s Saugen und 10s R�cksp�len
             T1:=MemCount+(i*70);       //60s+10s=70s
             T2:=T1+60;                 //60s flush
             T3:=T2+10;                 //10s reflush, insgesamt 5 x 70=350s

             if (Counter>T1) and (Counter<=T2) then
                PWMport1:=DutyFlush;    //flush ohne Schwimmerabfrage
                Led2:=true;             //also praktisch zwangssp�len
                PWMPort2:=0;
                Led3:=false;
                SLP:=false;
                FanOn;
             endif;

             if (Counter>T2) and (Counter<=T3) then
                PWMport1:=0;
                Led2:=false;
                PWMport2:=DutyReflush;  //Reflush ohne Schwimmerabfrage
                Led3:=true;             //also praktisch zwangsr�cksp�len
                FanOn;
             endif;

         endfor;

         if (Servicecounter>MemCount+350) then
            Pause;
            SLP:=false;
            FanOff;
            CleanFlag:=false;           //Reinigung zu ende
            mode:=1;                    //Clearwaterflushing zu ende
            MenueFlag:=false;           //zur�ck zum automatic-modus
         endif;
       |
  endcase;
end;

{--------------------------------------------------------------}

procedure TestControl;                  //Test der Komponenten
begin
  case mode of
    45 : inc (Counter);                 //Test Bel�ftung
         Servicecounter:=(Testdauer-Counter);
         if (Counter>=0) and (Counter<=Testdauer) then
            FanOn;                      //Bel�ftung an
            Pause;                      //beide Pumpen aus
            SLPOff;                     //Schlammpumpe aus
         endif;

         if (Counter>Testdauer) then
            FanOff;
            Testflag:=false;            //Test beendet
            mode:=1;                    //und wieder aus dem men�-mode raus
            Menueflag:=false;
         endif;
       |
    46 : inc (Counter);                 //Test flush
         Servicecounter:=(Testdauer-Counter);
         if (Counter>=0) and (Counter<=Testdauer) then
            SLPOff;
            Flush;
            FanOff;
         endif;

         if (Counter>Testdauer) then
            Pause;
            FanOff;
            SLPOff;
            Testflag:=false;
            mode:=1;
            Menueflag:=false;
         endif;
       |
    47 : inc (Counter);                 //Test backflush
         Servicecounter:=(Testdauer-Counter);
         if (Counter>0) and (Counter<=Testdauer) then
            SLPOff;
            Reflush;
            FanOff;
         endif;

         if (Counter>Testdauer) then
            Pause;
            FanOff;
            SLPOff;
            Testflag:=false;
            mode:=1;
            MenueFlag:=false;
         endif;
       |
    48 : inc (Counter);                 //Test SLP
         Servicecounter:=(Testdauer-Counter);
         if (Counter>0) and (Counter<=Testdauer) then
            Pause;                      //pumpen abschalten

            //SS sieht kein Wasser = 0 (z.B. Beh�lter leer)
            //SS sieht Wasser = 1 (z.B. Beh�lter voll)

            if SSBRmin then             //BR muss voll sein
               SLPOn;                   //dann kann SLP laufen
               FanOff;                  //Bel�ftung dabei aus
            else
               SLPOff;
               FanOff;
            endif;
         endif;

         if (Counter>Testdauer) then
            Pause;
            FanOff;
            SLPOff;
            Testflag:=false;
            mode:=1;
            MenueFlag:=false;
         endif;
       |
  endcase;
end;

{--------------------------------------------------------------}

procedure OutControl;                   //frei programmierbarer 24V-Ausgang
begin
  if Out then                           //if true: 24V f�r air
     OutTimer:=0;                       //OutTimer jede Sekunde zur�cksetzen
  else
     if OutFlag then                    //if true: jetzt 24v f�r das filter
        FreeOut:=true;                  //24V-Ausgang setzen
        if IncTolimWrap(OutTimer, OutSekunde, 0) then
           FreeOut:=false;              //24V f�r Filter abschalten
           OutFlag:=false;              //und Filterroutine beenden
        endif;
     endif;
  endif;
end;

{--------------------------------------------------------------}

procedure ClearwaterControl;            //was machen, wenn Wasser 3 Wochen im
begin                                   //Klarwasserbeh�lter gestanden hat ohne
  if StagFlag then                      //dass gesp�lt oder r�ckgesp�lt wurde
     mode:=220;                         //und Menu 220 aufrufen (StagText)
  endif;

  if ClearwaterFlag then
     mode:=230;                         //men� 300 aufrufen (ClearwaterText)
  endif;

  if ADgFlag then
     mode:=240;
  endif;

  if StagFlag or ClearwaterFlag or ADgFlag then
     MenueFlag:=true;                   //menue freigeben

     Backlight:=blink;

     //if InctolimWrap(Piezocounter, 10, 0) then    //Piezo piepst alle 10s
     //   if S1_1 then                              //Piezo off mit S1 auf on
     //      Piezo:=true;
     //      mdelay(100);
     //      Piezo:=false;
     //      Piezocounter:=0;
     //   endif;
     //endif;

     if S1_1 then
        Piezo:=blink;
     else
        Piezo:=false;
     endif;
  endif;
end;

{--------------------------------------------------------------}

procedure SaveParms;
begin
  LastAction:=0;                        //normaler Spannungsausfall
  inc (Netzaus);                        //das allgemeine Netzaus
  if Led2 or Led3 then                  //spannungsausfall bei flush oder reflush
     LastAction:=1;
     inc (Netzaus1);                    //das netzaus w�hrend flush
  endif;
  if Led4 then                          //spannungsausfall bei bel�ftung
     LastAction:=2;
     inc (Netzaus2);                    //das netzaus w�hrend air
  endif;

  //Strobe:=true;
  //das Speichern der folgenden 42 Parameter dauert <100ms !
  //gemessen mit Hameg-Scope

  EP_StartFlag:=55;                     //Stromausfall
  EP_A:=A;                              //A speichern
  EP_A1:=A1;                            //A1 speichern
  EP_A2:=A2;                            //A2 speichern
  EP_A3:=A3;                            //A3 speichern
  EP_B:=B;                              //B speichern
  EP_Mem:=Mem;                          //modul speichern
  EP_SOweek:=SOweek;                    //Option speichern
  EP_SOweekCopy:=SOweek;
  EP_Netzaus:=Netzaus;                  //Anzahl Netzausf�lle auch sichern
  EP_Netzaus1:=Netzaus1;
  EP_Netzaus2:=Netzaus2;
  EP_Language:=Language;
  EP_GSekunde:=G_Sekunde;
  EP_PSekunde:=P_Sekunde;
  EP_SOSekunde:=SO_Sekunde;
  EP_SSekunde:=S_Sekunde;
  EP_OSekunde:=O_Sekunde;
  EP_OutSekunde:=OutSekunde;
  EP_OutTag:=OutTag;
  EP_Out:=Out;
  EP_SOzeit:=SOzeit;
  EP_SLPzeit:=SLPzeit;
  EP_Hour:=RTCgetHour;
  EP_Minute:=RTCgetMinute;
  EP_Second:=RTCgetSecond;
  EP_AirOn:=AirOn;
  EP_AirOff:=AirOff;
  EP_PT1Hour:=PT1Hour;
  EP_PT1Minute:=PT1Minute;
  EP_PT2Hour:=PT2Hour;
  EP_PT2Minute:=PT2Minute;
  EP_PT3Hour:=PT3Hour;
  EP_PT3Minute:=PT3Minute;
  EP_PTFlag:=PTFlag;
  EP_PT1:=PT1;
  EP_PT2:=PT2;
  EP_PT3:=PT3;
  EP_SLPday:=SLPday;
  EP_SLPdayCopy:=SLPdayCopy;
  EP_SLPminute:=SLPminute;
  EP_SLPminuteCopy:=SLPminuteCopy;
  EP_StagSekunde:=Stag_Sekunde;
  EP_StagFlag:=StagFlag;
  EP_ClearwaterFlag:=ClearwaterFlag;

  SpeicherFlag:=true;                   //alles ins eeprom gespeichert
                                        //daher speicherflag wieder zur�cksetzen
  //Strobe:=false;

end;

{--------------------------------------------------------------}

procedure CheckReset;                   //jede 1s auf reset pr�fen
begin
  if (Enter=false) and (Next=false) then  //keine Tasten, alle 1s abfragen
     inc(Tastenzeit);                   //tastenzeit erh�hen
     if (Tastenzeit=5) then             //wenn taste 5s lang gedr�ckt dann
        Tastenzeit:=0;                  //wieder zur�cksetzen
        Backlight:=true;                //LCD-Display Backlight einschalten
        Flag:=false;
        Led1:=true;                     //Reset
        Led2:=true;
        Led3:=true;
        Led4:=true;
        GeblaeseNetz:=false;            //gebl�se aus
        FreeOut:=false;                 //24V-Out aus
        SLP:=false;                     //Rel2 abschalten
        SLPFlag:=false;                 //Schlammpume soll nicht laufen
        PWMport1:=0;                    //saugpumpe u. r�cksp�lpumpe aus
        PWMport2:=0;
        EP_StartFlag:=$FF;              //wieder in urzustand versetzen
        repeat
          ResetText;                    //system start text
          mdelay(1000);
          Entertaste;
          if TickEnter then
             TickEnter:=false;          //TickEnter u. TickNext zur�cksetzen
             TickNext:=false;
             Flag:=true;
          endif;
        until (Flag);
                                        //nach reset defaults setzen
        Backlight:=true;                //LCD-Display Backlight einschalten
        Laden:=false;
        ResAkkuflag_Sekunde:=0;
        Piezocounter:=0;
        A:=4;
        A1:=4;
        A1Copy:=A1;
        A2:=4;
        A2Copy:=A2;
        A3:=4;
        A3Copy:=A3;
        B:=0;
        Mem:=1;
        TC:=0;
        SOweek:=0;
        SOweekCopy:=SOweek;
        Serviceflag:=false;
        Testflag:=false;
        Cleanflag:=false;
        Netzaus:=0;
        Netzaus1:=0;
        Netzaus2:=0;
        Language:=true;
        G_Sekunde:=0;
        P_Sekunde:=0;
        SO_Sekunde:=0;
        S_Sekunde:=0;
        Lade_Sekunde:=0;
        ADg_Sekunde:=0;
        ADg_Counter:=0;
        ADgFlag:=false;
        SOzeit:=0;
        SLPzeit:=0;
        Stag_Sekunde:=0;
        StagFlag:=false;
        ClearwaterFlag:=false;
        AirOn:=5;
        AirOff:=10;
        PT1Hour:=3;
        PT1Minute:=0;
        PT2Hour:=16;
        PT2Minute:=0;
        PT3Hour:=18;
        PT3Minute:=0;
        PTFlag:=false;                  //echtzeitabh�ngigen automaticstart zulassen
        PT1:=false;
        PT2:=false;
        PT3:=false;
        SOFlag:=false;
        SLPday:=0;
        SLPdayCopy:=0;
        SLPminute:=15;
        SLPminuteCopy:=15;
        RTCsetHour(12);
        RTCsetMinute(00);
        RTCsetSecond(0);
        mode:=1;
        MenueFlag:=false;
        SpeicherFlag:=false;            //speichern bei stomausfall zulassen
        SYSTEM_RESET;                   //system zur�cksetzen u. neustart
     endif;
  else
     Tastenzeit:=0;
  endif;
end;

{--------------------------------------------------------------}

procedure ADg_Check;                    //wird alle 10s aufgerufen
begin
  if GeblaeseNetz then                  //nur wenn Bel�ftung eingeschaltet ist
     GetADg;                            //erfolgt Pr�fung ob ADg Air im Bereich
     if (Uin3 >= -0.1) and (Uin3 < 0.02) then    //zwischen 0 und 0.02bar liegt, wenn
        if InctolimWrap(ADg_Counter, 3, 0) then  //nicht dann Drahtbruch od. kein Sensor
           ADgFlag:=true;               //nach ca. 30s Flag setzen (eine Art Mittelung)
        endif;
     else
        ADg_Counter:=0;
     endif;
  else
     ADg_Counter:=0;                    //Mittelungsz�hler zur�cksetzen
  endif;
end;

{--------------------------------------------------------------}

procedure AkkuLaden;                    //wird alle 10s aufgerufen
begin
  Get9V;
  if Uin4 < 9.6 then                    //Laden wenn Akku unter 9,6V
     Laden:=true;
     Led1:=true;
  else
     Laden:=false;                      //Laden einstellen, wenn Akku > 9,6
     Led1:=blink;
  endif;
end;

{--------------------------------------------------------------}

procedure PiezoWarnung;
begin
  if IsSysTimerZero(Sekundentimer) then           //1 Sekunde abgelaufen
     SetSysTimer(Sekundentimer, c_SekundenTimer); //Sekundentimer neu starten
     Led1:=not Led1;                              //Led blinkt sek�ndlich

     inc (Akkuzeit);                              //Akkuzeit ist die Zeit in s
     if not ClearwaterFlag then                   //in der das Ger�t auf Akku l�uft
        if Akkuzeit >= 7200 then                  //wenn das Ger�t l�nger als 2h
        //if Akkuzeit >= 300 then                 //zum Test auf 5 min
           ClearwaterFlag:=true;                  //auf Akku l�uft, wird nach
           EP_ClearwaterFlag:=ClearwaterFlag;     //der R�ckkehr aus dem Akkubetrieb
        endif;                                    //die Clearwateranzeige
     endif;                                       //aufgerufen, die quittiert werden muss

     if InctolimWrap(Piezocounter, 10, 0) then    //Piezo piepst alle 10s
        if S1_1 then                              //Piezo off mit S1 auf on
           Piezo:=true;
           mdelay(100);
           Piezo:=false;
           Piezocounter:=0;
        endif;
     endif;
  endif;
end;

{--------------------------------------------------------------}

procedure NachAkkubetrieb;
begin
  Akkuzeit:=0;                                    //die Zeit in s in der das Ger�t
                                                  //im Akubetrieb war, wieder
                                                  //zur�cksetzen
  if IsSysTimerZero(Sekundentimer) then           //1 Sekunde abgelaufen
     SetSysTimer(Sekundentimer, c_SekundenTimer); //Sekundentimer neu starten
     blink:=not blink;
     if not Laden then
        Led1:=blink;
     endif;

     Backlight:=blink;

     IncToLimWrap(G_Sekunde, G_Zeit2, 0);         //Gebl�se aktivieren, auch
                                                  //w�hrend der Wartezeit
     FanControl;                                  //auf die Best�tigung
                                                  //dass das Netz ausgefallen war
     if IncToLimWrap(Lade_Sekunde, 20, 0) then    //wenn akku, dann akku=true
        AkkuLaden;                                //alle 20s Akkuspanung pr�fen
     endif;                                       //und ggflls. laden

     //if IncToLimWrap(ResAkkuflag_Sekunde, 30, 0) then
     //   Akkuflag:=false;                          //Reset Akkuflag nach 30s
     //   mode:=1;                                  //raus aus dem Men�betrieb
     //   MenueFlag:=false;
     //endif;

     //if IncToLimWrap(Piezocounter, 3, 0) then     //Piezo piepst alle 2s
     //   if S1_1 then                              //Piezo off mit S1 auf on
     //      Piezo:=true;
     //      mdelay(100);
     //      Piezo:=false;
     //      Piezocounter:=0;
     //   endif;

     if S1_1 then
        Piezo:=blink;
     else
        Piezo:=false;
     endif;

  endif;
end;

{--------------------------------------------------------------}

procedure ExitTest;
begin
  Testflag:=false;                      //bei ExitTest alles ausschalten
  Pause;
  SLPOff;
  FanOff;
  mode:=1;
  MenueFlag:=false;
end;

{--------------------------------------------------------------}

procedure ExitCleaning;
begin
  Cleanflag:=false;                     //cleaning beendet
  mode:=1;                              //default mode
  MenueFlag:=false;                     //raus aus men�
end;

{--------------------------------------------------------------}

procedure UpdateKeys;
begin
  Entertaste;
  if TickEnter then
     TickEnter:=false;
     ExitTest;
  endif;

  Nexttaste;
  if TickNext then
     TickNext:= false;
     ExitTest;
  endif;
end;

{--------------------------------------------------------------}

procedure menue;                        //Men� start hier
begin
  MenueFlag:=true;                      //jetzt ist man im men�modus

  if Akkuflag or StagFlag or Clearwaterflag or ADgFlag then  //au�er bei diesen Flags
     nop;
  else
     Backlight:=true;                   //immer backlight vom lcd-Display an
  endif;

  case mode of
   1 : MembranText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Mem1Text;                     //membrananzahl abfragen
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          SOText;
       endif;
     |
   11: Mem1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;                     //nur im mode1 komme ich mit next
       //if TickNext then               //in mode2
       //   TickNext:= false;           //TickNext zur�cksetzen
       if (Next=false) then             //Taste gedr�ckt
          Click;
          mdelay(75);
          inc (mem);
          mdelay(75);
          if (mem>6) then
             mem:=1;
          endif;
       endif;
     |
   20: SOText;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          SOweekCopy:=SOweek;           //copy = original machen
          SO1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          SLPText;
       endif;
     |
   21: SO1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          SO2Text;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc(SOweekCopy);
          mdelay(75);
          if (SOweekCopy>5) then
             SOweekCopy:=0;
          endif;
       endif;
     |
   22: SO2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (SOweek=SOweekCopy) then   //nichts ge�ndert
             nop;
          else
             SOweek:=SOweekCopy;        //bei �nderung der option startet
             SO_Sekunde:=0;             //die wochenzeit immer von vorn
             SOTimer:=0;                //und Timer zur�cksetzen
          endif;
          mode:=1;
          MenueFlag:=false;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
   30: SLPText;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          SLP1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          ServText;
       endif;
     |
   31: SLP1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          SLP2Text;
       endif;

       if (Next=false) then
          Click;
          mdelay(75);
          inc (SLPdayCopy);
          mdelay(75);
          if SLPdayCopy>=60 then        //SLPday l�uft von 0 bis 60
             SLPdayCopy:=0;
          endif;
       endif;
     |
   32: SLP2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          SLPminute:=SLPminuteCopy;
          SLP3Text;
          //mode:=1;                    //startet die zugeh�rige Zeit bei 0
          //MenueFlag:=false;
       endif;

       if (Next=false) then
          Click;
          mdelay(75);
          inc (SLPminuteCopy);
          mdelay(75);
          if SLPminuteCopy>=61 then     //SLPminute l�uft von 1 bis 60
             SLPminuteCopy:=1;
          endif;
       endif;
     |
   33: SLP3Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (SLPday=SLPdayCopy) then   //nichts ge�ndert
             nop;
          else
             SLPday:=SLPdayCopy;        //bei �nderung von slpweek startet
             S_Sekunde:=0;              //die wochenzeit immer von vorn
             SLPTimer:=0;               //Timer zur�cksetzen
             SLPOff;                    //noch laufende Prozesse ausschalten
          endif;
          mode:=1;
          MenueFlag:=false;             //und raus aus dem menue
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
   40: ServText;                        //40 ist Testbetrieb
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Testflag:=true;               //test ist gew�nscht, aber noch kein test
          Pause;                        //ausgew�hlt, daher erstmal alles ausschalten
          FanOff;
          SLPOff;
          Serv1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          Testflag:=false;              //auf jeden Fall kein Test
          StatusText;
       endif;
     |
   41: Serv1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Testflag:=true;
          Counter:=0;
          Test1Text;                    //zum Test bel�ftung
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Serv2Text;
       endif;
     |
   42: Serv2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Testflag:=true;
          Counter:=0;
          Test2Text;                    //zum Test flush
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Serv3Text;
       endif;
     |
   43: Serv3Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Testflag:=true;
          Counter:=0;
          Test3Text;                    //zum Test reflush
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          Serv4Text;
       endif;
     |
   44: Serv4Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Testflag:=true;
          Counter:=0;
          Test4Text;                    //zum Test SLP
       endif;

       Nexttaste;                       //es wurde kein Test gew�hlt
       if TickNext then
          TickNext:= false;
          Testflag:=false;              //zur Sicherheit testflag=false
          mode:=1;
          MenueFlag:=false;
       endif;
     |
   45: Test1Text;
       UpdateKeys;
     |
   46: Test2Text;
       UpdateKeys;
     |
   47: Test3Text;
       UpdateKeys;
     |
   48: Test4Text;
       UpdateKeys;
     |
   50: StatusText;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Status1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          ZeitText;
       endif;
     |
   51: Status1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Status2Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Status2Text;
       endif;
     |
   52: Status2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Status3Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Status3Text;
       endif;
     |
   53: Status3Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Status4Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Status4Text;
       endif;
     |
   54: Status4Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Status5Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Status5Text;
       endif;
     |
   55: Status5Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Status6Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Status6Text;
       endif;
     |
   56: Status6Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
   60: ZeitText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          RTCHour:=RTCgetHour;
          RTCMinute:=RTCgetMinute;
          StundenText;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          PT1Text;
       endif;
     |
   61: StundenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          RTCsetHour(RTCHour);
          MinutenText;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (RTCHour);
          mdelay(75);
          if RTCHour>=24 then
             RTCHour:=0;
          endif;
       endif;
     |
   62: MinutenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          RTCsetMinute(RTCMinute);
          RTCsetSecond(0);              //wenn minute gesetzt wird, sekunde immer auf 0
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (RTCMinute);
          mdelay(75);
          if RTCMinute>=60 then
             RTCMinute:=0;
          endif;
       endif;
     |
   70: PT1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          PT1StundenText;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          PT2Text;
       endif;
     |
   71: PT1StundenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          PT1MinutenText;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (PT1HourCopy);
          mdelay(75);
          if PT1HourCopy>=24 then
             PT1HourCopy:=0;
          endif;
       endif;
     |
   72: PT1MinutenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (PT1Hour=PT1HourCopy) and (PT1Minute=PT1MinuteCopy) then
             nop;                       //keien �nderung, pumpen l�uft weiter
          else
             PTFlag:=false;             //pumpen einstellen
          endif;
          PT1Hour:=PT1HourCopy;
          PT1Minute:=PT1MinuteCopy;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (PT1MinuteCopy);
          mdelay(75);
          if PT1MinuteCopy>=60 then
             PT1MinuteCopy:=0;
          endif;
       endif;
     |
   80: PT2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          PT2StundenText;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          //PT3Text;
          AText;
       endif;
     |
   81: PT2StundenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          PT2MinutenText;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (PT2HourCopy);
          mdelay(75);
          if PT2HourCopy>=24 then
             PT2HourCopy:=0;
          endif;
       endif;
     |
   82: PT2MinutenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (PT2Hour=PT2HourCopy) and (PT2Minute=PT2MinuteCopy) then
             nop;                       //keine �nderung, pumpen l�uft weiter
          else
             PTFlag:=false;             //Pumpen einstellen
          endif;
          PT2Hour:=PT2HourCopy;
          PT2Minute:=PT2MinuteCopy;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (PT2MinuteCopy);
          mdelay(75);
          if PT2MinuteCopy>=60 then
             PT2MinuteCopy:=0;
          endif;
       endif;
     |
  90: PT3Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          PT3StundenText;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          AText;
       endif;
     |
   91: PT3StundenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          PT3MinutenText;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (PT3HourCopy);
          mdelay(75);
          if PT3HourCopy>=24 then
             PT3HourCopy:=0;
          endif;
       endif;
     |
   92: PT3MinutenText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (PT3Hour=PT3HourCopy) and (PT3Minute=PT3MinuteCopy) then
             nop;                       //keine �nderung, pumpen l�uft weiter
          else
             PTFlag:=false;             //Pumpen einstellen
          endif;
          PT3Hour:=PT3HourCopy;
          PT3Minute:=PT3MinuteCopy;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (PT3MinuteCopy);
          mdelay(75);
          if PT3MinuteCopy>=60 then
             PT3MinuteCopy:=0;
          endif;
       endif;
     |
  100: AText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if PTFlag then                //im laufenden betrieb kann
             NoSetAText;                //a1 oder a2 nicht ge�ndert werden
          else
            SetA1Text;
          endif;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          AirText;
       endif;
     |
  101: SetA1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          A1:=A1Copy;
          SetA2Text;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (A1Copy);
          mdelay(75);
          if A1Copy>=46 then            //A1, A2 laufen im Bereich von 1 bis 45
             A1Copy:=0;
          endif;
       endif;
     |
  102: SetA2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          A2:=A2Copy;
          //SetA3Text;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (A2Copy);
          mdelay(75);
          if A2Copy>=46 then
             A2Copy:=0;
          endif;
       endif;
     |
  103: SetA3Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          A3:=A3Copy;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (A3Copy);
          mdelay(75);
          if A3Copy>=46 then
             A3Copy:=0;
          endif;
       endif;
     |
  104: NoSetAText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
  110: AirText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          AirOnText;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          CleanText;
          //mode:=1;
          //MenueFlag:=false;
       endif;
     |
  111: AirOnText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if AirOn=0 then               //AirOn=0 hei�t Bel�ftung aus
             AirOff:=15;                //daher Bel�ftung aus auf 15min setzen
             G_Sekunde:=0;              //neu starten
             mode:=1;                   //und raus aus dem men�
             MenueFlag:=false;
          else
             AirOffText;
          endif;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (AirOn);
          mdelay(75);
          if AirOn>=16 then
             AirOn:=0;
          endif;
       endif;
     |
  112: AirOffText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if AirOff=0 then              //AirOff=0 hei�t Bel�ftung immer an
             AirOn:=15;                 //daher Bel�ftung aus auf 15min setzen
          endif;
          G_Sekunde:=0;                 //Gebl�se-Sekundenz�hler neu starten
          mode:=1;                      //damit das Gebl�setiming von vorn losgeht
          MenueFlag:=false;             //und vorl�ufiges Men�ende
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (AirOff);
          mdelay(75);
          if AirOff>=16 then
             AirOff:=0;
          endif;
       endif;
     |
  120: CleanText;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Clean1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          OutText;
       endif;
     |
  121: Clean1Text;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          //an der Stelle wird die Reinigung best�tigt,
          //daher alle anderen Aktionen einstellen
          CleanFlag:=true;              //true: reinigung l�uft

          //jetzt die Aktionen, die stattfinden sollen bei der chem. Reinigung
          SLPOn;                        //Start Schlammpumpe
          FanOff;                       //L�fter abstellen
          Clean2Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
  122: Clean2Text;                      //Sludgepump working, Step2
       if SSBRmin then                  //genug Wasser im BR
          nop;
       else
          SLPOff;                       //Schlammpumpe abstellen
          Counter:=0;                   //vorbereiten f�r backflush
          Clean3Text;
       endif;

       Entertaste;                      //abbrechen sowohl mit enter
       if TickEnter then
          TickEnter:= false;
          SLPOff;
          ExitCleaning;
       endif;

       Nexttaste;                       //als auch mit next
       if TickNext then
          TickNext:= false;
          SLPOff;
          ExitCleaning;
       endif;
     |
  123: Clean3Text;                      //Backflush, Step3
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Pause;                        //Pumpen abstellen
          Clean4Text;                   //und zum n�chsten men�
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Pause;                        //Pumpen abstellen
          ExitCleaning;                 //und raus
       endif;
     |
  124: Clean4Text;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          //Clean6Text;                 //zum Test um den Reinigungsstep zu umgehen
          Counter:=0;                   //Arbeitsz�hler auf 0
          ServiceCounter:=3755;         //Z�hler f�r reinigung step5
          Clean5Text;                   //auf 3755s=62min, 35s
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          ExitCleaning;
       endif;
     |
  125: Clean5Text;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          ExitCleaning;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          ExitCleaning;
       endif;
     |
  126: Clean6Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Counter:=0;
          Clean7Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Pause;
          FanOff;
          ExitCleaning;
       endif;
     |
  127: Clean7Text;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Pause;
          FanOff;
          ExitCleaning;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Pause;
          FanOff;
          ExitCleaning;
       endif;
     |
  130: OutText;

       OutOld:=Out;                     //alten Zustand sichern

       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Out1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
  131: Out1Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Out3Text;                     //Best�tigungstext f�r air
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Out2Text;
       endif;
     |
  132: Out2Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Out4Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Out1Text;                     //noch nicht ausgew�hlt, daher zur�ck
       endif;                           //zum Out1Text
     |
  133: Out3Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Out:=true;                    //24V-Output=Air
          mode:=1;                      //ohne �nderung raus
          MenueFlag:=false;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;                      //ohne �nderung raus
          MenueFlag:=false;
       endif;
     |
  134: Out4Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          Out:=false;                    //24V-Output=Filter
          Out5Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;                      //ohne �nderung raus
          MenueFlag:=false;
       endif;
     |
  135: Out5Text;                        //Filterparameter anzeigen

       if OutOld then                   //vorher war Air, jetzt Filter
          O_Sekunde:=0;                 //daher O_Sekunde zur�cksetzen
       endif;

       Entertaste;
       if TickEnter then
          TickEnter:=false;
          OutSekundeCopy:=OutSekunde;   //erstmal Kopien machen und mit diesen
          OutTagCopy:=OutTag;           //die neuen Einstellungen festlegen
          Out6Text;                     //Filterparameter �ndern
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;                      //Filterparameter nicht �ndern
          MenueFlag:=false;
       endif;
     |
  136: Out6Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (OutSekunde=OutSekundeCopy) then
             nop;
          else
             OutSekunde:=OutSekundeCopy;
             O_Sekunde:=0;              //und mit dem Out-Zyklus von vorn anfangen
          endif;
          Out7Text;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (OutSekundeCopy);
          mdelay(75);
          if OutSekundeCopy>=121 then
             OutSekundeCopy:=1;
          endif;
       endif;
     |
  137: Out7Text;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          if (OutTag=OutTagCopy) then
             nop;
          else
             OutTag:=OutTagCopy;
             O_Sekunde:=0;
          endif;
          mode:=1;
          MenueFlag:=false;
       endif;

       //Nexttaste;
       //if TickNext then
       //   TickNext:= false;
       if (Next=false) then
          Click;
          mdelay(75);
          inc (OutTagCopy);
          mdelay(75);
          if OutTagCopy>=31 then
             OutTagCopy:=1;
          endif;
       endif;
     |
  140: LanguageText;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Lang1Text;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          RTCHour:=RTCgetHour;          //wenns zum zeiteinstellen geht
          RTCMinute:=RTCgetMinute;      //erst einmal die aktuelle Uhrzeit laden
          ZeitText;
       endif;
     |
  141: Lang1Text;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Language:=true;               //Englisch
          mode:=1;
          MenueFlag:=false;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          Lang2Text;
       endif;
     |
  142: Lang2Text;
       Entertaste;
       if TickEnter then
          TickEnter:= false;
          Language:=false;              //Deutsch
          mode:=1;
          MenueFlag:=false;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:= false;
          mode:=1;
          MenueFlag:=false;
       endif;
     |
  210: PowerFailureText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
          Akkuflag:=false;              //kein Akkubetrieb mehr
          Piezo:=false;
          Backlight:=true;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          mode:=1;
          MenueFlag:=false;
          Akkuflag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;
     |
  211: PowerFailureText1;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
          Akkuflag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          mode:=1;
          MenueFlag:=false;
          Akkuflag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;
     |
  212: PowerFailureText2;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
          Akkuflag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          mode:=1;
          MenueFlag:=false;
          Akkuflag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;
     |
  220: StagText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
          Stag_Sekunde:=0;
          StagFlag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          mode:=1;
          MenueFlag:=false;
          Stag_Sekunde:=0;
          StagFlag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;
     |
  230: ClearwaterText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
          ClearwaterFlag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          mode:=1;
          MenueFlag:=false;
          ClearwaterFlag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;
     |
  240: ADgText;
       Entertaste;
       if TickEnter then
          TickEnter:=false;
          mode:=1;
          MenueFlag:=false;
          ADg_Counter:=0;
          ADgFlag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;

       Nexttaste;
       if TickNext then
          TickNext:=false;
          mode:=1;
          MenueFlag:=false;
          ADg_Counter:=0;
          ADgFlag:=false;
          Piezo:=false;
          Backlight:=true;
       endif;
     |
    else
       TickEnter:=false;
       TickNext:=false;
       mode:=1;
       MenueFlag:=false;
  endcase;

end;

{--------------------------------------------------------------}

procedure SetDefaults;
begin

  Backlight:=true;                //LCD-Display Hintergrundbeleuchtung einschalten
  mode:=1;                        //1.auswahlmen�
  MenueFlag:=false;               //kein men�
  Tastenzeit:=0;                  //wird nur als ++ beim reset gebraucht
  Serviceflag:=false;             //true, wenn testmode l�uft
  Testflag:=false;                //true, wenn test l�uft
  CleanFlag:=false;               //ist true, wenn reinigung l�uft
  SpeicherFlag:=false;            //speichern von parms bei stromausfall m�glich
  Laden:=false;                   //Akku laden erstmal aus
  Lade_Sekunde:=0;                //Timer zum Laden des Akkus startet bei 0
  ADg_Sekunde:=0;                 //Timer zum Pr�fen Bel�fterdruck startet bei 0
  ADg_Counter:=0;                 //Counter zum Mitteln des ADg-Drucks
  ADgFlag:=false;
  ResAkkuflag_Sekunde:=0;
  Piezocounter:=0;
  Akkuzeit:=0;                    //Zeit, in der das Ger�t auf Akku l�uft, auf 0
  GeblaeseNetz:=false;            //beide Relais abschalten, Gebl�se
  SLP:=false;                     //und Schlammpumpe
  SLPFlag:=false;                 //SLP soll nicht laufen
  SOFlag:=false;                  //true, wenn SO aktiv
  Language:=true;                 //true=Englisch, false=Deutsch
  A1:=4;                          //A gesetzt nach Anweisung von Michael
  A1Copy:=A1;                     //auf meine Frage hin
  A2:=4;                          //erhalten per email am 19.9.2014
  A2Copy:=A2;
  A3:=9;
  A3Copy:=A3;
  B:=0;                           //�u�ere laufvariable beim pumpen auch auf 0
  mem:=1;                         //membran = 1
  TC:=0;                          //Tastencounter auf 0
  SOweek:=0;                      //pumpen immer zulassen, kein aussetzen
  SOweekCopy:=0;                  //nach einer, 2, 3 oder 4 Wo
  Netzaus:=0;                     //Netzausfallz�hler zur�cksetzen
  Netzaus1:=0;                    //1=w�hrend flush/reflush
  Netzaus2:=0;                    //w�hrend air
  G_Sekunde:=0;                   //Gebl�sezeit
  P_Sekunde:=0;                   //Pumpzeit
  SO_Sekunde:=0;                  //Wochenzeit f�r Option
  S_Sekunde:=0;                   //Wochenzeit f�r Schlammpumpe
  SLPTimer:=0;                    //Z�hler f�r die Schlammpumpe
  SOTimer:=0;                     //Z�hler f�r SO
  SOzeit:=0;                      //eine Wochen in s
  SLPzeit:=0;                     //eine Wochen in s
  AirOn:=5;                       //Bel�ftung On - Zeit
  AirOff:=10;                     //Bel�ftung Off - Zeit
  PT1Hour:=3;                     //default zeit f�r pumpzyklus 1
  PT1Minute:=0;
  PT2Hour:=16;                    //default zeit f�r pumpzyklus 2
  PT2Minute:=0;
  PT3Hour:=17;                    //default zeit f�r pumpzyklus 3
  PT3Minute:=0;
  PTFlag:=false;                  //automatikbetrieb erstmal aus
  PT1:=false;                     //Pumpzeit1 Flag
  PT2:=false;                     //Pumpzeit2 Flag
  PT3:=false;                     //Pumpzeit3 Flag
  SLPday:=0;                      //Schlammpumpe l�uft nach 7 Tagen
  SLPdayCopy:=0;                  //bei 0 l�uft nie
  SLPminute:=15;                  //wenn aktiviert l�uft schlammpumpe 15min
  SLPminuteCopy:=15;
  Out:=true;                      //default true: der dritte 24V output wird als
                                  //Gebl�se parallel zur air benutzt
  OutFlag:=false;
  O_Sekunde:=0;                   //die O_Sekunde f�ngt bei 0 an
  OutTag:=5;                      //nach 5 Tagen wird der 24V-Ausgang auf das
                                  //Filter geschaltet f�r OutOnZeit in s
  OutSekunde:=60;                 //default Out on in s f�r die 24V f�r das Filter
  Stag_Sekunde:=0;                //Z�hler Stagnationszeit
  StagFlag:=false;                //Stagnation nicht eingetreten
  ClearwaterFlag:=false;          //gebrauchtes durch neues Wasser ersetzen
                                  //z.B. nach 3 w�chigem Urlaub

  if EP_StartFlag <> 55 then      //urstart, eeprom ist leer, daher daten ins
                                  //eeprom schreiben, damit da kein unsinn drin steht
     EP_StartFlag:=55;
     EP_A:=A;
     EP_A1:=A1;
     EP_A2:=A2;
     EP_A3:=A3;
     EP_B:=B;
     EP_Mem:=Mem;
     EP_SOweek:=SOweek;
     EP_SOweekCopy:=SOweek;
     EP_Netzaus:=Netzaus;
     EP_Netzaus1:=Netzaus1;
     EP_Netzaus2:=Netzaus2;
     EP_Language:=Language;
     EP_GSekunde:=G_Sekunde;
     EP_PSekunde:=P_Sekunde;
     EP_SOSekunde:=SO_Sekunde;
     EP_SSekunde:=S_Sekunde;
     EP_OSekunde:=O_Sekunde;
     EP_SOzeit:=SOzeit;
     EP_SLPZeit:=SLPZeit;
     EP_Hour:=RTCgetHour;
     EP_Minute:=RTCgetMinute;
     EP_Second:=RTCgetSecond;
     EP_AirOn:=AirOn;
     EP_AirOff:=AirOff;
     EP_PT1Hour:=PT1Hour;
     EP_PT1Minute:=PT1Minute;
     EP_PT2Hour:=PT2Hour;
     EP_PT2Minute:=PT2Minute;
     EP_PT3Hour:=PT3Hour;
     EP_PT3Minute:=PT3Minute;
     EP_PTFlag:=PTFlag;
     EP_PT1:=PT1;
     EP_PT2:=PT2;
     EP_PT3:=PT3;
     EP_SLPday:=SLPday;
     EP_SLPdayCopy:=SLPdayCopy;
     EP_SLPminute:=SLPminute;
     EP_SLPminuteCopy:=SLPminuteCopy;
     EP_OutTag:=OutTag;
     EP_OSekunde:=O_Sekunde;
     EP_OutSekunde:=OutSekunde;
     EP_Out:=Out;
     EP_StagSekunde:=Stag_Sekunde;
     EP_StagFlag:=StagFlag;
     EP_ClearwaterFlag:=ClearwaterFlag;

  else                                  //eeprom ist nicht leer, daher parameter
                                        //mit den daten aus dem eeprom besetzen

     A:=EP_A;                           //restore A
     A1:=EP_A1;                         //restore A1
     A2:=EP_A2;                         //restore A2
     A3:=EP_A3;                         //restore A3
     B:=EP_B;                           //restore B
     Mem:=EP_Mem;                       //restore Membranzahl
     SOweek:=EP_SOweek;                 //restore option
     SOweekCopy:=EP_SOweekCopy;         //restore option copy
     Netzaus:=EP_Netzaus;               //restore Anzahl Netzausf�lle
     Netzaus1:=EP_Netzaus1;             //restore Anzahl Netzausf�lle beim pumpen
     Netzaus2:=EP_Netzaus2;             //restore Anzahl Netzausf�lle beim r�cksp�len
     Language:=EP_Language;             //restore Sprache
     G_Sekunde:=EP_GSekunde;            //restore G_Sekunde
     P_Sekunde:=EP_PSekunde;            //restore P_Sekunde
     SO_Sekunde:=EP_SOSekunde;          //restore SO_Sekunde
     S_Sekunde:=EP_SSekunde;            //restore S_Sekunde
     O_Sekunde:=EP_OSekunde;
     OutTag:=EP_OutTag;
     OutSekunde:=EP_OutSekunde;
     Out:=EP_Out;
     SOzeit:=EP_SOzeit;                 //restore Wochenzeit
     SLPzeit:=EP_SLPZeit;               //restore SLPzeit
     RTCsetHour(EP_Hour);
     RTCsetMinute(EP_Minute);
     RTCsetSecond(EP_Second);
     AirOn:=EP_AirOn;
     AirOff:=EP_AirOff;
     PT1Hour:=EP_PT1Hour;
     PT1Minute:=EP_PT1Minute;
     PT2Hour:=EP_PT2Hour;
     PT2Minute:=EP_PT2Minute;
     PT3Hour:=EP_PT3Hour;
     PT3Minute:=EP_PT3Minute;
     PTFlag:=EP_PTFlag;
     PT1:=EP_PT1;
     PT2:=EP_PT2;
     PT3:=EP_PT3;
     SLPday:=EP_SLPday;
     SLPdayCopy:=EP_SLPdayCopy;
     SLPminute:=EP_SLPminute;
     SLPminuteCopy:=EP_SLPminuteCopy;
     Stag_Sekunde:=EP_StagSekunde;
     StagFlag:=EP_StagFlag;
     ClearwaterFlag:=EP_ClearwaterFlag;

  endif;

end;

{--------------------------------------------------------------}

{ Main Program }
{$IDATA}

begin
  InitPorts;
  WatchDogStart;
  EnableInts;

  ADMux:=0;                             //Verwendung von ext. Referenzspannung

  //beim urstart ist das eeprom leer, wird dann sofort der strom ausgeschaltet
  //versucht das ger�t die parameter aus dem eeprom zu laden, da das eeprom
  //aber leer ist, wird da unfug geladen, z.B. mem=255
  //daher beim urstart das eeprom erstmal mit den defaults beschreiben
  //au�erdem nat�rlich auch die defaults setzen

  SetDefaults;

  SetSysTimer(SekundenTimer, c_SekundenTimer); //1-Sekundentimer starten

  loop
    WatchDogTrig;                       //watchdog triggern

    Get24V;
    Get9V;
    if Uin1 < Uin4 then                 //Akkubetrieb, Netz ausgefallen !!
       Backlight:=true;
       Akkuflag:=true;
       AkkuText;                        //text, dass jetzt der akku-betrieb l�uft
       if not Speicherflag then
              SaveParms;                //wichtige Parms ins eeprom retten
       endif;
       FanOff;                          //Fan abschalten
       SLPOff;                          //Schlammpumpe abschalten
       Pause;                           //saugen und r�cksp�len abschalten

       Piezowarnung;                    //Piezo piepst alle 10s

       G_Sekunde:=0;                    //Bel�ftung startet immer bei 0
       Lade_Sekunde:=0;                 //Akku-Ladezeit zur�cksetzen

    else

       if Akkuflag then                 //bei r�ckkehr aus akkubetrieb
          case LastAction of            //flag ist immer noch true
            0: PowerFailureText;        //flag wird hier zur�ckgesetzt
             |
            1: PowerFailureText1;       //oder hier
             |
            2: PowerFailureText2;       //oder hier
             |
           else PowerFailureText;
          endcase;

          menue;                        //men� aufrufen

          Speicherflag:=false;          //speichern der wichtigsten parms
                                        //f�r n�chsten stromausfall wieder freigeben
          Testflag:=false;              //kein testbetrieb;
          Cleanflag:=false;             //keine Reinigung
          NachAkkubetrieb;              //Piezo piepst immer noch alle 10s
                                        //Fan wird wieder gestartet
                                        //und ebenso akkuladen
                                        //und Akkuzeit-Z�hler zur�cksetzen
       else

          TickEnter:=false;             //versehentliche Enter-Eingabe l�schen

          //Nexttaste;
          if TickNext or MenueFlag then //entweder neu oder bereits im menue
             TickNext:=false;           //Tick von Next zur�cksetzen
             Menue;                     //ins menue
          endif;

          Nexttaste;

          UpdateDisplay;                //LCD-Display updaten

          if PTFlag=false then
             if ((RTCgetHour)=(PT1Hour)) and ((RTCgetMinute)=(Pt1Minute))
                                         and ((RTCgetSecond) = 0)  then
                if not (A1=0) then
                   PTFlag:=true;        //wenn PT1-Zeit=Echtzeit
                   PT1:=true;
                   PT2:=false;
                   P_Sekunde:=0;        //Sekunden-Timer starten
                   B:=0;                //�usserer schleifenindex auf 0
                endif;
             endif;

             if ((RTCgetHour)=(PT2Hour)) and ((RTCgetMinute)=(Pt2Minute))
                                         and ((RTCgetSecond) = 0)  then
                if not (A2=0) then
                   PTFlag:=true;        //wenn PT2-Zeit=Echtzeit
                   PT1:=false;
                   PT2:=true;
                   P_Sekunde:=0;
                   B:=0;
                endif;
             endif;

          endif;

          //Alles in der folgenden if-Schleife l�uft im gem�tlichen Sekundenrhytmus

          if IsSysTimerZero(Sekundentimer) then           //1 Sekunde abgelaufen
             SetSysTimer(Sekundentimer, c_SekundenTimer); //Sekundentimer neu starten
             blink:= not blink;
             if not Laden then
                Led1:=blink;
             endif;

             if InctolimWrap(TC, 300, 0) then  //Tastencounter wird jede sekunde hochgez�hlt
                mode:=1;                       //nach 5 Minuten wird das Men� verlassen wenn
                MenueFlag:=false;              //nicht vorher TC von Next- oder Entertaste
                TickNext:=false;               //zur�ckgesetzt wird, das kann auch mitlaufen
                TickEnter:=false;              //wenn man nicht im men� ist
             endif;

             InctolimWrap(P_Sekunde, 15000, 0);          //Pumpen-Timing
                                                         //zur Sicherheit bei 15000 wieder von vorn
             InctolimWrap(G_Sekunde, G_Zeit2, 0);        //Gebl�se-Timing

             if (SOweek>0) then
                if InctolimWrap(SO_Sekunde, SOzeit-1, 0) then //SO-Timing
                   SOTimer:=0;                          //Timer bis 24h erreicht ist
                   SOFlag:=true;                        //SO ab jetzt aktiv
                endif;                                  //SO immer 1,2,3 od. 4 wochen +24h
             endif;
             
             if (SLPday>0) then
                if IncTolimWrap(S_Sekunde, SLPzeit-1, 0) then  //nach SLP-Zeit soll die
                   SLPTimer:=0;                          //Timer bis (SLP-Minute*60) abl�uft
                   SLPFlag:=true;                        //Schlammpumpe ab jetzt aktiv
                endif;
             endif;
             
             if InctolimWrap(Lade_Sekunde, 20, 0) then   //alle 20s Akkuzustand pr�fen
                AkkuLaden;                               //und ggflls.laden
             endif;

             if InctolimWrap(ADg_Sekunde, 10, 0) then    //alle 10s Bel�fter Druck pr�fen
                ADg_Check;                               //und Flag setzen wenn Druck zu niedrig
             endif;

             //OutTagLong:=longword(OutTag)*86400;
             if IncTolimWrap(O_Sekunde, (longword(OutTag)*86400), 0) then
                OutTimer:=0;
                OutFlag:=true;
             endif;

             if IncTolimWrap(Stag_Sekunde, c_dreiWochen, 0) then
             //if IncTolimWrap(Stag_Sekunde, 60, 0) then  //Testzeit 60s
                StagFlag:=true;         //Stagnation im Klarwasserbeh�lter erreicht
                Piezocounter:=0;
             endif;

             if Cleanflag then          //bei Reinigung Men� nicht vorzeitig verlassen
                TC:=0;                  //daher jede sekunde zur�cksetzen
                PTFlag:=false;          //bei Reinigung kein pumpen
                SLPflag:=false;         //bei Reinigung keine schlammpumpe
                SOflag:=false;          //bei Reinigung kein suction overflow
                Testflag:=false;        //bei Reinigung kein test
             endif;

             if Testflag or StagFlag or ClearwaterFlag then   //bei Tests od.
                TC:=0;                  //Stagnations od. Clearwater
             endif;                     //men� nicht vorzeitig verlassen

             //Strobe:=true;
             CheckReset;                //wurde ein reset ausgel�st ?

             PumpControl;               //Pumpensteuerung

             FanControl;                //Gebl�sesteuerung

             CleanControl;              //Reinigungssteuerung

             TestControl;               //Servicesteuerung

             OutControl;                //Steuerung des freien 24V-Ausgangs

             ClearwaterControl;         //Steuerung, wenn Wasser 3 Wochen
                                        //im Klarwasserbeh�lter stand und
                                        //nicht gesp�lt oder r�ckgesp�lt wurde
                                        //oder wenn auf Akkubetrieb f�r >3
                                        //oder wenn Bel�fterdruck zu niedrig
             //Strobe:=false;
          endif;
       endif;
    endif;
  endloop;

end gwa2_20.

