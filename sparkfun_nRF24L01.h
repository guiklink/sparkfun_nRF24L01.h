/* 
 * File:   i2c_master_int.h
 * Author: klink
 *
 * Created on April 12, 2015, 4:41 PM
 */


#ifndef SPARKFUNNRF24LS01__H__
#define SPARKFUNNRF24LS01__H__

#define CS_CONFIG 		TRISBbits.TRISB8
#define CS 				LATBbits.LATB8        // use RB8 as CS
#define TX_CE_CONFIG 	TRISBbits.TRISB10 
#define TX_CE 			LATBbits.LATB10 

#endif