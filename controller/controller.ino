#define STANDBY 0
#define CONTROL 1

const int LED = 4;
const int button = 2; // interrupt pin

int old_val = 0;
volatile int state = STANDBY;
volatile int button_state = LOW;
int led_state = 1;

volatile unsigned long start_time;


void setup() {
  pinMode (LED, OUTPUT);
  pinMode (button, INPUT);
  Serial.begin(9600);
  
  attachInterrupt(digitalPinToInterrupt(button), button_interrupt, CHANGE);
}

void button_interrupt () {
  int but = digitalRead(button);
  if (but == HIGH && button_state == LOW) {
    Serial.println("HIGH");
    button_state = HIGH;
    start_time = millis();
  } else if (but == LOW && button_state == HIGH) {
    Serial.println("LOW\n----");
    button_state = LOW;
    int delta = millis() - start_time;
    if (delta >= 2000) {
      state = (state == STANDBY ? CONTROL : STANDBY);
    }
  }
}


void loop() {
  //Serial.println(digitalRead(button));
  
  if (state == STANDBY) {
    digitalWrite(LED, HIGH);
    /*
    while (digitalRead(button) == LOW) ;
    start_time = millis();
    delay(10);

    while (digitalRead(button) == HIGH && state == OFF) {
      if (millis() - start_time > 2000) {
        state = ON;
      }
    }*/
  }
  else {
    //digitalWrite(LED, LOW);
    Serial.println(led_state);
    if (!led_state) {
      digitalWrite(LED, HIGH);
      delay(100);
      led_state = 1;
    }
    else {
      digitalWrite(LED, LOW);
      delay(100);
      led_state = 0;
    }
  }
}
