#include "NU32.h"
#include "sparkfun_nRF24L01.h"

// interface with the LSM303D accelerometer/magnetometer using spi
// Wire GND to GND, VDD to 3.3V, Vin is disconnected (on Pololu breakout board)
// SDO4 (F5) ->  SDI (labeled SDA on Pololu board),
// SDI4 (F4) -> SDO
// SCK4 (B14) -> SCL
// RB8 -> CS

char msg[20];

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI4BUF = o;
  while(!SPI4STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI4BUF;
}

void retrieve_data_from_bf(int n_bytes, unsigned char *buffer){
  int counter;
  CS = 0;
  SPI4BUF = 0x61;
  while(!SPI4STATbits.SPIRBF) {;} // wait to receive the byte
  SPI4BUF;                        // read whatever is in the buffer

  // Loop to read all the bytes
  for (counter = 0; counter < n_bytes; counter++)
  {
    while(!SPI4STATbits.SPIRBF) {;} // wait to receive the byte
    buffer[counter] = SPI4BUF;
  }
  CS = 1;
}


unsigned char radio_write_register(unsigned char reg, unsigned char data)
{
  unsigned char status; 
  CS = 0;               // bring CS low to activate SPI
  status = spi_io(reg); // send cmd & retrieve radio's status
  spi_io(data);
  CS = 1;               // complete the command
  return status;
}

unsigned char radio_command(unsigned char reg)
{
  unsigned char data;
  CS = 0;               // bring CS low to activate SPI
  data = spi_io(reg);   // send cmd & retrieve data
  CS = 1;               // complete the command
  return data;
}

void transmit_data(char *data_to_send, int n_bytes){
  int counter;

  radio_write_register(0x27, 0x7E);   // Clear all FIFO interrupts and maximum number of retransmits

  radio_write_register(0x20, 0x3A);   // Power up to change to change to STAND BY state

  radio_command(0xE1);                // Clear TX fifo, the data sheet says that this is supposed to come up 0 after POR, but that doesn't seem to be the case

  CS = 0;
  spi_io(0xA0);
  for (counter = 0; counter < n_bytes; counter++)
  {
    spi_io(data_to_send[counter]);
  }
  CS = 1;                             

  CE = 1;                          //Pulse CE to start transmission

  for (counter = 0; counter < 80000; counter++){;}  // Wait for 1 ms (assuming the PIC clock is set to 80MHz)

  CE = 0;
}

void configure_transmitter(){
  CE = 0;                        // CE is reponsible to control state transitions

  // config radio registers
  // Write command is devined by 0b001A AAAA where A is substituted by the register address

  radio_write_register(0x20, 0x38); // PTX, CRC enabled, mask a couple of ints 

  radio_write_register(0X24, 0x00); // auto retransmit off SHOULD IT BE THOU?

  radio_write_register(0x23, 0x03); // address width = 5

  radio_write_register(0x26, 0x07); // data rate = 1MB

  radio_write_register(0x25, 0x02); // set channel 2, this is default

  //radio_write_register(0x30, 0xE7); // set address E7E7E7E7E7

  radio_write_register(0x21, 0x00); //disable auto-ack, RX mode, shouldn't have to do this, but it won't TX if you don't
}

void configure_receiver(){
  CE = 0;

  radio_write_register(0x20, 0x39); //PRX, CRC enabled

  radio_write_register(0x21, 0x00); //disable auto-ack for all channels

  radio_write_register(0x23, 0x03); // address width = 5

  radio_write_register(0x26, 0x07); // data rate = 1MB

  radio_write_register(0x31, 0x04); // 4 byte payload

  radio_write_register(0x25, 0x02); // set channel 2, this is default

  //radio_write_register(0x30, 0xE7); // set address E7E7E7E7E7

  radio_write_register(0x20, 0x3B); // Power up to change to change to STAND BY state

  CE = 1;

  int counter;
  for (counter = 0; counter < 80000; counter++){;}

  unsigned char status = 14;
  status = radio_command(0xFF);
  sprintf(msg,"%d", status);
  NU32_WriteUART1("Status: \r\n");
  NU32_WriteUART1(msg);
}

void reset_RX(unsigned char *output){
  retrieve_data_from_bf(4, output);

  radio_command(0xE2);              // Flush RX FIFO

  radio_write_register(0x27, 0x40); // Reset int

}

void radio_setup() {      // setup the radio, using SPI 4
  AD1PCFGbits.PCFG8 = 1;
  AD1PCFGbits.PCFG9 = 1;
  AD1PCFGbits.PCFG10 = 1;


  CS_CONFIG = 0;
  CS = 1;
  CE_CONFIG = 0;         // Set port as digital output
  RX_IRQ_CONFIG = 1;     // Set port as digital input

  // Master - SPI4, pins are: SDI4(F4), SDO4(F5), SCK4(B14).  
  // we manually control SS4 as a digital output (B8)
  // since the PIC is just starting, we know that spi is off. We rely on defaults here
 
  // setup SPI4
  SPI4CON = 0;              // stop and reset SPI4
  SPI4BUF;                  // read to clear the rx buffer
  SPI4BRG = 0x3;            // bit rate to 10MHz, SPI4BRG = 80000000/(2*desired)-1
  SPI4STATbits.SPIROV = 0;  // clear the overflow
  SPI4CONbits.MSTEN = 1;    // master mode
  SPI4CONbits.MODE16 = 1;   // 16 bit mode
  SPI4CONbits.MODE32 = 0; 
  SPI4CONbits.SMP = 1;      // sample at the end of the clock
  SPI4CONbits.ON = 1;       // turn spi on

}

/*
FROM ENCODER

SPI4CON = 0;              // stop and reset SPI4
  SPI4BUF;                  // read to clear the rx buffer
  SPI4BRG = 0x3;            // bit rate to 10MHz, SPI4BRG = 80000000/(2*desired)-1
  SPI4STATbits.SPIROV = 0;  // clear the overflow
  SPI4CONbits.MSTEN = 1;    // master mode
  SPI4CONbits.MODE16 = 1;   // 16 bit mode
  SPI4CONbits.MODE32 = 0; 
  SPI4CONbits.SMP = 1;      // sample at the end of the clock
  SPI4CONbits.ON = 1;       // turn spi on

*/

/*
FROM SAMPLES

  SPI4CON = 0;              // turn off the SPI module and reset it
  SPI4BUF;                  // clear the rx buffer by reading from it
  SPI4BRG = 0x3;            // baud rate to 10MHz [SPI4BRG = (80000000/(2*desired))-1]
  SPI4STATbits.SPIROV = 0;  // clear the overflow bit
  SPI4CONbits.CKE = 1;      // data changes when clock goes from active to inactive 
                            // (high to low since CKP is 0)
  SPI4CONbits.MSTEN = 1;    // master operation
  SPI4CONbits.ON = 1;       // turn on SPI 4

*/
