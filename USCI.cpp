/*
 * USCI.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */
#include "USCI.hpp"

#ifdef USE_USCI_A0
USCI_A0 USCIA0;
#endif

#ifdef USE_USCI_B0
USCI_B0 USCIB0;
#endif

uint8_t USCI_B0::m_lock=0;
uint8_t USCI_A0::m_lock=0;

sysStatus USCI_A0::initSPI(SPI_settings &settings){
    if(m_lock)
        return ERROR;
    else
        m_lock++;

  UCA0CTL0 |=UCSYNC;                          // Set USCI0 to SPI mode
  if(settings.clockSource== SPI_settings::SMCLK)
      UCA0CTL1 |= UCSSEL_2;                   //  select SMCLK as the clock source
  else
      UCA0CTL1 |= UCSSEL_0;                   //  select ACLK as the clock source

  /*SPI Flags*/
  UCA0CTL0 |=UCMST;                          // Run SPI in Master mode
  if(settings.clockPhase==SPI_settings::RISING)
      UCA0CTL0 |=UCCKPH;                     // Set the clock phase to sample on rising edge
  else
      UCA0CTL0 &=~UCCKPH;

  if(settings.clockPolarity==SPI_settings::NORM_LOW)
      UCA0CTL0 &= ~UCCKPL;                  // Set the clock phase to sample on rising edge
  else
      UCA0CTL0 |= UCCKPL;

  if(settings.endian==SPI_settings::MSB_FIRST)
      UCA0CTL0 |=UCMSB;                     // Shift data out MSB first
  else
      UCA0CTL0 &= ~UCMSB;

  //From datasheet: " UCMODE0 and UCMODE1 should NOT be set for 3 pin SPI mode"
  UCA0CTL0 &= ~UCMODE0;
  UCA0CTL0 &= ~UCMODE1;

  UCA0MCTL = 0;                             // Not used in SPI (modulation)
  UCA0CTL1 &= ~UCSWRST;                     // Exit Reset mode, enabling the module
  IE2 |= UCA0RXIE;                          // Enable USCI0 RX interrupt
  //IE2 |= UCA0TXIE;                          // Enable USCI0 TX interrupt
  //IFG2 &= ~UCA0TXIFG;

  UCA0BR0 = settings.prescalar;                          // Run the clock as fast as possible(prescalar registers to 0)
  UCA0BR1 = settings.prescalar>>8;

  /*SPI Pins*/
  // Set Pins 1.1 (MISO), 1.2(MOSI) and 1.4(SCLK) to USCIO mode
    P1SEL |= BIT1 + BIT2 + BIT4;
    P1SEL2 |= BIT1 + BIT2 + BIT4;
  //Note: Slave select Pin control is punted to user driver, in case of shared bus

  return SUCCESS;
}

sysStatus USCI_A0::initUART(UART_settings &settings){

    return ERROR; //TO DO
}

void USCI_A0::TxByte(uint8_t data){
    UCA0TXBUF = data;
}
uint8_t USCI_A0::RxByte(){
    uint8_t data=0;
    data=(uint8_t) UCA0RXBUF;
    return data;
}

sysStatus USCI_A0::TxReady(){
    if (IFG2 & UCA0TXIFG)
        return SUCCESS;
    return ERROR;
}



// B0 functions
sysStatus USCI_B0::initSPI(SPI_settings &settings){
    if(m_lock)
        return ERROR;
    else
        m_lock++;

  UCB0CTL0 |=UCSYNC;                          // Set USCI0 to SPI mode
  if(settings.clockSource== SPI_settings::SMCLK)
      UCB0CTL1 |= UCSSEL_2;                   //  select SMCLK as the clock source
  else
      UCB0CTL1 |= UCSSEL_0;                   //  select ACLK as the clock source

  /*SPI Flags*/
  UCB0CTL0 |=UCMST;                          // Run SPI in Master mode
  if(settings.clockPhase==SPI_settings::RISING)
      UCB0CTL0 |=UCCKPH;                     // Set the clock phase to sample on rising edge
  else
      UCB0CTL0 &=~UCCKPH;

  if(settings.clockPolarity==SPI_settings::NORM_LOW)
      UCB0CTL0 &= ~UCCKPL;                  // Set the clock phase to sample on rising edge
  else
      UCB0CTL0 |= UCCKPL;

  if(settings.endian==SPI_settings::MSB_FIRST)
      UCB0CTL0 |=UCMSB;                     // Shift data out MSB first
  else
      UCB0CTL0 &= ~UCMSB;

  //From datasheet: " UCMODE0 and UCMODE1 should NOT be set for 3 pin SPI mode"
  UCB0CTL0 &= ~UCMODE0;
  UCB0CTL0 &= ~UCMODE1;

  UCB0CTL1 &= ~UCSWRST;                     // Exit Reset mode, enabling the module
  IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt
  //IE2 |= UCB0TXIE;                          // Enable USCI0 TX interrupt
  //IFG2 &= ~UCB0TXIFG;

  UCB0BR0 = settings.prescalar;                          // Run the clock as fast as possible(prescalar registers to 0)
  UCB0BR1 = settings.prescalar>>8;

  /*SPI Pins*/
  // Set Pins 1.6 (MISO), 1.7(MOSI) and 1.5(SCLK) to USCIO mode
    P1SEL |= BIT6 + BIT7 + BIT5;
    P1SEL2 |= BIT6 + BIT7 + BIT5;
  //Note: Slave select Pin control is punted to user driver, in case of shared bus

  return SUCCESS;
}

sysStatus USCI_B0::initUART(UART_settings &settings){

    return ERROR; //Peripheral does not support UART
}

void USCI_B0::TxByte(uint8_t data){
    UCB0TXBUF = data;
}
uint8_t USCI_B0::RxByte(){
    uint8_t data=0;
    data=(uint8_t) UCB0RXBUF;
    return data;
}

sysStatus USCI_B0::TxReady(){
    if (IFG2 & UCB0TXIFG)
        return SUCCESS;
    return ERROR;
}





//Base class functions
void USCI::registerRxCallback(void* _comDriverObj, void (*_rxPtr)(void*, USCI&)){
    rxPtr=_rxPtr;
    comDriverObj=_comDriverObj;
}

void USCI::registerTxCallback(void* _comDriverObj, void (*_txPtr)(void*, USCI&)){
    txPtr=_txPtr;
    comDriverObj=_comDriverObj;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    if (IFG2 & UCA0RXIFG){
        USCIA0.rxPtr(USCIA0.comDriverObj, USCIA0);
    }else if(IFG2 & UCB0RXIFG){
        USCIB0.rxPtr(USCIB0.comDriverObj, USCIB0);
    }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
    if (IFG2 & UCA0TXIFG){
        USCIA0.txPtr(USCIA0.comDriverObj, USCIA0);
    }else if(IFG2 & UCB0TXIFG){
        USCIB0.txPtr(USCIB0.comDriverObj, USCIB0);
    }
}