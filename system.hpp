/*
 * system.hpp
 *
 *  Created on: Apr 6, 2020
 *      Author: Andrew DeMartino
 *		Company: The University of Southern California
 *
 *		Relevant Pins: 1.0 LED1
 *		               1.6 LED2
 */

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

//
#include <msp430.h>
#include <stdint.h>

//macros
#define PIN0                   (0x01)
#define PIN1                   (0x02)
#define PIN2                   (0x04)
#define PIN3                   (0x08)
#define PIN4                   (0x10)
#define PIN5                   (0x20)
#define PIN6                   (0x40)
#define PIN7                   (0x80)

enum sysStatus{SUCCESS, ERROR};

struct PIN{
    uint8_t outReg;
    uint8_t dirReg;
    uint8_t num;
    PIN(uint8_t _outReg, uint8_t _dirReg, uint8_t _num): outReg(_outReg), dirReg(_dirReg), num(_num){};
};
//Functions

//TO DO: Liter this with ASSERT, also write ASSERT
template <typename Type>
sysStatus sys_cpBuff(Type inBuff[], Type outBuff[], uint16_t size, uint16_t offset){
    for (uint16_t i = 0; i < size; i++){
        outBuff[i] = inBuff[i];
    }
    return SUCCESS;
}

#endif /* SYSTEM_HPP_ */
