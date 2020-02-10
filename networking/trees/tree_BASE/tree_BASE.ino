// Trees are on Leonardos
// ID 0 is for MASTER on Teensy
// ID A-B are for CLOUDS on Uno/Leonardo
// ID C-J are TREES and run on Arduino Leonardos
// ID K is the HUT

// Constants
int TIME_BETWEEN_UPDATES = 200;
char ID = 'C';  // index[0] in all communications
int NUM_OBJECTS = 11;  // how many objects are in the ecosystem
int NUM_STATES = 1;  // how many states will be updated for all objects - all the same at this point

// Variables
// For sending to server:
bool local_win = 0;
// For updating from server:
int trees_state = 0;  // index[1]
bool isBeacon = 0;  // index[2]
int weather_state = 0; // index[4]

int updateStringStartIndex = 0;
int updateStringEndIndex = 0;
String updateSubstring;
String updateFromServerString;

// For managing local state.
int lastUpdate;
int currentTime;

int lightPin = 11;

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);
}


void sendState() {
  Serial1.print("{");
  Serial1.print(ID);
  Serial1.print(local_win);
  Serial1.println("}");
}


void readServerState() {
  // UNUSED. JUST FOR REFERENCE.
  // This method uses the old style of state string {111211311411511611711811911A11B11}.
  if (Serial1.available()) {
    char ch;
    String cha;
    ch = (char) Serial1.read();
    cha = (String) ch;
    if (cha == "{") {
      updateFromServerString = cha;
    } else if (cha == "\n") {
      updateFromServerString = updateFromServerString + cha;
      updateFromServerString.trim();
      int strSize = updateFromServerString.length();
      if ((strSize == (2 + NUM_STATES * NUM_OBJECTS) && (updateFromServerString.indexOf('{') == 0) && (updateFromServerString.indexOf('}') == (2 + NUM_STATES * NUM_OBJECTS - 1)))) {

        // extract substring for this object
        updateStringStartIndex = updateFromServerString.indexOf(ID);  // find which part of the string belongs to this object
        updateStringEndIndex = updateStringStartIndex + NUM_STATES;  // find the end by moving over as many places as we expect there to be pieces of data.
        updateSubstring = updateFromServerString.substring(updateStringStartIndex, updateStringEndIndex); // get the substring.

        // Update the object parameters into local state.
        trees_state = (int) updateSubstring[1];
        isBeacon = (bool) updateSubstring[2];
        weather_state = (int) updateSubstring[4];

      } else {
        Serial.flush();
        Serial1.flush();
      }
    } else {
      updateFromServerString = updateFromServerString + cha;
    }
  }
}

void readServerStateUntil() {
  // This reads the state from Unity/ laptop over the Serial port
  if (Serial1.available()) {
    String updateFromServerString = Serial1.readStringUntil('\n');
    updateFromServerString.trim();  // trim that newline off
    int strSize = updateFromServerString.length();
    if (strSize == 6 && (updateFromServerString.indexOf('{') == 0) && (updateFromServerString.indexOf('}') == (6 - 1))) {
      // Update the object parameters into local state.
      trees_state = (int) updateFromServerString[1];
      isBeacon = (bool) updateFromServerString[2];
      weather_state = (int) updateFromServerString[4];

      // Printing back over serial to see that we get it
      Serial.print(updateFromServerString);
    } else {
      Serial.flush();
      Serial1.flush();
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  readServerStateUntil();

  currentTime = millis();
  if (currentTime - lastUpdate > TIME_BETWEEN_UPDATES) {
    sendState();
    lastUpdate = currentTime;
  }
}
