#include <ArduinoJson.h>

// Declare constants.
int TIME_BETWEEN_UPDATES = 250; // ms between updating the slaves (Trees, hut, clouds)
int NUM_TREES = 6;
int NUM_HUTS = 1;
int NUM_CLOUDS = 2;
int ID = 0;

// Declare variables
int lastUpdate;


// State syncing string for initial storage and JSON doc for loading variables. 
String updateSlavesString = "";
String updateFromSlavesString = "";
// Main state doc is the MASTER state of the whole project. It is a singlet.
DynamicJsonDocument mainStateDoc(2048);  
DynamicJsonDocument slaveStateDoc(2048); // this will hold the initial state sent over from the slave.
// There needs to be 1 state doc for each SLAVE. These get nested into the mainStateDoc.
// 6 trees
//DynamicJsonDocument Tree1Doc;  // Increase size as necessary - this SHOULD mean 3 items, but in practice there's more of a rel w/ what amt of data is within
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Tree2Doc;
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Tree3Doc;
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Tree4Doc;
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Tree5Doc;
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Tree6Doc;
// 2 clouds
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Cloud1Doc;
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> Cloud2Doc;
// 1 hut
//StaticJsonDocument<JSON_OBJECT_SIZE(6)> HutDoc;
// List of all the docs so we can iterate through them when rebuilding the master
//StaticJsonDocument documents[9] = {Tree1Doc, Tree2Doc, Tree3Doc, Tree4Doc, Tree5Doc, Tree6Doc, Cloud1Doc, Cloud2Doc, HutDoc}


void setup() {
  // put your setup code here, to run once:
  
  //Begin serial monitor port - this is the cable.
  Serial.begin(9600);
  //Begin HW serial - this is the radio.
  Serial1.begin(9600);
  delay(50);
  lastUpdate = millis();

  // 
}

void updateFromSlave(String slaveString){
  // This method parses the string, making sure it's valid to be serialized.
  // Then it deserializes it and updates local state.
  // Does it need a check on length?
  deserializeJson(mainStateDoc, slaveString);
  int currMem = mainStateDoc.memoryUsage();
  Serial.print(" and memory ");
  Serial.println(currMem);
  Serial.println(slaveString);
}

void readSlaveState(){
  if(Serial1.available()){
//    String s = Serial1.readStringUntil('\n');
    String s = Serial1.readStringUntil('\n');
    int strSize = s.length();
    
    if(strSize>0){
      Serial.print("received data over radio of length ");
      Serial.print(strSize);
      updateFromSlave(s);
    }
  }
}


//void updateFromSlave(String updateString){
  // updateString is a string that has already been checked for length and contents, so that it's definitely a valid slave string. 
  // This method takes a string from the receiver and updates the main document state again. It overwrites it. 
//  sendToServerDoc["focused"].set(focused);
//  sendToServerDoc["wearing"].set(wearing);
//  sendToServerDoc["userOverride"].set(userOverride);
//  
//  // Cast the JsonVariant to a string and send it over serial.
//  updateServerString = ""+sendToServerDoc.as<String>();
//  Serial.println(updateServerString);
//}

void updateSlaves(){
  // This method sends over radio the minified mainStateDoc JSON state. 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  readSlaveState();
}
