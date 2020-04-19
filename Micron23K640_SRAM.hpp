/*
 * Micron23K640_SRAM.h
 *
 *  Created on: Apr 2, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */

#ifndef MICRON23K640_SRAM_HPP_
#define MICRON23K640_SRAM_HPP_

#include "system.hpp"
#include "SPI.hpp"

enum class MicronSram_Mode {BYTE, PAGE, SEQUENTIAL};

class Micron23k640_SRAM{
public:
    Micron23k640_SRAM(SPI* _spiBus, PIN _chipSel);      // Constructor
    sysStatus init();                                   // Initalize the SPI bus and chip select
    sysStatus getStatusReg(uint8_t* returnData);        // Return the value of the SRAM's user status register
    sysStatus changeMode(MicronSram_Mode newMode);      // Change the Read/write mode in the user status register

    sysStatus writeByte(uint16_t address, uint8_t data);                        // Write a single byte to an address of SRAM
    //sysStatus writePage();                                                    // Currently unused
    sysStatus writeSequential(uint16_t address, uint8_t data[], uint16_t size); // Write a sequnce of n bytes to the SRAM, from starting address

    sysStatus readByte(uint16_t address, uint8_t* returnData);                  // Read a single byte to an address of SRAM
   // sysStatus readPage();                                                     // Currently unused
    sysStatus readSequential(uint16_t address, uint8_t data[], uint16_t size);  // Read a sequence of n bytes to the SRAM, from starting address
private:
    SPI* m_spiBus;
    MicronSram_Mode m_mode=MicronSram_Mode::BYTE;
    PIN m_chipSel;
};




#endif /* MICRON23K640_SRAM_HPP_ */
