#include "FastLED.h"
#include "AsyncDelay.h"

#define LED_PIN 10
#define LED_COUNT 16
#define BRIGHTNESS 255

#define BUTTON_LED 11
#define BUTTON_OUT 12
#define BUTTON_IN 13

CRGB leds[LED_COUNT];
AsyncDelay press_delay;
float hue = 0.0;
bool button_pressed = false;

void setup()
{
  // setup neopixel
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  // setup button
  pinMode(BUTTON_LED, OUTPUT);
  pinMode(BUTTON_IN, INPUT);
  pinMode(BUTTON_OUT, OUTPUT);
  digitalWrite(BUTTON_OUT, HIGH);
  digitalWrite(BUTTON_LED, HIGH);
}

void loop()
{
  if (button_pressed)
  {
    hue = hue > 255 ? 0.0 : hue += 0.2;
    FastLED.showColor(CHSV(hue, 255, 255));
    if (press_delay.isExpired())
    {
      hue = 0.0;
      button_pressed = false;
    }
  }
  else
  {
    digitalWrite(BUTTON_LED, HIGH);
    if (digitalRead(BUTTON_IN) == LOW)
    {
      button_pressed = true;
      press_delay.start(3000, AsyncDelay::MILLIS);
    }
  }
}