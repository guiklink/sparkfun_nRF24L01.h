# Sparkfun nRF24L01 (Radio Transmitter)
This library is a prototype and was created to be used with the [NU32](http://hades.mech.northwestern.edu/index.php/NU32) developed at Northwestern University. However, it need a minimum amount of modifications in order to be used with a standard PIC32.

The radio communicates with the chip through a [SPI communication](https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi) (currently using SPI4) and it uses three digital IO ports. The first is for the SPI Chip Select(CS), the second for the CE that is used to control the states of the radio and the last is used to detect available messages to be read in the RX buffer. The ports can be set in **sparkfun_nRF24L01.h**, still it needs to be point out that the NU32 set most of its pins as digital IO by default, so for a standard PIC32 you might have to add code in **radio_setup()** to configure your pins.

The datasheet for the breakout can be found [here](https://www.sparkfun.com/products/691), such as the electrical specifications and hookup guide.
