#include <SPI.h>

void setup() {
  pinMode(15, OUTPUT); // set the SS pin as an output
  pinMode(13, OUTPUT); // set the SS pin as an output
  pinMode(14, OUTPUT); // set the SS pin as an output
  SPI.begin();         // initialize the SPI library
  SPI.beginTransaction(SPISettings(10000, MSBFIRST, SPI_MODE0));


  
  sendSPICMD(0x0C, 0xFF);
  sendSPICMD(0x09, 0x00);
  sendSPICMD(0x0A, 0xFF);
  sendSPICMD(0x0B, 0xFF);
  sendSPICMD(0x0F, 0x00);
}

void sendSPICMD(uint8_t adr, uint8_t dat){
  digitalWrite(15, LOW);            // set the SS pin to LOW
  
  SPI.transfer(adr);
  SPI.transfer(dat);

  SPI.transfer(adr);
  SPI.transfer(dat);
  
  digitalWrite(15, HIGH);           // set the SS pin HIGH
}

void sendSPI(uint8_t adr, uint8_t dat[]){
  digitalWrite(15, LOW);            // set the SS pin to LOW
  
  SPI.transfer(adr);
  SPI.transfer(dat[0]);
  SPI.transfer(adr);
  SPI.transfer(dat[1]);
  
  digitalWrite(15, HIGH);           // set the SS pin HIGH
}

uint8_t lines[][2] = {0b00000000, 0b00000000,
                      0b00011100, 0b01111100,
                      0b00101100, 0b01100110,
                      0b01001100, 0b01100011,
                      0b10001100, 0b01100011,
                      0b11111110, 0b01100011,
                      0b00001100, 0b01100110,
                      0b00001100, 0b01111100,
                      };

uint8_t row=0;

void loop() {
  
  
  for(uint8_t  i=0; i<8; i++){
    sendSPI(i+1, lines[(i+row)%8]); 
  }

  row++;

  delay(250);
}
