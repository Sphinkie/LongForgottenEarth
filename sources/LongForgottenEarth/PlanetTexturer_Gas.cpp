/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer_Gas.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "PlanetTexturer_Gas.h"

#include "macros.h"


/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Gas::PlanetTexturer_Gas()
{
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer_Gas::~PlanetTexturer_Gas()
{
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete gazeuse.
/** 
La fonction choisit la texture la plus appropriée au type de planète.
	La palette doit être structurée ainsi: 
	* -1.0 ... -0.8 les petits tourbillons
	* -0.8 ... +0.8 le fond
	* +0.8 ... +1.0 les gros tourbillons
La fonction va génerer une colormap, et une normalmap (plane).
@param BitmapName Nom du fichier image qui sera généré (sans extension). 
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Gas::createTexture(String BitmapName)
{
	switch(mPlanetType) 
	{
	case GAZ_BROWN:
		this->createTexture_Jovian(BitmapName);
		break;
	case GAZ_BLUE:
		this->createTexture_Neptunian(BitmapName);
		break;
	case GAZ_RED:
		this->createTexture_Neptunian(BitmapName); /// TODO
		break;
	}

/*
	// On genere la normalmap toute plate.
	NoiseMap noiseMap;
	module::Const flatmap;
	flatmap.SetConstValue(0);
	this->buildSphericalTexture(flatmap, noiseMap);
	this->generateNormalMap(noiseMap,BitmapName+"_NM.bmp");
*/
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete gazeuse, de type Jupiter.
/** 
@param BitmapName Nom du fichier image qui sera généré (sans extension). 
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Gas::createTexture_Jovian(String BitmapName)
{
	DEBUG_LOG(">> createTexture_Jovian()");

	this->CheckParameters();

	// On genere une texture composée de cylindres
	module::Cylinders backnoise1;
	backnoise1.SetFrequency(2.2);	// 3 // augmente la densité des cylindres

	// On reduit l'amplitude des cylindres à -0.6 .. +0.6
	module::ScaleBias backnoise2;
	backnoise2.SetSourceModule(0,backnoise1);
	backnoise2.SetScale(0.6);	

	// On donne un effet cotoneux, (blur)
	module::Turbulence backnoise3;
	backnoise3.SetSourceModule (0, backnoise2);
	backnoise3.SetSeed (mSeed+2);
	backnoise3.SetFrequency (200);	// 6*8=48
	backnoise3.SetPower (0.002);		// 1/48=0.02
//	backnoise3.SetRoughness (2);

	// On genere des nuages 
	module::Billow clouds;
	clouds.SetSeed (mSeed+3);
	clouds.SetOctaveCount (8);
	clouds.SetFrequency (1);
	clouds.SetPersistence(0.25);
//	clouds.SetPersistence (0.375);
//	clouds.SetLacunarity (2.12109375);
//	clouds.SetNoiseQuality (QUALITY_BEST);

	// On extrait les gros tourbillons (zones hautes)
	// Ces gros tourbillons auront des couleurs foncées, et sont peu nombreux

	// On donne un effet de tourbillon (arrondi et adoucit) 
	// On cree des tourbillons plus petits et plus arrondis.
	// note: Ca ne marche pas à tous les coups: on peut s'en passer
//	module::Turbulence stormsBig;
//	stormsBig.SetSourceModule (0, clouds);
//	stormsBig.SetSeed (mSeed+4);
//	stormsBig.SetFrequency (0.50);	
//	stormsBig.SetPower (0.70);
//	stormsBig.SetRoughness (1);

	module::ScaleBias stormsBig2;
	stormsBig2.SetSourceModule (0, clouds);	// stormsBig);
//	stormsBig2.SetScale(0.9);
	stormsBig2.SetBias(-0.25);

	// On extrait les petits tourbillons (zones basses)
	// Ces petits tourbillons auront des couleurs claires, et sont plus nombreux
	// On donne un effet d'étirement, et de déchirement 
/// Note: on peut aussi faire ces nuages dans la seconde couche des nuages ! (cf eaxmples)
	module::Turbulence stormsSmall;
	stormsSmall.SetSourceModule (0, backnoise1); 
	stormsSmall.SetSeed (mSeed+5);
	stormsSmall.SetFrequency (3);
	stormsSmall.SetPower (0.15);
//	stormsSmall.SetRoughness (5);

	// On élargit un peu les petits tourbillons
	module::Exponent stormsSmall2;
	stormsSmall2.SetSourceModule (0, stormsSmall); 
	stormsSmall2.SetExponent(1.6);	


	// On combine les gros tourbillons avec le fond
	module::Max combinedNoise1;
	combinedNoise1.SetSourceModule (0, backnoise3);
	combinedNoise1.SetSourceModule (1, stormsBig2);

	// On combine les petits tourbillons avec le fond
	// Module Select: dans le range, on selectionne le source-module 1.
/// TODO = Ajuster la bonne combinaison......
/// TODO = ajuster le gradient (foncer le mauve)
	module::Select combinedNoise2;
	combinedNoise2.SetSourceModule (0, combinedNoise1);
	combinedNoise2.SetSourceModule (1, stormsSmall2);
	combinedNoise2.SetControlModule(stormsSmall2);
	combinedNoise2.SetBounds(-2.0, -0.80);
	combinedNoise2.SetEdgeFalloff(0.3);

	// On remplit la noise-map avec cette texture et on génère la bitmap
	NoiseMap noiseMap;
	this->buildSphericalTexture(backnoise3, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_1.bmp", 7, false);

	this->buildSphericalTexture(clouds, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_2.bmp", 7, false);

//	this->buildSphericalTexture(stormsBig, noiseMap);
//	this->generateColormap (noiseMap,"gaz_step_3.bmp", 7, false);

	this->buildSphericalTexture(stormsBig2, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_3b.bmp", 7, false);

	this->buildSphericalTexture(combinedNoise1, noiseMap);
	this->generateColormap (noiseMap, "gaz_step_3c.bmp", 7, false);

	this->buildSphericalTexture(stormsSmall, noiseMap);	
	this->generateColormap (noiseMap,"gaz_step_4.bmp", 7, false);

	this->buildSphericalTexture(stormsSmall2, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_4b.bmp", 7, false);

	this->buildSphericalTexture(combinedNoise2, noiseMap);
	this->generateColormap (noiseMap, "gaz_step_4c.bmp", 7, false);
	this->generateColormap (noiseMap, "gaz_step_3d.bmp", 7, false);
	
	this->generateColormap (noiseMap, BitmapName+".bmp", 7, false);
	this->generateColormap (noiseMap, "gaz_step_5-Jup.bmp", 8, false);




	/************************************************************************/
	/* variations  sur les effets de turbulences                            */
	/************************************************************************/
	Real f=0.01;
	Real p=0.01;
	int  k =0;
	for (k=0; k<0; k++, p+=0.1)
	{
		// On donne un effet de tourbillon
		module::Turbulence stormsV;
		stormsV.SetSourceModule (0, stormsSmall);
		stormsV.SetSeed (mSeed+5);
		stormsV.SetFrequency (f);
		stormsV.SetPower (p);		
		stormsV.SetRoughness (1);
		module::Select combinedNoiseV;
		combinedNoiseV.SetSourceModule (0, combinedNoise1);
		combinedNoiseV.SetSourceModule (1, stormsV);
		combinedNoiseV.SetControlModule(stormsV);
		combinedNoiseV.SetBounds(-2.0, -0.8);
		combinedNoiseV.SetEdgeFalloff(3);
		this->buildSphericalTexture(combinedNoiseV, noiseMap);
		this->generateColormap (noiseMap, "var-F"+StringConverter::toString(f,2)+"-P"+StringConverter::toString(p,2)+".bmp", 7, false);
	}

/*	f=5.1;
	p=0.01;
	for (k=0; k<200; k++, p*=0.09)
	{
		// On donne un effet de tourbillon
		module::Turbulence storms3;
		storms3.SetSourceModule (0, storms2);
		storms3.SetSeed (2*mSeed+2);
		storms3.SetFrequency (f);	
		storms3.SetPower (p);		// init= 1.25  good  xx ..10
		storms3.SetRoughness (2);
		module::Max combinedNoiseV;
		combinedNoiseV.SetSourceModule (0, backnoise3);
		combinedNoiseV.SetSourceModule (1, storms3);
		this->buildSphericalTexture(combinedNoiseV, noiseMap);
		this->generateColormap (noiseMap, "gaz_var-F"+StringConverter::toString(f,2)+"-P"+StringConverter::toString(p,2)+".bmp", 7, false);
	}
*/
	DEBUG_LOG("<< createTexture_Jovian");
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete gazeuse, de type Neptune.
/** 
@param BitmapName Nom du fichier image qui sera généré (sans extension). 
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Gas::createTexture_Neptunian(String BitmapName)
{
	DEBUG_LOG(">> createTexture_Neptunian");

	this->CheckParameters();

	// On genere une texture composée de cylindres
	module::Cylinders backnoise0;
	backnoise0.SetFrequency(3);	// 2.2 // augmente la densité des cylindres

	// On genere une texture composée de cylindres
	module::Cylinders backnoise1;
	backnoise1.SetFrequency(1);	// fable densité de cylindre

	// On reduit l'amplitude des cylindres à -0.6 .. +0.6
//	module::ScaleBias backnoise2;
//	backnoise2.SetSourceModule(0,backnoise1);
//	backnoise2.SetScale(0.6);	
	module::Multiply backnoise;
	backnoise.SetSourceModule(0,backnoise0);
	backnoise.SetSourceModule(1,backnoise1);
//	backnoise.SetScale(0.6);	

	// On donne un effet cotoneux, (blur)
	module::Turbulence backnoise3;
	backnoise3.SetSourceModule (0, backnoise);
	backnoise3.SetSeed (mSeed+2);
	backnoise3.SetFrequency (200);	// 6*8=48
	backnoise3.SetPower (0.002);		// 1/48=0.02
//	backnoise3.SetRoughness (2);

	// On genere des nuages 
	module::Billow clouds;
	clouds.SetSeed (mSeed+3);
	clouds.SetOctaveCount (8);
	clouds.SetFrequency (1);
	clouds.SetPersistence(0.25);
//	clouds.SetPersistence (0.375);
//	clouds.SetLacunarity (2.12109375);
//	clouds.SetNoiseQuality (QUALITY_BEST);

	// On extrait les gros tourbillons (zones hautes)
	// Ces gros tourbillons auront des couleurs foncées, et sont peu nombreux

	// On donne un effet de tourbillon (arrondi et adoucit) 
	// On cree des tourbillons plus petits et plus arrondis.
	// note: Ca ne marche pas à tous les coups: on peut s'en passer
//	module::Turbulence stormsBig;
//	stormsBig.SetSourceModule (0, clouds);
//	stormsBig.SetSeed (mSeed+4);
//	stormsBig.SetFrequency (0.50);	
//	stormsBig.SetPower (0.70);
//	stormsBig.SetRoughness (1);

	module::ScaleBias stormsBig2;
	stormsBig2.SetSourceModule (0, clouds);	// stormsBig);
//	stormsBig2.SetScale(0.9);
	stormsBig2.SetBias(-0.25);

	// On extrait les petits tourbillons (zones basses)
	// Ces petits tourbillons auront des couleurs claires, et sont plus nombreux
	// On donne un effet d'étirement, et de déchirement 
/// Note: on peut aussi faire ces nuages dans la seconde couche des nuages ! (cf eaxmples)
	module::Turbulence stormsSmall;
	stormsSmall.SetSourceModule (0, backnoise1); 
	stormsSmall.SetSeed (mSeed+5);
	stormsSmall.SetFrequency (3);
	stormsSmall.SetPower (0.15);
//	stormsSmall.SetRoughness (5);

	// On élargit un peu les petits tourbillons
	module::Exponent stormsSmall2;
	stormsSmall2.SetSourceModule (0, stormsSmall); 
	stormsSmall2.SetExponent(1.6);	


	// On combine les gros tourbillons avec le fond
	module::Max combinedNoise1;
	combinedNoise1.SetSourceModule (0, backnoise3);
	combinedNoise1.SetSourceModule (1, stormsBig2);

	// On combine les petits tourbillons avec le fond
	// Module Select: dans le range, on selectionne le source-module 1.
/// TODO = Ajuster la bonne combinaison......
/// TODO = ajuster le gradient (foncer le mauve)
	module::Select combinedNoise2;
	combinedNoise2.SetSourceModule (0, combinedNoise1);
	combinedNoise2.SetSourceModule (1, stormsSmall2);
	combinedNoise2.SetControlModule(stormsSmall2);
	combinedNoise2.SetBounds(-2.0, -0.80);
	combinedNoise2.SetEdgeFalloff(0.3);

	// On remplit la noise-map avec cette texture et on génère la bitmap
	NoiseMap noiseMap;
	this->buildSphericalTexture(backnoise3, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_1.bmp", 7, false);

	this->buildSphericalTexture(clouds, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_2.bmp", 7, false);

//	this->buildSphericalTexture(stormsBig, noiseMap);
//	this->generateColormap (noiseMap,"gaz_step_3.bmp", 7, false);

	this->buildSphericalTexture(stormsBig2, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_3b.bmp", 7, false);

	this->buildSphericalTexture(combinedNoise1, noiseMap);
	this->generateColormap (noiseMap, "gaz_step_3c.bmp", 7, false);

	this->buildSphericalTexture(stormsSmall, noiseMap);	
	this->generateColormap (noiseMap,"gaz_step_4.bmp", 7, false);

	this->buildSphericalTexture(stormsSmall2, noiseMap);
	this->generateColormap (noiseMap,"gaz_step_4b.bmp", 7, false);

	this->buildSphericalTexture(combinedNoise2, noiseMap);
	this->generateColormap (noiseMap, "gaz_step_4c.bmp", 7, false);
	this->generateColormap (noiseMap, "gaz_step_3d.bmp", 7, false);
	
	this->generateColormap (noiseMap, BitmapName+".bmp", 7, false);
	this->generateColormap (noiseMap, "gaz_step_5-Jup.bmp", 8, false);


	/************************************************************************/
	/* variations  sur les effets de turbulences                            */
	/************************************************************************/
	Real f=0.01;
	Real p=0.01;
	int  k =0;
	for (k=0; k<0; k++, p+=0.1)
	{
		// On donne un effet de tourbillon
		module::Turbulence stormsV;
		stormsV.SetSourceModule (0, stormsSmall);
		stormsV.SetSeed (mSeed+5);
		stormsV.SetFrequency (f);
		stormsV.SetPower (p);		
		stormsV.SetRoughness (1);
		module::Select combinedNoiseV;
		combinedNoiseV.SetSourceModule (0, combinedNoise1);
		combinedNoiseV.SetSourceModule (1, stormsV);
		combinedNoiseV.SetControlModule(stormsV);
		combinedNoiseV.SetBounds(-2.0, -0.8);
		combinedNoiseV.SetEdgeFalloff(3);
		this->buildSphericalTexture(combinedNoiseV, noiseMap);
		this->generateColormap (noiseMap, "var-F"+StringConverter::toString(f,2)+"-P"+StringConverter::toString(p,2)+".bmp", 7, false);
	}

/*	f=5.1;
	p=0.01;
	for (k=0; k<200; k++, p*=0.09)
	{
		// On donne un effet de tourbillon
		module::Turbulence storms3;
		storms3.SetSourceModule (0, storms2);
		storms3.SetSeed (2*mSeed+2);
		storms3.SetFrequency (f);	
		storms3.SetPower (p);		// init= 1.25  good  xx ..10
		storms3.SetRoughness (2);
		module::Max combinedNoiseV;
		combinedNoiseV.SetSourceModule (0, backnoise3);
		combinedNoiseV.SetSourceModule (1, storms3);
		this->buildSphericalTexture(combinedNoiseV, noiseMap);
		this->generateColormap (noiseMap, "gaz_var-F"+StringConverter::toString(f,2)+"-P"+StringConverter::toString(p,2)+".bmp", 7, false);
	}
*/
	DEBUG_LOG("<< createTexture_Neptunian()");
}



/* ------------------------------------------------------------------------- */
/// Définit la palette de couleurs qui sera utilisée pour la planete.
/// @param renderer L'image à coloriser
/// @param PlanetType Le type de palette à choisir en fonction du type de planete (tempérée, rocheuse, etc)
/* ------------------------------------------------------------------------- */
void PlanetTexturer_Gas::renderColors(RendererImage& renderer, int PlanetType)
{
	String GradientFile = "../media/materials/gradients/temperate_1.svg"; // texture par defaut

	renderer.ClearGradient ();

	switch(PlanetType) 
	{
	case GAZ_RED:
		// Palette pour les planetes de type Naines Gazeuses 
		GradientFile = "../media/materials/gradients/gaz_red.svg";
		break;
	case GAZ_BLUE:
		// Palette pour les planetes de type Neptune
		GradientFile = "../media/materials/gradients/gaz_blue.svg";
		break;
	case GAZ_BROWN:
		// Palette pour les planetes de type Jupiter
		GradientFile = "../media/materials/gradients/gaz_brown.svg";
		break;
	}
	this->openGradientFile(GradientFile);
	while (this->addGradient(renderer)) {}
	this->closeGradientFile();
}
