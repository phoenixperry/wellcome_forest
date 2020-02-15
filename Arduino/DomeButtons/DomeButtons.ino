// DOME Game for Phoenix' Wellcome Collection exhibiton

// designed to run on a Teensy that has an Xbee connected on Serial1, and optionally an ESP8266 on Serial2 (for additional effects), and two LED strips with 180 pixel each.

// LED strip layout:


//                strip 1                              strip 2 (with masking tape)       extra strip at end
// [ ================================ ] Arduino [ ==================================== | ================== ]
//           |                  |                          |                      |                    |
// LEDPos: Button 3          Button 4                   Button 5               Button 2             Button 1

//		light bluegreen 	bluepurple					purplepink				yellowgreen 		orange

// spring: purple             yellow                    green                    aqua               pink

// button 1 is button 3 


/*
Button mapping is messed up. so we're re-assigning stuff.
Going around to the right from the outside.


		   ButtonPos3
		    -----
		BP4/     \  ButtonPos2
		  /       \ 
strip 2	 |         | strip 1
		  \       /
		BP5\     / ButtonPos1
		    -----
		         A

ButtonPos 1 on Pin:
ButtonPos 2 on Pin:
ButtonPos 3 on Pin:
ButtonPos 4 on Pin:
ButtonPos 5 on Pin:


            BUTTONS
\   \2/     \1/     \5/     \4/     \3/
/ \ / \ / \ / \ / \ / \ / \ / \ / \ / \
 5   |   4   | A 3   |   2   |   1   | 
           STRIP PARTS



Ringbutton that lights up
    5     2      3     1     4

Pin
    2     3      4     5     1

         BUTTONS
\  \2/   \1/    \5/   \4/   \3/
/ \/ \/ \/ \ / \/ \/ \/ \/ \/ \
 5  |  4  | A 3  |  2  |  1  | 
           STRIP PARTS



*/

// The LEDPos is on the opposite side of the dome from where the actual button is. 
  

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

//const int buttonPins[] = { 15, 16,17,18,19 }; // ORIGINAL MAPPING
const int buttonPins[] = { 17,16,15,19,18 };
int buttonState[] = { 0,0,0,0,0 };
float buttonPower[] = { 0,0,0,0,0 };

//const int ringMap[] = { 3, 0, 4, 2, 1 }; // send a message to ringmap[i] to light up button i
const int ringMap[] = {3, 4, 1, 2, 0}; // send a message to ringmap[i] to light up button i

/* standard colors / hue
0 - lime green  75
1 - dark green  110
2 - blue / purple 180
3 - yellow   64
4 - orange   32
*/


int buttonCenterHue[] = {75, 110, 180, 64, 32};

int ledSegments[] = {
   119, 190,  // segment for button 1
    51, 118,  // segment for button 2
   -17,  50,  // segment for button 3
   -85, -18,  // segment for button 4
  -150, -86   // segment for button 4
};



Bounce pushButtons[5];

bool win = false;      // winning the hut game (i.e., all 5 buttons pressed at the same time. Game shows winning animation for winDuration milliseconds)
bool superWin = false; // overall win! (all buttons pressed in all games. game shows winning animation for superWinDuration milliseconds)

long timeWin = 0;

String lastServerMessage = "";

int winDuration = 63000; // ms
int superWinDuration = 73000; // ms

int dt = 0; // deltatime

long previousTime = 0;

long regularUpdateTimer = 100;

int fps = 0;

const int SPRING = 0;
const int FALL = 1;
const int RAIN = 2;

int weather = SPRING;

int testPixel = 0;
int testPixelCounter = 0;


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
	else if (pos >= NUM_LEDS2 && pos < NUM_LEDS2 + NUM_LEDS1)
	{
		// wrap around at the positive end
		leds1[NUM_LEDS1 - 1 - (pos - NUM_LEDS2)] = col;
	}
	else if (pos < -NUM_LEDS1 && pos > -(NUM_LEDS1 + NUM_LEDS2))
	{
		// wrap around the negative end
		leds2[NUM_LEDS2 - 1 - (-pos - NUM_LEDS1)] = col;
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
	int state = buttonState[ringMap[0]] + buttonState[ringMap[1]] * 2 + buttonState[ringMap[2]] * 4 + buttonState[ringMap[3]] * 8 + buttonState[ringMap[4]] * 16;

	//if (win) state += 32;
	//if (superWin) state += 64;

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
				
				// new: invert button state!				
				buttonState[i] = 1 - buttonState[i];
				//buttonState[i] = 1;

				Serial.print("Triggerd button "); Serial.print(i); Serial.print(" new state: "); Serial.println(buttonState[i]);
				somethingChanged = true;
			}
			else if (pushButtons[i].risingEdge())
			{
				// button released, doesn't matter any more
				//buttonState[i] = 0;
				//Serial.print("Released button "); Serial.println(i);
				//somethingChanged = true;
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

	if (!s.equals(lastServerMessage) && s.length() == 4)
	{
		Serial.print("Server message changed to: {Z"); Serial.print(s); Serial.println("}");
		lastServerMessage = s;
	}


	while (Serial1.available() > 0 && Serial1.peek() != '{') Serial1.read(); // discard the rest until the next '{'

	weather = s[3] - 48;

	if (weather == 3)
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
	int h1 = 135;
	int h2 = 224;
	int counter = millis() / 20;
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
		int hue = h1 + (h2 - h1) * fmod((float(counter) / float(NUM_LEDS1 + NUM_LEDS2)), 1.0f);

		if (random(100) > 70)
		{
			setPixel(i, CHSV(hue, 255, 255));
		}

		counter++;

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
	float decaySpeed = 4.0f;

	/*

	for (int i = 0; i < 5; i++)
	{
		if (buttonState[i] == 1)
		{
			for (int j = 0; j <= i; j++)
			{
				setPixel(10 * i + j, CHSV(i * 30, 255, 200));

			}

		}
	}

	return;
	*/

	for (int i = 0; i < 5; i++)
	{
		if (buttonState[i] == 1)
		{
			buttonPower[i] = 1; //buttonPower[i]*(1 + decaySpeed * dt/1000.0f) + decaySpeed * (dt / 1000.0f);
			if (buttonPower[i] >= 1) buttonPower[i] = 1;

		}
		else
		{
			//buttonPower[i] *= (1 - decaySpeed * dt/1000.0f);
			buttonPower[i] = 0.2f;
			if (buttonPower[i] < 0.2f) buttonPower[i] = 0.2f;
		}

		for (int x = ledSegments[2 * i]; x <= ledSegments[2 * i + 1]; x++)
		{
			if (buttonState[i] == 0)
			{
				setPixel(x, CHSV(buttonCenterHue[i], 255, int(10)));
				// occasional sparkles
				if (random(999) > 990)
				{
					setPixel(x, CHSV(buttonCenterHue[i], 255, 255));
				}
			}
			
			else
			{
					setPixel(x, CHSV(buttonCenterHue[i], 255, int(150)));
				if (random(999) > 980)
				{
					setPixel(x, CRGB(255, 255, 255));
				}
			}
		}

		/*
		for (int x = 0; x <= 36; x++)
		{
			float sineModifier = sin(millis() / 100.0f + buttonCenterPixelPos[i] / 10.0f) + 1;
			setPixel(buttonCenterPixelPos[i] - x, CHSV(buttonCenterHue[i], 180 + x * sineModifier, int((180.0f + x * sineModifier)) * buttonPower[i]));
			setPixel(buttonCenterPixelPos[i] + x, CHSV(buttonCenterHue[i], 180 + x * sineModifier, int((180.0f + x * sineModifier)) * buttonPower[i]));
		}
		*/
	}




}

void drawBackground()
{
	// just dim old background a bit
	int decay = 100;
	nscale8(leds1, NUM_LEDS1, decay);
	nscale8(leds2, NUM_LEDS2, decay);
}

void resetButtons() {
	for (int i = 0; i < 5; i++)
	{
		buttonState[i] = 0;
		buttonPower[i] = 0;
	}
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

	// Serial.print(win); Serial.print(superWin); Serial.println("");

	if (!win && !superWin)
	{
		drawButtonEffects();
	}



	if (superWin)
	{
		if ((millis() - timeWin) < superWinDuration)
		{
			drawSuperWin();
			resetButtons();
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
			resetButtons();
		}
		else
		{
			Serial.print("Win over!");
			win = false;
		}
	}

	FastLED.show();

}
