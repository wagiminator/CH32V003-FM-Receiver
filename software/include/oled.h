// ===================================================================================
// SSD1306 128x64 Pixels OLED Functions                                       * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306 128x64 pixels
// I2C OLED for the display of text.
//
// Functions available:
// --------------------
// OLED_init()              init OLED display
// OLED_clearScreen()       clear screen
// OLED_clearLine()         clear rest of the current line
// OLED_setCursor(x, y)     set the cursor to specified position
// OLED_print(str)          print a string
// OLED_println(str)        print a string with new line
//
// OLED_printVal(val, digits, decimal)    print value with number of digits and 
//                                        decimal on specified position
//
// References:
// -----------
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - TinyOLEDdemo:          https://github.com/wagiminator/attiny13-tinyoleddemo
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "i2c_soft.h"

void OLED_init(void);                             // OLED init function
void OLED_clearScreen(void);                      // OLED clear screen
void OLED_clearLine(void);                        // OLED clear rest of the current line
void OLED_setCursor(uint8_t xpos, uint8_t ypos);  // OLED set the cursor
void OLED_print(const char* str);                 // OLED print a string
void OLED_println(const char* str);               // OLED print a string with new line

// OLED print value with number of digits and decimal on specified position
void OLED_printVal(uint16_t value, uint8_t digits, uint8_t decimal);

#ifdef __cplusplus
};
#endif
