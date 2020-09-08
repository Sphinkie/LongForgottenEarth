/* ------------------------------------------------------------------------- */
// File       : Gradient.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _GRADIENT_H_
#define _GRADIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"
#include "XMLparser_T.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Analyse un fichier gradient au format SVG.
/* ------------------------------------------------------------------------- */
class Gradient
{
public:
	Gradient(String SVGfilename);
	~Gradient();
	ColourValue getGradient(int stop_n);
	Real        getOffset(int stop_n);
	ColourValue getColour(Real G_offset);
	ColourValue getNextGradient();
	Real        getLastOffset();

protected:


	XMLParser_T* mSVGfile;    /// Fichier de gradients de couleur
	int          mIndex;      /// Index des elements stop dans le fichier SVG.
	Real         mLastOffset; /// La valeur de l'offset du dernier gradient (ou "stop") lu.


	bool openGradientFile(String SVGfilename);
	void closeGradientFile();
	ColourValue parseRGB1(String colorString);
	ColourValue parseRGB2(String colorString);
	Ogre::uint8 dec(char C);


};

#endif 
