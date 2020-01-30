// This master lives with the laptop and feeds Unity with all the info it needs. It also sends out lovely data to the trees, clouds, and yurt!

// Declare constants.
int TIME_BETWEEN_UPDATES = 200; // ms between updating the slaves (Trees, hut, clouds)
int TIME_BETWEEN_SLAVE_UPDATES = 200;
int NUM_TREES = 8;
int NUM_HUTS = 1;
int NUM_CLOUDS = 2;
int NUM_STATES = 3;
int ID = 0;

// Declare variables
int lastUpdate;
int currentTime;
String updateFromServerString = "";
uint8_t buf[128];


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


void readUpdateSlaveState(){
  if(Serial1.available()){
    String s = Serial1.readStringUntil('\n');
    s.trim();  // trim that newline off
    int strSize = s.length();
    if((strSize==4) && (s.indexOf('{')==0) && (s.indexOf('}')==3)){
        Serial.println(s);
    }else{
        Serial.flush();
    }
  }
}




void updateSlaves(){
  // This method sends over radio the minified mainStateDoc JSON state. 
  
}

void readServerStateUntil(){
  if(Serial.available()){
    
    String s = Serial.readStringUntil('\n');
    s.trim();  // trim that newline off
    int strSize = s.length();
    if((strSize==(2+NUM_STATES*(NUM_TREES+NUM_HUTS+NUM_CLOUDS))) && (s.indexOf('{')==0) && (s.indexOf('}')==(2+NUM_STATES*(NUM_TREES+NUM_HUTS+NUM_CLOUDS)-1))){
      Serial1.println(s);
//      Serial.println(s);
    }else{
      Serial.flush();
      Serial1.flush();
    }
  }
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
      if((strSize==(2+NUM_STATES*(NUM_TREES+NUM_HUTS+NUM_CLOUDS))) && (updateFromServerString.indexOf('{')==0) && (updateFromServerString.indexOf('}')==(2+NUM_STATES*(NUM_TREES+NUM_HUTS+NUM_CLOUDS)-1))){
        Serial1.println(updateFromServerString);
        Serial.println(updateFromServerString);
      }else{
        Serial.flush();
        Serial1.flush();
      }
    }else{
      updateFromServerString = updateFromServerString + cha;
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  readUpdateSlaveState();
  readServerState();
  
  // determine whether the slaves have been kept waiting too long. If they have, update them.
  currentTime = millis();
  if((currentTime-lastUpdate)  > TIME_BETWEEN_SLAVE_UPDATES){
//    updateSlaves();
    lastUpdate = currentTime;
  }
}
