
#define HWSERIAL Serial1
void setup() {
  // put your setup code here, to run once:
  HWSERIAL.begin(9600); 
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  HWSERIAL.println("_hello!"); 
  Serial.println("hello"); 
}
