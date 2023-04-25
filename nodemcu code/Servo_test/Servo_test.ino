#include <Servo.h>

Servo servo_in;
Servo servo_out;

int pos_in = 0, pos_out = 0;    // variable to store the servo position

void setup() {
  servo_in.attach(5, 1010, 2010);  // attaches the servo on pin 9 to the servo object
  servo_out.attach(4, 930, 1970);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
}

void loop() {
  servo_in.write(pos_in);
  servo_out.write(pos_out);

  /*if(Serial.available()){
    char c = Serial.read();
    if(c == 'i'){
      pos_in = 180 - pos_in;
    }
    if(c == 'o'){
      pos_out = 180 - pos_out;
    }
  }*/
  
  if(digitalRead(D8)){
    pos_in = 180;
    Serial.println(180);
  }else{
    pos_in = 0;
    Serial.println(0);
  }
}
