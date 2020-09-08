/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Venusian.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_VENUSIAN_H_
#define _PLANET_TEXTURER_VENUSIAN_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "PlanetTexturer.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Création de textures pour les planètes rocheuses et chaudes (Venusian Planet).
/** 
Spécialisation de la classe PlanetTexturer pour les planètes de type Vénus.
- VENUSIAN pour les planetes avec faible atmosphère: donc peu d'érosion et des 
montagnes abruptes.
- VENUSIAN_2: pour des planètes avec une atmosphère épaisse et des vents violents: 
donc beaucoup d'érosion et un relief en terrasses.
*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer_Venusian : public PlanetTexturer
{
public:
	PlanetTexturer_Venusian();
	virtual ~PlanetTexturer_Venusian();
	void createTexture(String BitmapName);

protected:
	void renderColors(RendererImage& renderer, int PlanetType);

};

#endif
