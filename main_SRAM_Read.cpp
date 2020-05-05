/**
 * main.cpp
 */
#include "system.hpp"
#include "SPI.hpp"
#include "UART.hpp"
#include "Micron23K640_SRAM.hpp"

SPI SPI1(USCIB0);
UART UART0(USCIA0);

PIN SRAM_CS(&P1OUT,&P1DIR,PIN3);
Micron23k640_SRAM SRAM(&SPI1, SRAM_CS);
uint16_t SRAM_address=0x0000;
int iterator=0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer (oh TI)

    uint8_t buffer=0;
    uint8_t returnData=0;

    P1DIR |= PIN0;                  // configure P1.0 as output
    P1DIR |= PIN6;                  // configure P1.6 as output
    P1OUT |=PIN0;
    P1OUT &=~PIN6;

    P2DIR |= PIN6;  // configure P2.6 as output
    P2OUT |= PIN6; //Set Output to 1
//    //spi
    P1DIR |= PIN3;
    P1OUT |=PIN3;

    volatile uint32_t i;        // volatile to prevent optimization
    SPI1.init();                // Initialize buses
    UART0.init();
    __enable_interrupt();
    SRAM.init();


    while(1)
    {
        P1OUT ^= PIN0;              // toggle P1.0, LED1
        P1OUT ^= PIN6;              // toggle P1.6 LED2
       // for(i=100000; i>0; i--);     //

//
//        P1OUT |=PIN3;
//
//        P1OUT &=~PIN3;
//        uint8_t cmd=0x05;
//        SPI1.write(&cmd, 1);
//        SPI1.read(1);
        //SPI1.getRxBuff(&buffer, 1);
        for(iterator=0;iterator<5;iterator++)
        {
//            P2OUT ^= PIN6;//Switch On

//            __delay_cycles(1000000);
//            P2OUT ^= PIN6;//Switch OFF
//            __delay_cycles(1000000);

            SRAM.getStatusReg(&buffer);

            //SRAM.getStatusReg(&buffer);
            //SRAM.writeByte(0xDEAD, 0x42);
    //        SRAM.readByte(0x0595, &returnData);
            SRAM.readByte(SRAM_address, &returnData);

    //        __delay_cycles(10);
    //        SRAM.readByte(0x3FFF, &returnData);
    //        __delay_cycles(5000);

           // UART0.write((uint8_t* )"test",4);
            UART0.write(&returnData,sizeof(returnData));
        }
        iterator=0;
        SRAM_address+=1;
//        P1OUT &=~PIN3;
//        SPI1.write(0x01);
//        __delay_cycles(500);
//        SPI1.write(0x01);
//        //SPI1.write(0x00);
//        __delay_cycles(5000);
//        P1OUT |=PIN3;

    }

}

