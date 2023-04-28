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

void set_all_leds(uint16_t ledStates);
void set_all_leds(uint8_t *ledStates);

uint8_t read_ir_sensor(uint8_t sensorNumber);

#endif
