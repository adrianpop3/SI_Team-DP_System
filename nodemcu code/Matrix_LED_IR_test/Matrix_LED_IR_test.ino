#include "matrix_led_ir.h"

void setup() {
  // put your setup code here, to run once:
  init_matrixes();
}

uint8_t t=0;
void loop() {
  // put your main code here, to run repeatedly:
  delay(50);
  char str[10];
  sprintf(str,"%02X",t);
  show_assign_matrix(str[0], str[1]);
  sprintf(str,"%02X",0xFF-t);
  show_free_matrix(str[0], str[1]);
  
  t++;
}
