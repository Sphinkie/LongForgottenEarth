/* ------------------------------------------------------------------------- */
// File       : PlanetTexturer.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "PlanetTexturer.h"

#include "macros.h"


/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer::PlanetTexturer()
{
	// Dimensions de la Map (en pixels)
	GRID_WIDTH  = 1024;	// 512;	// 1024;
	GRID_HEIGHT = 512;	// 256;	// 512;

	mSeed       = 0;
	mPlanetType = DEFAULT_PLANET;

	// D�finition du niveau de la mer (entre -1 et +1)
	mSeaLevel = 0.0;

	// Par defaut, on utilise une lumi�re blanche.
	mLightColor = Color(255, 255, 255, 255);

	mResInMeters = 0.1;	// Default value for planetmaps.
	mSVGfile=NULL;
	mIndex  = 0;
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
PlanetTexturer::~PlanetTexturer()
{
}


/* ------------------------------------------------------------------------- */
/// Creation d'une texture de planete avec un d�coupage en continents simple.
/** @param BitmapName Nom du fichier BMP qui sera g�n�r�. */
/* ------------------------------------------------------------------------- */
void PlanetTexturer::createTexture(String BitmapName)
{
	DEBUG_LOG(">> createTexture()");

	this->CheckParameters();

	// On genere une texture bruit�e basique
	module::Perlin basicNoiseModule;
	basicNoiseModule.SetSeed(mSeed);

	// On remplit la noise-map avec cette texture.
  	NoiseMap noiseMap;
	this->buildSphericalTexture(basicNoiseModule, noiseMap);

	// On g�n�re la bitmap
	this->generateColormap (noiseMap, BitmapName+".bmp", mPlanetType, true);
	this->generateSpecMap  (noiseMap, BitmapName+"_SP.bmp");
 	this->flattenSea(noiseMap);
	this->generateBumpMap  (noiseMap, BitmapName+"_BM.bmp");
	this->generateNormalMap(noiseMap, BitmapName+"_NM.bmp");

	DEBUG_LOG("<< createTexture()");
}


/* ------------------------------------------------------------------------- */
/// Fonction de reference directement issue du tutorial. G�n�re une texture de base de type planete.
/* ------------------------------------------------------------------------- */
void PlanetTexturer::createReferencePlanet()
{
	module::Perlin myModule;

	NoiseMap heightMap;

	NoiseMapBuilderSphere heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (1024, 512);
	heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.Build ();

	RendererImage renderer;
	NoiseImage image;

	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.ClearGradient ();
	renderer.AddGradientPoint (-1.0000, Color (  0,   0, 128, 255)); // deeps
	renderer.AddGradientPoint (-0.2500, Color (  0,   0, 255, 255)); // shallow
	renderer.AddGradientPoint ( 0.0000, Color (  0, 128, 255, 255)); // shore
	renderer.AddGradientPoint ( 0.0625, Color (240, 240,  64, 255)); // sand
	renderer.AddGradientPoint ( 0.1250, Color ( 32, 160,   0, 255)); // grass
	renderer.AddGradientPoint ( 0.3750, Color (224, 224,   0, 255)); // dirt
	renderer.AddGradientPoint ( 0.7500, Color (128, 128, 128, 255)); // rock
	renderer.AddGradientPoint ( 1.0000, Color (255, 255, 255, 255)); // snow
	renderer.EnableLight ();
	renderer.SetLightContrast (3.0);
	renderer.SetLightBrightness (2.0);
	renderer.Render ();

	WriterBMP writer;
	writer.SetSourceImage (image);
	writer.SetDestFilename ("planet_reference.bmp"); 
	writer.WriteDestFile ();
}


/* ------------------------------------------------------------------------- */
/// D�finition de la couleur de la lumi�re qui sera appliqu�e sur la planete (cad la lumi�re du soleil).
/** @param couleur La couleur de la lumi�re qui �claire la plan�te.
On convertit la couleur du format Ogre au format libnoise. */
/* ------------------------------------------------------------------------- */
void PlanetTexturer::setLightColor(Ogre::ColourValue couleur)
{
	mLightColor = Color(255.0*couleur.r, 255.0*couleur.g, 255.0*couleur.b, 255);
}

/* ------------------------------------------------------------------------- */
/// D�finition de la couleur de la lumi�re qui sera appliqu�e sur la planete (cad la lumi�re du soleil).
/** @param couleur La couleur a m�moriser. */
/* ------------------------------------------------------------------------- */
void PlanetTexturer::setPlanetType(int PlanetType)
{
	mPlanetType = PlanetType;
}

/* ------------------------------------------------------------------------- */
/// Initialise le moteur al�atoire, pour le relief de la planete.
/// @param seed Planet seed. A changer pour g�n�rer une autre planete.
/* ------------------------------------------------------------------------- */
void PlanetTexturer::setSeed(int seed)
{
	mSeed = seed;
}


/* ------------------------------------------------------------------------- */
/// On applique une lumi�re artificielle sur la map.
/** @param renderer L'image � modifier. */
/* ------------------------------------------------------------------------- */
void PlanetTexturer::renderLight(RendererImage& renderer)
{
	renderer.EnableLight (true);
	renderer.SetLightElev (50.0);
	renderer.SetLightContrast (3);
	renderer.SetLightBrightness (2);
	renderer.SetLightColor(mLightColor);
}


/* ------------------------------------------------------------------------- */
/// On applique une lumi�re artificielle sur un paysage.
/** 
	@param renderer L'image � modifier. Celle doit avoir �t� convertie en m�tres.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::renderLandscapeLight(RendererImage& renderer)
{
	renderer.SetLightAzimuth (135.0);
	renderer.SetLightElev (40.0);
	renderer.SetLightContrast (1/mResInMeters);
	renderer.SetLightIntensity(2);
	renderer.SetLightColor(mLightColor);
}


/* ------------------------------------------------------------------------- */
/// D�finit la palette de couleurs qui sera utilis�e pour la planete.
/// @param renderer L'image � coloriser
/// @param PlanetType Le type de palette � choisir en fonction du type de planete (temp�r�e, rocheuse, etc)
/* ------------------------------------------------------------------------- */
void PlanetTexturer::renderColors(RendererImage& renderer, int PlanetType)
{
	String GradientFile = "../media/materials/gradients/temperate_1.svg"; // texture par defaut

	renderer.ClearGradient ();

	switch(PlanetType) 
	{
	case GRANIT:
		// Palette Noire et Rose, caract�ristique du granit.
		GradientFile = "../media/materials/gradients/granit.svg";
		break;
	case TEMPERATE_1:
		// Palette simple pour des planetes temp�r�es (tutorial).
		GradientFile = "../media/materials/gradients/temperate_1.svg";
		break;
	case TEMPERATE_2:
		// Palette am�lior�e pour des plan�tes temp�r�es.
		GradientFile = "../media/materials/gradients/temperate_2.svg";
		break;
	case TERRESTRIAL:
		// texture am�lior�e alternative
		GradientFile = "../media/materials/gradients/temperate_3.svg";
		break;
	case TERRESTRIAL_2:
		// Palette Musgrave pour des plan�tes temp�r�es.
		GradientFile = "../media/materials/gradients/earth_musgrave.svg";
		break;
	case ROCK:
		// Palette pour des plan�tes rocheuses de type Mercure.
		GradientFile = "../media/materials/gradients/rock.svg";
		break;
	case ROCK_2:
		// Palette pour des plan�tes rocheuses de type Pluton.
		GradientFile = "../media/materials/gradients/rock_2.svg";
		break;
	}
	this->openGradientFile(GradientFile);
	while (this->addGradient(renderer)) {}
	this->closeGradientFile();
}


/* ------------------------------------------------------------------------- */
/// A partir d'un noise-module, cette fonction va remplir une noise-map plane.
/** 
	Il faut avoir GRID_WIDTH = GRID_HEIGHT, pour obtenir une texture carr�e.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::buildPlanarTexture (const module::Module& noiseModule, bool seamless, NoiseMap& noiseMap)
{
	NoiseMapBuilderPlane plane;
	plane.SetBounds (-1.0, 1.0, -1.0, 1.0);
	plane.SetDestSize (GRID_WIDTH, GRID_HEIGHT);
	plane.SetSourceModule (noiseModule);
	plane.SetDestNoiseMap (noiseMap);
	plane.EnableSeamless (seamless);
	plane.Build ();
}


/* ------------------------------------------------------------------------- */
/// A partir d'un noise-module, cette fonction va remplir une noise-map spherique.
/** 
	Il faut avoir GRID_WIDTH = GRID_HEIGHT *2.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::buildSphericalTexture(const module::Module& noiseModule, NoiseMap& noiseMap)
{
	NoiseMapBuilderSphere sphere;
	sphere.SetBounds(-90.0, 90.0, -180.0, 180.0); // degrees
	sphere.SetDestSize(GRID_WIDTH, GRID_HEIGHT);
	sphere.SetSourceModule(noiseModule);
	sphere.SetDestNoiseMap(noiseMap);
	sphere.Build();
}


/* ------------------------------------------------------------------------- */
/// A partir de la noise-map, on g�n�re un fichier BMP qui pourra �tre utilis� comme texture.
/* ------------------------------------------------------------------------- */
void PlanetTexturer::generateColormap(const NoiseMap& noiseMap, String filename, int PlanetType ,bool light)
{
	RendererImage renderer;
	NoiseImage    destImage;
	renderer.SetSourceNoiseMap (noiseMap);
	renderer.SetDestImage (destImage);

	// On applique des couleurs � la texture
	this->renderColors(renderer, PlanetType);

	// Gestion d'une lumi�re artificielle
//	renderer.EnableLight(light);
	if (light) this->renderLight(renderer);

	// On g�n�re la texture.
	renderer.Render();

	// On g�n�re le fichier BMP
	WriterBMP bitmapWriter;
	bitmapWriter.SetSourceImage (destImage);
	bitmapWriter.SetDestFilename (filename.c_str());
	bitmapWriter.WriteDestFile ();
}


/* ------------------------------------------------------------------------- */
/// Generation de la 'specularity map'.                                        
/** Cette Map d�finit la brillance de la planete. 
	Le noir indique une zone mate, sans reflets. Le blanc indique une zone tr�s brillante.
	Les mers sont les plus brillantes, ainsi que les zones �lev�es en raison de la neige. 
	@param noiseMap Ref�rence de la noise Map � analyser.
	@param filename Nom du fichier BMP qui sera g�n�r�.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::generateSpecMap(const NoiseMap& noiseMap, String filename)
{
	DEBUG_LOG(">> generateSpecmap()");

	RendererImage renderer;
	NoiseImage destImage;

	renderer.SetSourceNoiseMap (noiseMap);
	renderer.SetDestImage (destImage);

	// On applique la couleur (des niveaux de gris)
	renderer.ClearGradient ();
	renderer.AddGradientPoint (-1.0              , Color (255, 255, 255, 255));  // Mers    = blanc
	renderer.AddGradientPoint (mSeaLevel         , Color (255, 255, 255, 255));  // Mers    = blanc
	renderer.AddGradientPoint (mSeaLevel + 0.0001, Color (  0,   0,   0, 255));  // rivage  = noir
	renderer.AddGradientPoint (+1.0              , Color (128, 128, 128, 255));  // Sommets = gris clair

	// Pas de lumi�re artificielle
	renderer.EnableLight (false);

	// On genere l'image.
	renderer.Render ();

	// On g�n�re le fichier BMP.
	WriterBMP bitmapWriter;
	bitmapWriter.SetSourceImage (destImage);
	bitmapWriter.SetDestFilename (filename.c_str()); 
	bitmapWriter.WriteDestFile ();
}


/* ------------------------------------------------------------------------- */
/// Generation de la 'Bump map'.                                        
/** Cette Map retranscrit directement l'�l�vation de la planete. 
 Les zones basses ont en noir, et les zones �lev�es en blanc.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::generateBumpMap(const NoiseMap& noiseMap, String filename)
{
	DEBUG_LOG(">> generateBumpMap()");

	RendererImage renderer;
	NoiseImage destImage;

	renderer.SetSourceNoiseMap (noiseMap);
	renderer.SetDestImage (destImage);

	// On applique la couleur (des niveaux de gris)
	renderer.ClearGradient ();
	renderer.BuildGrayscaleGradient();

	// Pas de lumi�re artificielle
	renderer.EnableLight (false);

	// On genere l'image.
	renderer.Render ();

	// On g�n�re le fichier BMP.
	WriterBMP bitmapWriter;
	bitmapWriter.SetSourceImage (destImage);
	bitmapWriter.SetDestFilename (filename.c_str()); 
	bitmapWriter.WriteDestFile ();
}


/* ------------------------------------------------------------------------- */
/// Generation de la 'Raw map'.                                        
/** A raw terrain (*.raw) file has its elevation points measured in meters.  
 This terrain file contains 16-bit signed big-endian values, in row-major order, 
 ordered south to north.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::generateRawMap(NoiseMap& noiseMap, String filename)
{
	int x, y;
	
	std::ofstream os;
	noise::uint8* pLineBuffer = new noise::uint8[GRID_WIDTH * 2];

	os.open (filename.c_str(), std::ios::out | std::ios::binary);
	for (y = 0; y < GRID_HEIGHT; y++) 
	{
		float* pSource = noiseMap.GetSlabPtr (y);
		noise::uint8* pDest = pLineBuffer;
		for (x = 0; x < GRID_WIDTH; x++) 
		{
			int16 elev = (int16)(floor (*pSource));
			*pDest++ = (noise::uint8)(((noise::uint16)elev & 0xff00) >> 8);
			*pDest++ = (noise::uint8)(((noise::uint16)elev & 0x00ff)     );
			++pSource;
		}
		os.write ((char*)pLineBuffer, GRID_WIDTH * 2);
	}
	os.close ();
	delete[] pLineBuffer;
}


/* ------------------------------------------------------------------------- */
/// Generate a Terragen file (elevation grid)                            
/** A Terragen terrain (*.ter) file that has its elevation points measured in meters.  
 This file is generated only if the grid of elevation points has a spatial resolution <= 240 meters.
 The grid of elevation points can have a maximum horizontal resolution of
 0.25 arcseconds (~7.5 meters.)  A higher-resolution grid will cause the
 terrain to appear blurry.
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::generateTerragenMap(NoiseMap& noiseMap, String filename)
{
	if (mResInMeters < 240) 
	{
		WriterTER terrainWriter;
		terrainWriter.SetSourceNoiseMap (noiseMap);
		terrainWriter.SetDestFilename   (filename.c_str());
		terrainWriter.SetMetersPerPoint (mResInMeters);
		terrainWriter.WriteDestFile ();
		TRACE("Terragen Map "+filename+" done with a resolution of "+ StringConverter::toString((Real)mResInMeters) +" meters.");
	}
	else
	{
		TRACE("*** WARNING: generateTerragenMap("+filename+"): mResInMeters is too high for generating a TER file.");
		TRACE("*** WARNING: "+ StringConverter::toString((Real)mResInMeters) +" is higher than 240 ");
	}
}


/* ------------------------------------------------------------------------- */
/// Generation de la 'Normal map'.                                        
/** Cette Map retranscrit le relief de la planete, sous forme de vecteurs RGB.
  Il faut utiliser cette normal map avec une colormap (sans lumi�re artificielle), 
  et un moteur3D (tel que Ogre), pour obtenir des lumi�res r�alistes en temps r�el
  de la plan�te.
  Le relief est repr�sent� par les vecteur "normal" en chaque point de la map.
  R = composante X du vecteur. (0..255)
  G = composante Y du vecteur. (0..255)
  B = composante Z du vecteur. (0..255)
*/
/* ------------------------------------------------------------------------- */
void PlanetTexturer::generateNormalMap(NoiseMap& noiseMap, String filename)
{
	DEBUG_LOG(">> generateNormalMap()");

	RendererNormalMap renderer;
	NoiseImage destImage;

	renderer.SetSourceNoiseMap (noiseMap);
	renderer.SetDestImage (destImage);

	// On genere l'image.
	renderer.SetBumpHeight (1.0 / mResInMeters);	// Pour les paysages. Pour les maps, il faut environ 10.
	renderer.Render ();

	// On g�n�re le fichier BMP.
	WriterBMP bitmapWriter;
	bitmapWriter.SetSourceImage (destImage);
	bitmapWriter.SetDestFilename (filename.c_str()); 
	bitmapWriter.WriteDestFile ();
}

/* ------------------------------------------------------------------------- */
/// Transforme un niveau (-1..+1) en tenant compte du niveau de la mer.
/** 
	Convertit une Altitude (-1..+1) en hauteur (-1..+1).
	si x = 0, on renvoie SEA_LEVEL
	si x est dans [-1..0], on renvoie une valeur dans [-1..SEA_LEVEL]
	si x est dans [0..+1], on renvoie une valeur dans [SEA_LEVEL..+1]
	@param x Altitude � convertir (en coordonn�es planetaire -1..+1). La mer est en 0.
	@return Hauteur (en coordonn�es planetaire -1..+1). La mer est en SEA_LEVEL.
/* ------------------------------------------------------------------------- */
Real PlanetTexturer::altitude(Real x)
{
	// Si le niveau de la mer est 0, il n'y a pas besoin de correction d'altitude.
	if (mSeaLevel==0) return x;

	if (x>0) return (mSeaLevel + x*(1-mSeaLevel));
	else     return (mSeaLevel - x*(1+mSeaLevel));
}



/* ------------------------------------------------------------------------- */
/// Transforme un offset SVG en tenant compte du niveau de la mer.
/** 
	Convertit un offset (0..+1) en hauteur (-1..+1).
	si x = 0.5, on renvoie SEA_LEVEL
	si x est dans [0.0...0.5], on renvoie une valeur dans [-1..SEA_LEVEL]
	si x est dans [0.5...1.0], on renvoie une valeur dans [SEA_LEVEL..+1]
	@param x Offset � convertir (en coordonn�es 0...+1). La mer est en 0.5.
	@return Hauteur (en coordonn�es planetaire -1..+1). La mer est en SEA_LEVEL.
/* ------------------------------------------------------------------------- */
Real PlanetTexturer::toHauteur(Real x)
{
	Real H;

	if (x>0.5) H = 2*x*(1-mSeaLevel) - 1 + 2*mSeaLevel;
	else       H = 2*x*(1+mSeaLevel) - 1;

	return H;
}


/* ------------------------------------------------------------------------- */
/// Transforme la noiseMap en metres (au lieu de -1..+1).  
/** 
	@param noiseMap La noiseMap a modifier.
	@param minAltitude   L'�l�vation minimale de la plan�te en m�tres. (La plus profonde faille).
	@param maxAltitude   L'�l�vation maximale de la plan�te en m�tres. (La plus haute montagne).
	@param Circonference La circonference de la plan�te en m�tres. 
*/                                      
/* ------------------------------------------------------------------------- */
void PlanetTexturer::scalePlanetInMeters(NoiseMap& noiseMap, Real minAltitude, Real maxAltitude, Real Circonference)
{
	int x, y;

	double PLANET_CIRCUMFERENCE = Circonference;               // 44236800.0;
	double MIN_ELEV = minAltitude;	
	double MAX_ELEV = maxAltitude;  

	if (minAltitude > maxAltitude) 
	{
		
		TRACE (">> scalePlanetInMeters()");
		TRACE ("*** WARNING : unable to proceed: wrong min/max altitudes. ");
		return;
	}

	/// on convertit toute la map en m�tres en transformant de -1..+1 � MIN_ELEV..MAX_ELEV
	// nb: le calcul est un peu faux...
	for (y = 0; y < GRID_HEIGHT; y++) 
	{
		float* pCur = noiseMap.GetSlabPtr (y);
		for (x = 0; x < GRID_WIDTH; x++) 
		{
			if (*pCur > 0)    *pCur = (*pCur)*MAX_ELEV + mSeaLevel;
			else              *pCur = (*pCur)*MIN_ELEV + mSeaLevel;
		++pCur;
		}
	}
	/// On calcule le niveau de la mer (en metres)
	double seaLevelInMeters = (((mSeaLevel + 1.0) / 2.0) * (MAX_ELEV - MIN_ELEV)) + MIN_ELEV;

	/// On calcule la r�solution de la Map (en metres).
	// Assume that the spatial resolution is the same in both the x and y
	// directions.  This is needed by the Terragen file writer.
	double degExtent  = 360; // Largeur de la map en degr�s
	double gridExtent = (double)GRID_WIDTH;
	double metersPerDegree = (PLANET_CIRCUMFERENCE / 360.0);
	mResInMeters = (degExtent / gridExtent) * metersPerDegree;

}




/* ------------------------------------------------------------------------- */
/// On v�rifie que tous les param�tres sont bien dans les limites pr�vues.
/* ------------------------------------------------------------------------- */
void PlanetTexturer::CheckParameters()
{
//	assert (SOUTH_COORD < NORTH_COORD);
//	assert (WEST_COORD  < EAST_COORD);
	assert (GRID_WIDTH  > 0);
	assert (GRID_HEIGHT > 0);
	assert (PLANET_CIRCUMFERENCE >= 1.0);
	assert (mSeaLevel >= -1.0);
	assert (mSeaLevel <=  1.0);

	TRACE("   All parameters are OK !");
}




/* ------------------------------------------------------------------------- */
/// Lit un fichier gradient au format SVG (Vector Scalable Graphics)
/** 
	@param  SVGfilename Le nom du fichier SVG � lire.
	@return Renvoie TRUE si le node "linearGradient" a bien �t� trouv�.
*/
/* ------------------------------------------------------------------------- */
bool PlanetTexturer::openGradientFile(String SVGfilename)
{
	// On ouvre le fichier XML 
	mSVGfile = new XMLParser_T(SVGfilename);
	if (!mSVGfile->isPresent()) return false;
	
	mIndex = 0;
	// On parcourt le fichier XML, et on se positionne sur la d�finition du Gradient
	return mSVGfile->gotoElementPath("svg/defs/linearGradient");
}


/* ------------------------------------------------------------------------- */
/// Lit un gradient du fichier SVG (Vector Scalable Graphics)
/* ------------------------------------------------------------------------- */
bool PlanetTexturer::addGradient(RendererImage& renderer)
{
	// On se positionne sur l'�l�ment <stop>
	bool found = mSVGfile->gotoSubElement("stop",mIndex++);
	if (!found) return false;

	// On lit la valeur de l'offset
	Real stop_offset = mSVGfile->getRealAttribute("offset");
	Real stop_hauteur= this->toHauteur(stop_offset);  

	String stop_style  = mSVGfile->getStringAttribute("style");
	String stop_item;
	String stop_color;
	String stop_opacity;
	int opacity = 255;
	ColourValue couleur = ColourValue::Black;


	// On analyse le champ <style>
	StringVector style_vector = StringUtil::split(stop_style,";:");
	int style_number = style_vector.size();
	int index = 0;

	// On analyse les items du style:
	while (index<style_number)
	{
		stop_item = style_vector.at(index);
		StringUtil::trim(stop_item);		// On enleve les espaces
		if (stop_item == "stop-color")   stop_color=style_vector.at(index+1);
		if (stop_item == "stop-opacity") stop_opacity=style_vector.at(index+1);
		index++;
	}

	// On extrait l'opacity
//	DEBUG_LOG("   stop_opacity="+stop_opacity);
	if (stop_opacity!="") opacity = StringConverter::parseReal(stop_opacity)*255;

	// On extrait la couleur
//	DEBUG_LOG("   stop_color="+stop_color);
	if (stop_color!="")  
	{
		if (StringUtil::startsWith(stop_color,"rgb")) couleur=this->parseRGB2(stop_color);
		if (StringUtil::startsWith(stop_color,"#"))   couleur=this->parseRGB1(stop_color);
	}
	// on ajoute le gradient
	/// TODO: controler que l'on n'a pas dej� deja un gradient � cette hauteur: sinon plantage!
	renderer.AddGradientPoint (stop_hauteur, Color (couleur.r,couleur.g,couleur.b,opacity));

	return true;
}

/* ------------------------------------------------------------------------- */
/// On libere le parser XML du fichier SVG.
/* ------------------------------------------------------------------------- */
void PlanetTexturer::closeGradientFile()
{
	if (mSVGfile) delete (mSVGfile);
	mSVGfile=NULL;
	mIndex  =0;
}

/* ------------------------------------------------------------------------- */
/// Parse une couleur au format #rrggbb
/* ------------------------------------------------------------------------- */
ColourValue PlanetTexturer::parseRGB1(String colorString)
{
	int R = 16*dec(colorString[1]) + dec(colorString[2]);
	int G = 16*dec(colorString.at(3)) + dec(colorString.at(4));
	int B = 16*dec(colorString.at(5)) + dec(colorString.at(6));

	//DEBUG_LOG("   R="+StringConverter::toString(R));
	//DEBUG_LOG("   G="+StringConverter::toString(G));
	//DEBUG_LOG("   B="+StringConverter::toString(B));

	return ColourValue(R,G,B);
}

/* ------------------------------------------------------------------------- */
/// Parse une couleur au format rgb(R,G,B)
/* ------------------------------------------------------------------------- */
ColourValue PlanetTexturer::parseRGB2(String colorString)
{
	StringVector color_vector = StringUtil::split(colorString,"(,)");

	// Controle 
	if (color_vector.size() < 4) return ColourValue::Black;
	int R = StringConverter::parseInt(color_vector.at(1));
	int G = StringConverter::parseInt(color_vector.at(2));
	int B = StringConverter::parseInt(color_vector.at(3));

	//DEBUG_LOG("   R="+color_vector.at(1));
	//DEBUG_LOG("   G="+color_vector.at(2));
	//DEBUG_LOG("   B="+color_vector.at(3));

	return ColourValue(R,G,B);
}


/* ------------------------------------------------------------------------- */
/// Convertit un caractere ascii hexa en decimal
/** 
	@param C Le caract�re ASCII a convertir ('0'... 'F')
	@return La valeur d�cimale correspondante (0..15).
/* ------------------------------------------------------------------------- */
Ogre::uint8 PlanetTexturer::dec(char C)
{
	if (C<='9') return (C-'0');
	if (C<='F') return (C-'A'+10);
	if (C<='f') return (C-'a'+10);
	return 0;
}


/* ------------------------------------------------------------------------- */
/// Applatit le fond de la mer
/* ------------------------------------------------------------------------- */
void PlanetTexturer::flattenSea(NoiseMap& noiseMap)
{
	DEBUG_LOG(">> flattenSea()");
	// Le fond de la mer est un peut en dessous ni niveau de la mer.
	// On conserve ainsi deux zones � colorer dans la mer: "deep water" et "shallow water"
	int x, y;
	double SEA_BOTTOM = 0.9*(mSeaLevel+1) -1; // le Bottom est 10% en dessous de SEA_LEVEL
	for (y = 0; y < GRID_HEIGHT; y++) 
	{
		float* pCur = noiseMap.GetSlabPtr(y);
		for (x=0; x<GRID_WIDTH; x++) 
		{
			if (*pCur<SEA_BOTTOM) *pCur=SEA_BOTTOM;
			++pCur;
		}
	}
}
