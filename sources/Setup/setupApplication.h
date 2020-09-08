/* ----------------------------------------------------------------------- */
// File   :		SetupApplication.h
// Project:		Long Forgotten Earth
// Dscription:	Configuration video
/* ----------------------------------------------------------------------- */


#if !defined(_SETUPAPPLICATION_)
#define _SETUPAPPLICATION_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Ogre.h"
using namespace Ogre;

/* ----------------------------------------------------------------------- */
/* ----------------------------------------------------------------------- */
class SetupApplication
{
public:
	SetupApplication();
	virtual ~SetupApplication();
    void setupVIDEO();

protected:
	Root* mRoot;
	Log*  setuplogfile;

};

#endif 
