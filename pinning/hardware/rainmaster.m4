/* port the enc28j60 is attached to */
pin(SPI_CS_NET, SPI_CS_HARDWARE)


/* Heartbeat LED is attached to */
ifdef(`conf_STATUSLED_HB_ACT', `dnl
pin(STATUSLED_HB_ACT, PE2, OUTPUT)
')dnl
pin(STATUSLED_HB_ACT, PE2, OUTPUT)


/* Status LED is attached to */
ifdef(`conf_STATUSLED_ERROR', `dnl
pin(STATUSLED_ERROR, PE3, OUTPUT)
')dnl
pin(STATUSLED_ERROR, PE3, OUTPUT)


/* Netlink LED is attached to */
ifdef(`conf_STATUSLED_NETLINK', `dnl
pin(STATUSLED_NETLINK, PE4, OUTPUT)
')dnl


/* Booted LED is attached to */
ifdef(`conf_STATUSLED_BOOTED', `dnl
pin(STATUSLED_BOOTED, PE5, OUTPUT)
')dnl


/* LCD is attached to */
ifdef(`conf_HD44780', `
    pin(HD44780_RS, PC5)
    pin(HD44780_RW, PC6)
    pin(HD44780_EN1, PC4)
    pin(HD44780_D4, PC0)
    pin(HD44780_D5, PC1)
    pin(HD44780_D6, PC2)
    pin(HD44780_D7, PC3)
')dnl


ifdef(`conf_CH_A_PWM_GENERAL', `dnl
 pin(CHANNEL_A_PWM, PB5, OUTPUT)  
')dnl
ifdef(`conf_CH_B_PWM_GENERAL', `dnl
 pin(CHANNEL_B_PWM, PB6, OUTPUT)
')dnl
ifdef(`conf_CH_C_PWM_GENERAL', `dnl
 pin(CHANNEL_C_PWM, PB7, OUTPUT)
')dnl


ifdef(`conf_BUTTONS_INPUT', `dnl
  pin(BTN_NEXT, PF5, INPUT)         /* PF5 is button NEXT  on real hw (use Port D 4 and 5 as DUMMY for Testing !!   ) */
  pin(BTN_ENTER, PF6, INPUT)        /* PF6 is button ENTER on real hw (Port D is not connected) */
  pin(SW1_1, PA4, INPUT)
  pin(SW1_2, PA5, INPUT)

  #define BUTTONS_COUNT 4

  #define BUTTONS_CONFIG(_x) \
  _x(BTN_NEXT)\
  _x(BTN_ENTER)\
  _x(SW1_1)\
  _x(SW1_2)\

')dnl


pin(LADE_AKKU, PE6, OUTPUT)

pin(AL_SLUDGE_PUMP, PG0, OUTPUT)
pin(AL_COMPRESSOR_AIR, PG1, OUTPUT)
pin(AL_GUI_PIEZO, PG2, OUTPUT)

pin(RM_PUMPKONTAKT, PA0, INPUT)     /* AL_SSKLARMIN */
pin(RM_SCHWIMMER, PA1, INPUT)       /* AL_SSKLARMAX */
pin(RM_POSITION1, PA2, INPUT)       /* AL_SSBRMIN   */


ifdef(`conf_ONEWIRE', `dnl
  /* onewire port range */
  ONEWIRE_PORT_RANGE(PD6, PD6)
')dnl


ifdef(`conf_RFID', `dnl
  RFID_USE_INT(7, PE7)
  pin(RFID_SHD, PE5, OUTPUT)
')dnl


