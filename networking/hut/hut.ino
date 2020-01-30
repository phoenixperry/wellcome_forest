// HUT is on a Teensy for the IO

// Constants
int TIME_BETWEEN_UPDATES = 200;

String ID = "B";
// Variables
bool playing = 0;

void setup() {
//  Serial.begin(9600);
  //Begin HW serial
  Serial1.begin(9600);
}

void sendState(){
  Serial1.print("{");
  Serial1.print(ID);
  Serial1.print(playing);
  Serial1.println("}");
}

void loop() {
  // put your main code here, to run repeatedly:
  sendState();
  delay(TIME_BETWEEN_UPDATES);

}
