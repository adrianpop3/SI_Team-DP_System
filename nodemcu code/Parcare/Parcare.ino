#include <Servo.h>
#include <SPI.h>
#include <string.h>
#include "license_read.h"
#include "matrix_led_ir.h"
#include "wifi_comm.h"

#define RED_DELAY 1000
#define GREEN_DELAY 5000

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

bool offline_accept(char *license){
  if(license[0]=='T' && license[1]=='M')
    return true;
  else
    return false;
}

void show_assign(char a, char b){
  dsp_assign_a = a;
  dsp_assign_b = b;
}




char spaces_states[16];
uint8_t free_spaces;
uint8_t free_A, free_B, free_C, free_D;

struct car_data_str{
  char license[10];
  union{
    char zone;
    uint8_t space;
  };
}car_data[16];
int nr_car_data_entries = 0;

unsigned long time_since_IR_change[16];
bool prev_IR_state[16];


enum IN_STATES{IN_IDLE, IN_READ, IN_DECLINE, IN_ACCEPT, IN_UNDER, IN_LEAVING};
enum IN_STATES fsm_in_state = IN_IDLE, fsm_out_state = IN_IDLE;


void setup() {

  Serial.begin(9600);
  Serial.println();
  
  delay(100);
  
  init_pins();
  init_matrixes();
  show_assign('Z','Z');//bank matrix
  license_read_init();

  servo_in.attach(D1, 1010, 2010);
  servo_out.attach(D2, 930, 1970);

  servo_in.write(BARRIER_DOWN);
  servo_out.write(BARRIER_DOWN);

  if(read_ir_sensor(IR_IN_AFTER) == BLOCKED){
    fsm_in_state = IN_LEAVING;
    servo_in.write(BARRIER_UP);
  }
  if(read_ir_sensor(IR_OUT_AFTER) == BLOCKED){
    fsm_out_state = IN_LEAVING;
    servo_out.write(BARRIER_UP);
  }
  
  if(digitalRead(D0) == HIGH){
    func_mod = ONLINE_MODE;
    Serial.println("Online mode");
  }else{
    func_mod = OFFLINE_MODE;
    Serial.println("Offline mode");
  }

  //read inital

  if(func_mod == OFFLINE_MODE){
    free_spaces = 16;
    free_A = 4;free_B = 4;free_C = 4;free_D = 4;
    for(uint8_t i=0; i<16; i++){
      time_since_IR_change[i] = 0;
      prev_IR_state[i] = FREE;
      set_led(i, GREEN);
    }
  }else{//ONLINE_MODE
    init_matrixes();
    show_assign_matrix(0,0);
    wifi_init();
    send_reset();
    ack = 0;
    while(ack==0){
      delay(1000);
      send_reset();
    }
    //////////
    free_spaces = 16;
    free_A = 4;free_B = 4;free_C = 4;free_D = 4;
    for(uint8_t i=0; i<16; i++){
      spaces_states[i]='F';
      time_since_IR_change[i] = 0;
      prev_IR_state[i] = FREE;
      set_led(i, GREEN);
    }
  }
}

char offline_assign(){
  uint8_t max_free = 0;
  if(free_A > max_free) max_free = free_A;
  if(free_B > max_free) max_free = free_B;
  if(free_C > max_free) max_free = free_C;
  if(free_D > max_free) max_free = free_D;

  if(max_free == 0) return 'X';

  if(free_A == max_free) {free_A--; return 'A';}
  if(free_B == max_free) {free_B--; return 'B';}
  if(free_C == max_free) {free_C--; return 'C';}
  if(free_D == max_free) {free_D--; return 'D';}
  return 'X';
}

uint8_t online_assign(){
  uint8_t assign;
  for(assign = 0; assign<16; assign++)
    if(spaces_states[assign]=='F')
      break;
  spaces_states[assign] = 'W';
  return assign;
}

unsigned long prevMillis = 0, crMillis = 0, deltaTime = 0;
void loop() {
  crMillis = millis();
  deltaTime = crMillis - prevMillis;
  prevMillis = crMillis;
  
  init_matrixes();
  if(free_spaces<10)
    show_free_matrix(free_spaces+'0',0);
  else
    show_free_matrix(free_spaces/10+'0',free_spaces%10+'0');
  show_assign_matrix(dsp_assign_a, dsp_assign_b);
  if(func_mod == OFFLINE_MODE){
    //////////////

    char license[10];
    switch(fsm_in_state){
      case IN_IDLE: if(read_ir_sensor(IR_IN_BEFORE) == BLOCKED) fsm_in_state = IN_READ;
        break;
      case IN_READ: 
        if(read_license_in(license) == true){
          if(offline_accept(license)){
            char zone = offline_assign();
            if(zone != 'X'){
              servo_in.write(BARRIER_UP);
              strcpy(car_data[nr_car_data_entries].license, license);
              car_data[nr_car_data_entries].zone = zone;
              nr_car_data_entries++;
              show_assign(zone, 0);
              fsm_in_state = IN_ACCEPT;
            }
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

    for(uint8_t i=0; i<16; i++){
      if(read_ir_sensor(i) != prev_IR_state[i]){
        prev_IR_state[i] = read_ir_sensor(i);
        time_since_IR_change[i] = 0;
      }
      if(prev_IR_state[i] == BLOCKED && time_since_IR_change[i] >= RED_DELAY){
        set_led(i, RED);
      }else if(prev_IR_state[i] == FREE && time_since_IR_change[i] >= GREEN_DELAY){
        set_led(i, GREEN);
      }else{
        time_since_IR_change[i] += deltaTime;
      }
      
    }


    switch(fsm_out_state){
      case IN_IDLE: if(read_ir_sensor(IR_OUT_BEFORE) == BLOCKED) fsm_out_state = IN_READ;
        break;
      case IN_READ: 
        if(read_license_out(license) == true){
            servo_out.write(BARRIER_UP);
            fsm_out_state = IN_ACCEPT;

            uint8_t found = 0;
            for(uint8_t i=0; i<nr_car_data_entries; i++){
              if(strcmp(license, car_data[i].license) == 0){
                switch(car_data[i].zone){
                  case 'A': free_A++; break;
                  case 'B': free_B++; break;
                  case 'C': free_C++; break;
                  case 'D': free_D++; break;
                }
                for(; i+1<nr_car_data_entries; i++){
                  car_data[i] = car_data[i+1];
                }
                nr_car_data_entries--;
                found = 1;
                break;
              }
            }
            
        }
        break;
      case IN_ACCEPT: if(read_ir_sensor(IR_OUT_AFTER) == BLOCKED) fsm_out_state = IN_UNDER;
        break;
      case IN_UNDER: if(read_ir_sensor(IR_OUT_BEFORE) == FREE) fsm_out_state = IN_LEAVING;
        break;
      case IN_LEAVING: 
        if(read_ir_sensor(IR_OUT_AFTER) == FREE){
          servo_out.write(BARRIER_DOWN);
          fsm_out_state = IN_IDLE;
        }
        break;
    }

    free_spaces = free_A + free_B + free_C + free_D;
    
  }else{//ONLINE_MODE
    wifi_update();

    char license[10];
    switch(fsm_in_state){
      case IN_IDLE: if(read_ir_sensor(IR_IN_BEFORE) == BLOCKED) fsm_in_state = IN_READ;
        break;
      case IN_READ: 
        if(read_license_in(license) == true){
          static unsigned long timeSinceLastSend = 250;

          //if we already have the access status do not send request
          if(strcmp(access_license,license)==0){
            timeSinceLastSend = 300;
            if(verdict[0]=='Y'){
              if(free_spaces > 0){
                Serial.println("Accepted");
                servo_in.write(BARRIER_UP);
                uint8_t assigned_space = online_assign();
                show_assign('A'+assigned_space/4, '1'+assigned_space%4);
                
                strcpy(car_data[nr_car_data_entries].license, license);
                car_data[nr_car_data_entries].space = assigned_space;
                nr_car_data_entries++;
                
                fsm_in_state = IN_ACCEPT;
              }
            }else if(verdict[0]=='N'){
              fsm_in_state = IN_DECLINE;
            }else{//car has reservation
              Serial.println("Accepted with reservation");
              servo_in.write(BARRIER_UP);
              uint8_t assigned_space = 4*(verdict[0]-'A')+verdict[1]-'1';
              show_assign('A'+assigned_space/4, '1'+assigned_space%4);
              
              strcpy(car_data[nr_car_data_entries].license, license);
              car_data[nr_car_data_entries].space = assigned_space;
              nr_car_data_entries++;
              
              fsm_in_state = IN_ACCEPT;
              
            }
            access_license[0] = '\0';
          }else{//send or resend the request to get access status
            if(timeSinceLastSend > 250){
              send_license(license);
              timeSinceLastSend = 0;
            }else{
              timeSinceLastSend += deltaTime;
            } 
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

  


    switch(fsm_out_state){
      case IN_IDLE: if(read_ir_sensor(IR_OUT_BEFORE) == BLOCKED) fsm_out_state = IN_READ;
        break;
      case IN_READ: 
        if(read_license_out(license) == true){  
          static unsigned long timeSinceLastSend = 250;
          
          if(strcmp(exit_license, license) == 0){
            timeSinceLastSend = 300;
            servo_out.write(BARRIER_UP);
            fsm_out_state = IN_ACCEPT;

            uint8_t found = 0;
            for(uint8_t i=0; i<nr_car_data_entries; i++){
              if(strcmp(license, car_data[i].license) == 0){
                spaces_states[car_data[i].space] = 'F';
                for(; i+1<nr_car_data_entries; i++){
                  car_data[i] = car_data[i+1];
                }
                nr_car_data_entries--;
                found = 1;
                break;
              }
            }
          }else{
            if(timeSinceLastSend > 250){
              send_exit(license);
              timeSinceLastSend = 0;
            }else{
              timeSinceLastSend += deltaTime;
            }
          }
        }
        break;
      case IN_ACCEPT: if(read_ir_sensor(IR_OUT_AFTER) == BLOCKED) fsm_out_state = IN_UNDER;
        break;
      case IN_UNDER: if(read_ir_sensor(IR_OUT_BEFORE) == FREE) fsm_out_state = IN_LEAVING;
        break;
      case IN_LEAVING: 
        if(read_ir_sensor(IR_OUT_AFTER) == FREE){
          servo_out.write(BARRIER_DOWN);
          fsm_out_state = IN_IDLE;
        }
        break;
    }

    free_spaces=0;
    for(uint8_t i=0; i<16; i++){
      if(read_ir_sensor(i) != prev_IR_state[i]){
        prev_IR_state[i] = read_ir_sensor(i);
        time_since_IR_change[i] = 0;
      }
      if(time_since_IR_change[i] <= RED_DELAY)
        time_since_IR_change[i] += deltaTime;
      
      if(spaces_states[i] == 'F'){
        free_spaces++;
        set_led(i, GREEN);
      }else if(spaces_states[i] == 'R'){
        //R - reserved or car present
        set_led(i, RED);
      }else{
        //'W' - wait for car with no reservation to arrive at its assigned space
        if(prev_IR_state[i] == BLOCKED && time_since_IR_change[i] >= RED_DELAY){
          spaces_states[i] = 'R';
        }
      }
    }

    static unsigned long timeSinceLastStatus = 1000;
    if(timeSinceLastStatus > 1000){
      send_states(spaces_states);
      timeSinceLastStatus = 0;
    }else{
      timeSinceLastStatus += deltaTime;
    }

    
  }//end if online

  
  delay(10);
}//end loop
