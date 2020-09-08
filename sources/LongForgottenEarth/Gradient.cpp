/* ------------------------------------------------------------------------- */
// File       : Gradient.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Gradient.h"

#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Contructeur.
/** 
	@param  SVGfilename Le nom du fichier Gradient à lire (au format SVG).
*/
/* ------------------------------------------------------------------------- */
Gradient::Gradient(String SVGfilename)
{
	mIndex  = 0;
	mSVGfile=NULL;
	this->openGradientFile("../media/materials/gradients/"+SVGfilename);
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
Gradient::~Gradient()
{
	this->closeGradientFile();
}


/* ------------------------------------------------------------------------- */
/// Lit un fichier gradient au format SVG (Vector Scalable Graphics)
/** 
	@param  SVGfilename Le nom du fichier SVG à lire.
	@return Renvoie TRUE si le node "linearGradient" a bien été trouvé.
*/
/* ------------------------------------------------------------------------- */
bool Gradient::openGradientFile(String SVGfilename)
{
	// On ouvre le fichier XML 
	mSVGfile = new XMLParser_T(SVGfilename);
	if (!mSVGfile->isPresent()) return false;
	
	mIndex = 0;
	// On parcourt le fichier XML, et on se positionne sur la définition du Gradient
	return mSVGfile->gotoElementPath("svg/defs/linearGradient");
}


/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie la couleur associée à une valeur d'Offset quelconque.
/** 
	On réalise une interpolation linéaire entre 2 offsets.
*/
/* ------------------------------------------------------------------------- */
ColourValue Gradient::getColour(Real G_offset)
{
	DEBUG_LOG(">> Gradient::getColour("+StringConverter::toString(G_offset)+")");

	if (G_offset<0) return ColourValue::White;
	if (G_offset>1) return ColourValue::White;

	ColourValue finalColour = ColourValue::White;
	Real current_offset; 
	int index = 0;

	do 
	{
		current_offset = this->getOffset(index++);
	} while ((current_offset<G_offset) && (index<1000));	// securité
	
	// On s'arrete dès que l'on a dépassé ou atteint l'offset demandé.
	ColourValue colour1 = this->getGradient(index-2);
	ColourValue colour2 = this->getGradient(index-1);

	// ici on a 2 cas possibles: 
	// soit on a un gradient qui correspond exactement à l'offset demandé
	if (current_offset == G_offset) return colour1;
	// soit on a dépassé l'offset, et on doit interpoler entre cet offset et le précédent.
	Real offset1 = this->getOffset(index-2);
	Real offset2 = this->getOffset(index-1);
	DEBUG_LOG("  interpoling "+ITOA(G_offset)+" between "+ITOA(offset1)+" and "+ITOA(offset2));
	Real ratio   =	(offset2 - G_offset)/(offset2-offset1);
	finalColour  = colour1 + ratio*(colour2-colour1);
	return finalColour;
}



/* ------------------------------------------------------------------------- */
/// Lit un point "stop" du gradient.
/* ------------------------------------------------------------------------- */
ColourValue Gradient::getNextGradient()
{
	return (this->getGradient(mIndex++));
}


/* ------------------------------------------------------------------------- */
/// Lit un point "stop" du gradient, et renvoie sa couleur.
/** 
	@param stop_n Le No de l'offset à lire
	@return Sa couleur associée.
	@sa getLastOffset Utiliser cette fonction pour récupérer l'offset qui a été lu dans getGradient.
*/
/* ------------------------------------------------------------------------- */
ColourValue Gradient::getGradient(int stop_n)
{
	int opacity = 255;
	ColourValue couleur = ColourValue::Black;

	// On se positionne sur l'élément <stop> du numero demandé.
	bool found = mSVGfile->gotoSubElement("stop",stop_n);
	if (!found) return ColourValue::Black;

	// On affiche l'ID de ce <stop>
	DEBUG_LOG("   "+mSVGfile->getStringAttribute("id"));

	// On lit la valeur de l'offset
	mLastOffset = mSVGfile->getRealAttribute("offset");
	DEBUG_LOG("   mLastOffset="+StringConverter::toString(mLastOffset));


	// On analyse le champ <style>
	String stop_style  = mSVGfile->getStringAttribute("style");
	String stop_item, stop_color, stop_opacity;
	StringVector style_vector = StringUtil::split(stop_style,";:");
	int style_number = style_vector.size();
	int index = 0;

	// On analyse les items du style:
	while (index<style_number)
	{
		stop_item = style_vector.at(index);
		StringUtil::trim(stop_item);		// On enleve les espaces
		DEBUG_LOG("   stop_item="+stop_item);
		if (stop_item == "stop-color")   stop_color=style_vector.at(index+1);
		if (stop_item == "stop-opacity") stop_opacity=style_vector.at(index+1);
		index++;
	}

	// On extrait l'opacity
	DEBUG_LOG("   stop_opacity="+stop_opacity);
	if (stop_opacity!="") opacity = StringConverter::parseReal(stop_opacity)*255;

	// On extrait la couleur
	DEBUG_LOG("   stop_color="+stop_color);
	if (stop_color!="")  
	{
		if (StringUtil::startsWith(stop_color,"rgb")) couleur=this->parseRGB2(stop_color);
		if (StringUtil::startsWith(stop_color,"#"))   couleur=this->parseRGB1(stop_color);
	}

	// On affecte la transparence
	couleur.a = opacity;

	return couleur;
}

/* ------------------------------------------------------------------------- */
/// Lit un point "stop" du gradient, et renvoie son Offset.
/** 
	@param stop_n Le No de l'offset à lire
	@return Son offset associé.
*/
/* ------------------------------------------------------------------------- */
Real Gradient::getOffset(int stop_n)
{
	// On se positionne sur l'élément <stop> du numero demandé.
	bool found = mSVGfile->gotoSubElement("stop",stop_n);
	if (!found) return 0;

	// On affiche l'ID de ce <stop>
	DEBUG_LOG("   "+mSVGfile->getStringAttribute("id"));

	// On lit la valeur de l'offset
	Real stop_offset = mSVGfile->getRealAttribute("offset");
	DEBUG_LOG("   stop_offset="+StringConverter::toString(stop_offset));

	return stop_offset;
}

/* ------------------------------------------------------------------------- */
/// Cette fonction renvoie l'offset associé au dernier gradient lu.
/* ------------------------------------------------------------------------- */
Real Gradient::getLastOffset()
{
	return mLastOffset;
}

/* ------------------------------------------------------------------------- */
/// On libere le parser XML du fichier SVG.
/* ------------------------------------------------------------------------- */
void Gradient::closeGradientFile()
{
	if (mSVGfile) delete (mSVGfile);
	mSVGfile=NULL;
	mIndex  =0;
}

/* ------------------------------------------------------------------------- */
/// Parse une couleur au format #rrggbb
/* ------------------------------------------------------------------------- */
ColourValue Gradient::parseRGB1(String colorString)
{
	DEBUG_LOG(">> parseRGB1()");

	int R = 16*dec(colorString[1]) + dec(colorString[2]);
	int G = 16*dec(colorString.at(3)) + dec(colorString.at(4));
	int B = 16*dec(colorString.at(5)) + dec(colorString.at(6));


	DEBUG_LOG("   R="+StringConverter::toString(R));
	DEBUG_LOG("   G="+StringConverter::toString(G));
	DEBUG_LOG("   B="+StringConverter::toString(B));

	return ColourValue(R,G,B);
}

/* ------------------------------------------------------------------------- */
/// Parse une couleur au format rgb(R,G,B)
/* ------------------------------------------------------------------------- */
ColourValue Gradient::parseRGB2(String colorString)
{
	DEBUG_LOG(">> parseRGB2()");
	StringVector color_vector = StringUtil::split(colorString,"(,)");

	// Controle 
	if (color_vector.size() < 4) return ColourValue::Black;
	DEBUG_LOG("   R="+color_vector.at(1));
	DEBUG_LOG("   G="+color_vector.at(2));
	DEBUG_LOG("   B="+color_vector.at(3));

	int R = StringConverter::parseInt(color_vector.at(1));
	int G = StringConverter::parseInt(color_vector.at(2));
	int B = StringConverter::parseInt(color_vector.at(3));
	return ColourValue(R,G,B);
}


/* ------------------------------------------------------------------------- */
/// Cette fonction convertit un caractere ascii hexa en decimal.
/** 
	@param C Le caractère ASCII a convertir ('0'... 'F')
	@return La valeur décimale correspondante (0..15).
/* ------------------------------------------------------------------------- */
Ogre::uint8 Gradient::dec(char C)
{
	if (C<='9') return (C-'0');
	if (C<='F') return (C-'A'+10);
	if (C<='f') return (C-'a'+10);
	return 0;
}


