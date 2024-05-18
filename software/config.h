// ===================================================================================
// User Configuration
// ===================================================================================

// Pin assignments
#define PIN_SDA         PC1           // I2C Serial Data,  connect to OLED/RDA
#define PIN_SCL         PC2           // I2C Serial Clock, connect to OLED/RDA
#define PIN_VOL_UP      PC4           // VOL+ button (active low)
#define PIN_CH_UP       PA1           // CH+  button (active low)
#define PIN_VOL_DOWN    PA2           // VOL- button (active low)

// Firmware parameters
#define RDA_INIT_VOL    3             // volume on system start (0..15)
#define RDA_INIT_FREQ   102.60        // channel frequency on system start
#define RDA_HEADER      "FM Radio"    // default station name (8 characters)
#define OLED_BRIGHTNESS 96            // 0 .. 255 brightness of OLED
