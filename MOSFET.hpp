/*
 * MOSFET.hpp
 *
 *  Created on: May 18, 2020
 *      Author: Andrew DeMartino
 *      Company: The University of Southern California
 *
 *  Description:
 *       This driver abstracts  control of an N or P channel MOSFET
 *
 */
#ifndef MOSFET_HPP_
#define MOSFET_HPP_

#include "system.hpp"

// Enums
 enum MosfetType{P_CHANNEL_MOS, N_CHANNEL_MOS};

class MOSFET{
public:
    MOSFET(PIN gate, MosfetType channel);   // Constructor
    void enable();                                                   // Turn on the MOSFET, allowing current to flow from source to drain
    void disable();                                                  // Turn off MOSFET disabling current flow
protected:
    const MosfetType m_channel;                                 // NMOS or PMOS
    PIN m_gate;
};



#endif /* MOSFET_HPP_ */
