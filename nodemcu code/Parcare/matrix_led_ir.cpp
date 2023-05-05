#include "matrix_led_ir.h"

#define MATRIX_FREE 0
#define MATRIX_ASSIGN 1

//order D-B-C-A
uint32_t ir_led_states = 0;//the value to be sent to the last 4 registers

uint8_t extra_ir = 0;

uint8_t matrix_transfer[8];//cmd_free_L dat_free_L cmd_free_R dat_free_R cmd_assign_L dat_assign_L cmd_assign_R dat_assign_R

void send_all(){
  digitalWrite(MATRIX_LED_IR_CS, LOW);
  delayMicroseconds(10);
  SPI.beginTransaction(SPISettings(SPI_CLOCK_MATRIX, MSBFIRST, SPI_MODE0));
  ///we have MATRIX_ASSIGN(4 bytes) -> MATRIX_FREE(4 bytes) -> IR in_out(1 byte) -> IR+LED(4 bytes)
  SPI.transfer((ir_led_states>>24)&0xFF);
  SPI.transfer((ir_led_states>>16)&0xFF);
  SPI.transfer((ir_led_states>> 8)&0xFF);
  SPI.transfer((ir_led_states>> 0)&0xFF);
  SPI.transfer(extra_ir);
  for(uint8_t i=0; i<8; i++)
    SPI.transfer(matrix_transfer[i]);

  SPI.endTransaction();  
  digitalWrite(MATRIX_LED_IR_CS, HIGH);
  delayMicroseconds(10);
}

void send_all_matrixes(uint8_t cmd, uint8_t data){
  for(uint8_t i=0; i<8; i+=2){
    matrix_transfer[i]=cmd;
    matrix_transfer[i+1]=data;
  }
  send_all();
  for(uint8_t i=0; i<8; i++)
    matrix_transfer[i]=0;//in future send NOP
}

void init_matrixes(){
  SPI.begin();
  pinMode(MATRIX_LED_IR_CS, OUTPUT);
  digitalWrite(MATRIX_LED_IR_CS, HIGH);
  delayMicroseconds(50);

  send_all_matrixes(0x0C, 0xFF);//no shutdown
  send_all_matrixes(0x09, 0x00);//no decode
  send_all_matrixes(0x0A, 0xFF);//max intensity
  send_all_matrixes(0x0B, 0xFF);//scan all
  send_all_matrixes(0x0F, 0x00);//no test

      #ifdef DEBUG_MATR_IN_INIT
        static uint8_t patt = 0x55;
        //0x01 is up
        send_all_matrixes(0x01, patt);patt=~patt;//dummy data
        send_all_matrixes(0x02, patt);patt=~patt;//dummy data
        send_all_matrixes(0x03, patt);patt=~patt;//dummy data
        send_all_matrixes(0x04, patt);patt=~patt;//dummy data
        send_all_matrixes(0x05, patt);patt=~patt;//dummy data
        send_all_matrixes(0x06, patt);patt=~patt;//dummy data
        send_all_matrixes(0x07, 0);patt=~patt;//dummy data
        send_all_matrixes(0x08, patt);patt=~patt;//dummy data
        patt=~patt;
      #endif
  
}

void show_on_matrix(uint8_t matrix, char a, char b);

void show_free_matrix(char a, char b){
  show_on_matrix(MATRIX_FREE, a, b);
}

void show_assign_matrix(char a, char b){
  show_on_matrix(MATRIX_ASSIGN, a, b);
}

//0-9,A-F,BLANK; MSB is down
#include "font.h"

char convert_char_to_index(char c){
  if(c >= '0' && c <= '9')
    return (c-'0');
  if(c >= 'A' && c <= 'F')
    return (c-'A'+10);
  return (char)16;
}

void show_on_matrix(uint8_t matrix, char a, char b){
  uint8_t matr_offset = matrix==MATRIX_FREE?0:4;
  a = convert_char_to_index(a);
  if(b==0){
    for(uint8_t line = 0; line<8; line++){
      matrix_transfer[matr_offset+0] = line+1;
      matrix_transfer[matr_offset+1] = FONT[a][line]>>4;
      matrix_transfer[matr_offset+2] = line+1;
      matrix_transfer[matr_offset+3] = FONT[a][line]<<4;
      send_all();
    }
  }else{
    b = convert_char_to_index(b);
    
    for(uint8_t line = 0; line<8; line++){
      matrix_transfer[matr_offset+0] = line+1;
      matrix_transfer[matr_offset+1] = FONT[a][line];
      matrix_transfer[matr_offset+2] = line+1;
      matrix_transfer[matr_offset+3] = FONT[b][line];
      send_all();
    }
  }

  for(uint8_t i=0; i<8; i++)
    matrix_transfer[i]=0;//in future send NOP
}



//A1 A2 A3 A4 B1 B2 B3 B4 C1 C2 C3 C4 D1 D2 D3 D4
const uint8_t ir_shift_map[]={0, 2, 4, 6, 16, 18, 20, 22, 14, 12, 10, 8, 30, 28, 26, 24};

void set_led(uint8_t ledNum, uint8_t state){
  if(ledNum >= 16) return;
  
  if(state == HIGH){
    ir_led_states |= (1<<(ir_shift_map[ledNum]+1));
  }else{
    ir_led_states &= ~(1<<(ir_shift_map[ledNum]+1));
  }
  send_all();
}

#define IR_MASK 0x55555555

uint8_t read_ir_sensor(uint8_t sensorNumber){
  
  ir_led_states &= ~IR_MASK;
  extra_ir = 0;

  if(sensorNumber < 16)
    ir_led_states |= (1<<ir_shift_map[sensorNumber]);
  else
    extra_ir = 1<<(sensorNumber-16);
  send_all();
  delayMicroseconds(10);
  int adcValue = analogRead(A0);
  
  ir_led_states &= ~IR_MASK;
  extra_ir = 0;
  
  if(adcValue > ANALOG_TRESHOLD)
    return true;
  else
    return false;
}
