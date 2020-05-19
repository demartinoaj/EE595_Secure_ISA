/*
 * UART.hpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *      Company: The University of Southern California
 *
 *  Description:
 *       This driver functions as the state machine for the Universal Asynchronous Receive Transmit (UART). It is derived
 *       from the serial class, which partially implements its function. It requires a reference to a USCI object
 *       to manage the underlying hardware.
 *
 *  Sources:
 *      I used the MSP430 G2X13 Datasheet (https://www.ti.com/lit/ds/symlink/msp430g2553.pdf)
 *      and the MSP430x2xx Family Users's guide (https://www.ti.com/lit/ug/slau144j/slau144j.pdf)
 *      to implement the driver. The code works as is with any MSP430 that uses USCI for SPI communication.
 *
  * Relevant Pins:
 *      SPI_1: 1.1 Rx, 1.2 Tx
 *  To Do:
 *      Add Timeout Functionality
 *          Probably write a sys_clock module that tracks milliseconds
 */

#ifndef UART_HPP_
#define UART_HPP_

#define SPI_BUFF_SIZE 16
#include "Serial.hpp"

/* UART implementation*/
class UART: public Serial{
    friend void UART_RxCallbackISR(void* UART_obj,USCI& USCI_inst); // Recieve callback
    friend void UART_TxCallbackISR(void* UART_obj,USCI& USCI_inst); // Transmit callback

public:
    UART(USCI& _USCI_inst):
        Serial(_USCI_inst)
        {};

    virtual sysStatus init();       //Initializes UART bus with default settings
    virtual sysStatus writeAsync(uint8_t bytes[], uint16_t size); // Non-blocking write
    virtual sysStatus readAsync(uint16_t numBytes);               // Nonblocking read

};


#endif /* UART_HPP_ */
