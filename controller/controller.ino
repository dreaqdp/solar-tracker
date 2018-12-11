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
int posx, posy;
int oldx, oldy;

const int ldr_tl = 0; // top left
const int ldr_tr = 1; // top right
const int ldr_bl = 2; // bottom left
const int ldr_br = 3; // bottom right
int val_tl, val_tr, val_bl, val_br;
int old_top, old_bottom, old_left, old_right;

void setup() {
  pinMode (LED, OUTPUT);
  pinMode (button, INPUT);
  Serial.begin(9600);

  
  
  servox.attach(9);
  servoy.attach(10);
  

  old_top = old_bottom = old_left = old_right = 700;
  
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
    if (delta >= 1500) {
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
    posx = 180;
    posy = 90;
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
    posx = 90;
    //servox.write(posx);
    if (!led_state) {
      digitalWrite(LED, HIGH);
      delay(100);
      led_state = 1;
    }
    else {
      digitalWrite(LED, LOW);
      delay(100);
      led_state = 0;
    
      // ldr
      val_tl = analogRead(ldr_tl);
      Serial.println(val_tl);
      val_tr = analogRead(ldr_tr);
      Serial.println(val_tr);
      val_bl = analogRead(ldr_bl);
      Serial.println(val_bl);
      val_br = analogRead(ldr_br);
      Serial.println(val_br);
      int top = (val_tl + val_tr)/2;
      int bottom = (val_bl + val_br)/2;
      int left  = (val_tl + val_bl)/2;
      int right = (val_tr + val_br)/2;

      // servos
      if (top > 700 || bottom > 700 || left > 700 || right > 700) {
        if (top > bottom && posx > 0 && top >= old_top+5) {
          posx -= 5;
          //old_top = top;
          //old_bottom = bottom;
        }
        else if (top < bottom && posx < 180 && bottom >= old_bottom+5) {
          posx += 5; 
          
        }
        old_bottom = bottom;
        old_top = top;
        //servox.write(posx);
      
        if (left > right && posy < 180 && left > old_left+5) {
          posy += 5;
        }
        else if (left < right && posy > 0 && right > old_right+5) {
          posy -= 5;
        }
        old_left = left;
        old_right = right;
        //servoy.write(posy);
      }
      else old_top = old_bottom = old_left = old_right = 700;
    }
  }


  // Movem els motors
  if (posx != oldx) {
    Serial.println(posx);
    servox.write(posx);
    oldx = posx;
  }
  if (posy != oldy) {
    Serial.println(posy);
    servoy.write(posy);
    oldy = posy;
  }

  Serial.println("--------------");
  
  delay (500);
}
