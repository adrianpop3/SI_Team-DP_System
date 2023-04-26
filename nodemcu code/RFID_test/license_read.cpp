#include "license_read.h"


MFRC522 mfrc522_in(SS_PIN_IN, RST_PIN);   // Create MFRC522 instance
MFRC522 mfrc522_out(SS_PIN_OUT, RST_PIN);   // Create MFRC522 instance


void license_read_init(){
  SPI.begin();                                                  // Init SPI bus
  mfrc522_in.PCD_Init();                                              // Init MFRC522 card
  mfrc522_out.PCD_Init();                                              // Init MFRC522 card
}

bool read_license(MFRC522 mfrc522, char *license);

bool read_license_in(char *license){
  return read_license(mfrc522_in, license);
}

bool read_license_out(char *license){
  return read_license(mfrc522_out, license);
}

bool read_license(MFRC522 mfrc522, char *license){
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    byte buf[2];
    byte buf_len = 2;
    if(mfrc522.PICC_WakeupA(buf, &buf_len) != MFRC522::STATUS_OK )
      return false;
    //Serial.print("Same card: ");
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  MFRC522::StatusCode status;
  byte byteCount;
  byte buffer[18];
  
  // Try the mpages of the original Ultralight. Ultralight C has more pages.
  // Read pages
  byteCount = sizeof(buffer);
  status = mfrc522.MIFARE_Read(6, buffer, &byteCount);// Read returns data for 4 pages at a time.
  if (status != MFRC522::STATUS_OK) {
    return false;
  }
  // Dump data
  byte len = buffer[1]-1;
  if(4+len>16) len = 12;
  buffer[4+len]='\0';

  strcpy(license, (char*)(buffer+6));
  
  //Serial.println((char*)(buffer+6));
  

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return true;
}
