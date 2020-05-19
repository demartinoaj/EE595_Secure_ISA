/*
 * Mosfet.h
 *
 *  Created on: May 18, 2020
 *      Author: Andrew DeMartino
 *      Company: The University of Southern California
 *
 */

#include "MOSFET.hpp"

MOSFET::MOSFET(PIN gate, MosfetType channel): m_channel(channel), m_gate(gate){
    m_gate.setOutput();
}
void MOSFET::enable(){
    if(m_channel==N_CHANNEL_MOS)
        m_gate.setHigh();
    else
        m_gate.setLow();
}

void MOSFET::disable(){
    if(m_channel==N_CHANNEL_MOS)
        m_gate.setLow();
    else
        m_gate.setHigh();
}
