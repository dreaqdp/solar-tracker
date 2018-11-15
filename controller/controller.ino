#define STANDBY 0
#define CONTROL 1

const int LED = 4;
const int button = 2; // interrupt pin

int old_val = 0;
int state = STANDBY;

unsigned long start_time;
void setup() {
  pinMode (LED, OUTPUT);
  pinMode (button, INPUT);
  attachInterrupt(digitalPinToInterrupt(button), button_interrupt, CHANGE);
}

void button_interrupt () {
  start_time = millis();
}


void loop() {
  if (state == OFF) {
    digitalWrite(LED, LOW);
    while (digitalRead(button) == LOW) ;
    start_time = millis();
    delay(10);

    while (digitalRead(button) == HIGH && state == OFF) {
      if (millis() - start_time > 2000) {
        state = ON;
      }
    }
  }
  else digitalWrite(LED, HIGH);
  
  
  

}
