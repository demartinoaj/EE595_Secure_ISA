/*
 * SPI.hpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 *
 *	Description:
 *	    Universal Serial Communication Interface(USCI)
 *
 *	Sources:
 *	    I used the MSP430 G2X13 Datasheet (https://www.ti.com/lit/ds/symlink/msp430g2553.pdf)
 *	    and the MSP430x2xx Family Users's guide (https://www.ti.com/lit/ug/slau144j/slau144j.pdf)
 *	    to implement the driver. The code works as is with any MSP430 that uses USCI for SPI communication.
 *
 * Relevant Pins:
 *      SPI_1: 1.1 MISO, 1.2 MOSI, 1.3 SCLK
 *      SPI_2: 1.1 MISO, 1.2 MOSI, 1.3 SCLK* UPDATE
 *
 *  To Do:
 *      Implement Full-duplex Communication
 *              Probably need to modify the interrupts and the spiState enum
 *      Test SPI_2
 *          Its pretty identical to SPI_1 so it *should* be fine
 *      Fix getRxBuff
 *          It works, but is very unsafe
 *      Add Timeout Functionality
 *          Probably write a sys_clock module that tracks milliseconds
 */

#ifndef SPI_HPP_
#define SPI_HPP_

#define SPI_BUFF_SIZE 80
#include "USCI.hpp"

enum class spiState {IDLE, TRANSMIT, RECEIVE, DUPLEX};

/* SPI implementation
*/

class SPI{
    friend void SPI_RxCallbackISR(void* SPI_obj,USCI& USCI_inst);
    template <typename Type>
    friend sysStatus sys_cpBuff(Type inBuff[], Type outBuff[], uint16_t size, uint16_t offset);

public:
    SPI(USCI& _USCI_inst):USCI_inst(_USCI_inst){};
    virtual sysStatus init();
    virtual sysStatus ChangeSettings();
    virtual sysStatus write(uint8_t val[], uint16_t size);
    virtual sysStatus read(uint16_t numBytes);
    virtual sysStatus writeAsync(uint8_t bytes[], uint16_t size);
    virtual sysStatus readAsync(uint16_t numBytes);

    virtual spiState getStatus();

    virtual uint8_t getNumBytesRx();
    virtual sysStatus getRxBuff(uint8_t buffer[], uint16_t size);
protected:
    USCI& USCI_inst;
    volatile spiState status=spiState::IDLE;     // The status of the enum

};



#endif /* SPI_HPP_ */
