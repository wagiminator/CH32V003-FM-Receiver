// ===================================================================================
// Project:   FM Radio Receiver with RDS based on CH32V003 and RDA5807MP
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// This firmware implements the basic functionality of the Pocket Radio. By pressing
// the CH+ button the RDA5807 seeks the next radio station, presseng the VOL+/VOL-
// button increases/decreases the volume. Station name, frequency, signal strength, 
// volume and battery state of charge are shown on an OLED display.
//
// References:
// -----------
// RDA5807 datasheet:
// https://datasheet.lcsc.com/szlcsc/1806121226_RDA-Microelectronics-RDA5807MP_C167245.pdf
//
// SSD1306 OLED datasheet:
// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
//
// The implementation is based on the ATtiny412-version (PocketRadio):
// https://github.com/wagiminator/ATtiny412-PocketRadio
//
// The OLED implementation was adapted from Neven Boyanov and Stephen Denne:
// https://github.com/datacute/Tiny4kOLED
//
// The RDA8507 implementation was adapted from Maarten Janssen:
// https://hackaday.io/project/9009-arduino-radio-with-rds
//
// Wiring:
// -------
//                           +-\/-+
//      CH+ button --- PA1  1|°   |8  PD1 DIO --- PROG
//                     GND  2|    |7  PC4 ------- VOL+ button
//     VOL- button --- PA2  3|    |6  PC2 SCL --- I2C OLED/RDA5807
//                     VDD  4|    |5  PC1 SDA --- I2C OLED/RDA5807
//                           +----+
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                         // user configuration
#include <system.h>                         // system functions
#include <gpio.h>                           // GPIO functions
#include <oled.h>                           // OLED functions
#include <rda5807.h>                        // RDA 5807 functions
#include <i2c_soft.h>                       // I2C functions

// ADC check supply voltage; return "TRUE" if battery is weak
uint8_t BAT_isWeak(void) {
  return(ADC_read() > 384);                 // true if VCC < 3.2V (384=1023*1.2V/3.2V)
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint8_t volume = RDA_INIT_VOL;

  // Setup
  PIN_input_PU(PIN_VOL_UP);                 // enable pullups for button pins
  PIN_input_PU(PIN_VOL_DOWN);
  PIN_input_PU(PIN_CH_UP);
  ADC_init();                               // setup ADC
  ADC_input_VREF();                         // internal reference as ADC input
  I2C_init();                               // init I2C
  OLED_init();                              // setup OLED

  // Print start info
  OLED_clearScreen();
  OLED_println(OLED_HEADER);
  OLED_print("Starting ...");

  // Start the tuner
  RDA_init();
  RDA_setChannel((uint16_t)((RDA_INIT_FREQ - 87.0) * 10) + 1);
  RDA_waitTuning();

  // Loop
  while(1) {
    // Update information on OLED
    RDA_updateStatus();
    OLED_setCursor(0, 1);
    OLED_print("Station:  ");
    OLED_println(RDA_stationName);
    OLED_print("Vol: ");
    OLED_printVal(volume, 2, 0);
    OLED_print("   Frq: ");
    OLED_printVal(RDA_getFrequency(), 5, 2);
    OLED_print("Sig: ");
    OLED_printVal(RDA_signalStrength, 2, 0);
    OLED_print("   Bat: ");
    if(BAT_isWeak()) OLED_println("weak");
    else             OLED_println("OK");

    // Check CH+ button
    if(!PIN_read(PIN_CH_UP)) {
      OLED_setCursor(0, 1);
      OLED_println("Tuning ...");
      OLED_clearLine(); OLED_clearLine();
      RDA_seekUp();
      RDA_waitTuning();
      while(!PIN_read(PIN_CH_UP));
    }

    // Check VOL+ button
    if(!PIN_read(PIN_VOL_UP)) {
      if(volume < 15) volume++;
      RDA_setVolume(volume);
      while(!PIN_read(PIN_VOL_UP));
    }

    // Check VOL- button
    if(!PIN_read(PIN_VOL_DOWN)) {
      if(volume) volume--;
      RDA_setVolume(volume);
      while(!PIN_read(PIN_VOL_DOWN));
    }
  }
}
