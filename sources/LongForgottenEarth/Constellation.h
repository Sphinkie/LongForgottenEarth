/* ------------------------------------------------------------------------- */
// File       : constellation.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
// Author     : Chris Laurel
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
// Copyright (C) 2001,  <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
/* ------------------------------------------------------------------------- */

#ifndef _CONSTELLATION_H_
#define _CONSTELLATION_H_


#include "ogre.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Données générales sur la dénomination des étoiles et des constellations
/* ------------------------------------------------------------------------- */
class Constellation
{
public:
    Constellation();
    static int getConstellation(String name);
    String getName(int index);
    String getGenitive(int index);
    String getAbbreviation(int index);
	String getGreekLetter(String abbrev);

private:

	struct t_constellation
	{
    const char *name;
    const char *genitive;
    const char *abbr;
	};

	struct t_abbrev 
	{
    const char *name;
    const char *abbr;
	};


};

#endif // _CONSTELLATION_H_


