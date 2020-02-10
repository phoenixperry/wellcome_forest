void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600); 
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial1.available()){
  String s = Serial1.readStringUntil('\n');
  unsigned int my_size = s.length(); 
  
    if(s[0]=='_' && s[my_size-2]=='!') //check to make sure we have a complete data string
    {
      Serial.println(s);//send it out 
    } else{
        Serial.flush(); //clear the port 
      }
  }
     delay(50);//delay to have time to get data
}
