#include "matrix_led_ir.h"
#include <Servo.h>

Servo servo_in;
Servo servo_out;

void setup() {
  // put your setup code here, to run once:
  init_matrixes();
  servo_in.attach(D1, 1010, 2010);  // attaches the servo on pin 9 to the servo object
  servo_out.attach(D2, 930, 1970);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
}

extern uint32_t ir_led_states;
uint8_t t=0;
void loop() {
  // put your main code here, to run repeatedly:
  delay(10);

  //ir_led_states = ir_led_states<<2;
  //if(ir_led_states==0)ir_led_states=2;
  char str[10];
  sprintf(str,"%02X",t);
  show_assign_matrix(str[0], 0);
  sprintf(str,"%02X",0xFF-t);
  show_free_matrix(str[0], str[1]);
  for(int i=0; i<16; i++){
    Serial.print(read_ir_sensor(i));
    set_led(i, read_ir_sensor(i));
  }
  Serial.print(" ");
  Serial.print(read_ir_sensor(IR_IN_BEFORE));
  Serial.print(read_ir_sensor(IR_IN_AFTER));
  Serial.print(read_ir_sensor(IR_OUT_BEFORE));
  Serial.print(read_ir_sensor(IR_OUT_AFTER));
  servo_in.write((!read_ir_sensor(IR_IN_BEFORE) || !read_ir_sensor(IR_IN_AFTER))?0:180);
  servo_out.write((!read_ir_sensor(IR_OUT_BEFORE) || !read_ir_sensor(IR_OUT_AFTER))?0:180);
  Serial.println();
  t++;
}
