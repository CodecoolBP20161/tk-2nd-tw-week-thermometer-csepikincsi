#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool/codecool_i2c.h"
#include "codecool/codecool_lcd.h"
#include "codecool/codecool_serial.h"
#include "codecool/codecool_joystick.h"
#include "codecool/codecool_shield_names.h"

#define LM75_ADDRESS 0x90

gpio_t led1;
gpio_t led2;
gpio_t led3;

int receive(uint8_t *pointer) {

	memset(pointer, 0x00, sizeof(pointer));
	SERIAL_RECV(pointer, 16);
	LCD_CLS();
	LCD_LOCATE(0, 0);
	LCD_PRINTF("--- DATA RECEIVED ---");
	wait(2);
	return 0;
}

int main() {
	uint8_t buffer[16];
	float temp;

	gpio_init_out(&led1, LED_RED_SHIELD);
	gpio_init_out(&led2, LED_GREEN_SHIELD);
	gpio_init_out(&led3, LED_BLUE_SHIELD);
	SERIAL_BAUD(9600);
	SERIAL_SET_NON_BLOCKING();
	memset(buffer, 0x00, sizeof(buffer));

    // clear LCD and display a welcome message.
    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("mbed application started.");
    wait(2);

    while (true) {

	   // read message
    	while(JOYSTICK_PUSHED) {
    		receive(buffer);
    	}

		// get integer part of the temperature
		int8_t _int_part = (int8_t) buffer[0];

	   // add fraction part to the float number
		temp = _int_part + 0.5f * ((buffer[1]&0x80) >> 7);

		LCD_CLS();
		LCD_LOCATE(0, 0);
		LCD_PRINTF("Received: %0.1f", temp);
		gpio_write(&led1, 1);
		gpio_write(&led2, 1);
		gpio_write(&led3, 1);

		if(temp < 10.0) {
			gpio_write(&led3, 0);
		}
		else if (temp < 18.0) {
			gpio_write(&led3, 0);
			gpio_write(&led2, 0);
		}
		else if (temp < 26.0) {
			gpio_write(&led2, 0);
		}
		else if (temp < 34.0) {
			gpio_write(&led1, 0);
			gpio_write(&led2, 0);
		}
		else {
			gpio_write(&led1, 0);
		}

		wait(2);
    }
}
