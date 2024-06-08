// ===================================================================================
// Project:   FM Radio Receiver with RDS based on CH32V003 and RDA5807MP
// Version:   v1.2
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

// Libraries
#include <config.h>                         // user configuration
#include <gpio.h>                           // GPIO functions
#include <ssd1306_gfx.h>                    // OLED functions
#include <rda5807.h>                        // RDA 5807 functions

// Global Variables
uint8_t volume = RDA_INIT_VOL;              // current volume (0..15)

// ===================================================================================
// OLED Symbols
// ===================================================================================
const uint8_t BAT_OK[] = {
  0x7C, 0x04, 0x07, 0x01, 0x07, 0x84, 0xDC, 0x73, 0x46, 0x4C, 0x46, 0x43, 0x41, 0x7C
};

const uint8_t BAT_WEAK[] = {
  0x34, 0x64, 0xC7, 0x81, 0xC7, 0x64, 0x34, 0x58, 0x4C, 0x46, 0x43, 0x46, 0x4C, 0x58
};

const uint8_t ANT_BIG[] = {
  0x03, 0x0C, 0x30, 0xFF, 0x30, 0x0C, 0x03, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00
};

const uint8_t ANT[] = {
  0x01, 0x02, 0x04, 0x7F, 0x04, 0x02, 0x01
};

// ===================================================================================
// OLED Update Function
// ===================================================================================
void OLED_update(void) {
  RDA_updateStatus();

  OLED_clear();
  OLED_cursor(0, 0); OLED_textsize(OLED_SMOOTH); OLED_print(RDA_stationName);
  OLED_drawBitmap(121, 0, 7, 16, PVD_isLow() ? BAT_WEAK : BAT_OK);

  OLED_cursor(-10, 20); OLED_printSegment(RDA_getFrequency(), 5, 1, 2);
  OLED_cursor(94, 36); OLED_print("MHz");

  OLED_drawBitmap(94, 20, 7, 8, ANT);
  OLED_drawRect(104, 20, 24, 7, 1);
  uint8_t strength = RDA_signalStrength;
  if(strength > 64) strength = 64;
  strength = (strength >> 2) + (strength >> 4);
  if(strength) OLED_fillRect(106, 22, strength, 3, 1);

  OLED_cursor(0, 56); OLED_textsize(1); OLED_print("Volume:");
  OLED_drawRect(50, 56, 78, 7, 1);
  uint8_t xpos = 47;
  uint8_t vol  = volume;
  while(vol--) OLED_fillRect(xpos+=5, 58, 4, 3, 1);

  OLED_refresh();
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint8_t CH_UP_state = 0;                  // CH+ button state

  // Setup
  PIN_input_PU(PIN_VOL_UP);                 // enable pullups for button pins
  PIN_input_PU(PIN_VOL_DOWN);
  PIN_input_PU(PIN_CH_UP);
  PVD_enable();                             // enable programmable voltage detector
  PVD_set_3V15();                           // supply voltage detection level 3.15V
  I2C_init();                               // init I2C
  OLED_init();                              // setup OLED
  RDA_init();                               // setup RDA tuner
  OLED_update();                            // draw screen

  // Set initial frequency
  RDA_setChannel((uint16_t)((RDA_INIT_FREQ - 87.0) * 10));
  RDA_waitTuning();
  RDA_seekUp();

  // Loop
  while(1) {
    // Update information on OLED
    OLED_update();

    // Check CH+ button
    if(!PIN_read(PIN_CH_UP)) {
      if(!CH_UP_state) RDA_seekUp();
      DLY_ms(20);
    }
    CH_UP_state = !PIN_read(PIN_CH_UP);

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
