g to // Trees are on Leonardos
// ID 0 is for MASTER on Teensy
// ID 1-2 are for CLOUDS on Uno/Leonardo
// ID 3-A (3-9, A) are TREES and run on Arduino Leonardos
// ID B is the HUT

// Constants
int TIME_BETWEEN_UPDATES = 200;
int ID = 1;  // index[0] in all communications
int NUM_OBJECTS = 11;  // how many objects are in the ecosystem
int NUM_STATES = 3;  // how many states will be updated for all objects - all the same at this point

// Variables
// For sending to server:
bool playing = 0;  
// For updating from server:
bool victory = 0;  // index[1]
bool resetGame = 0;  // index[2]
int updateStringStartIndex = 0;
int updateStringEndIndex = 0;
String updateSubstring;
String updateFromServerString;
// For managing local state.
int lastUpdate;

int lightPin = 11;  // for testing


void setup() {
  Serial1.begin(9600);
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);
}


void sendState(){
  Serial1.print("{");
  Serial1.print(ID);
  Serial1.print(playing);
  Serial1.println("}");
}


void readServerState(){
  if(Serial1.available()){
    char ch;
    String cha;
    ch = (char) Serial1.read();
    cha = (String) ch;
    if (cha == "{"){
      updateFromServerString = cha;
    }else if(cha == "\n"){
      updateFromServerString = updateFromServerString + cha;
      updateFromServerString.trim();
      int strSize = updateFromServerString.length();
      if((strSize==(2+NUM_STATES*NUM_OBJECTS) && (updateFromServerString.indexOf('{')==0) && (updateFromServerString.indexOf('}')==(2+NUM_STATES*NUM_OBJECTS-1)))){
        // extract substring for this object
        updateStringStartIndex = updateFromServerString.indexOf(ID);  // find which part of the string belongs to this object
        updateStringEndIndex = updateStringStartIndex + NUM_STATES;  // find the end by moving over as many places as we expect there to be pieces of data.
        updateSubstring = updateFromServerString.substring(updateStringStartIndex,updateStringEndIndex);  // get the substring.

        
        
        // Update the object parameters into local state. 
        victory = (bool) updateSubstring[1];
        resetGame = (bool) updateSubstring[2];

        Serial.print("Hut received " + updateSubstring + "And updated victory to: ");
        Serial.print(victory);
        Serial.print(" and resetGame to ");
        Serial.println(resetGame);
        
      }else{
        Serial.flush();
        Serial1.flush();
      }
    }else{
      updateFromServerString = updateFromServerString + cha;
    }
  }
}


void testServerInput(){
  // it turns on an LED when Victory is positive. Just to prove that it's actually receiving and updating in an "embodied" way.
  if (victory && resetGame){
    digitalWrite(lightPin, HIGH);
    victory = false;
    delay(1000);
    digitalWrite(lightPin,LOW);
    delay(1000);
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  readServerState();
  // testServerInput();
  currentTime = millis();
  if(currentTime-lastUpdate > TIME_BETWEEN_UPDATES){
    sendState();
    lastUpdate = currentTime;
  }
}
