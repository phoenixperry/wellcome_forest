#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include "hsv.h"

#define LED_PIN 10
#define LED_COUNT 16
#define BRIGHTNESS 255

#define BUTTON_LED 11
#define BUTTON_OUT 12
#define BUTTON_IN 13

bool buttonPressed = false;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pinMode(BUTTON_LED, OUTPUT);
  pinMode(BUTTON_IN, INPUT);
  pinMode(BUTTON_OUT, OUTPUT);

  digitalWrite(BUTTON_OUT, HIGH);
  digitalWrite(BUTTON_LED, HIGH);

  strip.begin();                   // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                    // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
}

float col[3];
float hueDefault = 0.0;

void loop()
{
  if (digitalRead(BUTTON_IN) == LOW)
  {
    buttonPressed = true;
    digitalWrite(BUTTON_LED, LOW);
  }

  // rainbow
  if (buttonPressed)
  {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      int hue = i * 255 * 6 / strip.numPixels();
      strip.setPixelColor(i, hsv2rgb(hue, 255, 255));
    }
    strip.show();

    delay(1000);
    for (float i = 0; i < PI / 2 + 0.01; i += 0.005)
    {
      float val = sin(i);
      int out = (int)(val * 255);
      analogWrite(BUTTON_LED, out);
      delay(10);
    }
    buttonPressed = false;
    digitalWrite(BUTTON_LED, HIGH);
  }
  else
  {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, hsv2rgb(hueDefault, 255, 255));
      strip.show();
      hueDefault += 0.2;
      if (hueDefault > 255 * 6)
      {
        hueDefault = 0.0;
      }
    }
  }
}