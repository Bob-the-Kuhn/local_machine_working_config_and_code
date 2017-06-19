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

/**
 * AZTEEG_X3_PRO (Arduino Mega) pin assignments
 */

#if HOTENDS > 5 || E_STEPPERS > 5
  #error "Azteeg X3 Pro supports up to 5 hotends / E-steppers. Comment out this line to continue."
#endif

#if ENABLED(CASE_LIGHT_ENABLE)  && !PIN_EXISTS(CASE_LIGHT)
  #define CASE_LIGHT_PIN 44     // must define it here or else RAMPS will define it
#endif


#define BOARD_NAME "Azteeg X3 Pro"

#include "pins_RAMPS.h"

#ifndef __AVR_ATmega2560__
  #error "Oops! Make sure you have 'Arduino Mega 2560' selected from the 'Tools -> Boards' menu."
#endif
#define SPINDLE_LASER_ENABLE_PIN  45  
#define SPINDLE_LASER_PWM_PIN     44
#define SPINDLE_DIR_PIN           21

//
// Servos
//
// Tested this pin with bed leveling on a Delta with 1 servo.
// Physical wire attachment on EXT1: GND, 5V, D47.
//
#undef SERVO0_PIN
#undef SERVO1_PIN
#undef SERVO2_PIN
#define SERVO0_PIN         57
//#define SERVO1_PIN  64

  #undef X_MIN_PIN
  #undef X_MAX_PIN
  #undef Y_MIN_PIN
  #undef Y_MAX_PIN
  #undef Z_MIN_PIN
  #undef Z_MAX_PIN

//
// Limit Switches
//
// Swap the MIN and MAX endstop pins because the X3 Pro comes with only
// MIN endstop pin headers soldered onto the board.
//
#if ENABLED(DELTA)
  #define X_MIN_PIN         2
  #define X_MAX_PIN         3
  #define Y_MIN_PIN        15
  #define Y_MAX_PIN        14
  #define Z_MIN_PIN        19
  #define Z_MAX_PIN        18
#else

  #define X_MIN_PIN         3
  #define X_MAX_PIN         2
  #define Y_MIN_PIN        15  // 14 reversed from normal order because of cable length
  #define Y_MAX_PIN        14  // 15 reversed from normal order because of cable length
  #define Z_MIN_PIN        19  // 18  reversed to get DUAL_Z_ENDSTOPS to work
  #define Z_MAX_PIN        18  // 19

#endif

//
// Z Probe (when not Z_MIN_PIN)
//
#undef Z_MIN_PROBE_PIN
#define Z_MIN_PROBE_PIN  64  

//
// Steppers
//
//#define E2_STEP_PIN        23  // E2 socket is bad
//#define E2_DIR_PIN         25
//#define E2_ENABLE_PIN      40

#define E2_STEP_PIN        27  // shift Z2 stepper to E3 socket
#define E2_DIR_PIN         29  // but must tell Marlin it's E2
#define E2_ENABLE_PIN      41

#define E4_STEP_PIN        43
#define E4_DIR_PIN         37
//#define E4_ENABLE_PIN      42

//
// Temperature Sensors
//
#define TEMP_2_PIN         12   // Analog Input
#define TEMP_3_PIN         11   // Analog Input
#define TEMP_4_PIN         10   // Analog Input
#define TC1                 4   // Analog Input (Thermo couple on Azteeg X3Pro)
#define TC2                 5   // Analog Input (Thermo couple on Azteeg X3Pro)

//
// Heaters / Fans
//
//
// Heaters / Fans
//
#define HEATER_1_PIN       11
#define HEATER_2_PIN       16
#define HEATER_3_PIN       17
//#define HEATER_4_PIN        4
//#define HEATER_5_PIN        5
//#define HEATER_6_PIN        6
//#define HEATER_7_PIN       11

#undef FAN_PIN
#define FAN_PIN             6 //Part Cooling System


//#define CONTROLLERFAN_PIN   4 //Pin used for the fan to cool motherboard (-1 to disable)


// Fans/Water Pump to cool the hotend cool side.
#undef RAMPS_D9_PIN
#define E0_AUTO_FAN_PIN   9             // so don't get confused by the debug pins report
#define E1_AUTO_FAN_PIN   9
#define E2_AUTO_FAN_PIN   9
#define E3_AUTO_FAN_PIN   9
#define E4_AUTO_FAN_PIN   9

//
// LCD / Controller
//
#undef BEEPER_PIN
#define BEEPER_PIN         33

#if ENABLED(VIKI2) || ENABLED(miniVIKI)
  #undef  BEEPER_PIN



  #undef  BTN_EN1
  #undef  BTN_EN2
  #undef  BTN_ENC
  #undef  DOGLCD_A0
  #undef  DOGLCD_CS
  #undef  KILL_PIN
  #undef  SDSS
  #undef  SD_DETECT_PIN
  #undef  STAT_LED_BLUE_PIN
  #undef  STAT_LED_RED_PIN

  #define  BEEPER_PIN  12
  #define  BTN_EN1   22
  #define  BTN_EN2  7
  #define  BTN_ENC   39
  #define  DOGLCD_A0   42
  #define  DOGLCD_CS  47
  #define  KILL_PIN  31
  #define  SDSS 53
  #define  SD_DETECT_PIN   49
  #define  STAT_LED_BLUE_PIN    35
  #define  STAT_LED_RED_PIN 32
#endif

//
// Misc. Functions
//
#if ENABLED(CASE_LIGHT_ENABLE)  && PIN_EXISTS(CASE_LIGHT) && defined(DOGLCD_A0) && DOGLCD_A0 == CASE_LIGHT_PIN
  #undef DOGLCD_A0            // Steal pin 44 for the case light; if you have a Viki2 and have connected it
  #define DOGLCD_A0        57 // following the Panucatt wiring diagram, you may need to tweak these pin assignments
                              // as the wiring diagram uses pin 44 for DOGLCD_A0
#endif

/**
//
// M3/M4/M5 - Spindle/Laser Control
//
#undef SPINDLE_LASER_PWM_PIN    // Definitions in pins_RAMPS.h are no good with the AzteegX3pro board
#undef SPINDLE_LASER_ENABLE_PIN
#undef SPINDLE_DIR_PIN

#if ENABLED(SPINDLE_LASER_ENABLE)   // use EXP2 header
  #if ENABLED(VIKI2) || ENABLED(miniVIKI)
    #undef BTN_EN2
    #define BTN_EN2             31  // need 7 for the spindle speed PWM
  #endif
  #define SPINDLE_LASER_PWM_PIN     7  // must have a hardware PWM
  #define SPINDLE_LASER_ENABLE_PIN 20  // Pin should have a pullup!
  #define SPINDLE_DIR_PIN          21
#endif
*/
