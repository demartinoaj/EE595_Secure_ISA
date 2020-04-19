/*
 * SPI.cpp
 *
 *  Created on: Mar 26, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */
#include "system.hpp"
#include "SPI.hpp"


//******************************************************************************
// SPI Interrupt ***************************************************************
//******************************************************************************

// buffers and flags
static volatile uint8_t USCI0_rxBuff[SPI_BUFF_SIZE];      // The Rx buffer
static volatile uint8_t USCI0_txBuff[SPI_BUFF_SIZE];      // The Tx buffer
static volatile uint8_t USCI0_rxAvail=0;                  // The number of bytes currently in the Rx buffer
static volatile uint16_t USCI0_txAvail=0;                 // The number of bytes currently in the Tx buffer
static volatile uint8_t USCI0_rxPos=0;                    // The index of the next byte to send in the Rx buffer
static volatile uint16_t USCI0_txPos=0;                   // The index of the next byte to send in the the Tx buffer
//static volatile spiState USCI0_status=spiState::IDLE;


static const uint8_t DUMMY_COMMAND=0x00;

void SPI_RxCallbackISR(void*,USCI&);

sysStatus SPI::init(){
    SPI_settings set={
      SPI_settings::SMCLK,
      SPI_settings::RISING,
      SPI_settings::NORM_LOW,
      SPI_settings::MSB_FIRST
    };
    USCI_inst.initSPI(set);
    USCI_inst.registerRxCallback((void*) this, SPI_RxCallbackISR);
    return ChangeSettings();
}
sysStatus SPI::ChangeSettings(){
    return SUCCESS;
}
sysStatus SPI::writeAsync(uint8_t val[], uint16_t size){
   if(sys_cpBuff(val, (uint8_t*) USCI0_txBuff, size, (uint16_t) USCI0_txPos)==ERROR) //Have to cast as complier gets mad with volatile
       return ERROR;
   USCI0_txAvail+=size;
   status=spiState::TRANSMIT;
   if (USCI_inst.TxReady()==SUCCESS){ //TX reg is empty

       __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)
       USCI_inst.TxByte(USCI0_txBuff[USCI0_txPos]);
       USCI0_txAvail--;
       USCI0_txPos++;
       __enable_interrupt();
   }
   //UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
   return SUCCESS;
}

sysStatus SPI::write(uint8_t val[], uint16_t size){
    if(writeAsync(val, size)==ERROR)
        return ERROR;
    while (USCI0_txAvail!=0 || status!=spiState::IDLE);              // wait on Tx flush
    return SUCCESS;
}

sysStatus SPI::readAsync(uint16_t numBytes){
   for(uint16_t i=0; i<numBytes; i++){
       USCI0_txBuff[i]=DUMMY_COMMAND;
   }
   USCI0_txAvail+=numBytes;
   status=spiState::RECEIVE;
   if (USCI_inst.TxReady()==SUCCESS){ //TX reg is empty

       __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)
       USCI_inst.TxByte(USCI0_txBuff[USCI0_txPos]);
       USCI0_txAvail--;
       USCI0_txPos++;
       __enable_interrupt();
   }
   return SUCCESS;
}

sysStatus SPI::read(uint16_t numBytes){
    if(readAsync(numBytes)==ERROR)
        return ERROR;
    while (USCI0_rxAvail<numBytes);              // wait on Rx finish
    status=spiState::IDLE;
    USCI0_txPos=0;
    return SUCCESS;
}

sysStatus SPI::getRxBuff(uint8_t buffer[], uint16_t size){
     USCI0_rxAvail=0;
     USCI0_rxPos=0;
     return sys_cpBuff((uint8_t*) USCI0_rxBuff, buffer, size, (uint16_t) 0);
}

uint8_t SPI::getNumBytesRx(){
     return USCI0_rxAvail;
}

spiState SPI::getStatus(){
    return status;
}


void SPI_RxCallbackISR(void* SPI_obj,USCI& USCI_inst){
    SPI* spi_inst = reinterpret_cast<SPI*>(SPI_obj);

    if( spi_inst->status==spiState::RECEIVE || spi_inst->status==spiState::DUPLEX){ //SPI is in RECIEVE MODE
        if(USCI0_rxAvail>=SPI_BUFF_SIZE) //if the buffer is full
            return;
        USCI0_rxBuff[USCI0_rxPos] = USCI_inst.RxByte();
        USCI0_rxAvail++;

    }else if(spi_inst->status==spiState::TRANSMIT){ //SPI is in TRANSMIT MODE
        uint8_t garbage = USCI_inst.RxByte(); //clear Rx interrupt
        (void)garbage;

        if(USCI0_txAvail==0){
            spi_inst->status=spiState::IDLE;
            USCI0_txPos=0;
        }else{
            USCI_inst.TxByte(USCI0_txBuff[USCI0_txPos]);
            USCI0_txAvail--;
            USCI0_txPos++;
        }
    }

}

//#pragma vector=USCIAB0TX_VECTOR
//__interrupt void USCIA0TX_ISR(void){
//
//    if (IFG2 & UCA0TXIFG){ //Sanity check, I Flag 2 corresponds to the USCI 0, and it should be a Tx interrupt
//        if(USCI0_txAvail>=SPI_BUFF_SIZE) //Logically impossible, but safety is best
//            return;
//        else if(USCI0_txAvail==0){
//            USCI0_status=spiState::IDLE;
//            IFG2 &= ~UCA0TXIFG;
//            return;
//        }
//        UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1]; //SUUUUUUUUUUPer unsafe, fix later, clears flag
//        USCI0_txAvail--;
//
//    }
//}
