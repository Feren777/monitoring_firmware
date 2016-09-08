





#define TRINKWASSER   0x20
#define REGENWASSER   0x21
#define VENTIL_AUF    0x22
#define VENTIL_ZU     0x23

#define TANK_MIN      200
#define TANK_MAX      950

#define PUMPE???

#define PIN_DRUCKSCHALTER   
#define ANZAHL_GERAETE  3

uint16_t tanklevel;
uint8_t  magnetventil; // an = 1 aus = 0
uint8_t  rmventil;     // trinkwasser = 1 regenwasser = 0
bool pumpe_status = false;

void send_tanklevel (uip_t ip, uint16_t tanklevel);
uint16_t get_tanklevel (nip_t ip);

uip_ipaddr_t geraete_IPs[ANZAHL_GERAETE];

uip_ipaddr_t requested_ID = 0;

uint16_t tanklevel_geraete[ANZAHL_GERAETE];


void rainmaster_mainloop(){

  save_fuellstaende();
  check_for_request();
  check_pressure_status();
}

void save_fuellstaende(void){
  for(i = 0; i < ANZAHL_GERAETE; i++){
    tanklevel_geraete[i] = get_tanklevel(geraete_IPs[i]);
  }
}

void check_for_request(){
  if(EINGABE == "VENTILAUF" && ZENTRALVENTIL_ZU) {
    set.valve(ip, on)  
  }
  if(EINGABE == "VENTILZU" && ZENTRALVENTIL_AUF) {
    set.valve(ip, off)
  }
  if(EINGABE == "NOWATER" && ZENTRALVENTIL_ZU){
    requested_ID = 0;
    send_request();
  }
  if(EINGABE == "DELIVERPOSSIBLE?"){
    if(requested_ID == 0){
      send_true();
    }
    else{
      send_false();
    }
  }
  if(EINGABE == "ABORTREQUEST"){
    requested_ID = 0;
    set.valve(ip, off);
  }
}

void check_pressure_status(void){

  //TANK LEER REQUEST
  if (tanklevel <= TANK_MIN && rm_status.SchwimmSchalter == false) {
    //rm_status.SchwimmSchalter = true;
    if(requested_ID != 0){
      send_OUT_OF_WATER(geraete_IPs[requested_ID - 1]);
      requested_ID = 0;
    }
    send_request();
    PIN_SET(ZENTRALVENTIL, OFF);  //VENTIL_ZU (aus sicherheit)
  }

  if (tanklevel >= TANK_MAX && rm_status.SchwimmSchalter == true) { 
    rm_status.SchwimmSchalter = false;
    abort_request();
  }

  //STANDARD RM ECO
  if (!PIN_HIGH(DRUCKSCHALTER) && !pumpe_status) {
    PIN_SET(PUMPE, ON);
    pumpe_status = true;
  }
  if (PIN_HIGH(DRUCKSCHALTER) && pumpe_status) {
    PIN_SET(PUMPE, OFF);
    pumpe_status = false;
  }

}




void send_request(void){
  int i = 0;
  for(i; !res && i < ANZAHL_GERAETE; i++){
    res = send_request_for_water(ip);
  }
  requested_ID = i + 1;
}

void abort_request(void){
  send_abort_request(geraete_IPs[requested_ID - 1]);
  requested_ID = 0;
}





  // Menuscreens abfragen, um ggfls. Parameteränderungen vorher zu erfassen (Statusvariablen)
  //al_menu_control();

  // Basis RAINMASTER FUNKTION mit vollem Tank
  if(my_tanklevel > TANK_MIN) {


    // Eigenbedarf: Magnetventil schließen, brauchen keine Fremdeinspeisung, können auch nichts einspeisen
    // TODO:
    if(rm_status.PumpenKontakt == true) {
      setpwm('c', 0);
      rm_status.Magnetventil = 0;
    }

    // Kugelhahn umstellen auf Regenwasser
    //setpwm('b', 0);
    PIN_CLEAR(RM_KUGELHAHN);
    rm_status.Kugelhahn = 0;

    // Eigener Verbrauch wird benötigt
    if(rm_status.PumpenKontakt == true) {
      setpwm('a', 255);             // Pumpe einschalten
      rm_status.PumpenFlag = true;  // Pumpenflag setzen
    } else {
      setpwm('a', 0);                // Pumpe ausschalten
      rm_status.PumpenFlag = false;  // PumpenFlag setzen
      rm_offcounter = 0;             // Pumpe läuft nicht, also counter auf 0  
    }

  // nicht (mehr) genügend Wasser im Tank
  } else {

    // TODO:
    uip_ipaddr_t ip_addr;
    uip_ipaddr(ip_addr, 192, 168, 1, 101);

    // Kugelhahn umstellen
    //setpwm('b', 255);
    PIN_SET(RM_KUGELHAHN);
    rm_status.Kugelhahn = 1;

    // müssen aus dem Netz nachspeisen weil Bedarf
    if(rm_status.PumpenKontakt == true) {

      // Eigenes Magnetventil schließen
      setpwm('c', 0);
      rm_status.Magnetventil = 0;


      // TODO:
      if(rm_001.busy == 0) {
        // Fremdgerät kann Magnetventil öffnen u Pumpe starten, falls kein Eigenbedarf u genügend Wasser bei sich
        //uip_ipaddr(ip_addr, 192, 168, 1, 101);
        ecmd_sender_send_command_P(&ip_addr, NULL, PSTR("rainmaster_request 1\n"));
        rm_001.busy = 1;
      }

      /* PROBLEM: Wir wissen nicht, ob Wasser zum Pumpen vorhanden ist... */

      //if(rm_status.SchwimmSchalter == true) { // Haben genügend Wasser im Vorratsspeicher

      // Eigene Pumpe starten um Druck aufzubauen
      setpwm('a', 255);             // Pumpe einschalten
      rm_status.PumpenFlag = true;  // Pumpenflag setzen
      
      //}

    } else { // Pumpenkontakt aus (keine eigene Nachfrage trotz leeren Tanks)

      // Wir haben nichts im Tank und können auch nichts nachspeisen
      setpwm('a', 0);                // Pumpe ausschalten
      rm_status.PumpenFlag = false;  // PumpenFlag setzen
      rm_offcounter = 0;             // Pumpe läuft nicht, also counter auf 0

      // TODO:
      if(rm_001.tanklevel < TANK_MAX && rm_001.busy == 1) {
        //uip_ipaddr(ip_addr, 192, 168, 1, 101);
        ecmd_sender_send_command_P(&ip_addr, NULL, PSTR("rainmaster_request 0\n"));
        rm_001.busy = 0;
      }

    }




  }
  





  







#if 0

  // Prüfen wann aus Regenwassertank oder Nachspeisung gepumpt werden soll 
  if(rm_status.PumpenFlag == true) {
    // Wir pumpen gerade selbst und benötigen das Wasser für uns

  }


  // Wir haben selbst NICHT genügend Regenwasser im Tank
  if(my_tanklevel <= TANK_MIN && rm_status.SchwimmSchalter == true) {

    // Kugelhahn umstellen
    //setpwm('b', 255);
    //rm_status.Kugelhahn = 1;

    // Magnetventil anderes Gerät umschalten, falls noch nicht erfragt
    if(rm_001.tanklevel > TANK_MIN && rm_001.request_state == 0) {

      uip_ipaddr_t ip_addr;
      uip_ipaddr(ip_addr, 192, 168, 1, 101);
      ecmd_sender_send_command_P(&ip_addr, NULL, PSTR("pwm set b 255\n"));
      rm_001.request_state = 1;
    }


  } else if(my_tanklevel > TANK_MIN /* && rm_status.SchwimmSchalter == true */) {

    // Kugelhahn umstellen
    //setpwm('b', 0);
    //rm_status.Kugelhahn = 0;

  }


#endif


  // Nach einer gewissen Zeit abschalten (200ms * 14400 = 2880min = 48h = 2 Tage Dauerbetrieb)
#if 0
  if(rm_status.PumpenFlag == true) {

    if(rm_offcounter > 14400) {

    setpwm('a', 0);       // Pumpe ausschalten
    setpwm('b', 0);       // Ladepumpe ausschalten

      while (true) {
        PIN_TOGGLE(STATUSLED_HB_ACT);
      _delay_ms(500);
      }

    }

  } else

    rm_offcounter = 0;
#endif









50/100