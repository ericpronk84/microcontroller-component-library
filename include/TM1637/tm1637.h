#ifndef _TM1637_H
#define _TM1637_H

#include "pico/stdlib.h"

// Function to initialize GPIOs
void tm1637_gpio_init();

// Function to set display digits (takes an array of 4 digits)
void tm1637_display_digits(uint8_t digits[4], int num_digits);

// Function to set brightness (0-7)
void tm1637_set_brightness(uint8_t brightness);

//Reset display
void tm1637_clear_display();

#endif
