#include <ArduinoJson.h>

// Uno requries declaring the Serial1. Others not so much.
//#define HWSERIAL Serial1

int TIME_BETWEEN_UPDATES = 250;
int ID = 1;

String updateServerString = "";

StaticJsonDocument<JSON_OBJECT_SIZE(4)> sendToServerDoc;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  //Begin HW serial
  Serial1.begin(9600);

}

void sendState(){

  sendToServerDoc["id"].set(ID);
  sendToServerDoc["lit"].set(false);
  sendToServerDoc["brightness"].set(47);
  sendToServerDoc["victory"].set(false);
  
  // Cast the JsonVariant to a string and send it over serial.
  updateServerString = ""+sendToServerDoc.as<String>();
  Serial1.println(updateServerString);
}

void readState(){
  // put in JSON
  // if self.id in the JSON as a key, it's the master! Update state based on that bit.
}

void loop() {
  // put your main code here, to run repeatedly:
  sendState();
  delay(TIME_BETWEEN_UPDATES);

}
