//#define NU32_STANDALONE  // uncomment if program is standalone, not bootloaded
#include "NU32.h"          // config bits, constants, funcs for startup and UART
//#include "LCD.h"
#include "sparkfun_nRF24L01.h"

#define MSG_LEN 20

#define TRANSMITER 0

void tx_routine();
void rx_routine();

char msg[MSG_LEN];
unsigned char to_send[4] = {1, 2, 3, 4};
unsigned char to_receive[4];

int main() {
  
  NU32_Startup();         // cache on, interrupts on, LED/button init, UART init

  DDPCONbits.JTAGEN = 0; // Disable JTAG, make pins 4 and 5 of Port A available.
  TRISA = 0xFFCF;        // Pins 4 and 5 of Port A are LED1 and LED2.  Clear
                         // bits 4/5 to zero, for output.  Others are inputs.
  LATAbits.LATA4 = 0;    // Turn LED1 on and LED2 off.  These pins sink ...
  LATAbits.LATA5 = 1;    // ... current on NU32, so "high" = "off."


  //LCD_Setup();
  
  radio_setup();

  if(TRANSMITER){
    configure_transmitter();
    tx_routine();
  }
  else{
    configure_receiver();
    rx_routine();
  }
  return 0;
}

void tx_routine(){
  int timer;
  //NU32_WriteUART1("Start transmitter: \r\n");
  while(1){
    //NU32_WriteUART1("TX \r\n");
    LATAINV = 0x30;
    transmit_data(to_send, 4);
    for(timer = 0; timer < 100000000; timer++){;}
  }
}

void rx_routine(){
  int irq;
  NU32_WriteUART1("Start receiver: \r\n");
  while(1){
    irq = RX_IRQ;
    sprintf(msg, "IRQ = %d \r\n\n", irq);
    NU32_WriteUART1(msg);
    //NU32_WriteUART1("Message received: \r\n");
    /*if(!RX_IRQ){
      NU32_WriteUART1("Message received: \r\n");
      reset_RX(to_receive);
      sprintf(msg, "%d | %d | %d | %d \r\n\n", to_receive[0], to_receive[1], to_receive[2], to_receive[3]);
      NU32_WriteUART1(msg);
    }*/
  } 
}
