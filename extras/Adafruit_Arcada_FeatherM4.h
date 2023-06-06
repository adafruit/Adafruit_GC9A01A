// Header file to use Adafruit_Arcada library on Feather M4 with GC9A01A
// displays. Allows M4_Eyes sketch to run on this hardware combo.
// As written, this only handles a single GC9A01A screen. See
// Adafruit_Arcada_ItsyBitsyM4.h for insights on handling 2 screens
// (requires setting up a second SPI SERCOM).
// Place file in Adafruit_Arcada/Boards and add line to Adafruit_Arcada.h:
// #include "Boards/Adafruit_Arcada_FeatherM4.h"

#if defined(_VARIANT_FEATHER_M4_)
#include <Adafruit_GC9A01A.h>

#define ARCADA_TFT_SPI SPI
#define ARCADA_TFT_CS 10  // Display CS Arduino pin number
#define ARCADA_TFT_DC 9   // Display D/C Arduino pin number
#define ARCADA_TFT_RST -1 // Display reset Arduino pin number
#define ARCADA_TFT_ROTATION 2
#define ARCADA_TFT_DEFAULTFILL 0xFFFF
#define ARCADA_TFT_WIDTH 240
#define ARCADA_TFT_HEIGHT 240
// #define ARCADA_TFT_LITE 47

#define ARCADA_AUDIO_OUT A0

#define ARCADA_CALLBACKTIMER 4
#define ARCADA_CALLBACKTIMER_HANDLER TC4_Handler

#define ARCADA_USE_JSON

#include "arcadatype.h"

class Adafruit_Arcada : public Adafruit_Arcada_SPITFT {
public:
  Adafruit_Arcada(void) { _has_accel = false; };

  bool variantBegin(void) { return true; }

  void displayBegin(void) {
    Adafruit_GC9A01A *tft = new Adafruit_GC9A01A(&ARCADA_TFT_SPI, ARCADA_TFT_DC,
                                                 ARCADA_TFT_CS, ARCADA_TFT_RST);
    tft->begin();
    tft->setRotation(ARCADA_TFT_ROTATION);
    tft->fillScreen(ARCADA_TFT_DEFAULTFILL);
    uint8_t rtna =
        0x01; // Screen refresh rate control (datasheet 9.2.18, FRCTRL2)
    tft->sendCommand(0xC6, &rtna, 1);
    tft->setSPISpeed(50000000); // yes fast
    display = tft;
  }

  uint32_t variantReadButtons(void) { return 0; }

private:
};

#endif
