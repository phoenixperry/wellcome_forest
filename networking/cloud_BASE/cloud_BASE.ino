// CLOUD 1 IS ON THE UNO - SO IT ONLY HAS 1 SERIAL PORT.

// Constants
int TIME_BETWEEN_UPDATES = 200;
int ID = 1;  // Uno!

// Variables
bool playing = 0;


void setup() {
  // Uno only has 1 serial port. 
  Serial.begin(9600);
}


void sendState(){
  Serial.print("{");
  Serial.print(ID);
  Serial.print(playing);
  Serial.println("}");
}


void loop() {
  // put your main code here, to run repeatedly:
  sendState();
  delay(TIME_BETWEEN_UPDATES);
}
