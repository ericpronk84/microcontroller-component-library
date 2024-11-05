#include "pico/stdlib.h"

// Define the GPIO pins
#define CLK_PIN 4  // Use GPIO 4 for the clock line
#define DIO_PIN 5  // Use GPIO 5 for the data line

// Hex values for digits 0-9 and some letters on the 7-segment display
static const uint8_t digit_to_segment[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Function to set the GPIOs as outputs
void tm1637_gpio_init() {
    gpio_init(CLK_PIN);
    gpio_init(DIO_PIN);
    gpio_set_dir(CLK_PIN, GPIO_OUT);
    gpio_set_dir(DIO_PIN, GPIO_OUT);
    gpio_put(CLK_PIN, 1);  // Start with CLK high
    gpio_put(DIO_PIN, 1);  // Start with DIO high
}

// Function to generate a delay (about 10us)
void tm1637_delay() {
    sleep_us(10);
}

// Start communication with the TM1637
void tm1637_start() {
    gpio_put(CLK_PIN, 1);
    gpio_put(DIO_PIN, 1);
    tm1637_delay();
    gpio_put(DIO_PIN, 0);
    tm1637_delay();
    gpio_put(CLK_PIN, 0);
}

// Stop communication with the TM1637
void tm1637_stop() {
    gpio_put(CLK_PIN, 0);
    gpio_put(DIO_PIN, 0);
    tm1637_delay();
    gpio_put(CLK_PIN, 1);
    tm1637_delay();
    gpio_put(DIO_PIN, 1);
    tm1637_delay();
}

// Write a byte to the TM1637
void tm1637_write_byte(uint8_t b) {
    for (int i = 0; i < 8; i++) {
        gpio_put(CLK_PIN, 0);
        gpio_put(DIO_PIN, (b >> i) & 1);  // Send each bit LSB first
        tm1637_delay();
        gpio_put(CLK_PIN, 1);
        tm1637_delay();
    }

    // ACK bit (ignore the response)
    gpio_put(CLK_PIN, 0);
    gpio_set_dir(DIO_PIN, GPIO_IN);  // Change to input for ACK
    tm1637_delay();
    gpio_put(CLK_PIN, 1);
    tm1637_delay();
    gpio_set_dir(DIO_PIN, GPIO_OUT);  // Change back to output
}

// Set the display to show digits
void tm1637_display_digits(uint8_t *digits, int num_digits) {
    uint8_t i;
    int leading_zero = 1;  // Flag to track leading zeros

    // Start transmission
    tm1637_start();

    // Send the command to set the display mode to auto-increment mode
    tm1637_write_byte(0x40);  // Auto increment address mode
    tm1637_stop();

    // Start sending data to each digit starting at address 0
    tm1637_start();
    tm1637_write_byte(0xC0);  // Command to set the starting address to 0

    for (i = 0; i < 4; i++) {
        if (i < num_digits) {
            // Display digits without leading zeros unless it's the last digit
            if (digits[i] != 0 || !leading_zero || i == num_digits - 1) {
                leading_zero = 0;  // Stop skipping leading zeros
                tm1637_write_byte(digit_to_segment[digits[i]]);
            } else {
                tm1637_write_byte(0x00);  // Write blank for leading zeros
            }
        } else {
            tm1637_write_byte(0x00);  // Write blank for extra digits
        }
    }

    tm1637_stop();  // Stop transmission
}

// Set the brightness of the display (0-7)
void tm1637_set_brightness(uint8_t brightness) {
    if (brightness > 7) brightness = 7;
    tm1637_start();
    tm1637_write_byte(0x88 | brightness);  // Command: Set display control and brightness
    tm1637_stop();
}

void tm1637_clear_display() {
    // Start transmission
    tm1637_start();

    // Send the command to set the display mode to auto-increment mode
    tm1637_write_byte(0x40);  // Auto increment address mode
    tm1637_stop();

    // Start clearing each of the 4 digits
    tm1637_start();
    tm1637_write_byte(0xC0);  // Command to set the starting address to 0

    for (int i = 0; i < 4; i++) {
        tm1637_write_byte(0x00);  // Clear each segment (blank)
    }

    tm1637_stop();  // Stop transmission
}