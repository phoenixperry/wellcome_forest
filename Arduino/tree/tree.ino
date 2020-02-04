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
uint16_t time = 0;  // timer used for animations

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

void setHSV(float h, float s, float v)
{
  FastLED.showColor(CHSV(h, s, v));
}

void loop()
{
  /*--- IDLE STATE ---*/
  if (current_state == idle)
  {
    setHSV(100, 255, 255);
  }
  /*--- BEACON STATE ---*/
  else if (current_state == beacon)
  {
    if (!button_pressed)
    {
      // beacon animation
      time++;
      setHSV(20 + abs(sin(time * 0.005)) * 60, 240, 240);
      // handle button press
      if (digitalRead(BUTTON_IN) == LOW)
      {
        button_pressed = true;
        Serial.println();
      }
    }
    else
    {
      // reset to orange
      setHSV(20, 255, 255);
    }
  }
  /*--- WAITING STATE ---*/
  /*--- FAIL STATE ---*/
  /*--- WIN STATE ---*/
  /*--- WIN_GLOBAL STATE ---*/
}
