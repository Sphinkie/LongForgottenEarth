/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Granit.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "PlanetTexturer_Granit.h"

#include "macros.h"


/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Granit::PlanetTexturer_Granit()
{
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Granit::~PlanetTexturer_Granit()
{
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete.
/** @param BitmapName Nom du fichier BMP qui sera généré. */
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Granit::createTexture(String BitmapName)
{
	DEBUG_LOG(">> PlanetTexturer_Granit::createTexture()");

	this->CheckParameters();

	// On génère la texture granitique 
	module::Cache finalTerrain;

	// Primary granite texture.  
	// This generates the "roughness" of the texture when lit by a light source.
	module::Billow primaryGranite;
	primaryGranite.SetSeed (mSeed+0);
	primaryGranite.SetFrequency (8.0);
	primaryGranite.SetPersistence (0.625);
	primaryGranite.SetLacunarity (2.18359375);
	primaryGranite.SetOctaveCount (6);
	primaryGranite.SetNoiseQuality (QUALITY_STD);

	// Use Voronoi polygons to produce the small grains for the granite texture.
	module::Voronoi baseGrains;
	baseGrains.SetSeed (mSeed+1);
	baseGrains.SetFrequency (16.0);
	baseGrains.EnableDistance (true);

	// Scale the small grain values so that they may be added to the base
	// granite texture.  Voronoi polygons normally generate pits, so apply a
	// negative scaling factor to produce bumps instead.
	module::ScaleBias scaledGrains;
	scaledGrains.SetSourceModule (0, baseGrains);
	scaledGrains.SetScale (-0.5);
	scaledGrains.SetBias (0.0);

	// Combine the primary granite texture with the small grain texture.
	module::Add combinedGranite;
	combinedGranite.SetSourceModule (0, primaryGranite);
	combinedGranite.SetSourceModule (1, scaledGrains);

	// Finally, perturb the granite texture to add realism.
	module::Turbulence finalGranite;
	finalGranite.SetSourceModule (0, combinedGranite);
	finalGranite.SetSeed (mSeed+2);
	finalGranite.SetFrequency (4.0);
	finalGranite.SetPower (1.0 / 8.0);
	finalGranite.SetRoughness (6);
	
	// On remplit la noise-map.
  	NoiseMap noiseMap;
	this->buildSphericalTexture(finalGranite, noiseMap);

	// On génère la bitmap
	this->generateColormap(noiseMap, BitmapName+".bmp", GRANIT);
	this->generateNormalMap(noiseMap,BitmapName+"_NM.bmp");

	// On génère quelques autres cartes pour les tests:
	this->generateSpecMap(noiseMap,"granit_specmap.bmp");
	this->generateBumpMap(noiseMap,"granit_bumpmap.bmp");

	DEBUG_LOG("<< PlanetTexturer_Granit::createTexture");
}






/* ------------------------------------------------------------------------- */
/// Fabrique une texture de type granitique. TESTS
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Granit::_renderGranitTexture(module::Cache& finalTerrain)
{
	DEBUG_LOG(">> renderGranitTexture()");

	// Primary granite texture.  
	// This generates the "roughness" of the texture when lit by a light source.
	module::Billow primaryGranite;
	primaryGranite.SetSeed (mSeed+0);
	primaryGranite.SetFrequency (8.0);
	primaryGranite.SetPersistence (0.625);
	primaryGranite.SetLacunarity (2.18359375);
	primaryGranite.SetOctaveCount (6);
	primaryGranite.SetNoiseQuality (QUALITY_STD);

	// Use Voronoi polygons to produce the small grains for the granite texture.
	module::Voronoi baseGrains;
	baseGrains.SetSeed (mSeed+1);
	baseGrains.SetFrequency (16.0);
	baseGrains.EnableDistance (true);

	// Scale the small grain values so that they may be added to the base
	// granite texture.  Voronoi polygons normally generate pits, so apply a
	// negative scaling factor to produce bumps instead.
	module::ScaleBias scaledGrains;
	scaledGrains.SetSourceModule (0, baseGrains);
	scaledGrains.SetScale (-0.5);
	scaledGrains.SetBias (0.0);

	// Combine the primary granite texture with the small grain texture.
	module::Add combinedGranite;
	combinedGranite.SetSourceModule (0, primaryGranite);
	combinedGranite.SetSourceModule (1, scaledGrains);

	// Finally, perturb the granite texture to add realism.
	module::Turbulence finalGranite;
	finalGranite.SetSourceModule (0, combinedGranite);
	finalGranite.SetSeed (mSeed+2);
	finalGranite.SetFrequency (4.0);
	finalGranite.SetPower (1.0 / 8.0);
	finalGranite.SetRoughness (6);

	// On met en cache le terrain généré
	finalTerrain.SetSourceModule(0,finalGranite);
	DEBUG_LOG("<< renderGranitTexture()");

}

