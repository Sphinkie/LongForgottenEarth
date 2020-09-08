/* ------------------------------------------------------------------------- */
// File       : Spares.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SPARES_H_
#define _SPARES_H_

#include "Ogre.h"


enum tSpare  
	{
	NOTHING, CABLES, BOLT, INOX, MICROCHIP, // valeurs = 0-1-2-3-4
	FREON, HYPCOND, ADAPT, MOD, DEMOD, 
	REP, PLASTA, MEM
	};

const Ogre::String SpareName[] = 
	{
	"none", "wires", "bolts", "inox", "microchip" ,
	"freon cartridge", "hypercond. circuit", "Continious adaptator", "YP modulator", "YP demodulator",
	"HHF repartitor", "Plastacier plate", "DDL memory bank"
	};


#endif
