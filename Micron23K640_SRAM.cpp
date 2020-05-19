/*
 * Micron23K640_SRAM.cpp
 *
 *  Created on: Apr 2, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */
#include "Micron23K640_SRAM.hpp"

// Flags
static const uint8_t SRAM_MODE1=0x80;
static const uint8_t SRAM_MODE0=0x40;
static const uint8_t SRAM_HOLD=0x01;
static const uint8_t SRAM_STATUSREG_MAGICNUM=0x02;

//Commands
static const uint8_t SRAM_READ_STATUSREG=0x05;
static const uint8_t SRAM_WRITE=0x02;
static const uint8_t SRAM_READ=0x03;


Micron23k640_SRAM::Micron23k640_SRAM(SPI* spiBus, PIN chipSel): m_spiBus(spiBus), m_chipSel(chipSel){
}

sysStatus Micron23k640_SRAM::init(){

    /*Set Chip select to output, and set it to high*/
    m_chipSel.setOutput();
    m_chipSel.setHigh();

    /*Get the status Reg and confirm Magic Number*/
    uint8_t reg;
    if(getStatusReg(&reg)==ERROR)
       // return ERROR;
    reg&=~(SRAM_MODE1+SRAM_MODE0+SRAM_HOLD);
    /*If magic num is confirmed, communication is established*/
    if(reg ==SRAM_STATUSREG_MAGICNUM)
        return SUCCESS;
    return ERROR;
}

sysStatus Micron23k640_SRAM::getStatusReg(uint8_t* returnData){

    m_chipSel.setLow(); // Enable chip select
    m_spiBus->write((uint8_t*) &SRAM_READ_STATUSREG, 1);
    m_spiBus->read(1);
    m_chipSel.setHigh(); // Disable chip select

    m_spiBus->getRxBuff(returnData, 1);
    return SUCCESS;
}
sysStatus Micron23k640_SRAM::writeByte(uint16_t address, uint8_t data){
    uint8_t MSB=address>>8;
    uint8_t LSB=address;

    m_chipSel.setLow();                         // Enable chip select
    m_spiBus->write((uint8_t*) &SRAM_WRITE, 1);
    m_spiBus->write((uint8_t*) &MSB, 1);
    m_spiBus->write((uint8_t*) &LSB, 1);
    m_spiBus->write((uint8_t*) &data, 1);
    m_chipSel.setHigh();                        // Disable chip select

    return SUCCESS;
}
sysStatus Micron23k640_SRAM::writeSequential(uint16_t address, uint8_t data[], uint16_t size){
 return ERROR; // TODO
}
sysStatus Micron23k640_SRAM::readByte(uint16_t address, uint8_t* returnData){
    uint8_t MSB=address>>8;
    uint8_t LSB=address;

    m_chipSel.setLow();                         // Enable chip select
    m_spiBus->write((uint8_t*) &SRAM_READ, 1);
    m_spiBus->write((uint8_t*) &MSB, 1);
    m_spiBus->write((uint8_t*) &LSB, 1);
    m_spiBus->read(1);
    m_chipSel.setHigh();                        // Disable chip select

    m_spiBus->getRxBuff(returnData, 1);
    return SUCCESS;
}
sysStatus Micron23k640_SRAM::readSequential(uint16_t address, uint8_t* returnData, uint16_t size){
    return ERROR;// TODO
}


