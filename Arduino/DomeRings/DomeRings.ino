// DOME Rings for Phoenix' Wellcome Collection exhibiton

// Designed to run on an ESP8266 (Wemos D1 Mini).
// The master is connected via Serial to the Teensy that runs the DomeButtons

#include <Bounce2.h>
#include <c_types.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define NUM_LEDS 16 * 2

#define LED_PIN 5

CRGB leds[NUM_LEDS];

bool receiverUnit = false;

int buttonCenterHue[] = {20, 70, 130, 172, 200};
int buttonState[] = { 0,0,0,0,0 };
float buttonPower[] = { 0,0,0,0,0 };

bool win = false;
bool superWin = false;

int buttonPulseState = 0;

int pressedButtonsPulseState = 0;

long timeWin = 0;

int winDuration = 5000; // ms
int superWinDuration = 20000; // ms

int dt = 0; // deltatime

long previousTime = 0;

int fps = 0;
int myButton = 0;


// broadcast address
uint8_t remoteMac[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define WIFI_CHANNEL 4

struct __attribute__((packed)) DataStruct {
	char text[32];
	unsigned long time;
};

DataStruct myData;
unsigned long lastSentMillis;
unsigned long sendIntervalMillis = 1000;
unsigned long sentMicros;
unsigned long ackMicros;

unsigned long lastBlinkMillis;
unsigned long fastBlinkMillis = 200;
unsigned long slowBlinkMillis = 700;
unsigned long blinkIntervalMillis = slowBlinkMillis;

void sendESPNowData(uint8_t data)
{
	uint8_t bs[1];
	bs[0] = data;
	esp_now_send(NULL, bs, 1); // NULL means send to all peers
}


void sendData() 
{
	if (millis() - lastSentMillis >= sendIntervalMillis) 
	{
		lastSentMillis += sendIntervalMillis;
		myData.time = millis();
		uint8_t bs[sizeof(myData)];
		memcpy(bs, &myData, sizeof(myData));
		sentMicros = micros();
		esp_now_send(NULL, bs, sizeof(myData)); // NULL means send to all peers
		Serial.println("sent data");
	}
}

void receiveCallBackFunction(uint8_t* senderMac, uint8_t* incomingData, uint8_t len) 
{
	memcpy(&myData, incomingData, sizeof(myData));
	Serial.print("NewMsg ");
	Serial.print("MacAddr ");
	for (byte n = 0; n < 6; n++) {
		Serial.print(senderMac[n], HEX);
	}
	Serial.print("  MsgLen ");
	Serial.print(len);
	Serial.print("  Text ");
	//Serial.print(myData.text);
	Serial.print(incomingData[0]);
	setButtons(incomingData[0]);
	Serial.println();
}

void sendCallBackFunction(uint8_t* mac, uint8_t sendStatus) {

}

void setup() 
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600); // to PC
	delay(100);
	FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

	receiverUnit = !(WiFi.softAPmacAddress().equals("5E:CF:7F:2B:B6:AD"));
	Serial.print("Set as receiver: "); Serial.println(receiverUnit);
	Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
	Serial.print("This node STA mac: "); Serial.println(WiFi.macAddress());
	// espnow stuff
	WiFi.mode(WIFI_STA); // Station mode for esp-now controller
	WiFi.disconnect();
	esp_now_init();
	if (receiverUnit)
	{
		esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
		esp_now_register_recv_cb(receiveCallBackFunction);
	}
	else
	{
		esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
		esp_now_add_peer(remoteMac, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0);
		esp_now_register_send_cb(sendCallBackFunction);
	}
	myButton = 0;
	String s = WiFi.softAPmacAddress();
	// identify our button number by the macaddress of the ESP
	if (s.endsWith("50")) myButton = 1;
	if (s.endsWith("F4")) myButton = 2;
	if (s.endsWith("E1")) myButton = 3;
	if (s.endsWith(":82")) myButton = 4;
	Serial.print("My button: "); Serial.println(myButton + 1);
}

void setPixel(int pos, CRGB col)
{
	if (pos >= 0 && pos < NUM_LEDS)
	{
		leds[pos] = col;
	}
}

void setButtons(int input)
{
	buttonState[0] = input % 2 == 1;
	buttonState[1] = (input / 2) % 2 == 1;
	buttonState[2] = (input / 4) % 2 == 1;
	buttonState[3] = (input / 8) % 2 == 1;
	buttonState[4] = (input / 16) % 2 == 1;
}

void checkSerial()
{
	int available = Serial.available();
	if (available == 0) return;

	byte input = Serial.read();
	Serial.print("Sending button value: "); Serial.println(input);
	sendESPNowData(input);

	setButtons(input);

}

// win and superwin aren't actually transmitted to the rings yet
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
	for (int i = 0; i < NUM_LEDS; i++)
	{
		if (random(0, 100) > 97) // sparkle
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
	for (int i = 0; i < NUM_LEDS; i++)
	{
		int pulse = beatsin8(120, 50, 255, 0, i);
		int hue = (i + (wintime / 10)) % 255;
		setPixel(i, CHSV(hue, 255, brightness));

		if (random(0, 100) > 95) // sparkle
			setPixel(i, CRGB(brightness, brightness, brightness));
	}
}

void drawButtonEffects()
{
	float decaySpeed = 3.0f;
	int pressedButtons = 0;
	for (int i = 0; i < 5; i++)
	{
		pressedButtons += buttonState[i];
	}
	
	pressedButtonsPulseState += dt * (pressedButtons + 1);
	int i = myButton;
	int hue = buttonCenterHue[i];

	if (buttonState[i] == 1)
	{
		buttonPulseState += dt * 12;

	}
	else
	{
		hue = (hue + 128) % 255;
		buttonPulseState += dt * 4;
	}
	float sineModifier = sin(buttonPulseState / 1000.0f) + 1.0f;
	for (int j = 0; j < 16; j++)
	{
		if (buttonState[i] == 0)
			sineModifier = sin(buttonPulseState / 1000.0f + j / 16.0f * 3.14f * 2) + 1.0f;
		float sineModifier2 = sin(pressedButtonsPulseState/ 1000.0f + j / 16.0f * 3.14f * 2) + 1.0f;
		
		// outward facing button ring
		setPixel(16 + j, CHSV(hue, 255, int((250.0f - 100 * sineModifier))));
		
		// inside facing
		setPixel(j, CHSV(hue + (((j + pressedButtonsPulseState)) % 20) * 3, 255, int((250.0f - 100 * sineModifier2))));

		if (win)
		{

		}
	}


}


void drawBackground()
{
	// just dim old background a bit
	int decay = 100;
	nscale8(leds, NUM_LEDS, decay);
}

void loop() 
{
	dt = millis() - previousTime;
	previousTime = millis();



	if (receiverUnit)
	{
		// for testing with usb connection
		if (Serial.available() > 0)
		{
			String s = Serial.readString();
			if (s.equals("w"))
			{
				//superWin = !superWin;
				win = !win;
				timeWin = millis();
			}
			setButtons(s.toInt());
		}
	}
	else
	{
		checkSerial();
		//sendData();
	}
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
