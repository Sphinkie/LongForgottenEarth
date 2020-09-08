/* ------------------------------------------------------------------------- */
// Code from Koutsi (Ogre Forum)
/* ------------------------------------------------------------------------- */

#ifndef _LINE_3D_H_
#define _LINE_3D_H_

#include "Ogre.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Classe pour tracer des lignes en 3D
/* ------------------------------------------------------------------------- */
class Line3D : public Ogre::SimpleRenderable 
{ 
	public: 

		Line3D(Vector3 point1, Vector3 point2); 
		~Line3D(void); 
		Real getSquaredViewDepth(const Camera* cam) const; 
		Real getBoundingRadius(void) const; 
		void setColor(ColourValue LineColour);
}; 


#endif