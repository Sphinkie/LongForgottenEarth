/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Rock.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "PlanetTexturer_Rock.h"

#include "macros.h"


/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Rock::PlanetTexturer_Rock()
{
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Rock::~PlanetTexturer_Rock()
{
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete recouverte de cratères (type ROCK).
/** @param BitmapName Nom du fichier BMP qui sera généré. */
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Rock::createTexture(String BitmapName)
{
	DEBUG_LOG(">> Rock::createTexture");

	Real LowLand = 0.25;      // Mettre ici le pourcentage de lowland que l'on veut.
	LowLand = (LowLand-0.50);

	this->CheckParameters();

	// On genere une texture de crateres
	module::Crater CraterNoiseModule;
	CraterNoiseModule.SetSeed(mSeed);
	CraterNoiseModule.SetXsize(GRID_WIDTH);
	CraterNoiseModule.SetYsize(GRID_HEIGHT);
	CraterNoiseModule.Generate(GRID_WIDTH*20); // plus la map est grande, plus on peut mettre de crateres
	/// TODO Note: attention lors du mipmapping: de nouveaux cratères peuvent apparaitre...

	// On ajoute un leger flou sour lisser les cratères (BUG: fait planter mon module Crater...)
//	module::Turbulence blurCrater; 
//	blurCrater.SetSourceModule (0, CraterNoiseModule); 
//	blurCrater.SetSeed (mSeed+1);
//	blurCrater.SetFrequency(0.1);
//	blurCrater.SetPower(0.15);

	// Un module pour les collines.
	module::Billow baseHillTerrain;
	baseHillTerrain.SetSeed(mSeed);
	baseHillTerrain.SetFrequency (2.0);
	baseHillTerrain.SetLacunarity(2.5); // des collines un peu rugueuses
	module::ScaleBias HillTerrain;
	HillTerrain.SetSourceModule (0, baseHillTerrain);
	HillTerrain.SetScale(0.15);		// on aplatit les colines
	noise::module::Clamp baseTerrain;
	baseTerrain.SetSourceModule (0, baseHillTerrain);
	baseTerrain.SetBounds (LowLand, 999);

	
	// Un module plat pour les plaines
	module::Const LowTerrain;
	LowTerrain.SetConstValue(LowLand); 

	// On combine les plaines et les collines
	module::Select CliffedTerrain;
	CliffedTerrain.SetSourceModule (0, baseTerrain);
	CliffedTerrain.SetSourceModule (1, LowTerrain);
	CliffedTerrain.SetControlModule(HillTerrain);
	CliffedTerrain.SetBounds(-999, LowLand); // dans le range, on prend: module 1
	//CliffedTerrain.SetEdgeFalloff(0.05);  a tester...
	
	// On combine avec les crateres
	module::Add combinedNoise; 
	combinedNoise.SetSourceModule (0, CliffedTerrain);
	combinedNoise.SetSourceModule (1, CraterNoiseModule);

	// On remplit la noise-map avec cette texture.
  	NoiseMap noiseMap;
	DEBUG_LOG("   building");
//	this->buildSphericalTexture(combinedNoise, noiseMap); // Texture sphérique non gérée par le module Crater
	this->buildPlanarTexture(combinedNoise,false,noiseMap);
	DEBUG_LOG("   build done");

	// On génère la bitmap
	this->generateColormap (noiseMap, BitmapName+".bmp", mPlanetType, true);
	this->generateNormalMap(noiseMap, BitmapName+"_NM.bmp");

	DEBUG_LOG("<< Rock::createTexture");
}




