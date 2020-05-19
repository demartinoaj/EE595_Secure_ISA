/*
 * USCI.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 *
 *  Description:
 *      The Universal Serial Communication Interface(USCI) is used by the MSP430 series processor to control all types of
 *      serial communication. Varrious varries support diffren modes, but UART and SPI are implmented here for use of this project
 *
 *
 *  Sources:
 *      I used the MSP430 G2X13 Datasheet (https://www.ti.com/lit/ds/symlink/msp430g2553.pdf)
 *      and the MSP430x2xx Family Users's guide (https://www.ti.com/lit/ug/slau144j/slau144j.pdf)
 *      to implement the driver. The code works as is with any MSP430 that uses USCI for SPI communication.
 *
 * How to use:
 *  The actual hardware classes A0 and B0 are derived from the abstract base
 *  The classes shouldn't be created directly, but instead the exported instances should be used
 *  The A0 and B0 classes can be used to abstract harware to a software onlt serial controller class, for portability and ease of use
 *
 *
 *  To Do:
 *      Add I2C and CAN bus
 */
#include "USCI.hpp"

//Declarations

#ifdef USE_USCI_A0
USCI_A0 USCIA0;
#endif

#ifdef USE_USCI_B0
USCI_B0 USCIB0;
#endif


// Initialize the two static variables
uint8_t USCI_B0::m_lock=0;
uint8_t USCI_A0::m_lock=0;

sysStatus USCI_A0::initSPI(SPI_settings &settings){
    /* Ensure USCI isn't in use already*/
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
    /* Ensure USCI isn't in use already*/
    if(m_lock)
        return ERROR;
    else
        m_lock++;

    //------------------- Configure the Clocks -------------------//

     if (CALBC1_1MHZ==0xFF)   // If calibration constant erased
        {
           while(1);          // do not load, trap CPU!!
        }
     UCA0CTL1 |= UCSWRST;             // First disable UCA0

      DCOCTL  = 0;             // Select lowest DCOx and MODx settings
      BCSCTL1 = CALBC1_1MHZ;   // Set range
      DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation



     //------------ Configuring the UART(USCI_A0) ----------------//

     // UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
      UCA0CTL1 |=  UCSSEL_2;
      UCA0BR0   =  104;                 // 104 From datasheet table-
      UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
      UCA0MCTL  =  UCBRS_1;             // Modulation value = 1 from datasheet
      //UCA0STAT |=  UCLISTEN;          // loop back mode enabled
      UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0

     //---------------- Enabling the interrupts ------------------//

      //IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
      IE2 |= UCA0RXIE;                  // Enable the Receive  interrupt
      /* UART PINS */

       P1SEL  |=  BIT1 + BIT2;  // P1.1 UCA0RXD input
       P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output

    return SUCCESS;
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
    /*Check the Tx bit of the interrupt Flag*/
    /* TXIFG is still set even if the interrupt is disabled, it indicates the module is ready to transmit*/
    if (IFG2 & UCA0TXIFG)
        return SUCCESS;
    return ERROR;
}

BOOL USCI_A0::isBusy(){
    if (UCA0STAT & UCBUSY)
        return TRUE;
    return FLASE;
}

// B0 functions
sysStatus USCI_B0::initSPI(SPI_settings &settings){
    /* Ensure USCI isn't in use already*/
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
  //IE2 |= UCB0TXIE;                        // Enable USCI0 TX interrupt
  //IFG2 &= ~UCB0TXIFG;

  /* Configure Pre-scalar*/
  UCB0BR0 = settings.prescalar;             // Run the clock as fast as possible(prescalar registers to 0)
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
    /*Check the Tx bit of the interrupt Flag*/
    /* TXIFG is still set even if the interrupt is disabled, it indicates the module is ready to transmit*/
    if (IFG2 & UCB0TXIFG)
        return SUCCESS;
    return ERROR;
}

BOOL USCI_B0::isBusy(){
    if (UCB0STAT & UCBUSY)
        return TRUE;
    return FLASE;
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

/* Actual interrupts*/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    if ((IFG2 & UCA0RXIFG) && (IE2 & UCA0RXIE )){
        if(USCIA0.rxPtr==0) // Ensure it exists!
            return;
        USCIA0.rxPtr(USCIA0.comDriverObj, USCIA0); // call function pointer
    }
    if((IFG2 & UCB0RXIFG) && (IE2 & UCB0RXIE )){
        if(USCIB0.rxPtr==0)
            return;
        USCIB0.rxPtr(USCIB0.comDriverObj, USCIB0);
    }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
    if ((IFG2 & UCA0TXIFG) && (IE2 & UCA0TXIE )){
        if(USCIA0.txPtr==0)
            return;
        USCIA0.txPtr(USCIA0.comDriverObj, USCIA0);
    }
    if((IFG2 & UCB0TXIFG) && (IE2 & UCB0TXIE )){
        if(USCIB0.txPtr==0)
            return;
        USCIB0.txPtr(USCIB0.comDriverObj, USCIB0);
    }
}





