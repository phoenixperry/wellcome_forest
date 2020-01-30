// CLOUD 1 IS ON THE UNO - SO IT ONLY HAS 1 SERIAL PORT.

// Constants
int TIME_BETWEEN_UPDATES = 200;
int ID = 1;  // Uno! index[0] in all communications
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
int currentTime;

int lightPin = 11;  // for testing


void setup() {
  Serial.begin(9600);

  // for testing
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);
}


void sendState(){
  Serial.print("{");
  Serial.print(ID);
  Serial.print(playing);
  Serial.println("}");
}


void readServerState(){
  if(Serial.available()){
    char ch;
    String cha;
    ch = (char) Serial.read();
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
        
      }else{
        Serial.flush();
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
