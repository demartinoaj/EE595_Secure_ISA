/*
 * SPI.hpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 *
 *	Description:
 *	     This driver functions as the state machine for the Serial Peripheral Interface(SPI). It is derived
 *	     from the serial class, which partially implments its function. It requires a reference to a USCI object
 *	     to manage the underlying hardware.
 *
 *	Sources:
 *	    I used the MSP430 G2X13 Datasheet (https://www.ti.com/lit/ds/symlink/msp430g2553.pdf)
 *	    and the MSP430x2xx Family Users's guide (https://www.ti.com/lit/ug/slau144j/slau144j.pdf)
 *	    to implement the driver. The code works as is with any MSP430 that uses USCI for SPI communication.
 *
 * Relevant Pins:
 *      SPI_1: 1.1 MISO, 1.2 MOSI, 1.3 SCLK
 *      SPI_2: 1.6 MISO, 1.7 MOSI, 1.5 SCLK
 *
 *  To Do:
 *      Test Full-duplex Communication
 *              It seems to work but a more thorough test is needed to weed out weird states
 *      Add Timeout Functionality
 *          Probably write a sys_clock module that tracks milliseconds
 */

#ifndef SPI_HPP_
#define SPI_HPP_

#include "Serial.hpp"
/* SPI implementation
*/

class SPI: public Serial{
    friend void SPI_RxCallbackISR(void* SPI_obj,USCI& USCI_inst); // The Rx callback for when a byte is recieved

public:
    SPI(USCI& _USCI_inst):              //Constructor
        Serial(_USCI_inst)
        {};

    virtual sysStatus init();          // Initializes the SPI bus, registers callback
    virtual sysStatus writeAsync(uint8_t bytes[], uint16_t size); // Write a byte to the SPI bus without blocking
    virtual sysStatus readAsync(uint16_t numBytes);               // Request bytes from the SPI bus without blocking

};



#endif /* SPI_HPP_ */
