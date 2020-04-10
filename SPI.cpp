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
static volatile spiState USCI0_status=spiState::IDLE;

static const uint8_t DUMMY_COMMAND=0x00;

sysStatus SPI_1::init(){

    UCA0CTL0 |=UCSYNC;                      // Set USCI0 to SPI mode
    UCA0CTL1 |= UCSSEL_2;                   //  select SMCLK as the clock source

    /*SPI Flags*/

    UCA0CTL0 |=UCMST;                       // Run SPI in Master mode
    UCA0CTL0 |=UCCKPH;                      // Set the clock phase to sample on rising edge
    UCA0CTL0 &= ~UCCKPL;                    // Set the clock polarity to normally low
    UCA0CTL0 |=UCMSB;                       // Shift data out MSB first

    //From datasheet: " UCMODE0 and UCMODE1 should NOT be set for 3 pin SPI mode"
    //UCA0CTL0 &= ~UCMODE0;
    //UCA0CTL0 &= UCMODE1;

    UCA0MCTL = 0;                             // Not used in SPI (modulation)
    UCA0CTL1 &= ~UCSWRST;                     // Exit Reset mode, enabling the module
    IE2 |= UCA0RXIE;                          // Enable USCI0 RX interrupt
    //IE2 |= UCA0TXIE;                          // Enable USCI0 TX interrupt
    //IFG2 &= ~UCA0TXIFG;

    UCA0BR0 |= 0x10;                          // Run the clock as fast as possible(prescalar registers to 0)
    UCA0BR1 |= 0X00;

    /*SPI Pins*/
    // Set Pins 1.1 (MISO), 1.2(MOSI) and 1.4(SCLK) to USCIO mode
      P1SEL = BIT1 + BIT2 + BIT4;
      P1SEL2 = BIT1 + BIT2 + BIT4;
    //Note: Slave select Pin control is punted to user driver, in case of shared bus
    return ChangeSettings();
}
sysStatus SPI_1::ChangeSettings(){
    return SUCCESS;
}
sysStatus SPI_1::writeAsync(uint8_t val[], uint16_t size){
   if(sys_cpBuff(val, (uint8_t*) USCI0_txBuff, size, (uint16_t) USCI0_txAvail)==ERROR) //Have to cast as complier gets mad with volatile
       return ERROR;
   USCI0_txAvail+=size;
   USCI0_status=spiState::TRANSMIT;
   if (IFG2 & UCA0TXIFG){ //TX reg is empty

       __disable_interrupt();// Critical Section Need to avoid race condition (note: THIS OCCURS OTHERWISE!)
       UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
       USCI0_txAvail--;
       __enable_interrupt();
   }
   //UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
   return SUCCESS;
}

sysStatus SPI_1::write(uint8_t val[], uint16_t size){
    if(writeAsync(val, size)==ERROR)
        return ERROR;
    while (USCI0_txAvail!=0 || USCI0_status!=spiState::IDLE);              // wait on Tx flush
    return SUCCESS;
}

sysStatus SPI_1::readAsync(uint16_t numBytes){
   for(uint16_t i=0; i<numBytes; i++){
       USCI0_txBuff[i]=DUMMY_COMMAND;
   }
   USCI0_txAvail+=numBytes;
   USCI0_status=spiState::RECEIVE;
   if (IFG2 & UCA0TXIFG){ //TX reg is empty

       __disable_interrupt();// Critical Section
       UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
       USCI0_txAvail--;
       __enable_interrupt();
   }
   return SUCCESS;
}

sysStatus SPI_1::read(uint16_t numBytes){
    if(readAsync(numBytes)==ERROR)
        return ERROR;
    while (USCI0_rxAvail<numBytes);              // wait on Rx finish
    USCI0_status=spiState::IDLE;
    return SUCCESS;
}

sysStatus SPI_1::getRxBuff(uint8_t buffer[], uint16_t size){
     USCI0_rxAvail=0;
     return sys_cpBuff((uint8_t*) USCI0_rxBuff, buffer, size, (uint16_t) 0);
}

uint8_t SPI_1::getNumBytesRx(){
     return USCI0_rxAvail;
}

spiState SPI_1::getStatus(){
    return USCI0_status;
}

//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCIA0RX_ISR(void){
//    if (IFG2 & UCA0RXIFG){ //Sanity check, I Flag 2 corresponds to the USCI 0, and it should be a Rx interrupt
//
//        if( USCI0_status==spiState::RECEIVE || USCI0_status==spiState::DUPLEX){ //SPI is in RECIEVE MODE
//            if(USCI0_rxAvail>=SPI_BUFF_SIZE) //if the buffer is full
//                return;
//            USCI0_rxBuff[USCI0_rxAvail] = (uint8_t) UCA0RXBUF; //SUUUUUUUUUUPer unsafe, fix later
//            USCI0_rxAvail++;
//
//        }else if(USCI0_status==spiState::TRANSMIT){ //SPI is in TRANSMIT MODE
//            uint8_t garbage = UCA0RXBUF; //clear Rx interrupt
//            (void)garbage;
//
//            if(USCI0_txAvail==0){
//                USCI0_status=spiState::IDLE;
//            }else{
//                UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1];
//                USCI0_txAvail--;
//            }
//        }
//    }
//}
//
////#pragma vector=USCIAB0TX_VECTOR
////__interrupt void USCIA0TX_ISR(void){
////
////    if (IFG2 & UCA0TXIFG){ //Sanity check, I Flag 2 corresponds to the USCI 0, and it should be a Tx interrupt
////        if(USCI0_txAvail>=SPI_BUFF_SIZE) //Logically impossible, but safety is best
////            return;
////        else if(USCI0_txAvail==0){
////            USCI0_status=spiState::IDLE;
////            IFG2 &= ~UCA0TXIFG;
////            return;
////        }
////        UCA0TXBUF = USCI0_txBuff[USCI0_txAvail-1]; //SUUUUUUUUUUPer unsafe, fix later, clears flag
////        USCI0_txAvail--;
////
////    }
////}
