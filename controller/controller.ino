#include <Servo.h>
#include <SoftwareSerial.h>
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
int top, bottom, left, right;
int old_top, old_bottom, old_left, old_right;
float vfactor = (5.0/1023.0);
float vout_tl, vout_tr, vout_bl, vout_br;

SoftwareSerial BTSerial(6, 7);
char bt_state;

void setup() {
  pinMode (LED, OUTPUT);
  pinMode (button, INPUT);
  Serial.begin(9600);  
  BTSerial.begin(9600);
  
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

void ldr_stats (int val_ldr) {
  float tmp = (5.0/1023.0);
      float vout = val_ldr * tmp;
      float res = (5-vout)*10000/vout; // RL = (Vin - Vout)Rs/Vout
      BTSerial.println(vout);
      BTSerial.println(res);
}

void loop() {
  //Serial.println(digitalRead(button));
  bt_state = ' ';
  if (state == STANDBY) {
    digitalWrite(LED, HIGH);
    posx = 180;
    posy = 90;
  }
  else {
    posx = 90;
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
      vout_tl = val_tl * vfactor;
      //Serial.println(val_tl);
      val_tr = analogRead(ldr_tr);
      vout_tr = val_tr * vfactor;
      //Serial.println(val_tr);
      val_bl = analogRead(ldr_bl);
      vout_bl = val_bl * vfactor;
      //Serial.println(val_bl);
      val_br = analogRead(ldr_br);
      vout_br = val_br * vfactor;
      //Serial.println(val_br);
      top = (val_tl + val_tr)/2;
      bottom = (val_bl + val_br)/2;
      left  = (val_tl + val_bl)/2;
      right = (val_tr + val_br)/2;

      // servos
      if (top > 700 || bottom > 700 || left > 700 || right > 700) {
        if (top > bottom && posx > 0 && top >= old_top+5) {
          posx -= 5;
        }
        else if (top < bottom && posx < 180 && bottom >= old_bottom+5) {
          posx += 5;
        }
        old_bottom = bottom;
        old_top = top;
      
        if (left > right && posy < 180 && left > old_left+5) {
          posy += 5;
        }
        else if (left < right && posy > 0 && right > old_right+5) {
          posy -= 5;
        }
        old_left = left;
        old_right = right;
      }
      else old_top = old_bottom = old_left = old_right = 700;
    }
  }


  // Movem els motors
  if (posx != oldx) {
    //Serial.println(posx);
    servox.write(posx);
    oldx = posx;
  }
  if (posy != oldy) {
    //Serial.println(posy);
    servoy.write(posy);
    oldy = posy;
  }
  
  // integracio amb bluetooth
  if (BTSerial.available()) {
    bt_state = BTSerial.read();
    Serial.write(bt_state);
    if (bt_state == 'x') { // canviar pel que ens retorni l'app al pitjar el boto de desactivar stma
      state = STANDBY;
    }
    else if (bt_state == 'y') { // suposat boto de passar a activar stma
      state = CONTROL;
    }
    else if (bt_state == 'a') { // boto petició estat stma
      BTSerial.println(state);
    }
    else if (bt_state == 'b') { // boto peticio pos servos
      BTSerial.println(posx);
      BTSerial.println(posy);
    }
    else if (bt_state == 'c') { // boto peticio nivells lluminusitat, top, bottom
      BTSerial.println(top);
      BTSerial.println(bottom);
      BTSerial.println(left);
      BTSerial.println(right);
    }
    else if (bt_state == 'd') { // boto peticio voltatge
      BTSerial.println(vout_tl);
      BTSerial.println(vout_tr);
      BTSerial.println(vout_bl);
      BTSerial.println(vout_br);
      
      
    }
    else if (bt_state == 'e') { // boto peticio ldr resistencies
      float res_tl = (5-vout_tl)*10000/vout_tl; // RL = (Vin - Vout)Rs/Vout
      float res_tr = (5-vout_tr)*10000/vout_tr;
      float res_bl = (5-vout_bl)*10000/vout_bl;
      float res_br = (5-vout_br)*10000/vout_br;
      BTSerial.println(res_tl);
      BTSerial.println(res_tr);
      BTSerial.println(res_bl);
      BTSerial.println(res_br);      
    }
  }

  Serial.println("--------------");
  
  delay (500);
}
