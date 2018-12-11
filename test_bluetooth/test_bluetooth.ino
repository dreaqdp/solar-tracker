#include <SoftwareSerial.h>

SoftwareSerial BTSerial(6, 7);

const int led = 4;
int state = 0;

void setup() {
  // put your setup code here, to run once:
  BTSerial.begin(9600);
  Serial.begin(9600);

  /*BTSerial.write("AT+RESET\n");
  BTSerial.write("AT+NAMELULZ\n");*/

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

}


char c = ' ';

void loop() {
  // put your main code here, to run repeatedly:
  if (BTSerial.available() > 0) {
      
    c = BTSerial.read();
    Serial.write(c);
    //BTSerial.println("AT");
  }

  if (Serial.available()) {
    c = Serial.read();

    Serial.write(c);
    BTSerial.write(c);
    }
  

  
  
  
  
  
  if (state) {
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
  state ^= 0;
}
