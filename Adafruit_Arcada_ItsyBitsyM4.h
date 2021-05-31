// Header file to use Adafruit_Arcada library on ItsyBitsy M4 with two
// GC9A01A displays. Allows M4_Eyes sketch to run on this hardware combo.
// As written, enables two screens. See Adafruit_Arcada_FeatherM4.h for
// insights on handling just one screen.
// Place file in Adafruit_Arcada/Boards and add line to Adafruit_Arcada.h:
// #include "Boards/Adafruit_Arcada_ItsyBitsyM4.h"

// This code supplied as-is and is not provided nor supported by Adafruit.

#if defined(ADAFRUIT_ITSYBITSY_M4_EXPRESS)
#include <Adafruit_GC9A01A.h>
#include "wiring_private.h"
#include <Adafruit_DotStar.h>

#define ARCADA_TFT_SPI SPI
#define ARCADA_TFT_CS 9  // Display CS Arduino pin number
#define ARCADA_TFT_DC 7  // Display D/C Arduino pin number
#define ARCADA_TFT_RST -1 // Display reset Arduino pin number
//#define ARCADA_TFT_LITE 47
#define ARCADA_TFT_ROTATION 1
#define ARCADA_TFT_DEFAULTFILL 0xFFFF
#define ARCADA_TFT_WIDTH 240
#define ARCADA_TFT_HEIGHT 240
// SECOND DISPLAY ON SEPARATE SPI BUS:
#if defined(GLOBAL_VAR)
// Default SPI is on sercom0, I2C on sercom1. sercom3 is normally serial1
// but let use it for 2nd SPI. Pins 0 and 1 are RX, TX respectively - that's
// PORTA16, 17. PIO_SERCOM_ALT settings are SERCOM3/PAD1 and SERCOM3/PAD0.
// This puts MOSI on TX, SCK on RX.
SPIClass SPI1(&sercom3, 0, 1, 2, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
// Because we don't set pinPeripheral() for pin 2 (MOSI isn't needed for
// eyes nor provided on breakout), the pin should still be usable as a
// digital output (e.g. if wanting to use for CS or DC).
#else
extern SPIClass SPI1;
#endif
// Recent M4_eyes change checks for ARCADA_LEFTTFT_SPI to enable 2 eyes.
#define ARCADA_LEFTTFT_SPI SPI1
#define ARCADA_LEFTTFT_CS A5  // Display CS Arduino pin number
#define ARCADA_LEFTTFT_DC A4  // Display D/C Arduino pin number
#define ARCADA_LEFTTFT_RST -1 // Display reset Arduino pin number

#define TFT_RESET 10 // Single reset used for BOTH displays

#define ARCADA_AUDIO_OUT A0

#define ARCADA_CALLBACKTIMER 4
#define ARCADA_CALLBACKTIMER_HANDLER TC4_Handler

#define ARCADA_USE_JSON

#include "arcadatype.h"

class Adafruit_Arcada : public Adafruit_Arcada_SPITFT {
public:
  Adafruit_Arcada(void) { _has_accel = false; };
  Adafruit_GC9A01A *_display, *display2;

  bool variantBegin(void) {
    Adafruit_DotStar dot = Adafruit_DotStar(1, 8, 6);
    dot.show(); // Turn off DotStar
    return true;
  }

  void displayBegin(void) {
    // Hard reset both displays - must do this before begin()
    pinMode(TFT_RESET, OUTPUT);
    digitalWrite(TFT_RESET, LOW);
    delay(10);
    digitalWrite(TFT_RESET, HIGH);
    delay(10);

    pinPeripheral(0, PIO_SERCOM_ALT); // Enable SPI1 on RX, TX
    pinPeripheral(1, PIO_SERCOM_ALT);

    _display = new Adafruit_GC9A01A(&ARCADA_TFT_SPI, ARCADA_TFT_DC,
                                    ARCADA_TFT_CS, ARCADA_TFT_RST);
    display2 = new Adafruit_GC9A01A(&ARCADA_LEFTTFT_SPI, ARCADA_LEFTTFT_DC,
                                    ARCADA_LEFTTFT_CS, ARCADA_LEFTTFT_RST);
    _display->begin();
    display2->begin();

    uint8_t rtna =
        0x01; // Screen refresh rate control (datasheet 9.2.18, FRCTRL2)
    _display->sendCommand(0xC6, &rtna, 1);
    display2->sendCommand(0xC6, &rtna, 1);
    _display->setRotation(ARCADA_TFT_ROTATION);
    display2->setRotation(ARCADA_TFT_ROTATION);
    _display->setSPISpeed(50000000); // yes fast
    display2->setSPISpeed(50000000); // 50 MHz!
    _display->fillScreen(ARCADA_TFT_DEFAULTFILL);
    display2->fillScreen(ARCADA_TFT_DEFAULTFILL);
    display = _display;
  }

  uint32_t variantReadButtons(void) {
    return 0;
  }

private:
};

#endif
