/* -------------------------------------------------------------------------- */
// File:        Winmain.cpp
// Project    : Long Forgotten Earth - Menu Principal
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#include "process.h"
#include "menuApplication.h"


#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	// Create application object: MENU
	menuApplication menu;   

	SET_TERM_HANDLER;

	
	try 
	{
		int MenuChoice = menu.go();
		//  we launch the game application
		if (MenuChoice != -1) execl("lfe.exe",NULL);
	}
	catch( Ogre::Exception& e )
	{
		MessageBox( NULL, e.getFullDescription().c_str(), "Ogre Error: ", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}
