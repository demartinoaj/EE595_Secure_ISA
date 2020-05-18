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
    while (txAvail!=0 || status!=SerialState::IDLE || USCI_inst.isBusy()==TRUE);              // wait on Tx flush
    return SUCCESS;
}


sysStatus Serial::read(uint16_t numBytes){
    if(readAsync(numBytes)==ERROR)
        return ERROR;
    while (rxAvail<numBytes||status!=SerialState::IDLE ||USCI_inst.isBusy()==TRUE);              // wait on Rx finish
    numBytes=0;
    return SUCCESS;
}

sysStatus Serial::getRxBuff(uint8_t buffer[], uint16_t size){
    __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)
     if(size>rxAvail)
         size=rxAvail;
     else
         rxAvail-=size;

     sysStatus error=sys_cpBuff((uint8_t*) rxBuff, buffer, size, (uint16_t) 0);
     __enable_interrupt();

     return error;
}

void Serial::flushRxBuff(){
    rxAvail=0;
}
uint8_t Serial::getNumBytesRx(){
     return rxAvail;
}

SerialState Serial::getStatus(){
    return status;
}



