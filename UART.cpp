/*
 * UART.cpp
 *
 *  Created on: May 2, 2020
 *      Author: Andrew DeMartino
 *      Company: The University of Southern California
 */
#include "system.hpp"
#include "UART.hpp"

/* Macros*/

/*Callback*/
void UART_RxCallbackISR(void* UART_obj,USCI& USCI_inst){
    UART* UART_inst = reinterpret_cast<UART*>(UART_obj);

        if(UART_inst->rxAvail>=SERIAL_BUFF_SIZE) //if the buffer is full
            return;
        UART_inst->rxBuff[UART_inst->rxAvail] = USCI_inst.RxByte();
        UART_inst->rxAvail++;

        if(UART_inst->bytesToRead==0){
            UART_inst->status=SerialState::IDLE;
        }else{
            UART_inst->bytesToRead--;
        }
}

void UART_TxCallbackISR(void* UART_obj,USCI& USCI_inst){
    UART* UART_inst = reinterpret_cast<UART*>(UART_obj);

    if(UART_inst->txAvail==0){
        UART_inst->status=SerialState::IDLE;
        UART_inst->txPos=0;
        IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
    }else{
        USCI_inst.TxByte(UART_inst->txBuff[UART_inst->txPos]);
        UART_inst->txAvail--;
        UART_inst->txPos++;
    }


}

/* Member functions*/
sysStatus UART::init(){
    UART_settings set={
      UART_settings::SMCLK,
//      UART_settings::RISING,
//      UART_settings::NORM_LOW,
//      UART_settings::MSB_FIRST
    };
    USCI_inst.registerRxCallback((void*) this, UART_RxCallbackISR);
    USCI_inst.registerTxCallback((void*) this, UART_TxCallbackISR);
    return USCI_inst.initUART(set);
}


sysStatus UART::writeAsync(uint8_t val[], uint16_t size){
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
   IE2 |= UCA0TXIE;                       // Disable USCI_A0 TX interrupt
   __enable_interrupt();
   //UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
   return SUCCESS;
}


sysStatus UART::readAsync(uint16_t numBytes){

    __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)

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





