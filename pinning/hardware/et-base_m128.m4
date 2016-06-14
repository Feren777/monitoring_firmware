/* port the enc28j60 is attached to */
pin(SPI_CS_NET, SPI_CS_HARDWARE)



ifdef(`conf_STATUSLED_HB_ACT', `dnl
pin(STATUSLED_HB_ACT, PA0, OUTPUT)
')dnl

ifdef(`conf_STATUSLED_ERROR', `dnl
pin(STATUSLED_ERROR, PA1, OUTPUT)
')dnl

ifdef(`conf_STATUSLED_NETLINK', `dnl
pin(STATUSLED_NETLINK, PA2, OUTPUT)
')dnl

ifdef(`conf_STATUSLED_BOOTED', `dnl
pin(STATUSLED_BOOTED, PA3, OUTPUT)
')dnl


ifdef(`conf_HD44780', `
    pin(HD44780_RS, PG4)
    pin(HD44780_RW, PD6)
    pin(HD44780_EN1, PD7)
    pin(HD44780_D4, PG0)
    pin(HD44780_D5, PG1)
    pin(HD44780_D6, PG2)
    pin(HD44780_D7, PG3)
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
