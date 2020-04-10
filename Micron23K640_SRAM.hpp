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
    Micron23k640_SRAM(SPI* spiBus, PIN chipSel);
    sysStatus init();
    sysStatus getStatusReg(uint8_t* returnData);
    sysStatus changeMode();

    sysStatus writeByte(uint16_t address, uint8_t data);
    //sysStatus writePage();
    sysStatus writeSequential(uint16_t address, uint8_t data[], uint16_t size);

    sysStatus readByte(uint16_t address, uint8_t* returnData);
   // sysStatus readPage();
    sysStatus readSequential(uint16_t address, uint8_t* returnData, uint16_t size);
private:
    SPI* m_spiBus;
    MicronSram_Mode m_mode=MicronSram_Mode::BYTE;
    PIN m_chipSel;


};




#endif /* MICRON23K640_SRAM_HPP_ */
