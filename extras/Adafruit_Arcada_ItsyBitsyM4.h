#if defined(ADAFRUIT_ITSYBITSY_M4_EXPRESS)
// INCLUDE ONLY **ONE** OF THESE DISPLAY TYPES:
#include <Adafruit_GC9A01A.h> // Round 240 pixel diameter TFT
// #include <Adafruit_ST7789.h>   // Square 240x240 TFT
// #include <Adafruit_ILI9341.h>  // Rectangular 320x240 TFT
#include "wiring_private.h"
#include <Adafruit_DotStar.h>

#define TWO_DISPLAYS

#define ARCADA_TFT_SPI SPI
#define ARCADA_TFT_DC A5
#define ARCADA_TFT_CS A4
#define ARCADA_TFT_RST -1
// #define ARCADA_TFT_LITE 47
#define ARCADA_TFT_ROTATION 1
#define ARCADA_TFT_DEFAULTFILL 0xFFFF
#if defined(_ADAFRUIT_ILI9341H_)
#define ARCADA_TFT_WIDTH 320
#else
#define ARCADA_TFT_WIDTH 240
#endif
#define ARCADA_TFT_HEIGHT 240

#if defined(TWO_DISPLAYS)
// SECOND DISPLAY ON SEPARATE SPI BUS:
#if defined(GLOBAL_VAR)
// SPI is on sercom0, I2C on sercom1, 2-5 are available
// Sercom3 is normally serial1 but let use it for 2nd SPI
// 0 and 1 are RX, TX respectively - PORTA16, 17
// sercom_alts are SERCOM3/PAD1 and SERCOM3/PAD0
// This puts MOSI on TX, SCK on RX
SPIClass SPI1(&sercom3, 0, 1, 2, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
#else
extern SPIClass SPI1;
#endif
#define ARCADA_LEFTTFT_SPI SPI1
#define ARCADA_LEFTTFT_DC 9
#define ARCADA_LEFTTFT_CS 7
#define ARCADA_LEFTTFT_RST -1
#endif // end TWO_DISPLAYS

#define TFT_RESET 2 // Used for BOTH displays

#define ARCADA_AUDIO_OUT A0

#define ARCADA_CALLBACKTIMER 4
#define ARCADA_CALLBACKTIMER_HANDLER TC4_Handler

#define ARCADA_USE_JSON

#include "arcadatype.h"

class Adafruit_Arcada : public Adafruit_Arcada_SPITFT {
public:
  Adafruit_Arcada(void) { _has_accel = false; };
#if defined(_ADAFRUIT_ST7789H_)
  Adafruit_ST7789 *_display, *display2;
#elif defined(_ADAFRUIT_ILI9341H_)
  Adafruit_ILI9341 *_display, *display2;
#else
  Adafruit_GC9A01A *_display, *display2;
#endif

  bool variantBegin(void) {
    Adafruit_DotStar dot = Adafruit_DotStar(1, 8, 6);
    dot.show(); // Turn off DotStar
    return true;
  }

  void displayBegin(void) {
    // Hard reset both displays
    pinMode(TFT_RESET, OUTPUT);
    digitalWrite(TFT_RESET, LOW);
    delay(10);
    digitalWrite(TFT_RESET, HIGH);
    delay(10);

    // IMPORTANT: CS and DC are different order among displays
#if defined(_ADAFRUIT_ST7789H_)
    _display = new Adafruit_ST7789(&ARCADA_TFT_SPI, ARCADA_TFT_CS,
                                   ARCADA_TFT_DC, ARCADA_TFT_RST);
    _display->init(240, 240);
#elif defined(_ADAFRUIT_ILI9341H_)
    _display = new Adafruit_ILI9341(&ARCADA_TFT_SPI, ARCADA_TFT_DC,
                                    ARCADA_TFT_CS, ARCADA_TFT_RST);
    _display->begin();
#else
    uint8_t rtna = 0x01; // Screen refresh rate control
    _display = new Adafruit_GC9A01A(&ARCADA_TFT_SPI, ARCADA_TFT_DC,
                                    ARCADA_TFT_CS, ARCADA_TFT_RST);
    _display->begin();
    _display->sendCommand(0xC6, &rtna, 1); // Helps contrast a little
#endif
    _display->setSPISpeed(50000000); // yes fast
    _display->setRotation(ARCADA_TFT_ROTATION);
    _display->fillScreen(ARCADA_TFT_DEFAULTFILL);

#if defined(ARCADA_LEFTTFT_SPI)
    pinPeripheral(0, PIO_SERCOM_ALT); // Enable SPI1 on RX, TX
    pinPeripheral(1, PIO_SERCOM_ALT);
    // IMPORTANT: CS and DC are different order among displays
#if defined(_ADAFRUIT_ST7789H_)
    display2 = new Adafruit_ST7789(&ARCADA_LEFTTFT_SPI, ARCADA_LEFTTFT_CS,
                                   ARCADA_LEFTTFT_DC, ARCADA_LEFTTFT_RST);
    display2->init(240, 240);
#elif defined(_ADAFRUIT_ILI9341H_)
    display2 = new Adafruit_ILI9341(&ARCADA_LEFTTFT_SPI, ARCADA_LEFTTFT_DC,
                                    ARCADA_LEFTTFT_CS, ARCADA_LEFTTFT_RST);
    display2->begin();
#else
    display2 = new Adafruit_GC9A01A(&ARCADA_LEFTTFT_SPI, ARCADA_LEFTTFT_DC,
                                    ARCADA_LEFTTFT_CS, ARCADA_LEFTTFT_RST);
    display2->begin();
    display2->sendCommand(0xC6, &rtna, 1); // Helps contrast a little
#endif
    display2->setSPISpeed(50000000);
    display2->setRotation(ARCADA_TFT_ROTATION);
    display2->fillScreen(ARCADA_TFT_DEFAULTFILL);
#endif

    display = _display;
  }

  uint32_t variantReadButtons(void) { return 0; }

private:
};

#endif
