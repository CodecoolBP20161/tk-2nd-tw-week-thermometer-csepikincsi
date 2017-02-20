/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   codecool_uart.h
 * Author: zsoltmazlo
 *
 * Created on January 16, 2017, 4:56 PM
 */

#ifndef CODECOOL_SERIAL_H
#define CODECOOL_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Serial.h"

mbed::Serial conn(PTD3, PTD2); // rx, tx

// storing received data in this buffer for later usage
uint8_t conn_buffer[255];
uint8_t conn_buffer_idx = 0;

void conn_recv_isr() {
	conn_buffer[conn_buffer_idx] = conn._getc();
	conn_buffer_idx++;
}

#define SERIAL_BAUD(baudrate) conn.baud(baudrate);


/**
 * Set non-blocking mode for Serial.
 * 
 * With this setup you can setup your system to receive data in an interrupt 
 * handler and store the received data in an internal buffer. Later this buffer 
 * could be read with calling <b>SERIAL_RECV</b>.
 */
#define SERIAL_SET_NON_BLOCKING() conn_buffer_idx = 0; conn.attach(&conn_recv_isr)

/**
 * Send number of <b>len</b> bytes of data over serial reading them from 
 * <b>buffer</b> from index 0.
 * 
 * <b>NOTE</b>: the <b>buffer</b> needs to have at least the size of <b>len</b>, 
 * otherwise it will cause a Segmentation fault.
 * 
 * @param buffer: the byte buffer which we want to transmit
 * @param len: how many bytes we want to transmit
 * 
 * <b>Example</b>:<br>
 * uint8_t buffer[4] = {0x00, 0x01, 0x02, 0x03};<br> 
 * SERIAL_SEND(buffer, 4);
 */
#define SERIAL_SEND(buffer, len) { uint8_t i=0; do { conn._putc(buffer[i++]); } while(i<len); }

/**
 * Waits for receiving number of <b>len</b> bytes of data over serial and loads
 * them into <b>buffer</b> from index 0.
 * 
 * <b>NOTE</b>: This is a blocking wait, and the microcontroller will not move 
 * along until the given amount of data not have been received.
 * 
 * <b>NOTE</b>: the <b>buffer</b> needs to have at least the size of <b>len</b>, 
 * otherwise it will cause a Segmentation fault.
 * 
 * @param buffer: the byte buffer which could be used to store data
 * @param len: how many bytes we want to receive
 * 
 * <b>Example</b>:<br>
 * uint8_t buffer[12];<br> 
 * SERIAL_RECV_BLOCKING(buffer, 12);
 */
#define SERIAL_RECV_BLOCKING(buffer, len) { uint8_t i=0; do { buffer[i++] = conn._getc(); } while(i< len); }

/**
 * Reads the number of <b>len</b> bytes from internal buffer and loads them into 
 * <b>buffer</b> from index 0.
 * 
 * <b>NOTE</b>: The internal buffer is always filled if there were serial receiving,
 * therefore you should use the <b>SERIAL_AVAILABLE()<b> macro to find out how many
 * bytes arrived yet not read.
 * 
 * <b>NOTE</b>: the <b>buffer</b> needs to have at least the size of <b>len</b>, 
 * otherwise it will cause a Segmentation fault.
 * 
 * @param buffer: the byte buffer which could be used to store data
 * @param len: how many bytes we want to read out
 * 
 * <b>Example</b>:<br>
 * uint8_t buffer[64];<br> 
 * SERIAL_RECV(buffer, SERIAL_AVAILABLE());
 */
#define SERIAL_RECV(buffer, len) { \
	uint8_t i=0, k=conn_buffer_idx-len; \
	do { \
		buffer[i++] = conn_buffer[k++]; \
	} while(i< len); \
	conn_buffer_idx -= len; \
}

/**
 * Reads the number of bytes received in asynchronous mode.
 * 
 * @returns: number of bytes received yet
 */
#define SERIAL_AVAILABLE() conn_buffer_idx-1

#ifdef __cplusplus
}
#endif

#endif /* CODECOOL_UART_H */

