/* ----------------------------------------------------------------------- */
// File   :		SetupApplication.cpp
// Project:		Long Forgotten Earth
// Dscription:	Configuration video
/* ----------------------------------------------------------------------- */

#include "SetupApplication.h"


/* ----------------------------------------------------------------------- */
// Construction
/* ----------------------------------------------------------------------- */
SetupApplication::SetupApplication()   
{
	mRoot = new Root();
	setuplogfile = LogManager::getSingleton().createLog("setup.log",true);
}

/* ----------------------------------------------------------------------- */
// Destruction
/* ----------------------------------------------------------------------- */
SetupApplication::~SetupApplication()  {}



/* ----------------------------------------------------------------------- */
// Ecran de configuration des options graphiques
/* ----------------------------------------------------------------------- */
void SetupApplication::setupVIDEO()
{
	LogManager::getSingleton().logMessage(LML_NORMAL, "setup VIDEO");
	// If OK, the options are stored in ogre.cfg
	// If CANCEL, the options are not stored
    mRoot->showConfigDialog();

}


