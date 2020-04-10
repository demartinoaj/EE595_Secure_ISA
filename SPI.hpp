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

#define SPI_BUFF_SIZE 32

enum class spiState {IDLE, TRANSMIT, RECEIVE, DUPLEX};

/*  Abstract base class for all SPI instances*/
class SPI{
public:
    virtual sysStatus init()=0;                                        // Initialize one of the USCI in SPI mode
    virtual sysStatus ChangeSettings()=0;                              // A user-defined function called in init(). Used to override default SPI settings
    virtual sysStatus write(uint8_t bytes[], uint16_t size)=0;         // Write size bytes to the SPI bus. Blocking Note: size probably overkill
    virtual sysStatus read(uint16_t numBytes)=0;                       // Read size bytes from the SPI bus. Blocking
    virtual sysStatus writeAsync(uint8_t bytes[], uint16_t size)=0;    // Write size bytes to the SPI bus. Non-Blocking
    virtual sysStatus readAsync(uint16_t numBytes)=0;                  // Read size bytes from the SPI bus. Non-Blocking. Use GetRxData() to read
    virtual uint8_t getNumBytesRx()=0;                                 // Get the current number of bytes received
    virtual spiState getStatus()=0;                                    // Return the current SPI state
    virtual sysStatus getRxBuff(uint8_t returnBuff[], uint16_t size)=0;// Populates returnBuffer with size bytes from the Rx buffer, clears size bytes Rx buffer
};

/* SPI implementation for USCI A0
 *      Derive from this class and implement ChangeSettings() to change the default settings
*/

class SPI_1:public SPI{
    friend void USCIA0RX_ISR(void);
    template <typename Type>
    friend sysStatus sys_cpBuff(Type inBuff[], Type outBuff[], uint16_t size, uint16_t offset);

public:
    sysStatus init();
    virtual sysStatus ChangeSettings();
    sysStatus write(uint8_t val[], uint16_t size);
    sysStatus read(uint16_t numBytes);
    sysStatus writeAsync(uint8_t bytes[], uint16_t size);
    sysStatus readAsync(uint16_t numBytes);

    spiState getStatus();

    uint8_t getNumBytesRx();
    sysStatus getRxBuff(uint8_t buffer[], uint16_t size);
//protected:
//    spiState status=spiState::IDLE;     // The status of the enum

};



#endif /* SPI_HPP_ */
