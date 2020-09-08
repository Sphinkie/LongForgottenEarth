/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Venusian.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "PlanetTexturer_Venusian.h"

#include "macros.h"


/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Venusian::PlanetTexturer_Venusian()
{
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Venusian::~PlanetTexturer_Venusian()
{
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete avec un découpage en plaines et montagnes.
/** 
Cette fonction est assez adaptée pour des planetes desertiques. Elle génères 
deux types de reliefs un peu différents selon mPlanetType: un relief accidenté, 
et un relief avec de nombreux plateaux (terrace)
Cette texture est générée à partir de 6 modules.
@param BitmapName Nom du fichier image qui sera généré (sans extension). 
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Venusian::createTexture(String BitmapName)
{
	DEBUG_LOG(">> Venusian::createTexture");

	// % de montagnes (faible si beaucoup de vent)
	Real moutainRatio = 0;   // -1..+1   (-1 = no high montains)

	this->CheckParameters();

	// Un module pour les montagnes
	module::RidgedMulti mountainTerrain;
	mountainTerrain.SetSeed (mSeed);
	mountainTerrain.SetLacunarity(2.7);	// montagnes abruptes (1.5..3.5)
	mountainTerrain.SetFrequency(1.7);  // entre 1.0 et 2.0 (chaines de montagnes +/- continues)

	// Un module pour les plaines.
	module::Billow baseFlatTerrain;
	baseFlatTerrain.SetFrequency (2.0);
	baseFlatTerrain.SetSeed (mSeed+1);

	// Un module géographique (séparateur de zones montagneuses)
    module::Perlin terrainType;
	terrainType.SetFrequency (1.0);
	terrainType.SetPersistence (0.25);
	terrainType.SetSeed (mSeed+2);
	module::Select moutainFinal;
	moutainFinal.SetControlModule (terrainType);
	moutainFinal.SetSourceModule (0, baseFlatTerrain); // grosses collines
	moutainFinal.SetSourceModule (1, mountainTerrain); // montagnes
	moutainFinal.SetBounds (moutainRatio, 5);	
	moutainFinal.SetEdgeFalloff (0.125);

	// On applatit les plaines
	module::ScaleBias flatTerrain;
	flatTerrain.SetSourceModule (0, baseFlatTerrain);
	flatTerrain.SetScale(0.20);		// on aplatit les collines
	flatTerrain.SetBias (-0.60);	// on les abaisse

	// La combinaison des modules
	module::Blend finalTerrain;
	finalTerrain.SetControlModule (mountainTerrain);
	finalTerrain.SetSourceModule (0, flatTerrain);
	finalTerrain.SetSourceModule (1, moutainFinal);

  	NoiseMap noiseMap;

	if (mPlanetType==VENUSIAN_2)
	{
		// On terrasse tout le relief
		noise::module::Terrace finalTerrain_te;
		finalTerrain_te.SetSourceModule (0, finalTerrain);
		finalTerrain_te.AddControlPoint (-1.0);
//		finalTerrain_te.AddControlPoint (-0.9);
		finalTerrain_te.AddControlPoint (-0.8);
//		finalTerrain_te.AddControlPoint (-0.7);
		finalTerrain_te.AddControlPoint (-0.6);
//		finalTerrain_te.AddControlPoint (-0.5);
		finalTerrain_te.AddControlPoint (-0.4);
		finalTerrain_te.AddControlPoint (-0.3);
//		finalTerrain_te.AddControlPoint (-0.2);
		finalTerrain_te.AddControlPoint (0.0);
//		finalTerrain_te.AddControlPoint (0.1);
		finalTerrain_te.AddControlPoint (0.2);
//		finalTerrain_te.AddControlPoint (0.3);
		finalTerrain_te.AddControlPoint (0.4);
//		finalTerrain_te.AddControlPoint (0.5);
		finalTerrain_te.AddControlPoint (0.6);
//		finalTerrain_te.AddControlPoint (0.7);
		finalTerrain_te.AddControlPoint (0.8);
//		finalTerrain_te.AddControlPoint (0.9);
//		finalTerrain_te.AddControlPoint (1.0);
		/// TODO ajuster les terraces pour etre raccord avec venus_2.svg
		this->buildSphericalTexture(finalTerrain_te, noiseMap);
	}
	else
		this->buildSphericalTexture(finalTerrain, noiseMap);

	// On génère la bitmap et la normalmap
	this->generateColormap (noiseMap, BitmapName+".bmp", mPlanetType, true);
	this->generateNormalMap(noiseMap, BitmapName+"_NM.bmp");

	DEBUG_LOG("<< Venusian::createTexture");
}




/* ------------------------------------------------------------------------- */
/// Définit la palette de couleurs qui sera utilisée pour la planete.
/// @param renderer L'image à coloriser
/// @param PlanetType Le type de palette à choisir en fonction du type de planete.
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Venusian::renderColors(RendererImage& renderer, int PlanetType)
{
	renderer.ClearGradient ();

	// Palette Musgrave pour des planètes rocheuses lissée.
	String GradientFile = "../media/materials/gradients/venus_1.svg"; // VENUSIAN

	if (PlanetType == VENUSIAN_2)
	{
		// Palette Musgrave pour des planètes rocheuses en terraces.
		GradientFile = "../media/materials/gradients/venus_2.svg";
	}

	this->openGradientFile(GradientFile);
	while (this->addGradient(renderer)) {}
	this->closeGradientFile();
}