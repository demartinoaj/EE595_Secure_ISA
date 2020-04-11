/*
 * USCI.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */
#include "USCI.hpp"

USCI_B0 USCIB0;
USCI_A0 USCIA0;

uint8_t USCI_B0::lock=0;
uint8_t USCI_A0::lock=0;


sysStatus USCI_A0::init(){
    if(lock)
        return ERROR;
    else
        lock++;
return SUCCESS;
}

sysStatus USCI_B0::init(){
    if(lock)
        return ERROR;
    else
        lock++;
return SUCCESS;
}

void USCI::registerRxCallback(void (*_rxPtr)(void)){
    rxPtr=_rxPtr;
}

void USCI::registerTxCallback(void (*_txPtr)(void)){
    txPtr=_txPtr;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void){
    if (IFG2 & UCA0RXIFG){
        USCIA0.rxPtr();
    }else if(IFG2 & UCA0RXIFG){
        USCIB0.rxPtr();
    }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIA0TX_ISR(void){
    if (IFG2 & UCA0TXIFG){
        USCIA0.txPtr();
    }else if(IFG2 & UCA0TXIFG){
        USCIB0.txPtr();
    }
}
