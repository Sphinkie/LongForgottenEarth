/* -------------------------------------------------------------------------- */
// File       : inputEngine.h
// Project    : Long Forgotten Earth 
// Modified by: David de Lorenzo
// Author     : Copyright (C) 2003, 2005 "Motorsport" developers
// Part of the "Motorsport" project (http://motorsport.sourceforge.net)
// Licensed under the GNU General Public License (GPL)
/* -------------------------------------------------------------------------- */

#ifndef _INPUT_ENGINE_H_
#define _INPUT_ENGINE_H_

#if _MSC_VER > 1000
#pragma once
#endif 


#include <SDL_types.h>

#include "ogre.h"
#include "axis.h"

/* -------------------------------------------------------------------------- */
/// This class manages everything related to user input.
/** The InputEngine can manages everything related to user input, such as keyboard 
keypresses, mouse movement, steering wheel movement, joystick, etc... Essentially, 
it's the interface between the end-user and the simulated world, allowing the user 
to transparently make changes in this simulated world. Input devices calibration 
issues and many other things are handled by this engine.
For LFE, we manage only JOYSTICK as keyboard and mouse are already managed by CEGUI.
@sa http:://www.libsdl.org/
*/
/* -------------------------------------------------------------------------- */
class InputEngine
{
  public:
		InputEngine(int width, int height);
		~InputEngine(void);
		int  computeStep(void);
		void clearLogicEventAxis();
		void clearGraphicsEventAxis();


  private:

		std::map <int, Axis * > mAxisMap;
		int mWidth ;
		int mHeight;

		// en doublon ??? DDL
		void initialise(int width, int height);

};

#endif 
