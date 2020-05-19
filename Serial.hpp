/*
 * Serial.hpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *      Company: The University of Southern California
 *
 *  Description:
 *      This module provides the abstract base class for all serial state machines
 *      it requires a USCI instance to interact with the hardware
 *
 *  Sources:
 *      I used the MSP430 G2X13 Datasheet (https://www.ti.com/lit/ds/symlink/msp430g2553.pdf)
 *      and the MSP430x2xx Family Users's guide (https://www.ti.com/lit/ug/slau144j/slau144j.pdf)
 *      to implement the driver. The code works as is with any MSP430 that uses USCI for SPI communication.
 *
 * How to use:
 *  Serial provides a base class for all serial communication types
 *  Since hardware is abstracted by the USCI there should be little to no difference between protocols
 *      should only be initilzation and duplexity
 *  Serial implements a state machine for serial comuication
 *  Derived classes need to implement interrupt callback, init function and async functions
 *  Sync functions call async and wait for buffers to empty
 *
 *  To Do:
 *      Add Timeout Functionality
 *          Probably write a sys_clock module that tracks milliseconds
 */

#ifndef SERIAL_HPP_
#define SERIAL_HPP_


#define SERIAL_BUFF_SIZE 64
#include "USCI.hpp"

enum class SerialState {IDLE, TRANSMIT, RECEIVE, DUPLEX};

/* Serial implementation
*/
class Serial{

public:
    Serial(USCI& _USCI_inst):USCI_inst(_USCI_inst) {};
    virtual sysStatus init()=0;
    virtual sysStatus writeAsync(uint8_t bytes[], uint16_t size)=0;
    virtual sysStatus readAsync(uint16_t numBytes)=0;

    virtual sysStatus write(uint8_t val[], uint16_t size);
    virtual sysStatus read(uint16_t numBytes);
    virtual SerialState getStatus();
    virtual uint8_t getNumBytesRx();
    virtual sysStatus getRxBuff(uint8_t buffer[], uint16_t size);
    virtual void flushRxBuff();

protected:
    USCI& USCI_inst;
    volatile SerialState status=SerialState::IDLE;     // The status of the enum

    volatile uint8_t  rxBuff[SERIAL_BUFF_SIZE];   // The Rx buffer
    volatile uint8_t  txBuff[SERIAL_BUFF_SIZE];   // The Tx buffer
    volatile uint8_t  rxAvail=0;                  // The number of bytes currently in the Rx buffer
    volatile uint8_t  bytesToRead=0;              // The number of bytes waiting to be read
    volatile uint16_t txAvail=0;                  // The number of bytes currently in the Tx buffer
    volatile uint16_t txPos=0;                    // The index of the next byte to send in the the Tx buffer
};



#endif /* SERIAL_HPP_ */
