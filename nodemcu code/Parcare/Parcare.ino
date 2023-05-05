#include <Servo.h>
#include <SPI.h>
#include "license_read.h"
#include "matrix_led_ir.h"

#define OFFLINE_MODE 0
#define ONLINE_MODE 1

int func_mod;

Servo servo_in;
Servo servo_out;
#define BARRIER_DOWN 180
#define BARRIER_UP 0

void init_pins(){
  pinMode(D0,  INPUT);//mode button
  pinMode(D1, OUTPUT);//servo in
  pinMode(D2, OUTPUT);//servo out
  pinMode(D3, OUTPUT);//RFID in CS
    digitalWrite(D3,HIGH);
  pinMode(D4, OUTPUT);//RFID out CS
    digitalWrite(D4,HIGH);
  pinMode(D5, OUTPUT);//SPI CLK
  pinMode(D6,  INPUT);//SPI MISO
  pinMode(D7, OUTPUT);//SPI MOSI
  pinMode(D8, OUTPUT);//matrix_led_ir CS
    digitalWrite(D8,HIGH);
  
  SPI.begin();
}

uint8_t dsp_assign_a, dsp_assign_b;

bool accept(char *license){
  if(license[0]=='T' && license[1]=='M')
    return true;
  else
    return false;
}

void show_assign(char a, char b){
  dsp_assign_a = a;
  dsp_assign_b = b;
}

uint8_t free_spaces;

void setup() {

  delay(100);
  
  init_pins();
  init_matrixes();
  show_assign('Z','Z');//bank matrix
  license_read_init();

  servo_in.attach(D1, 1010, 2010);
  servo_out.attach(D2, 930, 1970);

  servo_in.write(BARRIER_UP);
  servo_out.write(BARRIER_UP);

  if(read_ir_sensor(IR_IN_AFTER) == FREE){
    servo_in.write(BARRIER_DOWN);
  }
  if(read_ir_sensor(IR_OUT_AFTER) == FREE){
    servo_out.write(BARRIER_DOWN);
  }
  
  if(digitalRead(D0) == HIGH){
    func_mod = ONLINE_MODE;
  }else{
    func_mod = OFFLINE_MODE;
  }

  //read inital

  if(func_mod = OFFLINE_MODE){
    free_spaces = 16;
  }else{//ONLINE_MODE
    
  }
  Serial.begin(9600);
  Serial.println();
}

enum IN_STATES{IN_IDLE, IN_READ, IN_DECLINE, IN_ACCEPT, IN_UNDER, IN_LEAVING};
enum IN_STATES fsm_in_state = IN_IDLE;

void loop() {
  init_matrixes();
  if(func_mod == OFFLINE_MODE){
    if(free_spaces<10)
      show_free_matrix(free_spaces+'0',0);
    else
      show_free_matrix(free_spaces/10+'0',free_spaces%10+'0');
  show_assign_matrix(dsp_assign_a, dsp_assign_b);
    //////////////

    char license[10];
    switch(fsm_in_state){
      case IN_IDLE: if(read_ir_sensor(IR_IN_BEFORE) == BLOCKED) fsm_in_state = IN_READ;
        break;
      case IN_READ: 
        if(read_license_in(license) == true){
          if(accept(license)){
            servo_in.write(BARRIER_UP);
            static char zona = 'A';
            zona++;
            show_assign(zona, 0);
            fsm_in_state = IN_ACCEPT;
          }else{
            fsm_in_state = IN_DECLINE;
          }
        }
        break;
      case IN_DECLINE: if(read_ir_sensor(IR_IN_BEFORE) == FREE) fsm_in_state = IN_IDLE;
        break;
      case IN_ACCEPT: if(read_ir_sensor(IR_IN_AFTER) == BLOCKED) fsm_in_state = IN_UNDER;
        break;
      case IN_UNDER: if(read_ir_sensor(IR_IN_BEFORE) == FREE) fsm_in_state = IN_LEAVING;
        break;
      case IN_LEAVING: 
        if(read_ir_sensor(IR_IN_AFTER) == FREE){
          servo_in.write(BARRIER_DOWN);
          fsm_in_state = IN_IDLE;
        }
        break;
    }
    
  }else{//ONLINE_MODE
    
  }

  
  delay(10);
}
