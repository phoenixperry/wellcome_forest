#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN 10
#define LED_COUNT 16
#define BRIGHTNESS 100

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
    for (int i = 0; i < 4; i++)
    {
      strip.setPixelColor(i, strip.Color(255, i * 64, 0));
      strip.show();
    }
    for (int i = 0; i < 4; i++)
    {
      strip.setPixelColor(4 + i, strip.Color(255 - i * 64, 255, 0));
      strip.show();
    }
    for (int i = 0; i < 4; i++)
    {
      strip.setPixelColor(8 + i, strip.Color(0, 255, i * 64));
      strip.show();
    }
    for (int i = 0; i < 4; i++)
    {
      strip.setPixelColor(12 + i, strip.Color(0, 255 - i * 64, 255));
      strip.show();
    }

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
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
    }
  }
}

float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float step(float e, float x) { return x < e ? 0.0 : 1.0; }

float *hsv2rgb(float h, float s, float b, float *rgb)
{
  rgb[0] = b * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[1] = b * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[2] = b * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  return rgb;
}

float *rgb2hsv(float r, float g, float b, float *hsv)
{
  float s = step(b, g);
  float px = mix(b, g, s);
  float py = mix(g, b, s);
  float pz = mix(-1.0, 0.0, s);
  float pw = mix(0.6666666, -0.3333333, s);
  s = step(px, r);
  float qx = mix(px, r, s);
  float qz = mix(pw, pz, s);
  float qw = mix(r, px, s);
  float d = qx - min(qw, py);
  hsv[0] = abs(qz + (qw - py) / (6.0 * d + 1e-10));
  hsv[1] = d / (qx + 1e-10);
  hsv[2] = qx;
  return hsv;
}
