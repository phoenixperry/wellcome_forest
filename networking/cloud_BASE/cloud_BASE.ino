#include <ArduinoJson.h>

// CLOUD 1 IS ON THE UNO - SO IT ONLY HAS 1 SERIAL PORT.
// Declare constants
int TIME_BETWEEN_UPDATES = 250;
int ID = 1;  // Uno!

// Declare vars
String updateServerString = "";

StaticJsonDocument<JSON_OBJECT_SIZE(4)> sendToServerDoc;

void setup() {
  // Uno only has 1 serial port. 
  Serial.begin(9600);
}

void sendState(){

  sendToServerDoc["id"].set(ID);
  sendToServerDoc["lit"].set(false);
  sendToServerDoc["brightness"].set(47);
  sendToServerDoc["victory"].set(false);
  
  // Cast the JsonVariant to a string and send it over serial.
  updateServerString = ""+sendToServerDoc.as<String>();
  Serial.println(updateServerString);
}

void loop() {
  // put your main code here, to run repeatedly:
  sendState();
  delay(TIME_BETWEEN_UPDATES);

}
