// This master lives with the laptop and feeds Unity with all the info it needs. It also sends out lovely data to the trees, clouds, and yurt!

// Declare constants.
int TIME_BETWEEN_SLAVE_UPDATES = 100;
int NUM_TREES = 8;
int NUM_HUTS = 1;
int NUM_CLOUDS = 2;
int NUM_STATES = 1;  // number of states for each slave
int NUM_GLOBAL_STATES = 3;
char ID = 'Z';
int GAMETIME_LIMIT = 60000;
int TIME_LIMIT = GAMETIME_LIMIT; // 60000ms = 1 minute timer. This is for the whole game. 
int TREE_WIN_DURATION = 30000; // 30s Time allowed for players to get both the win states for the hut and the trees. This should correspond to either's winning animation.
int TREES_FAIL_ANIMATION_DURATION = 30000;  // 3 seconds for failure animation
int HUT_WIN_DURATION = 30000; // 30s duration for hut playing animation. This is time for the tree players to also win
int GLOBAL_WIN_DURATION = 30000; // 30s for global win state


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
int server_weather_state = 0;  // since there may be conflicts between the weather that the server sends us and this. 
char trees_button = '0';  // 0 I guess if nothing is pressed. Otherwise the letter ID of the button.
char hut_button = '0';  // 0 if not pressed

// Clouds IDs A-B

// Trees IDs C-J
int trees_state = 0; // idle/playing/win/lose (0,1,2,3)
char trees_current_beacon = 'C'; // ID of lit tree
//int trees_currently_lit = 0;

char t1_id = 'C';
bool t1_local_win = 0;
//bool t1_beacon = 0;

char t2_id = 'D';
bool t2_local_win = 0;
//bool t2_beacon = 0;

char t3_id = 'E';
bool t3_local_win = 0;
//bool t3_beacon = 0;

char t4_id = 'F';
bool t4_local_win = 0;
//bool t4_beacon = 0;

char t5_id = 'G';
bool t5_local_win = 0;
//bool t5_beacon = 0;

char t6_id = 'H';
bool t6_local_win = 0;
//bool t6_beacon = 0;

char t7_id = 'I';
bool t7_local_win = 0;
//bool t7_beacon = 0;

char t8_id = 'J';
bool t8_local_win = 0;
//bool t8_beacon = 0;
//
//bool trees_wins[8] = {}
//bool trees_beacons[8] = {}

// Hut ID K
bool hut_state = 0;  // idle/playing/win/lose (0,1,2,3)

// YES I SHOULD DO A MORE CLEVER WAY TO MANAGE STATES.

// TESTING
bool doTestOnStartup = 1;

void setup() {
  //Begin serial monitor port - this is the cable.
  Serial.begin(9600);
  //Begin HW serial - this is the radio.
  Serial1.begin(9600);
  delay(50);
  lastUpdate = millis();

  if (doTestOnStartup){
    testGames();
  }
  
}


// This method tests the game states, including animations. It's a visual test. 
// It waits for user input over serial to proceed. If Y, the whole thing proceeds. If N, the cycle continues.
bool testGames(){
  // cycle through the states one by one to get visual confirmation that everything functions properly.
  Serial.println("Testing the beacon assignments.");
  Serial.println("Z0C0000");  // starting idle state with no buttons
  Serial1.println("Z0C0000");
  delay(1000);
  Serial.println("Z1D0000");  // test game playing, beacon on 2nd tree
  Serial1.println("Z1D0000");
  delay(1000);
  Serial.println("Z1E0000");  // test game playing, beacon on 3rd tree
  Serial1.println("Z1E0000");
  delay(1000);
  Serial.println("Z1F0000");  // test game playing, beacon on 4th tree
  Serial1.println("Z1F0000");
  delay(1000);
  Serial.println("Z1G0000");  // test game playing, beacon on 5th tree
  Serial1.println("Z1G0000");
  delay(1000);
  Serial.println("Z1H0000");  // test game playing, beacon on 6th tree
  Serial1.println("Z1H0000");
  delay(1000);
  Serial.println("Z1I0000");  // test game playing, beacon on 7th tree
  Serial1.println("Z1I0000");
  delay(1000);
  Serial.println("Z1J0000");  // test game playing, beacon on 8th tree
  Serial1.println("Z1J0000");
  delay(1000);
  
  // Testing Tree victory state management
  Serial.println("Resetting Game and testing local win states to see that the game is won under correct conditions. Game timer currently set to 60 seconds.");
  resetAllTreesState();
  treeGameManager();
  updateSlaves();
  updateServer();
  delay(1000);
  t1_local_win = true;
  treeGameManager();
  updateSlaves();
  updateServer();
  Serial.print("Asserting tree_state == 1: ");
  Serial.println(trees_state==1);
  delay(1000);
  t2_local_win = true;
  treeGameManager();
  delay(100);
  updateSlaves();
  updateServer();
  delay(1000);
  t3_local_win = true;
  treeGameManager();
  delay(100);
  updateSlaves();
  updateServer();
  delay(1000);
  t4_local_win = true;
  treeGameManager();
  delay(100);
  updateSlaves();
  updateServer();
  delay(1000);
  t5_local_win = true;
  treeGameManager();
  delay(100);
  updateSlaves();
  updateServer();
  delay(1000);
  t6_local_win = true;
  treeGameManager();
  delay(100);
  updateSlaves();
  updateServer();
  delay(1000);
  t7_local_win = true;
  treeGameManager();
  delay(100);
  updateSlaves();
  updateServer();
  delay(1000);
  t8_local_win = true;
  treeGameManager();
  delay(100);
  Serial.print("Asserting that tree_state == 2 to signal game won.: ");
  Serial.println(trees_state==2);
  updateSlaves();
  updateServer();
  Serial.println("Check for trees victory animation. Please wait and watch for ");
  Serial.print(TREE_WIN_DURATION);
  Serial.println(" ms for it to complete.");
  delay(TREE_WIN_DURATION);
  
  Serial.println("Resetting game to test failure conditions. Please wait for: ");
  Serial.print(TIME_LIMIT);
  Serial.println(" ms for it to complete.");
  resetAllTreesState();  // reset game 
  treeGameManager();
  updateSlaves();
  updateServer();
  t1_local_win = true;
  treeGameManager();
  updateSlaves();
  updateServer();
  delay(TIME_LIMIT+1000);
  treeGameManager();
  updateSlaves();
  updateServer();
  Serial.print("Assert game state is now 3 to signal failure: ");
  Serial.println(trees_state==3);
  
  Serial.print("Check for failure animation. Please wait and watch it for ");
  Serial.print(TREES_FAIL_ANIMATION_DURATION);
  Serial.println(" ms for it to complete");
  delay(TREES_FAIL_ANIMATION_DURATION);
  
  treeGameManager();
  updateSlaves();
  updateServer();
  Serial.print("Assert game state is now 0 to signal automatic timed reset: ");
  Serial.println(trees_state==0);

  Serial.println("Resetting and testing weather tests");
  resetAllTreesState();
  hut_state = 2;
  trees_state = 2;
  treeTimer = millis();
  currentTime = millis();
  weatherManager();
  updateSlaves();
  updateServer();
  Serial.print("Asserting that weather state == 3: ");
  Serial.println(weather_state==3);
  Serial.print("Please wait and watch the global win animation for: ");
  Serial.print(GLOBAL_WIN_DURATION/2);
  Serial.println(" ms");
  weatherManager();
  Serial.print("Midpoint check: Asserting that weather state == 3: ");
  Serial.println(weather_state==3);
  delay(GLOBAL_WIN_DURATION/2 +1000);
  weatherManager();
  Serial.print("Animation completion check: Asserting that weather state == 0: ");
  Serial.println(weather_state==0);
  resetAllTreesState();
  resetAllHutState();
  resetWeatherState();

  Serial.println("Testing complete. If any errors visible, please hurry and fix them. <3 <3 <3");
}

void resetAllTreesState(){
  t1_local_win = 0;
  t2_local_win = 0;
  t3_local_win = 0;
  t4_local_win = 0;
  t5_local_win = 0;
  t6_local_win = 0;
  t7_local_win = 0;
  t8_local_win = 0;
  trees_state = 0;
  trees_current_beacon = 'C';
  treeTimer = 0;
}


void resetAllHutState(){
  hut_state = 0;
  hutTimer = 0;
}


void resetWeatherState(){
  weather_state = 0;
}


void readUpdateSlaveState() {
  // This method reads the states from Slaves as they come in, and updates states.
  if (Serial1.available()) {
    String s = Serial1.readStringUntil('\n');
    Serial.println(s);
    s.trim();  // trim that newline off
    int strSize = s.length();
    if ((strSize == 5) && (s.indexOf('{') == 0) && (s.indexOf('}') == 4)) {
      char switchChar = (char) s[1];
      
      if ((bool) s[3]){
        trees_button = (char) s[3];
        // directly send button?
      }
      switch (switchChar) {
        // Tree state updates
        case 'C':
          // cast a char containing an int to its corresponding int by getting the distance from the '0' char
          // '1' - '0' -> 1  but only 0 to 9.
          // There's another -48 thing but screw that.
          t1_local_win = (s[2]-'0');
          break;
        case 'D':
          t2_local_win = (s[2]-'0');
          break;
        case 'E':
          t3_local_win = (s[2]-'0');
          break;
        case 'F':
          t4_local_win = (s[2]-'0');
          break;
        case 'G':
          t5_local_win = (s[2]-'0');
          break;
        case 'H':
          t6_local_win = (s[2]-'0');
          break;
        case 'I':
          t7_local_win = (s[2]-'0');
          break;
        case 'J':
          t8_local_win = (s[2]-'0');
          break;
        default:
          break;
      }
    } else {
      Serial.flush();
    }
  }
}


void treeGameManager() {
  // Someone started the game. t1 is a local win but no others.
  if (trees_state == 0) {
    // Someone lit the first beacon
    if (t1_local_win && !t2_local_win && !t3_local_win && !t4_local_win && !t5_local_win && !t6_local_win && !t7_local_win && !t8_local_win) {
      gameTimer = millis();
      trees_state = 1;  // tree game playing
      trees_current_beacon = 'D'; // light up t2
    }
  } else if (trees_state == 1) {
    if (currentTime - gameTimer < TIME_LIMIT) {
      // beacon 1-2 local_win is good, set the next beacon
      if (t1_local_win & t2_local_win & !t3_local_win & !t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = 'E';

        // beacon 1-3 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & !t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = 'F';

        // beacon 1-4 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & !t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = 'G';

        // beacon 1-5 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & !t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = 'H';

        // beacon 1-6 local_win is good, set the next beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & t6_local_win & !t7_local_win & !t8_local_win) {
        trees_current_beacon = 'I';

        // beacon 1-7 local_win is good, set the final beacon
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & t6_local_win & t7_local_win & !t8_local_win) {
        trees_current_beacon = 'J';

        // The beacons of Minas Tirith, the beacons are lit! Gondor calls for aid!
        // The Trees game is won. All local_wins are 1's.
      } else if (t1_local_win & t2_local_win & t3_local_win & t4_local_win & t5_local_win & t6_local_win & t7_local_win & t8_local_win) {
        trees_state = 2;
        treeTimer = millis();  // set timer for victory animation
      }else{
        trees_state = 1;
      }
      // Game over: Ran out of time. Run fail animation.
    } else {
      trees_state = 3;
      treeTimer = millis(); // set timer for failure animation
    }

    // Run the tree victory animation for t=TREE_WIN_DURATION
  } else if (trees_state == 2) {
    // Victory animation has elapsed - reset the trees. Do nothing if it's not done.
    if (currentTime - treeTimer > TREE_WIN_DURATION) {
      trees_state = 0;  // reset to idle.
      trees_current_beacon = 'C'; // light up t1 to signal starting point
    }

    // End the tree failure animation after t=TREES_FAIL_ANIMATION_DURATION
  } else if (trees_state == 3 && (currentTime - treeTimer > TREES_FAIL_ANIMATION_DURATION)) {
    trees_state = 0;  // reset to idle.
    trees_current_beacon = 'C'; // light up t1 to signal starting point

    // Tree game should keep idling
  } else {
    trees_state = 0;
    trees_current_beacon = 'C';
  }
}

void weatherManager() {
  // If the hut is also in a winning state, but the treeTimer hasn't elapsed yet, run the tree part of the big win animation.
  if (hut_state == 2 && trees_state == 2 && (currentTime - treeTimer < TREE_WIN_DURATION) && (currentTime - hutTimer < HUT_WIN_DURATION) && weather_state != 3) {
    weather_state = 3;
    animationTimer = millis();  // set the animation timer

  // end the animation. Time is elapsed.
  } else if (weather_state == 3 && (currentTime - animationTimer > GLOBAL_WIN_DURATION)) {
    trees_state = 0;
    trees_current_beacon = 'C';
    hut_state = 0;
    weather_state = 0;

  // just keep going. Mostly a placeholder. Not very good code to use an else like this, but there's not really another state.
  } else {
    weather_state = weather_state;
  }
}


void updateSlaves() {
  // This method sends over radio the state string
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
  // This method sends over radio the state string
  // {0C10C0} {tree state, trees beacon, hut state, weather state, trees_button, hut_button}
  Serial.print("{");
  Serial.print(ID);
  Serial.print(trees_state);
  Serial.print(trees_current_beacon);
  Serial.print(hut_state);
  Serial.print(weather_state);
  Serial.print(trees_button);
  Serial.print(hut_button);
  Serial.println("}");  
}


void readServerStateUntil() {
  // This reads the state from Unity/ laptop over the Serial port.
  // TODO: UPDATE THIS BASED ON SERVER -> MASTER ARD STRING
  if (Serial.available()) {
    String updateFromServerString = Serial.readStringUntil('\n');
    updateFromServerString.trim();  // trim that newline off
    int strSize = updateFromServerString.length();
    if ((strSize == 3 && (updateFromServerString.indexOf('{') == 0) && (updateFromServerString.indexOf('}') == (3 - 1)))) {
//      Serial1.println(updateFromServerString);
      // update variables
      server_weather_state = updateFromServerString[1] -'0';  // not even sure there will be any communication server -> arduinos. 
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
  treeGameManager();
  weatherManager();

  // determine whether the slaves have been kept waiting too long. If they have, update them.
  currentTime = millis();
  if ((currentTime - lastUpdate)  > TIME_BETWEEN_SLAVE_UPDATES) {
    updateSlaves();
    updateServer();  // sanity checking
    lastUpdate = currentTime;
  }
}
