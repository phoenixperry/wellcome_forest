#include <ArduinoJson.h>

// Trees are on Leonardos
// Trees have IDs 3-10

// declare constants
int TIME_BETWEEN_UPDATES = 250;
int ID = 4;

// declare variables
String updateServerString = "";

StaticJsonDocument<JSON_OBJECT_SIZE(4)> sendToServerDoc;

void setup() {
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

void loop() {
  // put your main code here, to run repeatedly:
  sendState();
  delay(TIME_BETWEEN_UPDATES);

}
