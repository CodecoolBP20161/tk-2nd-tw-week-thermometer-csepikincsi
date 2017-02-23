#include "mbed.h"
#include "codecool/codecool_shield_names.h"
#include "codecool/codecool_i2c.h"
#include "codecool/codecool_lcd.h"
#include "codecool/codecool_serial.h"
#include "codecool/codecool_joystick.h"
#include "codecool/codecool_pot.h"

//round for the +/- temperature, and for the +/-50% temperature limits.
#define round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))
#define round2(x) ((x)>=0?(float)((x)):(float)((x)))
//for converting the potmeter to the required interval
#define temp_modify(x) (x-0.5)*20

#define LM75_ADDRESS 0x90

//Sending Data to another device through URAT, and printing the sent Data to the LCD screen.
void sendDataWith2Byte(uint8_t *buffer) {

    SERIAL_SEND(buffer, 16);
	LCD_CLS();
	LCD_LOCATE(20, 10);
	LCD_PRINTF("--SENDING DATA--");
	LCD_LOCATE(43, 0);
	LCD_PRINTF(" %0.1f C", ((int8_t) buffer[0]) + 0.5f * ((buffer[1]&0x80) >> 7));
	wait(2);
}


/*Sending data in a 4byte char array.
 * parameter : a pointer to the buffer array from the tempmeter
 * */
void sendDataWith4Byte(uint8_t *buffer) {
	uint8_t temperatureBits[32];
	//float converted from the buffer
	float temperature = ((int8_t) buffer[0]) + 0.5f * ((buffer[1]&0x80) >> 7);
	//copy float to the 32bit size data
	memcpy(temperatureBits, &temperature, sizeof temperature);

	LCD_CLS();
	LCD_LOCATE(0, 0);
	LCD_PRINTF("buffer : %.1f", temperature);
	wait(2);


	// sending data
	SERIAL_SEND(temperatureBits, 32);
	LCD_CLS();
	LCD_LOCATE(20, 10);
	LCD_PRINTF("--SENDING DATA--");
	LCD_LOCATE(43, 0);
	LCD_PRINTF(" %0.1f C", ((int8_t) buffer[0]) + 0.5f * ((buffer[1]&0x80) >> 7));
	wait(2);

	//TODO put this in Miki-s code to recive Data.
	/*
	uint8_t rec[32];
	// read message and copy to the float g
	memset(rec, 0x00, sizeof(rec));
	SERIAL_RECV(rec, 32);
	float g = 0.0f;
	memcpy(&g, rec, sizeof g);

	LCD_CLS();
	LCD_LOCATE(0, 0);
	LCD_PRINTF("Recived: %.1f", g);

	wait(3);*/

}

//This function is modifying the temperature limits by +/-50%, this should be in the rec side, but **** happens.
void potmodify() {
    float pot = temp_modify(POT1_READ)*5/100;

	int high =(34 + ((34-22)*round2(pot))+0.05);
	int mid =(26 + ((26-22)*round2(pot))+0.05);
	int normal =(18 - ((22-18)*round2(pot))+0.05);
	int low =(10 - ((22-10)*round2(pot))+0.05);

	LCD_CLS();
	LCD_LOCATE(0, 10);
	LCD_PRINTF("pot : %0.2f r: %0.2f", pot, round2(pot));
	LCD_LOCATE(0, 20);
	LCD_PRINTF("l:%i, n:%i, m:%i,h :%i,", low, normal, mid, high);


}

int main() {
	SERIAL_BAUD(9600);
	SERIAL_SET_NON_BLOCKING();

    // set frequency to 100kHz
    I2C_FREQ(100000);

    // clear LCD and display a welcome message.
    LCD_CLS();
    LCD_LOCATE(0, 10);
    LCD_PRINTF("mbed application started.");
    wait(2);

    uint8_t buffer[16];

    while (true) {
        // first, write target address to I2C
        buffer[0] = 0x00;
        I2C_WRITE(LM75_ADDRESS, buffer, 1);

        // then erase previous data from buffer
        memset(buffer, 0x00, sizeof (buffer));

        // receiving 2 bytes from sensor
        I2C_READ(LM75_ADDRESS, buffer, 2);

        float temp = (int8_t) buffer[0] + 0.5f * ((buffer[1]&0x80) >> 7);
        buffer[0] += round(temp_modify(POT1_READ));
		// if JOYSTICK is pushed, it calls the sendData method to send the current temp.
        while(JOYSTICK_PUSHED) {
        	sendDataWith2Byte(buffer);
        }
        while(JOYSTICK_LEFT) {
        	sendDataWith4Byte(buffer);
        }
		LCD_CLS();
		LCD_LOCATE(0, 0);
		LCD_PRINTF("Temperature: %0.1f", temp);
		LCD_LOCATE(0, 10);
		LCD_PRINTF("Modify : %0.1f", (temp + round(temp_modify(POT1_READ))));
		wait(0.2);
    }
}
