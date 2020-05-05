/*
 * SPI.cpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */
#include "system.hpp"
#include "SPI.hpp"

/* Macros*/
static const uint8_t DUMMY_COMMAND=0x00;

/*Callback*/
void SPI_RxCallbackISR(void* SPI_obj,USCI& USCI_inst){
    SPI* spi_inst = reinterpret_cast<SPI*>(SPI_obj);

    if( spi_inst->status==SerialState::RECEIVE || spi_inst->status==SerialState::DUPLEX){ //SPI is in RECIEVE MODE
        if(spi_inst->rxAvail>=SERIAL_BUFF_SIZE) //if the buffer is full
            return;
        spi_inst->rxBuff[spi_inst->rxAvail] = USCI_inst.RxByte();
        spi_inst->rxAvail++;

    }else if(spi_inst->status==SerialState::TRANSMIT){ //SPI is in TRANSMIT MODE
        uint8_t garbage = USCI_inst.RxByte(); //clear Rx interrupt
        (void)garbage;

        if(spi_inst->txAvail==0){
            spi_inst->status=SerialState::IDLE;
            spi_inst->txPos=0;
        }else{
            USCI_inst.TxByte(spi_inst->txBuff[spi_inst->txPos]);
            spi_inst->txAvail--;
            spi_inst->txPos++;
        }
    }

}

/* Member functions*/
sysStatus SPI::init(){
    SPI_settings set={
      SPI_settings::SMCLK,
      SPI_settings::RISING,
      SPI_settings::NORM_LOW,
      SPI_settings::MSB_FIRST
    };
    USCI_inst.registerRxCallback((void*) this, SPI_RxCallbackISR);

    return USCI_inst.initSPI(set);
}


sysStatus SPI::writeAsync(uint8_t val[], uint16_t size){
   __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)
   if(sys_cpBuff(val, (uint8_t*) txBuff, size, (uint16_t) txPos)==ERROR) //Have to cast as complier gets mad with volatile
       return ERROR;
   txAvail+=size;
   status=SerialState::TRANSMIT;
   if (USCI_inst.TxReady()==SUCCESS){ //TX reg is empty

       USCI_inst.TxByte(this->txBuff[this->txPos]);
       this->txAvail--;
       this->txPos++;

   }
   __enable_interrupt();
   //UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
   return SUCCESS;
}


sysStatus SPI::readAsync(uint16_t numBytes){

    __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)

   for(uint16_t i=0; i<numBytes; i++){
       txBuff[i]=DUMMY_COMMAND;
   }
   txAvail+=numBytes;
   status=SerialState::RECEIVE;
   if (USCI_inst.TxReady()==SUCCESS){ //TX reg is empty

       USCI_inst.TxByte(this->txBuff[this->txPos]);
       this->txAvail--;
       this->txPos++;
   }

   __enable_interrupt();
   return SUCCESS;
}



