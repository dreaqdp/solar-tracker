#include <Servo.h>
#define STANDBY 0
#define CONTROL 1

const int LED = 4;
const int button = 2; // interrupt pin

int old_val = 0;
volatile int state = STANDBY;
volatile int button_state = LOW;
int led_state = 1;

volatile unsigned long start_time;

Servo servox, servoy;
int posx = 0, posy = 0;

const int ldr_tl = 0; // top left
const int ldr_tr = 1; // top right
const int ldr_bl = 2; // bottom left
const int ldr_br = 3; // bottom right
int val_tl, val_tr, val_bl, val_br;

void setup() {
  pinMode (LED, OUTPUT);
  pinMode (button, INPUT);
  Serial.begin(9600);

  servox.attach(9);
  servoy.attach(10);
  servox.write(0);
  
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
    if (delta >= 1000) {
      state = (state == STANDBY ? CONTROL : STANDBY);
    }
  }
}

void rest () {
  servox.write(0);
  servoy.write(0);
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
    //Serial.println(led_state);
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
// Servo
  /*
  for (posx = 0; posx < 180; posx += 5) {
    servox.write(posx);
    delay(200);
  }
  for (posy = 0; posy < 180; posy +=5) {
    servoy.write(posy);
    delay(200);
  }*/
// ldr
  val_tl = analogRead(ldr_tl);
  Serial.println(val_tl);
  val_tr = analogRead(ldr_tr);
  Serial.println(val_tr);
  val_bl = analogRead(ldr_bl);
  Serial.println(val_bl);
  val_br = analogRead(ldr_br);
  Serial.println(val_br);
  Serial.println("--------------");
  
  delay (500);
}
