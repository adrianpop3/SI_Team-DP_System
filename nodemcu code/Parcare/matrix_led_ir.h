#ifndef __MATRIX_LED_IR_INCLUDED
#define __MATRIX_LED_IR_INCLUDED

#include <stdint.h>
#include <SPI.h>

#define SPI_CLOCK_MATRIX (100000u)
#define MATRIX_LED_IR_CS D8

//#define DEBUG_MATR_IN_INIT

void init_matrixes();

//display two charcters on one of the matrix displays
//if b is 0 then a will be displayed in the middle
void show_free_matrix(char a, char b);
void show_assign_matrix(char a, char b);
void wifi_flash();
void mqtt_flash();


#define RED LOW
#define GREEN HIGH

void set_led(uint8_t ledNum, uint8_t state);

#define ANALOG_TRESHOLD 512

#define IR_IN_BEFORE  19
#define IR_IN_AFTER   18
#define IR_OUT_BEFORE 16
#define IR_OUT_AFTER  17

#define FREE HIGH
#define BLOCKED LOW

uint8_t read_ir_sensor(uint8_t sensorNumber);

#endif
