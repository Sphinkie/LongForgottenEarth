/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Complex.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_COMPLEX_H_
#define _PLANET_TEXTURER_COMPLEX_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Création de textures pour les planètes complexes.
/** Spécialisation de la classe PlanetTexturer pour les planètes complexes.

Performances :
-  128x64   =  1 sec
-  256x128  =  5 sec
-  512x256  = 21 sec - acceptable de loin
- 1024x512  = 1min20 - pas mal.
- 2048x1024 = 5min30

*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Complex : public PlanetTexturer
{
public:
	PlanetTexturer_Complex();
	virtual ~PlanetTexturer_Complex();
	void createTexture(String BitmapName);

protected:
	
	void CheckParameters();

	// Changing Lacunarity value produces slightly different relief.  
	// For the best results, this value should be random, but close to 2.0.

	double MOUNTAIN_LACUNARITY;  /// Lacunarity of the planet's mountains.  
	double HILLS_LACUNARITY;     /// Lacunarity of the planet's hills.  
	double PLAINS_LACUNARITY;    /// Lacunarity of the planet's plains.  
	double BADLANDS_LACUNARITY;  /// Lacunarity of the planet's badlands.  
	double CONTINENT_LACUNARITY; /// Lacunarity of the planet's continents.

	double MOUNTAINS_TWIST;      /// Specifies the "twistiness" of the mountains.
	double HILLS_TWIST;          /// Specifies the "twistiness" of the hills.
	double BADLANDS_TWIST;       /// Specifies the "twistiness" of the badlands.

	double SHELF_LEVEL;          /// Specifies the level on the planet in which continental shelves appear, and must be less than SEA_LEVEL.  [-1..+1]

	double MOUNTAINS_AMOUNT;     /// Determines the amount of mountainous terrain that appears on the planet. [0..1]
	double HILLS_AMOUNT;         /// Determines the amount of hilly terrain that appears on the planet. [0..1]
	double BADLANDS_AMOUNT;      /// Determines the amount of badlands terrain that covers the planet. [0..1]
	double MOUNTAIN_GLACIATION;  /// Specifies the amount of "glaciation" on the mountains. [0..1]

	// Offset to apply to the terrain type definition.  Low values (< 1.0) cause
	// the rough areas to appear only at high elevations.  High values (> 2.0)
	// cause the rough areas to appear at any elevation.  The percentage of
	// rough areas on the planet are independent of this value.
	double TERRAIN_OFFSET;

	// Frequency of the planet's continents.  Higher frequency produces smaller,
	// more numerous continents.  This value is measured in radians.
	double CONTINENT_FREQUENCY;

	double CONTINENT_HEIGHT_SCALE; /// Scaling to apply to the base continent elevations, in planetary elevation units.
	double RIVER_DEPTH;            /// Maximum depth of the rivers, in planetary elevation units.

};

#endif
