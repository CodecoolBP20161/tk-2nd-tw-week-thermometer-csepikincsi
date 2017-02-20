#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool/codecool_i2c.h"
#include "codecool/codecool_lcd.h"
#include "codecool/codecool_serial.h"
void sendData(uint8_t *sending);

#define LM75_ADDRESS 0x90

void sendData(uint8_t *sending) {
	float temper;
	int8_t intpart = (int8_t) sending[0];
	temper = intpart + 0.5f * ((sending[1]&0x80) >> 7);

	LCD_CLS();
	LCD_LOCATE(0, 0);
	LCD_PRINTF("temperature: %i", temper);
/*
	while (1) {

		// sending hello there message over UART
		SERIAL_SEND(buffer, strlen((char*)buffer));
		LCD_CLS();
		LCD_LOCATE(0, 0);
		LCD_PRINTF("Sent: %s", buffer);
		wait(2);

		// wait while we receive at least 12 characters
		while(SERIAL_AVAILABLE() < 12 );

		// read message
		memset(buffer, 0x00, sizeof(buffer));
		SERIAL_RECV(buffer, 12);
		LCD_CLS();
		LCD_LOCATE(0, 0);
		LCD_PRINTF("Recived: %s", buffer);

		wait(5);
	}*/

}

int main() {

    // set frequency to 100kHz
    I2C_FREQ(100000);

    // clear LCD and display a welcome message.
    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("mbed application started.");
    wait(2);

    uint8_t buffer[16];
    float temp;

    while (true) {
        // first, write target address to I2C
        buffer[0] = 0x00;
        I2C_WRITE(LM75_ADDRESS, buffer, 1);

        // then erase previous data from buffer
        memset(buffer, 0x00, sizeof (buffer));

        // receiving 2 bytes from sensor
        I2C_READ(LM75_ADDRESS, buffer, 2);

        // D15            D14  D13  D12  D11 D10 D9  D8  D7    D6 D5 D4 D3 D2 D1 D0
        // Sign bit       MSB                            LSB
        // 1 = Negative   64°C 32°C 16°C 8°C 4°C 2°C 1°C 0.5°C X  X  X  X  X  X  X
        // 0 = Positive

        // get integer part of the temperature
        int8_t _int_part = (int8_t) buffer[0];

        // add fraction part to the float number
        temp = _int_part + 0.5f * ((buffer[1]&0x80) >> 7);
        uint8_t *sending;
        sending = buffer;

        float temper;
        int8_t intpart = (int8_t) sending[0];
		temper = intpart + 0.5f * ((sending[1]&0x80) >> 7);

		// debug temperature value
        LCD_CLS();
        LCD_LOCATE(0, 0);
        LCD_PRINTF("temperature this : %0.1f", temp);
        LCD_LOCATE(0, 10);
        LCD_PRINTF("sending this : %0.1f", temper);
        wait(2);
        sendData(sending);
        wait(0.2);
    }
}
