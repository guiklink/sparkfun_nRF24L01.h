/* 
 * File:   i2c_master_int.h
 * Author: klink
 *
 * Created on April 12, 2015, 4:41 PM
 */


#ifndef SPARKFUNNRF24LS01__H__
#define SPARKFUNNRF24LS01__H__

#define CS_CONFIG 		TRISBbits.TRISB8	
#define CS 				LATBbits.LATB8      // use B8 as CS
#define CE_CONFIG 		TRISBbits.TRISB10 
#define CE 				LATBbits.LATB10 	// use B10 as CE
#define RX_IRQ_CONFIG	TRISBbits.TRISB9
#define RX_IRQ			PORTBbits.RB9		// use B9 as IRQ

void radio_setup();
void reset_RX(unsigned char *output);
void configure_receiver();
void configure_transmitter();
void transmit_data(char *data_to_send, int n_bytes);

#endif