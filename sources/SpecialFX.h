/* ------------------------------------------------------------------------- */
// File       : specialFX.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SPECIALFX_H_
#define _SPECIALFX_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"

using namespace Ogre;


/* ------------------------------------------------------------------------- */
/// Biblioth�que de fonctions utiles et vari�es
/* ------------------------------------------------------------------------- */

ParticleSystem*     SFX_addSmoke      (String Name, SceneNode* SourceNode);
ParticleSystem*		SFX_addSparks     (String Name, SceneNode* SourceNode);
void                SFX_addClouds     (SceneManager* SceneMgr);
void                SFX_die           (ParticleSystem* pParticle);
void                SFX_rebirth       (ParticleSystem* pParticle);


String              readFile  (String filename);
void                writeFile (String filename, String data, bool append);
bool                readln    (FILE* stream, char* Buffer, int lgmax);


/* encore utilis� par Body */
int                 _getIntParameter    (String P);
String              getStringParameter (String P);
Vector3             getVectorParameter (String P);
bool                getBooleanParameter(String P);


#endif 
