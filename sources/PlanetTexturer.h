/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _PLANET_TEXTURER_H_
#define _PLANET_TEXTURER_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include <noise.h>
#include "lib_noiseutils/noiseutils.h"
#include "XMLparser_T.h"

using namespace Ogre;

#define DEFAULT_PLANET 0
#define TEMPERATE_1    1
#define TEMPERATE_2    2
#define TERRESTRIAL    3  // ex: TEMPERATE_3
#define TERRESTRIAL_2  4  // ex: TEMPERATE_4
#define ROCK           5
#define ROCK_2         6
#define GAZ_BLUE       7
#define GAZ_BROWN      8
#define GAZ_RED        9
#define VENUSIAN       20
#define VENUSIAN_2     21
#define MARTIAN        22   // OK = idem 5 ROCK
#define WATER          24
#define ICE            26
#define GRANIT         28

/* ------------------------------------------------------------------------- */
/// Création de textures pour les planètes.
/** Ce générateur crée une texture pour la planete, selon un certain nombre 
    de paramètres, tels que l'aridité, la température, etc.
	On utilise la librairie 'libnoise' dans une version modifiée par David.
	Chaque texture est différente, mais une même 'seed' fournit la même texture.
*/
/* ------------------------------------------------------------------------- */
class PlanetTexturer
{
public:
	PlanetTexturer();
	~PlanetTexturer();

	void createReferencePlanet();
	void createTexture(String BitmapName);

	void setLightColor(Ogre::ColourValue couleur);
	void setPlanetType(int PlanetType);
	void setSeed(int seed);

protected:

	void buildPlanarTexture   (const module::Module& noiseModule, bool seamless, NoiseMap& noiseMap);
	void buildSphericalTexture(const module::Module& noiseModule, NoiseMap& noiseMap);

	virtual void renderColors(RendererImage& renderer, int PlanetType);
	void renderLight (RendererImage& renderer);
	void renderLandscapeLight(RendererImage& renderer);
	bool openGradientFile(String SVGfilename);
	bool addGradient(RendererImage& renderer);
	void closeGradientFile();
	ColourValue parseRGB1(String colorString);
	ColourValue parseRGB2(String colorString);

	void generateColormap   (const NoiseMap& noiseMap, String filename, int PlanetType=0, bool light=true);
	void generateSpecMap    (const NoiseMap& noiseMap, String filename);
	void generateBumpMap    (const NoiseMap& noiseMap, String filename);
	void generateNormalMap  (NoiseMap& noiseMap, String filename);
	void generateRawMap     (NoiseMap& noiseMap, String filename);
	void generateTerragenMap(NoiseMap& noiseMap, String filename);

	void CheckParameters();
	void scalePlanetInMeters(NoiseMap& noiseMap, Real minAltitude, Real maxAltitude, Real Circonference);
	void flattenSea(NoiseMap& noiseMap);
	Real altitude(Real x);
	Real toHauteur(Real x);
	Ogre::uint8 dec(char C);

	int    GRID_WIDTH;     ///< Width of elevation grid, in points.
	int    GRID_HEIGHT;    ///< Height of elevation grid, in points.
	double mSeaLevel;      ///< Specifies the planet's sea level.  [-1..+1]
	int    mSeed;          ///< Planet seed.
	double mResInMeters;   ///< Resoltion of the map in meters (not used for planet maps)
	Color  mLightColor;    ///< Color of the sun light.
	int    mPlanetType;    ///< Type de la planete (temperée, rocheuse, etc)
	XMLParser_T* mSVGfile; ///< Fichier de description des couleurs pour texturer une planete
	int          mIndex;   ///< Index des elements stop dans le fichier SVG.
	
};

#endif 
