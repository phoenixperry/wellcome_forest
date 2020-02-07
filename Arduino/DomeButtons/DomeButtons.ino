// DOME Game for Phoenix' Wellcome Collection exhibiton

// designed to run on a Teensy that has an Xbee connected on Serial1, and optionally an ESP8266 on Serial2 (for additional effects), and two LED strips with 180 pixel each.

// LED strip layout is still to be finalized!
// each button has 72 LEDs.

// there are two LED strips on this controller, one is 5 meters (going clockwise from the arduino), 
// and one is ~7m (going counterclockwise. when using the 5 meter rolls, a second strip needs to be added to the end of the first one). This strip has a bit of masking tape on it

// 5 buttons are connected to the controller, with 3 wires each: button data (purple), ground (black), and 5V (red).


#include <FastLED.h>
#include <Bounce2.h>

#define NUM_LEDS1 5*30
#define NUM_LEDS2 7*30

#define LED_PIN1 14
#define LED_PIN2 22

CRGB leds1[NUM_LEDS1];
CRGB leds2[NUM_LEDS2];

const int buttonPins[] = { 15, 16,17,18,19 };
int buttonState[] = { 0,0,0,0,0 };
float buttonPower[] = { 0,0,0,0,0 };

int buttonCenterHue[] = {20, 70, 130, 172, 200};
int buttonCenterPixelPos[] = {-114,-42,30, 102, 174};
Bounce pushButtons[5];

bool win = false;      // winning the hut game (i.e., all 5 buttons pressed at the same time. Game shows winning animation for winDuration milliseconds)
bool superWin = false; // overall win! (all buttons pressed in all games. game shows winning animation for superWinDuration milliseconds)

long timeWin = 0;

String lastServerMessage = "";

int winDuration = 5000; // ms
int superWinDuration = 20000; // ms

int dt = 0; // deltatime

long previousTime = 0;

long regularUpdateTimer = 1000;

int fps = 0;

void setup() 
{
	pinMode(13, OUTPUT);
	Serial.begin(9600); // to PC
	Serial1.begin(9600); // to XBee
	Serial3.begin(9600); // to ESP8266 for dome rings

	FastLED.addLeds<WS2812B, LED_PIN1, GRB>(leds1, NUM_LEDS1);
	FastLED.addLeds<WS2812B, LED_PIN2, GRB>(leds2, NUM_LEDS2);

	analogWriteResolution(8);
	for (int i = 0; i < 5; i++)
	{
		pinMode(buttonPins[i], INPUT_PULLUP);
		pushButtons[i] = Bounce(buttonPins[i], 50); // 10ms debounce
	}
}

void setPixel(int pos, CRGB col)
{
	if (pos < 0 && pos >= -NUM_LEDS1)
	{
		leds1[-pos - 1] = col;
	}
	else if (pos >= 0 && pos < NUM_LEDS2)
	{
		leds2[pos] = col;
	}
}

void sendXbee()
{
	String s = "{B";
	

	// just send a 0 or 1 for each button for now
	for (int i = 0; i < 5; i++)
	{
		if (buttonState[i] == 0)
			s += "0";
		else
			s += "1";
	}
	s += "}";
	Serial.print("Sending state to xbee: "); Serial.println(s);
	Serial1.println(s);

	// encode state in a number, each bit is one button
	int state = buttonState[0] + buttonState[1] * 2 + buttonState[2] * 4 + buttonState[3] * 8 + buttonState[4] * 16;

	// send this to Serial3 (pin 8), where an ESP8266 might be connected to control the rings
	Serial3.write(state);
}


void checkButtons(bool sendAnyway = false)
{
	int pressedCount = 0;
	bool somethingChanged = false;
	for (int i = 0; i < 5; i++)
	{
		if (pushButtons[i].update()) {
			if (pushButtons[i].fallingEdge()) 
			{
				// button now pressed
				buttonState[i] = 1;
				Serial.print("Pressed button "); Serial.println(i);
				somethingChanged = true;
			}
			else if (pushButtons[i].risingEdge())
			{
				// button released
				buttonState[i] = 0;
				Serial.print("Released button "); Serial.println(i);
				somethingChanged = true;
			}
		}
		pressedCount += buttonState[i];
	}

	if (somethingChanged || sendAnyway)
	{
		digitalWrite(13, HIGH);
		sendXbee();
		digitalWrite(13, LOW);
	}

	if (pressedCount == 5)
	{
		win = true;
		if (!superWin) timeWin = millis();
	}


}

void checkXBee()
{
	int available = Serial1.available();
	if (available == 0) return;
	
	while (Serial1.available() > 0 && Serial1.read() != '{'); // discard everything until the first '{'
	
	available = Serial1.available();
	if (available == 0) return;

	char identifier = Serial1.read();

	if (identifier != 'Z')
	{
		// Message not from Server, discarding
		while (Serial1.available() > 0 && Serial1.peek() != '{') Serial1.read(); // discard the rest until the next '{'
		return;
	}

	String s = Serial1.readStringUntil('}');

	if (!s.equals(lastServerMessage))
	{
		Serial.print("Server message changed to: {Z"); Serial.print(s); Serial.println("}");
		lastServerMessage = s;
	}


	while (Serial1.available() > 0 && Serial1.peek() != '{') Serial1.read(); // discard the rest until the next '{'

	int weather = s[3] - 48;

	if (weather == 2)
	{
		// superwin!
		Serial.println("Received Superwin!! Whoa!");
		if (!superWin) timeWin = millis();
		superWin = true;
	}
}

void drawWin()
{
	int wintime = millis() - timeWin;
	int brightness = 255;
	if (superWin && wintime + 1000 > superWinDuration)
	{
		brightness = 255 * (superWinDuration - wintime) / 1000;
	}
	if (!superWin && win && wintime + 1000 > winDuration)
	{
		brightness = 255 * (winDuration - wintime) / 1000;
	}
	for (int i = -NUM_LEDS1; i < NUM_LEDS2; i++)
	{

		if (random(0, 100) > 95) // sparkle
			setPixel(i, CRGB(brightness, brightness, brightness));
	}
}

void drawSuperWin()
{
	int wintime = millis() - timeWin;
	int brightness = 255;
	if (superWin && wintime + 1000 > superWinDuration)
	{
		brightness = 255 * (superWinDuration - wintime) / 1000;
	}
	if (!superWin && win && wintime + 1000 > winDuration)
	{
		brightness = 255 * (winDuration - wintime) / 1000;
	}
	for (int i = -NUM_LEDS1; i < NUM_LEDS2; i++)
	{
		int pulse = beatsin8(120, 50, 255, 0, i);
		int hue = (i + NUM_LEDS1 + (wintime / 10)) % 255;
		setPixel(i, CHSV(hue, 255, brightness));

		if (random(0, 100) > 95) // sparkle
			setPixel(i, CRGB(brightness, brightness, brightness));
	}
}

void drawButtonEffects()
{
	int buttonsPressed = 0;
	float decaySpeed = 3.0f;
	for (int i = 0; i < 5; i++)
	{
		buttonsPressed += buttonState[i];
		if (buttonState[i] == 1)
		{
			buttonPower[i] = buttonPower[i]*(1 + decaySpeed * dt/1000.0f) + decaySpeed * (dt / 1000.0f);
			if (buttonPower[i] >= 1) buttonPower[i] = 1;

		}
		else
		{
			buttonPower[i] *= (1 - decaySpeed * dt/1000.0f);
			if (buttonPower[i] < 0.2f) buttonPower[i] = 0.2f;
		}
		for (int x = 0; x <= 36; x++)
		{
			float sineModifier = sin(millis() / 100.0f + buttonCenterPixelPos[i] / 10.0f) + 1;
			setPixel(buttonCenterPixelPos[i] - x, CHSV(buttonCenterHue[i], 180 + x * sineModifier, int((180.0f + x * sineModifier)) * buttonPower[i]));
			setPixel(buttonCenterPixelPos[i] + x, CHSV(buttonCenterHue[i], 180 + x * sineModifier, int((180.0f + x * sineModifier)) * buttonPower[i]));
		}
	}




}

void drawBackground()
{
	// just dim old background a bit
	int decay = 100;
	nscale8(leds1, NUM_LEDS1, decay);
	nscale8(leds2, NUM_LEDS2, decay);
}

void loop() 
{
	dt = millis() - previousTime;
	previousTime = millis();

	checkXBee();

	bool regularUpdate = false;
	if (millis() > regularUpdateTimer)
	{
		regularUpdateTimer = millis() + 1000;
		regularUpdate = true;
	}

	checkButtons(regularUpdate);

	drawBackground();
	drawButtonEffects();

	if (superWin)
	{
		if ((millis() - timeWin) < superWinDuration)
		{
			drawSuperWin();
		}
		else
		{
			Serial.print("Superwin over!");
			superWin = false;
			win = false;
		}
	}
	else if (win)
	{
		if ((millis() - timeWin) < winDuration)
		{
			drawWin();
		}
		else
		{
			Serial.print("Win over!");
			win = false;
		}
	}

	FastLED.show();

}
