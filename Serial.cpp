/*
 * SPI.cpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *      Company: The University of Southern California
 */
#include "system.hpp"
#include "Serial.hpp"




sysStatus Serial::write(uint8_t val[], uint16_t size){
    if(writeAsync(val, size)==ERROR)
        return ERROR;
    while (txAvail!=0 || status!=SerialState::IDLE);              // wait on Tx flush
    return SUCCESS;
}


sysStatus Serial::read(uint16_t numBytes){
    if(readAsync(numBytes)==ERROR)
        return ERROR;
    while (rxAvail<numBytes);              // wait on Rx finish
    status=SerialState::IDLE;
    txPos=0;
    return SUCCESS;
}

sysStatus Serial::getRxBuff(uint8_t buffer[], uint16_t size){
     rxAvail=0;
     return sys_cpBuff((uint8_t*) rxBuff, buffer, size, (uint16_t) 0);
}

uint8_t Serial::getNumBytesRx(){
     return rxAvail;
}

SerialState Serial::getStatus(){
    return status;
}



