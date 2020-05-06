/**
 * main.cpp
 */
#include "system.hpp"
#include "SPI.hpp"
#include "UART.hpp"
#include "Micron23K640_SRAM.hpp"
#include <string.h>
#define MAX_SRAM_ADDRESS 0x1FFF

enum pufStates{IDLE, RESET, P_ALL, P_ONE};
void main(void)
{
    const char* error="Error";
    const char* endline="\n\r";

    /* Constructors*/
    SPI SPI1(USCIB0);
    UART UART0(USCIA0);

    PIN SRAM_CS(&P1OUT,&P1DIR,PIN3);
    Micron23k640_SRAM SRAM(&SPI1, SRAM_CS);

    /*Initialize Hardware*/
	WDTCTL = WDTPW | WDTHOLD;	    // stop watchdog timer (oh TI)

#ifdef DEBUG
    P1DIR |= PIN0;                  // configure P1.0 as output
    P1DIR |= PIN6;                  // configure P1.6 as output
    P1OUT |=PIN0;
    P1OUT &=~PIN6;
#endif

    SPI1.init();// Initialize buses
    UART0.init();
    __enable_interrupt();

    /*Initialize Drivers*/
    SRAM.init();
    uint8_t buffer=0;
    SRAM.getStatusReg(&buffer);

#ifdef DEBUG
    char* msg="Intilization Complete\n\r";
    UART0.write((uint8_t*) msg ,strlen(msg));
#endif
    pufStates state= IDLE;
    while(1){

        uint8_t returnData=0;
        SRAM.getStatusReg(&buffer);
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
            else
                UART0.write((uint8_t*) error,strlen(error));
            //TODO: better errors
            break;
        case RESET:
            state=IDLE;
            //TODO
            break;

        case P_ALL:
            //TODO: use write sequential
            for(uint16_t i=0; i<=MAX_SRAM_ADDRESS; i++){
                if(SRAM.readByte(i, &returnData)== ERROR)
                    UART0.write((uint8_t*) error,strlen(error));
                if(UART0.write(&returnData,1) ==ERROR)
                    UART0.write((uint8_t*) error,strlen(error));
#ifdef DEBUG
                 __delay_cycles(1000); //delay long enough to not cause issues
#endif
            }
            state=IDLE;
            break;

        case P_ONE:
#ifdef DEBUG
            msg="Awaiting Command (single byte SRAM address)\n\r";
            UART0.write((uint8_t*) msg ,strlen(msg));
            SRAM.writeByte(0x70, 0x5E); //Useful to read a non-constant value (type p, responds with ^)
#endif
            UART0.read(1);
            UART0.getRxBuff(&buffer,1);
#ifdef DEBUG
            msg="Reading from: ";
            UART0.write((uint8_t*) msg ,strlen(msg));
            UART0.write((uint8_t*) &buffer ,1);
            UART0.write((uint8_t*)endline,strlen(endline));
#endif
            if(SRAM.readByte(buffer, &returnData)== ERROR)
                UART0.write((uint8_t*) error,strlen(error));
#ifdef DEBUG
            msg="Value: ";
            UART0.write((uint8_t*) msg ,strlen(msg));
#endif
            if(UART0.write(&returnData,1) ==ERROR)
                UART0.write((uint8_t*) error,strlen(error));
            if(UART0.write((uint8_t*)endline,strlen(endline)) ==ERROR)
                UART0.write((uint8_t*) error,strlen(error));
            state=IDLE;
            break;
        }
        UART0.flushRxBuff();

    }

}

