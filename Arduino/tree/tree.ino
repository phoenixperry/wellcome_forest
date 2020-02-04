#include "FastLED.h"
#include "AsyncDelay.h"

#define LED_PIN 10
#define LED_COUNT 32
#define BRIGHTNESS 255

#define BUTTON_LED 11
#define BUTTON_OUT 12
#define BUTTON_IN 13

CRGB leds[LED_COUNT];
bool button_pressed = false;
char tree_id = 'C'; // int from C - J; C is the starting tree
uint16_t t = 0;

enum Color
{
  idle,
  beacon,
  win,
  win_global,
  waiting,
  fail
};

Color current_state = tree_id == 'C' ? beacon : idle;

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
  t++;
  if (current_state == idle)
  {
    FastLED.showColor(CHSV(100, 255, 255));
  }
  else if (current_state == beacon)
  {
    if (!button_pressed)
    {
      // beacon animation
      FastLED.showColor(CHSV(20 + abs(sin(t * 0.005)) * 60, 240, 240));
      // handle button press
      if (digitalRead(BUTTON_IN) == LOW)
      {
        button_pressed = true;
        Serial.println();
      }
    }
    else
    {
      // stop blinking
      FastLED.showColor(CHSV(20, 255, 255));
    }
  }
}
