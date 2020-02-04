// This master lives with the laptop and feeds Unity with all the info it needs. It also sends out lovely data to the trees, clouds, and yurt!

// Declare constants.
int TIME_BETWEEN_UPDATES = 200; // ms between updating the slaves (Trees, hut, clouds)
int TIME_BETWEEN_SLAVE_UPDATES = 200;
int NUM_TREES = 8;
int NUM_HUTS = 1;
int NUM_CLOUDS = 2;
int NUM_STATES = 3;  // number of states for each slave
int NUM_GLOBAL_STATES = 3;
int ID = 0;
int TIME_LIMIT = 600000; // 60000ms = 1 minute timer. This is for the whole game.
int TREE_WIN_DURATION = 300000; // 30s Time allowed for players to get both the win states for the hut and the trees. This should correspond to either's winning animation.
int TREES_FAIL_ANIMATION_DURATION = 30000;  // 3 seconds for failure animation
int HUT_WIN_DURATION = 300000; // 30s duration for hut playing animation. This is time for the tree players to also win
int GLOBAL_WIN_DURATION = 300000; // 30s for global win state

// Declare local variables
int lastUpdate;
int currentTime;
String updateFromServerString = "";

// GAME STATE MANAGEMENT VARIABLES
// Global game state
int gameTimer;
int treeTimer;
int hutTimer;
int animationTimer;  // for use with global/weather timers. Trees and hut have their own individual timers for local stuff. See below.
int weather_state = 0;  // idle, night, summer storm, cherry blossoms (0, 1, 2, 3)

// Clouds IDs A-B

// Trees IDs C-J
int trees_state = 0; // idle/playing/win/lose (0,1,2,3)
int trees_current_beacon = "C"; // ID of lit tree
//int trees_currently_lit = 0;

int t1_id = "C";
bool t1_local_win = 0;
//bool t1_beacon = 0;

int t2_id = "D";
bool t2_local_win = 0;
//bool t2_beacon = 0;

int t3_id = "E";
bool t3_local_win = 0;
//bool t3_beacon = 0;

int t4_id = "F";
bool t4_local_win = 0;
//bool t4_beacon = 0;

int t5_id = "G";
bool t5_local_win = 0;
//bool t5_beacon = 0;

int t6_id = "H";
bool t6_local_win = 0;
//bool t6_beacon = 0;

int t7_id = "I";
bool t7_local_win = 0;
//bool t7_beacon = 0;

int t8_id = "J";
bool t8_local_win = 0;
//bool t8_beacon = 0;
//
//bool trees_wins[8] = {}
//bool trees_beacons[8] = {}

// Hut ID K
bool hut_state = 0;  // idle/playing/win/lose (0,1,2,3)


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


void readUpdateSlaveState() {
  // This method reads the states from Slaves as they come in, and updates states.
  if (Serial1.available()) {
    String s = Serial1.readStringUntil('\n');
    s.trim();  // trim that newline off
    int strSize = s.length();
    if ((strSize == 4) && (s.indexOf('{') == 0) && (s.indexOf('}') == 3)) {
      //        Serial.println(s);

      switch (s[1]):
          // Tree state updates
        case "C":
        t1_local_win = s[2]
                     case "D":
        t2_local_win = s[2]
                     case "E":
        t3_local_win = s[2]
                     case "F":
        t4_local_win = s[2]
                     case "G":
        t5_local_win = s[2]
                     case "H":
        t6_local_win = s[2]
                     case "I":
        t7_local_win = s[2]
                     case "J":
        t8_local_win = s[2]
                     default:
        break
      } else {
      Serial.flush();
    }
  }
}


void treeGameManager() {
  // Someone started the game. t1 is a local win but no others.
  if (trees_state == 0) {
    // Someone lit the first beacon
    if (t1_local_win & !t2_local_win & !t3_local_win & !t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
      gameTimer = millis();
      trees_state = 1;  // tree game playing
      trees_current_beacon = "D"; // light up t2
    }
  } else if (trees_state == 1) {
    if (currentTime - gameTimer < TIME_LIMIT) {
      // beacon 1-2 local_win is good, set the next beacon
      if (t1_local_win & t2_local_win & !t3_local_win & !t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = "E";

        // beacon 1-3 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & !t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = "F";

        // beacon 1-4 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = "G";

        // beacon 1-5 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = "H";

        // beacon 1-6 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = "I";

        // beacon 1-7 local_win is good, set the final beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & t6_local_win & t7_local_win & !t8_local_win) {
        trees_current_beacon = "J";

        // The beacons of Minas Tirith, the beacons are lit! Gondor calls for aid!
        // The Trees game is won. All local_wins are 1's.
      } else {
        trees_state = 2;
        treeTimer = millis();
      }

      // Game over: Ran out of time. Run fail animation.
    } else {
      tree_state = 3;
      treeTimer = millis();
    }

    // Run the tree victory animation for t=TREE_WIN_DURATION
  } else if (trees_state == 2) {
    // Victory animation has elapsed - reset the trees. Do nothing if it's not done.
    if (currentTime - treeTimer > TREE_WIN_DURATION) {
      trees_state = 0;  // reset to idle.
      trees_current_beacon = "C"; // light up t1 to signal starting point
    }

    // End the tree failure animation after t=TREES_FAIL_ANIMATION_DURATION
  } else if (trees_state == 3 && (currentTime - treeTimer > TREES_FAIL_ANIMATION_DURATION)) {
    trees_state = 0;  // reset to idle.
    trees_current_beacon = "C"; // light up t1 to signal starting point

    // Tree game should keep idling
  } else {
    tree_state = 0;
    trees_current_beacon = "C";
  }
}

void weatherManager() {
  // If the hut is also in a winning state, but the treeTimer hasn't elapsed yet, run the tree part of the big win animation.
  if (hut_state == 2 && tree_state == 2 && (currentTime - treeTimer > TREE_WIN_DURATION) && (currentTime - hutTimer > HUT_WIN_DURATION) && weather_state!=3) {
    weather_state = 3;
    animationTimer = millis();  // set the animation timer

  // end the animation. Time is elapsed.
  } else if (weather_state==3 && (currentTime - animationTimer > GLOBAL_WIN_DURATION)) {
    tree_state = 0;
    trees_current_beacon = "C";
    hut_win = 0;
    weather_state = 0;

  // just keep going. Mostly a placeholder. Not very good code to use an else like this, but there's not really another state. 
  } else if (weather_state==3 && (currentTime - animationTimer < GLOBAL_WIN_DURATION)){
    weather_state = 3;

  // defaults to idle
  }else{
    weather_state = 0;
  }
}


void updateSlaves() {
  // This method sends over radio the state string
  // {0A10} {tree state, trees beacon, hut state, weather state}
  if (Serial1.available()){
    Serial1.print("{");
    Serial1.print(trees_state);
    Serial1.print(trees_beacon);
    Serial1.print(hut_state);
    Serial1.print(weather_state);
    Serial1.println("}");
  }

}

void readServerStateUntil() {
  // This reads the state from Unity/ laptop over the Serial port
  if (Serial.available()) {
    String updateFromServerString = Serial.readStringUntil('\n');
    updateFromServerString.trim();  // trim that newline off
    int strSize = updateFromServerString.length();
    if ((strSize == (2 + NUM_STATES * (NUM_TREES + NUM_HUTS + NUM_CLOUDS))) && (updateFromServerString.indexOf('{') == 0) && (updateFromServerString.indexOf('}') == (2 + NUM_STATES * (NUM_TREES + NUM_HUTS + NUM_CLOUDS) - 1))) {
      Serial1.println(updateFromServerString);
      //        Serial.println(updateFromServerString);
    } else {
      Serial.flush();
      Serial1.flush();
    }
  }
}


//void readServerState(){
//  // This reads the state from Unity/ laptop over the Serial port
//  if(Serial.available()){
//    char ch;
//    String cha;
//    ch = (char) Serial.read();
//    cha = (String) ch;
//    if (cha == "{"){
//      updateFromServerString = cha;
//    }else if(cha == "\n"){
//      updateFromServerString = updateFromServerString + cha;
//      updateFromServerString.trim();
//      int strSize = updateFromServerString.length();
//      if((strSize==(2+NUM_STATES*(NUM_TREES+NUM_HUTS+NUM_CLOUDS))) && (updateFromServerString.indexOf('{')==0) && (updateFromServerString.indexOf('}')==(2+NUM_STATES*(NUM_TREES+NUM_HUTS+NUM_CLOUDS)-1))){
//        Serial1.println(updateFromServerString);
////        Serial.println(updateFromServerString);
//      }else{
//        Serial.flush();
//        Serial1.flush();
//      }
//    }else{
//      updateFromServerString = updateFromServerString + cha;
//    }
//  }
//}


void loop() {
  // put your main code here, to run repeatedly:
  readUpdateSlaveState();
  readServerStateUntil();

  // determine whether the slaves have been kept waiting too long. If they have, update them.
  currentTime = millis();
  if ((currentTime - lastUpdate)  > TIME_BETWEEN_SLAVE_UPDATES) {
    updateSlaves();
    lastUpdate = currentTime;
  }
}
