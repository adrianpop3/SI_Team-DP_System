#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DES.h>

#define OFFLINE_MODE 0
#define ONLINE_MODE 1

int func_mod;

Servo servo_intrare;
Servo servo_iesire;
#define BARIERA_JOS 0
#define BARIERA_SUS 90

void setup() {

  servo_intrare.attach(5);
  servo_iesire.attach(4);

  servo_intrare.write(BARIERA_SUS);
  servo_iesire.write(BARIERA_SUS);
  
  if(digitalRead(16) == HIGH){
    func_mod = ONLINE_MODE;
  }else{
    func_mod = OFFLINE_MODE;
  }

  if(func_mod = OFFLINE_MODE){
    
  }else{//ONLINE_MODE
    
  }

}

void loop() {
  if(func_mod = OFFLINE_MODE){
    
  }else{//ONLINE_MODE
    
  }
}
