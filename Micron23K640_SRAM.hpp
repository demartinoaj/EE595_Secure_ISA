/*
 * Micron23K640_SRAM.h
 *
 *  Created on: Apr 2, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 *
  * Description:
 *      This module provides a class used to control a Micron 23k640 SRAM module.
 *      Call Init() befor attampting to read or write to the SRAM
 *
 *  Sources:
 *      I used the chip's data-sheet to construct the functions http://ww1.microchip.com/downloads/en/DeviceDoc/22126E.pdf
 *  To Do:
 *      Implement the write page funtionaility.
 *
 */

#ifndef MICRON23K640_SRAM_HPP_
#define MICRON23K640_SRAM_HPP_

#include "system.hpp"
#include "SPI.hpp"

/* the possible read/write modes of the SRAM chip*/
enum class MicronSram_Mode {BYTE, PAGE, SEQUENTIAL};

class Micron23k640_SRAM{
public:
    Micron23k640_SRAM(SPI* _spiBus, PIN _chipSel);                                 // Constructor
    sysStatus init();                                   // Initialize the SPI bus and chip select
    sysStatus hardwareReset();                          // Perform a hardware rest of the SRAM, clearing all cell values
    sysStatus getStatusReg(uint8_t* returnData);        // Return the value of the SRAM's user status register
    sysStatus changeMode(MicronSram_Mode newMode);      // Change the Read/write mode in the user status register

    sysStatus writeByte(uint16_t address, uint8_t data);                        // Write a single byte to an address of SRAM
    //sysStatus writePage();                                                    // Currently unused
    sysStatus writeSequential(uint16_t address, uint8_t data[], uint16_t size); // Write a sequence of n bytes to the SRAM, from starting address

    sysStatus readByte(uint16_t address, uint8_t* returnData);                  // Read a single byte to an address of SRAM
   // sysStatus readPage();                                                     // Currently unused
    sysStatus readSequential(uint16_t address, uint8_t data[], uint16_t size);  // Read a sequence of n bytes to the SRAM, from starting address

private:
    SPI* m_spiBus;                                  // Pointer to the SPI Bus the Chip is on
    MicronSram_Mode m_mode=MicronSram_Mode::BYTE;   // Current R/w mode of the SRAM
    PIN m_chipSel;                                  // The chip select pin
};




#endif /* MICRON23K640_SRAM_HPP_ */
