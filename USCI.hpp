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

#define USCI_BUFF_SIZE 32   //Size of the USCI buffer, in bytes

class USCI{
public:
    virtual sysStatus init()=0;
    virtual void registerRxCallback( void (*rxPtr)(void));
    virtual void registerTxCallback( void (*txPtr)(void));
    volatile uint8_t rxBuff[USCI_BUFF_SIZE];      // The Rx buffer
    volatile uint8_t txBuff[USCI_BUFF_SIZE];      // The Tx buffer
    volatile uint8_t rxAvail=0;                  // The number of bytes currently in the Rx buffer
    volatile uint16_t txAvail=0;                 // The number of bytes currently in the Tx buffer

    //Registers
    const volatile uint8_t* CTL0;
    const volatile uint8_t* CTL1;
    const volatile uint8_t* BR0;
    const volatile uint8_t* BR1;
    const volatile uint8_t* RXBUF;
    const volatile uint8_t* TXBUF;

    const volatile uint8_t* MCTL;
    const volatile uint8_t* ABCTL;
    const volatile uint8_t* IRTCTL;
    const volatile uint8_t* IRRCTL;

    const volatile uint8_t* I2CIE;
    const volatile unsigned int* I2COA;
    const volatile unsigned int* I2CSA;
protected:
    void (*txPtr)(void);
    void (*rxPtr)(void);
};

class USCI_A0 :public USCI{
    friend void USCIA0RX_ISR(void);
    friend void USCIA0TX_ISR(void);
public:
    virtual sysStatus init();
    const volatile uint8_t* CTL0=&UCA0CTL0;
    const volatile uint8_t* CTL1=&UCA0CTL1;
    const volatile uint8_t* BR0=&UCA0BR0;
    const volatile uint8_t* BR1=&UCA0BR1;
    const volatile uint8_t* RXBUF=&UCA0RXBUF;
    const volatile uint8_t* TXBUF=&UCA0TXBUF;

    const volatile uint8_t* MCTL=&UCA0MCTL;
    const volatile uint8_t* ABCTL=&UCA0ABCTL;
    const volatile uint8_t* IRTCTL=&UCA0IRTCTL;
    const volatile uint8_t* IRRCTL=&UCA0IRRCTL;

    const volatile uint8_t* I2CIE=0x00;
    const volatile unsigned int* I2COA=0x00;
    const volatile unsigned int* I2CSA=0x00;
protected:
    static uint8_t lock;
};

class USCI_B0 :public USCI{
    friend void USCIA0RX_ISR(void);
    friend void USCIA0TX_ISR(void);
public:
    virtual sysStatus init();
    const volatile uint8_t* CTL0=&UCB0CTL0;
    const volatile uint8_t* CTL1=&UCB0CTL1;
    const volatile uint8_t* BR0=&UCB0BR0;
    const volatile uint8_t* BR1=&UCB0BR1;
    const volatile uint8_t* RXBUF=&UCB0RXBUF;
    const volatile uint8_t* TXBUF=&UCB0TXBUF;

    const volatile uint8_t* MCTL=0x00;
    const volatile uint8_t* ABCTL=0x00;
    const volatile uint8_t* IRTCTL=0x00;
    const volatile uint8_t* IRRCTL=0x00;

    const volatile uint8_t* I2CIE=&UCB0I2CIE;
    const volatile unsigned int* I2COA=&UCB0I2COA;
    const volatile unsigned int* I2CSA=&UCB0I2CSA;
protected:
    static uint8_t lock;

};

#endif /* USCI_HPP_ */
