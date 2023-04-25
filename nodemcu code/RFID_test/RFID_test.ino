#include <SPI.h>
#include "license_read.h"

//pins for RFID in license_read.h


uint8_t ir_debounce=0, prev_sens, prev_sure_sens = HIGH, arrived;
long long int prevmillis = 0, crmillis;

//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  Serial.println();
  license_read_init();
  prevmillis = millis();
}


//*****************************************************************************************//
void loop() {

  if(digitalRead(D8) != prev_sens){
    ir_debounce = 0;
  }
  prev_sens = digitalRead(D8);

  crmillis = millis();
  if(ir_debounce<200)
    ir_debounce += crmillis - prevmillis;
  prevmillis = crmillis;

  arrived = 0;
  if(ir_debounce >= 200){
    if(prev_sure_sens == HIGH && prev_sens == LOW)
      arrived = 1;
    prev_sure_sens = prev_sens;
  }

  if(!arrived) return;
  //-------------------------------------------

  char license[8];
  if(!read_license(license)){
    Serial.println("Error");
  }else{
    Serial.println(license);
  }
  
}
//*****************************************************************************************//
