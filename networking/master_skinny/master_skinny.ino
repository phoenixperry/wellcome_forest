// This code is part of the installation art game Forest Daydream for the Wellcome Collection.
// It primarily receives and sends radio signals from the rest of the devices with Xbees.
// It then processes them, manages the game state, and feeds the input over Serial to Unity.

// DECLARE CONSTANTS
const char ID = 'Z';
const int TIME_BETWEEN_SLAVE_UPDATES = 10;
const int TIME_BETWEEN_SERVER_UPDATES = 25;

// DECLARE VARIABLES
// GAME STATE MANAGEMENT VARIABLES
// Global game state
int weather_state = 0;  // idle, night, summer storm, cherry blossoms (0, 1, 2, 3)
int lastSlaveUpdate;
int lastServerUpdate;
int currentTime;
String updateFromServerString = "";

// Clouds IDs A-B

// Trees IDs C-J
int trees_state = 0; // idle/playing/win/lose (0,1,2,3)
char trees_current_beacon = 'C'; // ID of lit tree

char t1_id = 'C';
bool t1_active = 0;
bool t1_button_pressed = false;

char t2_id = 'D';
bool t2_active = 0;
bool t2_button_pressed = false;

char t3_id = 'E';
bool t3_active = 0;
bool t3_button_pressed = false;

char t4_id = 'F';
bool t4_active = 0;
bool t4_button_pressed = false;

char t5_id = 'G';
bool t5_active = 0;
bool t5_button_pressed = false;

char t6_id = 'H';
bool t6_active = 0;
bool t6_button_pressed = false;

char t7_id = 'I';
bool t7_active = 0;
bool t7_button_pressed = false;

char t8_id = 'J';
bool t8_active = 0;
bool t8_button_pressed = false;

// Hut ID K
int hut_state = 0;  // idle/playing/win/lose (0,1,2,3)
int hut_buttons[5] = {false, false, false, false,false};

void setup() {
  //Begin serial monitor port - this is the cable.
  Serial.begin(9600);
  delay(500);
  //Begin HW serial - this is the radio.
  Serial1.begin(9600);
  delay(500);
  lastSlaveUpdate = millis();
  lastServerUpdate = millis();
}

void zeroTreeButtons(){
  t1_button_pressed = 0;
  t2_button_pressed = 0;
  t3_button_pressed = 0;
  t4_button_pressed = 0;
  t5_button_pressed = 0;
  t6_button_pressed = 0;
  t7_button_pressed = 0;
  t8_button_pressed = 0;
}

void readSlaveState() {
  // This method reads the states from Slaves as they come in, and updates states.

  if (Serial1.available()) {
    String s = Serial1.readStringUntil('\n');
    s.trim();  // trim that newline off
    int strSize = s.length();

    // since the trees now only send on button press, we have to zero out the buttons as they never send a non-press
    zeroTreeButtons();


    // Check validity to ensure it's a Tree update. Could add additional check to see if s[1] is in CDEFGHIJ if needed.
    if ((strSize == 5) && (s.indexOf('{') == 0) && (s.indexOf('}') == 4)) {
      char switchChar = (char) s[1];
      

      
      switch (switchChar) {
        // Tree state updates
        case 'C':
          // cast a char containing an int to its corresponding int by getting the distance from the '0' char
          // '1' - '0' -> 1  but only 0 to 9.
          // There's another -48 tactic but this works.
          t1_active = (s[2]-'0');
          t1_button_pressed = (s[3]-'0');
          break;
        case 'D':
          t2_active = (s[2]-'0');
          t2_button_pressed = (s[3]-'0');
          break;
        case 'E':
          t3_active = (s[2]-'0');
          t3_button_pressed = (s[3]-'0');
          break;
        case 'F':
          t4_active = (s[2]-'0');
          t4_button_pressed = (s[3]-'0');
          break;
        case 'G':
          t5_active = (s[2]-'0');
          t5_button_pressed = (s[3]-'0');
          break;
        case 'H':
          t6_active = (s[2]-'0');
          t6_button_pressed = (s[3]-'0');
          break;
        case 'I':
          t7_active = (s[2]-'0');
          t7_button_pressed = (s[3]-'0');
          break;
        case 'J':
          t8_active = (s[2]-'0');
          t8_button_pressed = (s[3]-'0');
          break;
        default:
          break;
      }
      
    // Check validity to see if it's a Hut update
    } else if((strSize == 8) && s[1]=='K' && (s.indexOf('{') == 0) && (s.indexOf('}') == 5)){
      // Loop and update the hut buttons array. s[2] is first button.
      // Then tell the hut manager to update
      
      int button_count = 0;
      for (int i=0; i<5; i++){
        int value = (s[i+2]-'0');
        hut_buttons[i] = value;
        button_count += value;
      }
      
      
    }else {
      Serial.flush();
      Serial1.flush();
    }
  }
}


void updateSlaves() {
  // This method sends over radio the state string for coordination.
  // {0A10} {tree state, trees beacon, hut state, weather state}
    Serial1.print("{");
    Serial1.print(ID);
    Serial1.print(trees_state);
    Serial1.print(trees_current_beacon);
    Serial1.print(hut_state);
    Serial1.print(weather_state);
    Serial1.println("}");
}


void updateServer() {
  // This method sends over radio the state string. It's long, but since there are fewer things going through the Serial cable, we should be able to get away with it.
  // "{a00000000b00000000h00000}" {trees_active[0:8], trees_button[0:8], hut_button[0:5]}
  Serial.print("{");
  Serial.print("a");
  Serial.print(t1_active);
  Serial.print(t2_active);
  Serial.print(t3_active);
  Serial.print(t4_active);
  Serial.print(t5_active);
  Serial.print(t6_active);
  Serial.print(t7_active);
  Serial.print(t8_active);
  Serial.print("b");
  Serial.print(t1_button_pressed);
  Serial.print(t2_button_pressed);
  Serial.print(t3_button_pressed);
  Serial.print(t4_button_pressed);
  Serial.print(t5_button_pressed);
  Serial.print(t6_button_pressed);
  Serial.print(t7_button_pressed);
  Serial.print(t8_button_pressed);
  Serial.print("h");
  Serial.print(hut_buttons[0]);
  Serial.print(hut_buttons[1]);
  Serial.print(hut_buttons[2]);
  Serial.print(hut_buttons[3]);
  Serial.print(hut_buttons[4]);
  Serial.println("}");  
}


void readServerStateUntil() {
  // This reads the state from Unity/ laptop over the Serial port.
  // TODO: UPDATE THIS BASED ON SERVER -> MASTER ARD STRING
  // {s0000} {tree state, trees beacon, hut state, weather state}
  if (Serial.available()) {
    String updateFromServerString = Serial.readStringUntil('\n');
    updateFromServerString.trim();  // trim that newline off
    int strSize = updateFromServerString.length();
    if ((strSize == 6 && (updateFromServerString.indexOf('{') == 0) && (updateFromServerString.indexOf('}') == (6 - 1)))) {
//      Serial1.println(updateFromServerString);
        // update variables
        trees_state = updateFromServerString[2]-'0';
        trees_current_beacon = updateFromServerString[3]-'0';
        hut_state = updateFromServerString[4]-'0';
        weather_state = updateFromServerString[5]-'0';
    } else {
      Serial.flush();
      Serial1.flush();
    }
  }
}



void loop() {
  readSlaveState();
  readServerStateUntil();

  currentTime = millis();
  // determine whether the slaves have been kept waiting too long. If they have, update them.
  if ((currentTime - lastSlaveUpdate) > TIME_BETWEEN_SLAVE_UPDATES) {
    updateSlaves();
    lastSlaveUpdate = currentTime;
  }
  // update the server
  if ((currentTime - lastServerUpdate) > TIME_BETWEEN_SERVER_UPDATES) {
    updateServer();
    lastServerUpdate = currentTime;
  }  
}
