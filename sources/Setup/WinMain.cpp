/* ----------------------------------------------------------------------- */
// File   :		winmain.cpp
// Project:		Long Forgotten Earth
// Dscription:	Configuration video
/* ----------------------------------------------------------------------- */

#include "SetupApplication.h"
#include "ogre.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
    // Create application object
    SetupApplication app;

    SET_TERM_HANDLER;

    try {
        app.setupVIDEO();
        } 
    catch( Ogre::Exception& e )
        {
        MessageBox( NULL, e.getFullDescription().c_str(), "Exception error: ", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        }

    return 0;
}
