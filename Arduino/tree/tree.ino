#include "FastLED.h"
#include "AsyncDelay.h"

#define LED_PIN 10
#define LED_COUNT 32
#define BRIGHTNESS 255

#define BUTTON_LED 11
#define BUTTON_OUT 12
#define BUTTON_IN 13

CRGB leds[LED_COUNT];
bool button_was_pressed = false;
String tree_id = "D"; // int from C - J; C is the starting tree
uint16_t time = 0;    // timer used for animations
AsyncDelay asyncDelay;

enum State
{
  idle,
  beacon,
  win,
  win_global,
  waiting,
  fail
};

bool is_beacon = tree_id == "C";

State current_state = is_beacon ? beacon : idle;
double hue = is_beacon ? 20.0 : 100.0;

void setup()
{
  // setup Serial
  Serial.begin(9600);
  // setup neopixel
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  // setup button
  pinMode(BUTTON_LED, OUTPUT);
  pinMode(BUTTON_IN, INPUT);
  pinMode(BUTTON_OUT, OUTPUT);
  digitalWrite(BUTTON_OUT, HIGH);
  digitalWrite(BUTTON_LED, HIGH);
}

void setHSV(double h, double s, double v)
{
  FastLED.showColor(CHSV(h, s, v));
}

bool is_button_pressed() { return digitalRead(BUTTON_IN) == LOW; }

void loop()
{
  time++;
  /*--- IDLE STATE ---*/
  if (current_state == idle)
  {
    double speed = 0.002;
    double saturation = 150 + abs(sin(time * speed)) * 50;
    double value = 255 - abs(cos(time * speed)) * 55;
    if (is_button_pressed())
    {
      button_was_pressed = true;
      asyncDelay.start(3000, AsyncDelay::MILLIS);
      time = 0;
    }
    if (button_was_pressed)
    {
      saturation = 255 - time * 0.05;
      if (asyncDelay.isExpired())
      {
        button_was_pressed = false;
      }
    }
    setHSV(hue, saturation, value);
  }
  /*--- BEACON STATE ---*/
  else if (current_state == beacon)
  {
    if (!button_was_pressed)
    {
      // beacon animation
      hue = -sin(time * 0.007) * 30 + 50;
      double value = 220 + sin(time * 0.007) * 35;
      setHSV(hue, 255, value);
      // handle button press
      if (is_button_pressed())
      {
        Serial.println(tree_id + " pressed");
        button_was_pressed = true;
      }
    }
    else
    {
      // smoothly reset to orange
      hue = hue > 20 ? hue - 0.2 : 20.;
      setHSV(hue, 255, 255);
    }
  }
  /*--- WAITING STATE ---*/
  if (current_state == waiting)
  {
  }
  /*--- FAIL STATE ---*/
  if (current_state == fail)
  {
    setHSV(hue, 240, 200);
  }
  /*--- WIN STATE ---*/
  /*--- WIN_GLOBAL STATE ---*/
}
