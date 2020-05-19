/**
 * main.cpp
 */
#include "system.hpp"
#include "MOSFET.hpp"
#include "SPI.hpp"
#include "UART.hpp"
#include "Micron23K640_SRAM.hpp"
#include <string.h>
#define MAX_SRAM_ADDRESS 0x1FFF

enum pufStates{IDLE, RESET, P_ALL, P_ONE, READ_KEY};

void main(void)
{
    const char* error="Error";
    const char* endline="\r\n";
    uint8_t resultValues[8];

    /* Constructors*/
    SPI SPI1(USCIB0);
    UART UART0(USCIA0);

    PIN SRAM_CS(&P1OUT,&P1DIR,PIN3);
    PIN SRAM_En(&P1OUT,&P1DIR,PIN4);

    MOSFET SRAM_Pwr(SRAM_En,P_CHANNEL_MOS);
    Micron23k640_SRAM SRAM(&SPI1, SRAM_CS);

    /*Initialize Hardware*/
	WDTCTL = WDTPW | WDTHOLD;	    // stop watchdog timer (oh TI)

/* Initialize GPIO*/
#ifdef DEBUG
    P1DIR |= PIN0;                  // configure P1.0 as output
    P1OUT |=PIN0;
#endif
    SRAM_Pwr.enable();

    /* Initialize buses*/
    SPI1.init();
    UART0.init();
    __enable_interrupt();

    /* Initialize Drivers*/
    SRAM.init();
    uint8_t buffer=0;
    SRAM.getStatusReg(&buffer);

#ifdef DEBUG
    char* msg="Intilization Complete\r\n";
    UART0.write((uint8_t*) msg ,strlen(msg));
#endif
    pufStates state= IDLE;

    /*MAIN LOOP*/
    while(1){

        uint8_t returnData=0;
        SRAM.getStatusReg(&buffer);
        uint16_t sramAddress=0;

#ifdef DEBUG
        P1OUT ^= PIN0;              // toggle P1.0, LED1
#endif
        switch(state){
        case IDLE:
#ifdef DEBUG

    msg="Awaiting Command r: reset, a: print all, o: print one\n\r";
    UART0.write((uint8_t*) msg ,strlen(msg));
#endif
            UART0.read(1); //Blocking
            UART0.getRxBuff(&buffer,1);
            if(buffer=='r')
                state=RESET;
            else if(buffer=='a')
                state=P_ALL;
            else if(buffer=='o')
                state=P_ONE;
            else if(buffer=='s')
                state=READ_KEY;
            else
                UART0.write((uint8_t*) error,strlen(error));
            //TODO: better errors
            break;



        case RESET:
#ifdef DEBUG
            msg="SRAM Reseting.....";
            UART0.write((uint8_t*) msg ,strlen(msg));
#endif
            SRAM_Pwr.disable();
            __delay_cycles(5000000); //delay long enough to actually reset
            SRAM_Pwr.enable();
#ifdef DEBUG
            msg="Done\r\n";
            UART0.write((uint8_t*) msg ,strlen(msg));
#endif
            state=IDLE;

            msg="Reset Success\r\n";
            UART0.write((uint8_t*) msg ,strlen(msg));
            break;

        case P_ALL:
            for(uint16_t i=0; i<=MAX_SRAM_ADDRESS; i++){
                if(SRAM.readByte(i, &returnData)== ERROR)
                    UART0.write((uint8_t*) error,strlen(error));
                if(UART0.write(&returnData,1) ==ERROR)
                    UART0.write((uint8_t*) error,strlen(error));
                if(UART0.write((uint8_t*)endline,strlen(endline)) ==ERROR)
                    UART0.write((uint8_t*) error,strlen(error));
#ifdef DEBUG
                 __delay_cycles(1000); //delay long enough to not cause issues
#endif
            }
            state=IDLE;
            break;
/*
 *
 * CASE One
 *
 */
        case P_ONE:
#ifdef DEBUG
            msg="Awaiting Command (Two byte SRAM address)\n\r";
            UART0.write((uint8_t*) msg ,strlen(msg));
            SRAM.writeByte(0x6869, 0x5E); //Useful to read a non-constant value (type hi, responds with ^)
#endif
            /*Read two bytes from UART*/
            UART0.read(2);
            UART0.getRxBuff(resultValues,2);

#ifdef DEBUG
            msg="Reading from: ";
            UART0.write((uint8_t*) msg ,strlen(msg));
            UART0.write((uint8_t*) resultValues ,2);
            UART0.write((uint8_t*)endline,strlen(endline));
#endif
            /*Check The SRAM address received */
            sramAddress=resultValues[0]<<8;
            sramAddress|=resultValues[1];
            if(SRAM.readByte(sramAddress, &returnData)== ERROR)
                UART0.write((uint8_t*) error,strlen(error));
#ifdef DEBUG
            msg="Value: ";
            UART0.write((uint8_t*) msg ,strlen(msg));
#endif
            /*Print Response back to UART*/
            if(UART0.write(&returnData,1) ==ERROR)
                UART0.write((uint8_t*) error,strlen(error));
            if(UART0.write((uint8_t*)endline,strlen(endline)) ==ERROR)
                UART0.write((uint8_t*) error,strlen(error));
            state=IDLE;
            break;

        case READ_KEY:
            //UNUSED
            break;

        }
        UART0.flushRxBuff();

    }

}

