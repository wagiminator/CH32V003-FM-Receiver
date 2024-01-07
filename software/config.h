// ===================================================================================
// User Configuration
// ===================================================================================

// Pin assignments
#define PIN_SDA         PC1           // I2C Serial Data,  connect to OLED/RDA
#define PIN_SCL         PC2           // I2C Serial Clock, connect to OLED/RDA
#define PIN_VOL_UP      PC4           // VOL+ button
#define PIN_CH_UP       PA1           // CH+  button
#define PIN_VOL_DOWN    PA2           // VOL- button

// Firmware parameters
#define RDA_INIT_VOL    3             // volume on system start (0..15)
#define RDA_INIT_FREQ   102.60        // channel frequency on system start
#define OLED_CONTRAST   96            // 0 .. 255    brightness of OLED
#define OLED_HEADER     "FM Pocket Radio  v1.0"   // text on top of the screen
