dnl
dnl header.m4
dnl
dnl   Copyright (c) 2008 by Christian Dietrich <stettberger@dokucode.de>
dnl   Copyright (c) 2008,2009 by Stefan Siegl <stesie@brokenpipe.de>
dnl   Copyright (c) 2008 by Jochen Roessner <jochen@lugrot.de>
dnl  
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by 
dnl   the Free Software Foundation; either version 3 of the License, or
dnl   (at your option) any later version.
dnl  
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl  
dnl   You should have received a copy of the GNU General Public License
dnl   along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl  
dnl   For more information on the GPL, please go to:
dnl   http://www.gnu.org/copyleft/gpl.html
dnl
/* This file has been generated by pinning m4 system automatically.
   Please do not modify it, edit the m4 scripts below pinning/ instead. */

/* code to en-/disable RS485 transmitters depending on the usart used */
#undef RS485_HAVE_TE
#undef RS485_DISABLE_TX
#undef RS485_ENABLE_TX
#undef RS485_TE_SETUP
#if ((USE_USART + 0) == 0 && defined(HAVE_RS485TE_USART0))
  #define RS485_HAVE_TE     1
  #define RS485_DISABLE_TX  PIN_CLEAR(RS485TE_USART0)
  #define RS485_ENABLE_TX   PIN_SET(RS485TE_USART0)
  #define RS485_TE_SETUP    DDR_CONFIG_OUT(RS485TE_USART0)
#elif ((USE_USART + 0) == 1 && defined(HAVE_RS485TE_USART1))
  #define RS485_HAVE_TE     1
  #define RS485_DISABLE_TX  PIN_CLEAR(RS485TE_USART1)
  #define RS485_ENABLE_TX   PIN_SET(RS485TE_USART1)
  #define RS485_TE_SETUP    DDR_CONFIG_OUT(RS485TE_USART1)
#else
  #define RS485_HAVE_TE     0
  #define RS485_DISABLE_TX  /* empty */
  #define RS485_ENABLE_TX   /* empty */
  #define RS485_TE_SETUP    /* empty */
#endif

divert(-1)dnl

define(`define_divert', 1)
define(`alias_divert', 2)
define(`eof_divert', 3)

define(`port_mask_A', 0)
define(`port_mask_B', 0)
define(`port_mask_C', 0)
define(`port_mask_D', 0)
define(`port_mask_E', 0)
define(`port_mask_F', 0)
define(`port_mask_G', 0)

define(`ddr_mask_A', 0)
define(`ddr_mask_B', 0)
define(`ddr_mask_C', 0)
define(`ddr_mask_D', 0)
define(`ddr_mask_E', 0)
define(`ddr_mask_F', 0)
define(`ddr_mask_G', 0)

dnl forloop-implementation from gnu m4 example scripts ...
# forloop(var, from, to, stmt) - simple version
define(`forloop', `pushdef(`$1', `$2')_forloop($@)popdef(`$1')')
define(`_forloop',
       `$4`'ifelse($1, `$3', `', `define(`$1', incr($1))$0($@)')')

changecom(`//')
define(`PM', `port_mask_'$1)dnl
define(`DM', `ddr_mask_'$1)dnl
define(`PUM', `ddr_mask_'$1)dnl

define(`pin', `dnl
ifelse(regexp($2, `^P[A-Z][0-9]$'), `-1', `divert(alias_divert)', `divert(define_divert)')dnl
define(`pinname', `ifelse(regexp($2, `^P[A-Z][0-9]$'), `-1', `$2_PORT', `translit(substr(`$2', 1, 1), `a-z', `A-Z')')')dnl
define(`pinnum', `ifelse(regexp($2, `^P[A-Z][0-9]$'), `-1', `$2_PIN', `substr(`$2', 2, 1)')')dnl
ifelse(translit(`$1',`a-z', `A-Z'), `ONEWIRE', , `dnl
#define translit(`$1',`a-z', `A-Z')_PORT pinname
#define translit(`$1',`a-z', `A-Z')_PIN pinnum
')dnl
#define HAVE_'translit(`$1',`a-z', `A-Z')` ifelse(regexp($2, `^P[A-Z][0-9]$'), `-1', `HAVE_$2', `1')
ifelse(`$3', `OUTPUT', `define(`ddr_mask_'pinname, eval(DM(pinname) | (1 << pinnum)))')dnl

ifelse(translit(`$1',`a-z', `A-Z'), `ONEWIRE', `dnl
// support for legacy onewire pin defines
#define ONEWIRE_BUSCOUNT 1
#define ONEWIRE_STARTPIN pinnum
#define ONEWIRE_PORT format(PORT%s, pinname)
#define ONEWIRE_DDR format(DDR%s, pinname)
#define ONEWIRE_PIN format(PIN%s, pinname)
#define ONEWIRE_BUSMASK eval(1 << pinnum)U
')dnl

ifelse(regexp($2, `^P[A-Z][0-9]$'), `-1', `', `
#ifdef $2_USED
#  error Pinning Error: '__file__:__line__:` $1 has a double define on $2
#endif
#define $2_USED 1
define(`port_mask_'pinname, eval(PM(pinname) | (1 << pinnum)))dnl
')dnl

')

define(`RFM12_NO_INT', `dnl
#define RFM12_USE_POLL 1
')
define(`RFM12_USE_INT', `dnl
/* rfm12 module interrupt line */
#define RFM12_INT_PIN INT$1
#define RFM12_INT_VECTOR INT$1`_vect'
')

define(`RFM12_USE_PCINT', `dnl
/* RFM12 PinChange-Interrupt Line  PCINT$1 -> $2 */
pin(RFM12_PCINT, $2, INPUT)

dnl Configure pin-change-mask to monitor PCINTn and enable interrupt
#define rfm12_int_enable() \
  do { \
    _paste(PCMSK, eval($1/8)) |= _BV(PCINT$1); \
    PCICR  |= _BV(_paste(PCIE, eval($1/8)));   \
  } while(0)

#define rfm12_int_disable() \
  do { \
    _paste(PCMSK, eval($1/8)) &= ~_BV(PCINT$1); \
    PCICR  &= ~_BV(_paste(PCIE, eval($1/8)));   \
  } while(0)

#define RFM12_VECTOR _paste3(PCINT, eval($1/8), _vect)
')

define(`RC5_USE_INT', `dnl
/* rc5 interrupt line (TSOP Data out)*/
#define RC5_INT_PIN INT$1
#define RC5_INT_VECTOR INT$1`_vect'
#define RC5_ISC0 ISC$1`0'
#define RC5_ISC1 ISC$1`1'
')

define(`RFM12_ASK_SENSE_USE_INT', `dnl
/* rfm12 ask sense interrupt line */
#define RFM12_ASKINT_PIN INT$1
#define RFM12_ASKINT_ISC _ISC($1,0)
#define RFM12_ASKINT_ISCMASK (_ISC($1,0) | _ISC($1,1))
#define RFM12_ASKINT_VECTOR INT$1`_vect'
')

define(`RFM12_FS20_USE_INT', `dnl
/* rfm12 module interrupt line */
#define RFM12_FS20INT_PIN INT$1
#define RFM12_FS20INT_ISC _ISC($1,0)
#define RFM12_FS20INT_ISCMASK (_ISC($1,0) | _ISC($1,1))
#define RFM12_FS20INT_VECTOR INT$1`_vect'
pin(RFM12_FS20IN, format(PD%d,eval(2+$1)), INPUT)
')

define(`USB_USE_INT', `dnl
/* usb  interrupt line */
#define USB_INT_PIN INT$1
#define USB_INT_VECTOR INT$1`_vect'
#define USB_INTR_CFG_HACK(no) ((1 << ISC ## no ## 0) | (1 << ISC ## no ## 0))
#define USB_INTR_CFG_SET USB_INTR_CFG_HACK($1)
')

define(`DCF77_USE_PCINT', `dnl
/* DCF77 PinChange-Interrupt Line  PCINT$1 -> $2 */
pin(DCF1, $2, INPUT)
#define DCF77_PCINT_PIN $2

dnl Configure pin-change-mask to monitor PCINTn and enable interrupt
#define dcf77_configure_pcint() \
  _paste(PCMSK, eval($1/8)) |= _BV(PCINT$1); \
  PCICR  |= _BV(_paste(PCIE, eval($1/8)));

#define DCF77_VECTOR _paste3(PCINT, eval($1/8), _vect)
')

define(`DCF77_USE_INT', `dnl
/* DCF77 Interrupt Line  INT$1 -> $2 */
pin(DCF1, $2, INPUT)

/* Configure real interrupt $1, set sense control to trigger on any edge */
#define DCF77_INT_PIN INT$1
#define DCF77_INT_ISC _ISC($1,0)
#define DCF77_INT_ISCMASK (_ISC($1,0) | _ISC($1,1))
#define DCF77_VECTOR INT$1`_vect'
')

define(`PS2_USE_PCINT', `dnl
/* PS2 PinChange-Interrupt Line  PCINT$1 -> $2 */
pin(PS21, $2, INPUT)

dnl Configure pin-change-mask to monitor PCINTn and enable interrupt
#define ps2_configure_pcint() \
  _paste(PCMSK, eval($1/8)) |= _BV(PCINT$1); \
  PCICR  |= _BV(_paste(PCIE, eval($1/8)));

#define PS2_VECTOR _paste3(PCINT, eval($1/8), _vect)
')

define(`PS2_USE_INT', `dnl
/* PS2 Interrupt Line  INT$1 -> $2 */
pin(PS21, $2, INPUT)

/* Configure real interrupt $1, set sense control to trigger on any edge */
#define PS2_INT_PIN INT$1
#define PS2_INT_ISC _ISC($1,0)
#define PS2_INT_ISCMASK (_ISC($1,0) | _ISC($1,1))
#define PS2_VECTOR INT$1`_vect'
')




/* Configure RFID EM4095 interrupt connection */
define(`RFID_USE_PCINT', `dnl
/* RFID PinChange-Interrupt Line  PCINT$1 -> $2 */
pin(RFID, $2, INPUT)
#define RFID_PCINT_PIN $2

dnl Configure pin-change-mask to monitor PCINTn and enable interrupt
#define rfid_configure_pcint() \
  _paste(PCMSK, eval($1/8)) |= _BV(PCINT$1); \
  PCICR  |= _BV(_paste(PCIE, eval($1/8)));

#define RFID_VECTOR _paste3(PCINT, eval($1/8), _vect)
')

define(`RFID_USE_INT', `dnl
/* RFID Interrupt Line  INT$1 -> $2 */
pin(RFID_DEMOD, $2, INPUT)

/* Configure real interrupt $1, set sense control to trigger on any edge */
#define RFID_DATA_PIN $2
#define RFID_INT_PIN INT$1
#define RFID_INT_ISC _ISC($1,0)
#define RFID_INT_ISCMASK (_ISC($1,0) | _ISC($1,1))
#define RFID_VECTOR INT$1`_vect'
')






define(`ONEWIRE_PORT_RANGE', `dnl
define(`pinname', translit(substr(`$1', 1, 1), `a-z', `A-Z'))dnl
define(`start', substr(`$1', 2, 1))dnl
define(`stop', substr(`$2', 2, 1))dnl
  /* onewire port range configuration: */
  forloop(`itr', start, stop, `dnl

#ifdef format(P%s%d_USED, pinname, itr)
#  error Pinning Error: '__file__:__line__:` ONEWIRE has a double define on format(P%s%d_USED, pinname, itr)
#endif
#define format(P%s%d_USED, pinname, itr) 1
define(`port_mask_'pinname, eval(PM(pinname) | (1 << itr)))
define(`ddr_mask_'pinname, eval(DM(pinname) | (1 << itr)))

')dnl

#define ONEWIRE_BUSCOUNT eval(stop - start + 1)
#define ONEWIRE_STARTPIN start
#define ONEWIRE_PORT format(PORT%s, pinname)
#define ONEWIRE_DDR format(DDR%s, pinname)
#define ONEWIRE_PIN format(PIN%s, pinname)
#define ONEWIRE_BUSMASK eval(((1 << eval(stop-start+1)) - 1) << start)U

')


define(`MOTORCURTAIN_PORT_RANGE', `dnl
define(`pinname', translit(substr(`$1', 1, 1), `a-z', `A-Z'))dnl
define(`start', substr(`$1', 2, 1))dnl
define(`stop', substr(`$2', 2, 1))dnl
  /* motor curtain port range configuration: */
  forloop(`itr', start, stop, `dnl
#undef MOTORCURTAIN_PIN_PORT
#undef MOTORCURTAIN_PIN_PIN
#undef HAVE_MOTORCURTAIN_PIN  /* quite a hack, but should do the job *g*    \
                           this is just to keep the preprocessor from \
               complaining and get the port masks right. */
pin(MOTORCURTAIN_PIN, format(`P%s%d', pinname, itr))
  ' )dnl
#define MOCU_SENSOR_COUNT eval(stop-start+1)
#define MOCU_SENSOR_STARTPIN start
#define MOCU_SENSORS_PORT format(PORT%s, pinname)
#define MOCU_SENSORS_DDR_PORT format(DDR%s, pinname)
#define MOCU_SENSORS_PIN_PORT format(PIN%s, pinname)
')

define(`STELLA_USE_TIMER', `dnl

/* Configure stella timer */
#define STELLA_TC_PRESCALER_1024    format(TC%s_PRESCALER_1024, $1)
#define STELLA_TC_PRESCALER_256     format(TC%s_PRESCALER_256, $1)
#define STELLA_TC_PRESCALER_128     format(TC%s_PRESCALER_128, $1)
#define STELLA_TC_PRESCALER_64      format(TC%s_PRESCALER_64, $1)
#define STELLA_TC_INT_COMPARE_ON    format(TC%s_INT_COMPARE_ON, $1)
#define STELLA_TC_INT_COMPARE_OFF   format(TC%s_INT_COMPARE_OFF, $1)
#define STELLA_TC_INT_OVERFLOW_ON   format(TC%s_INT_OVERFLOW_ON, $1)
#define STELLA_TC_INT_OVERFLOW_OFF  format(TC%s_INT_OVERFLOW_OFF, $1)
#define STELLA_TC_VECTOR_COMPARE    format(TC%s_VECTOR_COMPARE, $1)
#define STELLA_TC_VECTOR_OVERFLOW   format(TC%s_VECTOR_OVERFLOW, $1)
#define STELLA_TC_COMPARE_REG       format(TC%s_COUNTER_COMPARE, $1)
')


define(`STELLA_PORT1_RANGE', `dnl
define(`pinname', translit(substr(`$1', 1, 1), `a-z', `A-Z'))dnl
define(`start', substr(`$1', 2, 1))dnl
define(`stop', substr(`$2', 2, 1))dnl
  /* stella port range configuration: */
  forloop(`itr', start, stop, `dnl
#undef STELLA_PIN_PORT
#undef STELLA_PIN_PIN
#undef HAVE_STELLA_PIN  /* quite a hack, but should do the job *g*    \
                           this is just to keep the preprocessor from \
               complaining and get the port masks right. */
pin(STELLA_PIN, format(`P%s%d', pinname, itr))
  ' )dnl
#define STELLA_PINS_PORT1 eval(stop-start+1)
#define STELLA_OFFSET_PORT1 start
#define STELLA_PORT1 format(PORT%s, pinname)
#define STELLA_DDR_PORT1 format(DDR%s, pinname)
')

define(`STELLA_PORT2_RANGE', `dnl
define(`pinname', translit(substr(`$1', 1, 1), `a-z', `A-Z'))dnl
define(`start', substr(`$1', 2, 1))dnl
define(`stop', substr(`$2', 2, 1))dnl
  /* stella port range configuration: */
  forloop(`itr', start, stop, `dnl
#undef STELLA_PIN_PORT
#undef STELLA_PIN_PIN
#undef HAVE_STELLA_PIN  /* quite a hack, but should do the job *g*    \
                           this is just to keep the preprocessor from \
               complaining and get the port masks right. */
pin(STELLA_PIN, format(`P%s%d', pinname, itr))
  ' )dnl
#define STELLA_PINS_PORT2 eval(stop-start+1)
#define STELLA_OFFSET_PORT2 start
#define STELLA_PORT2 format(PORT%s, pinname)
#define STELLA_DDR_PORT2 format(DDR%s, pinname)
')

define(`ST7626_DATA_PORT', `dnl
  forloop(`itr', 0, 7, `dnl
    pin(`ST7626_DATA_PIN'itr, format(`P%s%d', $1, itr), OUTPUT)
  ')dnl
  #define ST7626_DATA PORT$1
  #define ST7626_DATA_DDR DDR$1
  #define ST7626_DATAIN PIN$1
')

define(`S1D13305_DATA_PORT', `dnl
  forloop(`itr', 0, 7, `dnl
    pin(`S1D13305_DATA_PIN'itr, format(`P%s%d', $1, itr), OUTPUT)
  ')dnl
  #define S1D13305_DATA PORT$1
  #define S1D13305_DATA_DDR DDR$1
  #define S1D13305_DATAIN PIN$1
')

ifdef(`conf_SOFT_SPI', `', `dnl
  ifdef(`conf_RFM12', `define(need_spi, 1)')dnl
  ifdef(`conf_ENC28J60', `define(need_spi, 1)')dnl
  ifdef(`conf_DATAFLASH', `define(need_spi, 1)')dnl
  ifdef(`conf_SD_READER', `define(need_spi, 1)')dnl
  ifdef(`conf_USTREAM', `define(need_spi, 1)')dnl
  ifdef(`conf_SER_RAM_23K256', `define(need_spi, 1)')dnl
  ifdef(`conf_S1D15G10', `define(need_spi, 1)')dnl
')

define(`SHT_VOLTAGE_COMPENSATION', `dnl
  #define SHT_VOLTAGE_COMPENSATION_D1 SHT_VOLTAGE_COMPENSATION_D1_$1
')

divert(1)
`
#ifndef _PINNING_HEADER
#define _PINNING_HEADER

#define _ISC(n,m) _BV(ISC ## n ## m)
#define _paste(n,m) n ## m
#define _paste3(a,b,c) a ## b ## c

#define _PORT_CHAR(character) PORT ## character
#define PORT_CHAR(character) _PORT_CHAR(character)

#define _PIN_CHAR(character) PIN ## character
#define PIN_CHAR(character) _PIN_CHAR(character)

#define _DDR_CHAR(character) DDR ## character
#define DDR_CHAR(character) _DDR_CHAR(character)


#define DDR_CONFIG_IN(pin)  DDR_CHAR( pin ## _PORT) &= ~_BV((pin ## _PIN))
#define DDR_CONFIG_OUT(pin)  DDR_CHAR( pin ## _PORT) |= _BV((pin ## _PIN))

#define PIN_BV(pin) (_BV(pin ## _PIN))
#define PIN_NR(pin) (pin ## _PIN)
#define PIN_HIGH(pin) (PIN_CHAR(pin ## _PORT) & _BV(pin ## _PIN))
#define PIN_SET(pin) PORT_CHAR(pin ## _PORT) |= _BV(pin ## _PIN)
#define PIN_CLEAR(pin) PORT_CHAR(pin ## _PORT) &= ~_BV(pin ## _PIN)
#define PIN_TOGGLE(pin) PORT_CHAR(pin ## _PORT) ^= _BV(pin ## _PIN)
#define PIN_PULSE(pin) do { PORT_CHAR(pin ## _PORT) &= ~_BV(pin ## _PIN); \
                            PORT_CHAR(pin ## _PORT) ^=  _BV(pin ## _PIN); \
                          } while(0)'
