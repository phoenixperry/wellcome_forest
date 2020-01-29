#include <ArduinoJson.h>

// HUT is on a Teensy for the IO

int TIME_BETWEEN_UPDATES = 250;
int ID = 11;

String updateServerString = "";

StaticJsonDocument<JSON_OBJECT_SIZE(4)> sendToServerDoc;

void setup() {
//  Serial.begin(9600);
  //Begin HW serial
  Serial1.begin(9600);

}

void sendState(){

  sendToServerDoc["id"].set(ID);
  sendToServerDoc["lit"].set(false);
  sendToServerDoc["brightness"].set(0);
  sendToServerDoc["victory"].set(true);
  
  // Cast the JsonVariant to a string and send it over serial.
  updateServerString = ""+sendToServerDoc.as<String>();
  Serial1.println(updateServerString);
}

void loop() {
  // put your main code here, to run repeatedly:
  sendState();
  delay(TIME_BETWEEN_UPDATES);

}
