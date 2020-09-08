/* -------------------------------------------------------------------------- */
// File:        Winmain.cpp
// Project:     Long Forgotten Earth
// Author :     David de Lorenzo
/* -------------------------------------------------------------------------- */

#include "mainApplication.h"



/// @mainpage Long Forgotten Earth
///
/// @section intro Introduction
///
/// <i>Long Forgotten Earth</i> (aka LFE) is a RPG game in a first person view.
/// The game is base on Space exploration, on a large spaceship, in a realistic univers,
/// with numerous planets, NPC and quests.
///
/// @section classes Classes
///
/// <i>LFE</i> is known to compile under Windows XP using Microsoft Visual C++ 6.0.
/// The game use the following 3rd party libraries:
/// TinyXML, libnoise, stargen.
///
/// @section contact Contact
///
/// Contact the dev-team for questions at mailto:sphinx-games@noos.fr
///
/// The main game site is http://mapage.noos.fr/sphinx-games
///
/// The technical game site is http://www.keohosting.net/sphinxgames/index.html
///



#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	// Create application object: MAIN (the game)
	mainApplication game;   
	
	SET_TERM_HANDLER;

	AllocConsole();			// Cree une console de Débug
	try 
	{
		game.go();  // New game
	}
	catch( Ogre::Exception& e )
	{
		MessageBox( NULL, e.getFullDescription().c_str(), "Ogre Error: ", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	FreeConsole();		// libère la console de debug
	return 0;
}
