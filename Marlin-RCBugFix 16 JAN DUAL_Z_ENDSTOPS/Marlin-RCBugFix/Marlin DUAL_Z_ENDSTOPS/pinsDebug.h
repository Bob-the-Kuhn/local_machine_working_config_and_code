/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


bool endstop_monitor_flag = false;

#define NAME_FORMAT "%-28s"   // one place to specify the format of all the sources of names
                               // "-" left justify, "28" minimum width of name, pad with blanks

#define IS_ANALOG(P) ((P) >= analogInputToDigitalPin(0) && ((P) <= analogInputToDigitalPin(15) || (P) <= analogInputToDigitalPin(7)))

#define AVR_ATmega2560_FAMILY (defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__))
#define AVR_AT90USB1286_FAMILY (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1286P__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB646P__)  || defined(__AVR_AT90USB647__))
#define AVR_ATmega1284_FAMILY (defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__) || defined(__AVR_ATmega1284P__))

/**
 *  This routine minimizes RAM usage by creating a FLASH resident array to
 *  store the pin names, pin numbers and analog/digital flag.
 *
 *  Creating the array in FLASH is a two pass process.  The first pass puts the
 *  name strings into FLASH.  The second pass actually creates the array.
 *
 *  Both passes use the same pin list.  The list contains two macro names. The
 *  actual macro definitions are changed depending on which pass is being done.
 *
 */

// first pass - put the name strings into FLASH

#define _ADD_PIN_2(PIN_NAME, ENTRY_NAME) static const unsigned char ENTRY_NAME[] PROGMEM = {PIN_NAME};
#define _ADD_PIN(PIN_NAME, COUNTER)  _ADD_PIN_2(PIN_NAME, entry_NAME_##COUNTER)
#define REPORT_NAME_DIGITAL(NAME, COUNTER)  _ADD_PIN(#NAME, COUNTER)
#define REPORT_NAME_ANALOG(NAME, COUNTER)  _ADD_PIN(#NAME, COUNTER)

#line 0   // set __LINE__ to a known value for the first pass

#include "pinsDebug_list.h"

#line 59   // set __LINE__ to the correct line number or else compiler error messages don't make sense

// manually add pins that have names that are macros which don't play well with these macros
#if SERIAL_PORT == 0 && (AVR_ATmega2560_FAMILY || AVR_ATmega1284_FAMILY)
  static const unsigned char RXD_NAME[] PROGMEM = {"RXD"};
  static const unsigned char TXD_NAME[] PROGMEM = {"TXD"};
#endif

/////////////////////////////////////////////////////////////////////////////

// second pass - create the array

#undef _ADD_PIN_2
#undef _ADD_PIN
#undef REPORT_NAME_DIGITAL
#undef REPORT_NAME_ANALOG

#define _ADD_PIN_2( ENTRY_NAME, NAME, IS_DIGITAL) {(const char*) ENTRY_NAME, (const char*)NAME, (const char*)IS_DIGITAL},
#define _ADD_PIN( NAME, COUNTER, IS_DIGITAL)  _ADD_PIN_2( entry_NAME_##COUNTER, NAME, IS_DIGITAL)
#define REPORT_NAME_DIGITAL(NAME, COUNTER)  _ADD_PIN( NAME, COUNTER, (uint8_t)1)
#define REPORT_NAME_ANALOG(NAME, COUNTER)  _ADD_PIN( analogInputToDigitalPin(NAME), COUNTER, 0)


const char* const pin_array[][3] PROGMEM = {

/**
 *  [pin name]  [pin number]  [is digital or analog]  1 = digital, 0 = analog
 *  Each entry takes up 6 bytes in FLASH:
 *     2 byte pointer to location of the name string
 *     2 bytes containing the pin number
 *         analog pin numbers were convereted to digital when the array was created
 *     2 bytes containing the digital/analog bool flag
 */

 // manually add pins ...
  #if SERIAL_PORT == 0
    #if AVR_ATmega2560_FAMILY
      {RXD_NAME, 0, 1},
      {TXD_NAME, 1, 1},
    #elif AVR_ATmega1284_FAMILY
      {RXD_NAME, 8, 1},
      {TXD_NAME, 9, 1},
    #endif
  #endif

  #line 0   // set __LINE__ to the SAME known value for the second pass
  #include "pinsDebug_list.h"

};  // done populating the array

#line 109  // set __LINE__ to the correct line number or else compiler error messages don't make sense

#define n_array (sizeof (pin_array) / sizeof (const char *))/3

#if !defined(TIMER1B)    // working with Teensyduino extension so need to re-define some things
  #include "pinsDebug_Teensyduino.h"
#endif


#define PWM_PRINT(V) do{ sprintf(buffer, "PWM:  %4d", V); SERIAL_ECHO(buffer); }while(0)
#define PWM_CASE(N,Z) \
  case TIMER##N##Z: \
    if (TCCR##N##A & (_BV(COM##N##Z##1) | _BV(COM##N##Z##0))) { \
      PWM_PRINT(OCR##N##Z); \
      return true; \
    } else return false

/**
 * Print a pin's PWM status.
 * Return true if it's currently a PWM pin.
 */
static bool pwm_status(uint8_t pin) {
  char buffer[20];   // for the sprintf statements

  switch(digitalPinToTimer(pin)) {

    #if defined(TCCR0A) && defined(COM0A1)
      #if defined (TIMER0A)
        PWM_CASE(0,A);
      #endif
      PWM_CASE(0,B);
    #endif

    #if defined(TCCR1A) && defined(COM1A1)
      PWM_CASE(1,A);
      PWM_CASE(1,B);
     #if defined(COM1C1) && defined (TIMER1C)
      PWM_CASE(1,C);
     #endif
    #endif

    #if defined(TCCR2A) && defined(COM2A1)
      PWM_CASE(2,A);
      PWM_CASE(2,B);
    #endif

    #if defined(TCCR3A) && defined(COM3A1)
      PWM_CASE(3,A);
      PWM_CASE(3,B);
     #if defined(COM3C1)
      PWM_CASE(3,C);
     #endif
    #endif

    #ifdef TCCR4A
      PWM_CASE(4,A);
      PWM_CASE(4,B);
      PWM_CASE(4,C);
    #endif

    #if defined(TCCR5A) && defined(COM5A1)
      PWM_CASE(5,A);
      PWM_CASE(5,B);
      PWM_CASE(5,C);
    #endif

    case NOT_ON_TIMER:
    default:
      return false;
  }
  SERIAL_PROTOCOLPGM("  ");
} // pwm_status


#define COM_UNSHIFTED(T,L) (TCCR##T##A & (_BV(COM##T##L##1) | _BV(COM##T##L##0)))

#define COM_PRINT(N,Z)  do { \
     char temp_string[] = STRINGIFY(Z); \
     switch(temp_string[0]) { \
      case 'A' : \
        SERIAL_PROTOCOLPAIR("    COM" STRINGIFY(N) "" STRINGIFY(Z) ": ", (COM_UNSHIFTED(N,Z) >> 6)); \
        break; \
      case 'B' : \
        SERIAL_PROTOCOLPAIR("    COM" STRINGIFY(N) "" STRINGIFY(Z) ": ", (COM_UNSHIFTED(N,Z) >> 4)); \
        break; \
      case 'C' : \
        SERIAL_PROTOCOLPAIR("    COM" STRINGIFY(N) "" STRINGIFY(Z) ": ", (COM_UNSHIFTED(N,Z) >> 2)); \
        break; \
      default:  \
        SERIAL_PROTOCOLPAIR("  com_make L:",  temp_string); \
        SERIAL_PROTOCOLPGM("spacer");\
        for (uint8_t y = 0; y < 28; y++) {     \
          if (temp_string[y] != 0) MYSERIAL.write(temp_string[y]);\
          else break;\
        }\
        break; \
    } \
  } while(0);



#define ICR_VALUE(N)  (ICR##N)
#define CS_MAKE(N) (TCCR##N##B & (_BV(CS##N##0) | _BV(CS##N##1) | _BV(CS##N##2)) )
#define WGM_MAKE3(N) (((TCCR##N##B & _BV(WGM##N##2)) >> 1) | (TCCR##N##A & (_BV(WGM##N##0) | _BV(WGM##N##1))))
#define WGM_MAKE4(N) (WGM_MAKE3(N) | (TCCR##N##B & _BV(WGM##N##3)) >> 1)
#define TIMER_PREFIX(T,L,N) do{ \
    WGM = WGM_MAKE##N(T); \
    SERIAL_PROTOCOLPGM("    TIMER"); \
    SERIAL_PROTOCOLPGM(STRINGIFY(T) STRINGIFY(L)); \
    SERIAL_PROTOCOLPGM("   "); \
    PWM_PRINT(OCR##T##L); \
    SERIAL_PROTOCOLPAIR("    WGM: ", WGM); \
    COM_PRINT(T,L);\
    SERIAL_PROTOCOLPAIR("    CS: ", CS_MAKE(T)); \
    uint8_t temp = T; \
    switch (temp) { \
      case 1: \
        SERIAL_PROTOCOLPAIR("    ICR1: ", ICR1) ; \
        break; \
      case 3: \
        SERIAL_PROTOCOLPAIR("    ICR3: ", ICR3) ; \
        break; \
      case 0: \
      case 2: \
        SERIAL_PROTOCOLPGM("           "); \
        break; \
    } \
    SERIAL_PROTOCOLPAIR("    TCCR" STRINGIFY(T) "A: ", TCCR##T##A); \
    SERIAL_PROTOCOLPAIR("    TCCR" STRINGIFY(T) "B: ", TCCR##T##B); \
    SERIAL_PROTOCOLPAIR("    TIMSK" STRINGIFY(T) ": ", TIMSK##T); \
  }while(0)


#define WGM_TEST1 (WGM == 0 || WGM == 2 || WGM == 4 || WGM == 6)
#define WGM_TEST2 (WGM == 0 || WGM == 4 || WGM == 12 || WGM == 13)

static void err_is_counter() {
  SERIAL_PROTOCOLPGM("   non-standard PWM mode");
}
static void err_is_interrupt() {
  SERIAL_PROTOCOLPGM("   compare interrupt enabled ");

}
static void err_prob_interrupt() {
  SERIAL_PROTOCOLPGM("   overflow interrupt enabled");
}
static void can_be_used() { SERIAL_PROTOCOLPGM("   can be used as PWM   "); }

static void pwm_details(uint8_t pin) {
  char buffer[20];   // for the sprintf statements
  uint8_t WGM;

  switch(digitalPinToTimer(pin)) {


    #if defined(TCCR0A) && defined(COM0A1)

      #if defined (TIMER0A)
        case TIMER0A:
          TIMER_PREFIX(0,A,3);
          if (WGM_TEST1) err_is_counter();
          if (TEST(TIMSK0, OCIE0A)) err_is_interrupt();
          if (TEST(TIMSK0, TOIE0)) err_prob_interrupt();

          break;
      #endif
      case TIMER0B:
        TIMER_PREFIX(0,B,3);
        if (WGM_TEST1) err_is_counter();
        if (TEST(TIMSK0, OCIE0B)) err_is_interrupt();
        if (TEST(TIMSK0, TOIE0)) err_prob_interrupt();

        break;
    #endif

    #if defined(TCCR1A) && defined(COM1A1)
      case TIMER1A:
        TIMER_PREFIX(1,A,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK1, OCIE1A)) err_is_interrupt();
        if (TIMSK1 & (_BV(TOIE1) | _BV(ICIE1))) err_prob_interrupt();

        break;
      case TIMER1B:
        TIMER_PREFIX(1,B,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK1, OCIE1B)) err_is_interrupt();
        if (TIMSK1 & (_BV(TOIE1) | _BV(ICIE1))) err_prob_interrupt();

        break;
      #if defined(COM1C1) && defined (TIMER1C)
        case TIMER1C:
          TIMER_PREFIX(1,C,4);
          if (WGM_TEST2) err_is_counter();
          if (TEST(TIMSK1, OCIE1C)) err_is_interrupt();
          if (TIMSK1 & (_BV(TOIE1) | _BV(ICIE1))) err_prob_interrupt();

          break;
      #endif
    #endif

    #if defined(TCCR2A) && defined(COM2A1)
      case TIMER2A:
        TIMER_PREFIX(2,A,3);
        if (WGM_TEST1) err_is_counter();
        if (TIMSK2 & (_BV(TOIE2) | _BV(OCIE2A))) err_is_interrupt();
        if (TEST(TIMSK2, TOIE2)) err_prob_interrupt();

        break;
      case TIMER2B:
        TIMER_PREFIX(2,B,3);
        if (WGM_TEST1) err_is_counter();
        if (TEST(TIMSK2, OCIE2B)) err_is_interrupt();
        if (TEST(TIMSK2, TOIE2)) err_prob_interrupt();

        break;
    #endif

    #if defined(TCCR3A) && defined(COM3A1)
      case TIMER3A:
        TIMER_PREFIX(3,A,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK3, OCIE3A)) err_is_interrupt();
        if (TIMSK3 & (_BV(TOIE3) | _BV(ICIE3))) err_prob_interrupt();

        break;
      case TIMER3B:
        TIMER_PREFIX(3,B,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK3, OCIE3B)) err_is_interrupt();
        if (TIMSK3 & (_BV(TOIE3) | _BV(ICIE3))) err_prob_interrupt();

        break;
      #if defined(COM3C1)
        case TIMER3C:
          TIMER_PREFIX(3,C,4);
          if (WGM_TEST2) err_is_counter();
          if (TEST(TIMSK3, OCIE3C)) err_is_interrupt();
          if (TIMSK3 & (_BV(TOIE3) | _BV(ICIE3))) err_prob_interrupt();

          break;
      #endif
    #endif

    #ifdef TCCR4A
      case TIMER4A:
        TIMER_PREFIX(4,A,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK4, OCIE4A)) err_is_interrupt();
        if (TIMSK4 & (_BV(TOIE4) | _BV(ICIE4))) err_prob_interrupt();

        break;
      case TIMER4B:
        TIMER_PREFIX(4,B,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK4, OCIE4B)) err_is_interrupt();
        if (TIMSK4 & (_BV(TOIE4) | _BV(ICIE4))) err_prob_interrupt();

        break;
      case TIMER4C:
        TIMER_PREFIX(4,C,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK4, OCIE4C)) err_is_interrupt();
        if (TIMSK4 & (_BV(TOIE4) | _BV(ICIE4))) err_prob_interrupt();

        break;
    #endif

    #if defined(TCCR5A) && defined(COM5A1)
      case TIMER5A:
        TIMER_PREFIX(5,A,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK5, OCIE5A)) err_is_interrupt();
        if (TIMSK5 & (_BV(TOIE5) | _BV(ICIE5))) err_prob_interrupt();

        break;
      case TIMER5B:
        TIMER_PREFIX(5,B,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK5, OCIE5B)) err_is_interrupt();
        if (TIMSK5 & (_BV(TOIE5) | _BV(ICIE5))) err_prob_interrupt();

        break;
      case TIMER5C:
        TIMER_PREFIX(5,C,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK5, OCIE5C)) err_is_interrupt();
        if (TIMSK5 & (_BV(TOIE5) | _BV(ICIE5))) err_prob_interrupt();

        break;
    #endif

    case NOT_ON_TIMER: break;

  }
  SERIAL_PROTOCOLPGM("  ");

// on pins that have two PWMs, print info on second PWM
  #if AVR_ATmega2560_FAMILY || AVR_AT90USB1286_FAMILY
  // looking for port B7 - PWMs 0A and 1C
    if ( ('B' == digitalPinToPort(pin) + 64) && (0x80 == digitalPinToBitMask(pin))) {
      #if !defined(TEENSYDUINO_IDE)
        SERIAL_EOL;
        SERIAL_PROTOCOLPGM (" .                  TIMER1C is also tied to this pin             ");
        TIMER_PREFIX(1,C,4);
        if (WGM_TEST2) err_is_counter();
        if (TEST(TIMSK1, OCIE1C)) err_is_interrupt();
        if (TIMSK1 & (_BV(TOIE1) | _BV(ICIE1))) err_prob_interrupt();
      #else  
        SERIAL_EOL;
        SERIAL_PROTOCOLPGM (" .                  TIMER0A is also tied to this pin             ");
        TIMER_PREFIX(0,A,3);
        if (WGM_TEST1) err_is_counter();
        if (TEST(TIMSK0, OCIE0A)) err_is_interrupt();
        if (TEST(TIMSK0, TOIE0)) err_prob_interrupt();
      #endif  
    }
  #endif
} // pwm_details

bool get_pinMode(int8_t pin) { return *portModeRegister(digitalPinToPort(pin)) & digitalPinToBitMask(pin); }

#if !defined(digitalRead_mod)    // use Teensyduino's version of digitalRead - it doesn't disable the PWMs
  int digitalRead_mod(int8_t pin) { // same as digitalRead except the PWM stop section has been removed
    uint8_t port = digitalPinToPort(pin);
    return (port != NOT_A_PIN) && (*portInputRegister(port) & digitalPinToBitMask(pin)) ? HIGH : LOW;
  }
#endif

void print_port(int8_t pin) {   // print port number
  #if defined(digitalPinToPort)
    SERIAL_PROTOCOLPGM("  Port: ");
    uint8_t x = digitalPinToPort(pin) + 64;
    SERIAL_CHAR(x);
    uint8_t temp = digitalPinToBitMask(pin);
    for (x = '0'; (x < '9' && !(temp == 1)); x++){
      temp = temp >> 1;
    }
    SERIAL_CHAR(x);
  #else
    SERIAL_PROTOCOLPGM("          ")
  #endif
}

// pretty report with PWM info
inline void report_pin_state_extended(int8_t pin, bool ignore, bool extended = true) {
  uint8_t temp_char;
  char *name_mem_pointer;
  char buffer[30];   // for the sprintf statements
  bool found = false;
  bool multi_name_pin = false;
  for (uint8_t x = 0; x < n_array; x++)  {    // scan entire array and report all instances of this pin
    if (pgm_read_byte(&pin_array[x][1]) == pin) {
      if (found == true) multi_name_pin = true;
      found = true;
      if (multi_name_pin == false) {    // report digitial and analog pin number only on the first time through
        sprintf(buffer, "PIN:% 3d ", pin);     // digital pin number
        SERIAL_ECHO(buffer);
        print_port(pin);
        if (IS_ANALOG(pin)) {
          sprintf(buffer, " (A%2d)  ", int(pin - analogInputToDigitalPin(0)));    // analog pin number
          SERIAL_ECHO(buffer);
        }
        else SERIAL_ECHOPGM("        ");   // add padding if not an analog pin
      }
      else SERIAL_ECHOPGM(".                         ");  // add padding if not the first instance found
      name_mem_pointer = (char*) pgm_read_word(&pin_array[x][0]);
      for (uint8_t y = 0; y < 28; y++) {                   // always print pin name
        temp_char = pgm_read_byte(name_mem_pointer + y);
        if (temp_char != 0) MYSERIAL.write(temp_char);
        else {
          for (uint8_t i = 0; i < 28 - y; i++) MYSERIAL.write(" ");
          break;
        }
      }
      if (pin_is_protected(pin) && !ignore)
        SERIAL_ECHOPGM("protected ");
      else {
        if (!(pgm_read_byte(&pin_array[x][2]))) {
          sprintf(buffer, "Analog in =% 5d", analogRead(pin - analogInputToDigitalPin(0)));
          SERIAL_ECHO(buffer);
        }
        else {
          if (!get_pinMode(pin)) {
            pinMode(pin, INPUT_PULLUP);  // make sure input isn't floating
            SERIAL_PROTOCOLPAIR("Input  = ", digitalRead_mod(pin));
          }
          else if (pwm_status(pin)) {
            // do nothing
          }
          else SERIAL_PROTOCOLPAIR("Output = ", digitalRead_mod(pin));
        }
        if (multi_name_pin == false && extended) pwm_details(pin);  // report PWM capabilities only on the first pass & only if doing an extended report
      }
      SERIAL_EOL;
    }  // end of IF
  } // end of for loop

  if (found == false) {
    sprintf(buffer, "PIN:% 3d ", pin);
    SERIAL_ECHO(buffer);
    print_port(pin);
    if (IS_ANALOG(pin)) {
      sprintf(buffer, " (A%2d)  ", int(pin - analogInputToDigitalPin(0)));    // analog pin number
      SERIAL_ECHO(buffer);
    }
    else SERIAL_ECHOPGM("        ");   // add padding if not an analog pin
    sprintf(buffer, NAME_FORMAT, "<unused> ");
    SERIAL_ECHO(buffer);
    if (!pwm_status(pin)) SERIAL_ECHOPGM("          ");    // add padding if it's not a PWM pin
    if (extended) pwm_details(pin);  // report PWM capabilities only if doing an extended report
    SERIAL_EOL;
  }
}


inline void report_pin_state(int8_t pin) {

  report_pin_state_extended(pin, false, false);

}
