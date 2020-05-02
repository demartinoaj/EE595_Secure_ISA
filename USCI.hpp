/*
 * USCI.hpp
 *
 *  Created on: Apr 5, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 */

#ifndef USCI_HPP_
#define USCI_HPP_

#include "system.hpp"

//#define USCI_BUFF_SIZE 8   //Size of the USCI buffer, in bytes

struct SPI_settings{
    enum { ACLK, SMCLK}clockSource;
    enum { RISING, FALLING}clockPhase;
    enum { NORM_LOW, NORM_HIGH}clockPolarity;
    enum { MSB_FIRST, LSB_FIRST}endian;
    uint16_t prescalar;
};

struct UART_settings{
    //TO DO
   enum { ACLK, SMCLK,}clockSource;
//    enum { RISING, FALLING,}clockPhase;
//    enum { NORM_LOW, NORM_HIGH,}clockPolarity;
//    enum { MSB_FIRST, LSB_FIRST,}endian;
//    uint16_t prescalar;
};

/* abstract base class*/
class USCI{
public:
    virtual sysStatus initSPI(SPI_settings &settings)=0;    // Configure the USCI to transmit the SPI protocol
    virtual sysStatus initUART(UART_settings &settings)=0;  // Configure the USCI to transmit the UART protocol
    virtual void registerRxCallback(void*, void (*rxPtr)(void*, USCI&));  // Register a function to be called during a Rx interrupt
    virtual void registerTxCallback(void*,  void (*txPtr)(void*, USCI&));  // Register a function to be called during a Tx interrupt
    virtual void TxByte(uint8_t data)=0;
    virtual uint8_t RxByte()=0;
    virtual sysStatus TxReady()=0;

protected:
    void (*txPtr)(void*, USCI &USCI);
    void (*rxPtr)(void*, USCI &USCI);
    void* comDriverObj;
};

class USCI_A0 :public USCI{
    friend void USCI0RX_ISR(void);
    friend void USCI0TX_ISR(void);
public:
    virtual sysStatus initSPI(SPI_settings &settings);    // Configure the USCI to transmit the SPI protocol
    virtual sysStatus initUART(UART_settings &settings);  // Configure the USCI to transmit the UART protocol
    virtual void TxByte(uint8_t data);
    virtual uint8_t RxByte();
    virtual sysStatus TxReady();

protected:
    static uint8_t m_lock;
};

class USCI_B0 :public USCI{
    friend void USCI0RX_ISR(void);
    friend void USCI0TX_ISR(void);
public:
    virtual sysStatus initSPI(SPI_settings &settings);    // Configure the USCI to transmit the SPI protocol
    virtual sysStatus initUART(UART_settings &settings);  // Configure the USCI to transmit the UART protocol
    virtual void TxByte(uint8_t data);
    virtual uint8_t RxByte();
    virtual sysStatus TxReady();

protected:
    static uint8_t m_lock;
};

#ifdef USE_USCI_A0
extern USCI_A0 USCIA0;
#endif

#ifdef USE_USCI_B0
extern USCI_B0 USCIB0;
#endif

#endif /* USCI_HPP_ */
