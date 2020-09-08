/* -------------------------------------------------------------------------- */
// File       : inputEngine.h
// Project    : Long Forgotten Earth 
// Modified by: David de Lorenzo
// Author     : Copyright (C) 2003, 2005 "Motorsport" developers
// Part of the "Motorsport" project (http://motorsport.sourceforge.net)
// Licensed under the GNU General Public License (GPL)
/* -------------------------------------------------------------------------- */

#include "InputEngine.h"
#include <SDL.h>
#include <SDL_syswm.h>

#include "../macros.h"

/* -------------------------------------------------------------------------- */
/// Constructor. Detection of joysticks. Creates a new input engine.
/* -------------------------------------------------------------------------- */
InputEngine::InputEngine (int width, int height)
{
	mWidth = width;
	mHeight= height;
	this->initialise(width, height);

    DEBUG_LOG("Initializing mouse axis");
    mAxisMap[getIDMouseAxis(0)] = new Axis; // mouse #1 axis X
    mAxisMap[getIDMouseAxis(1)] = new Axis; // mouse #1 axis Y
    mAxisMap[getIDMouseAxis(0)]->setNewRawValue (mWidth);	
    mAxisMap[getIDMouseAxis(0)]->setNewRawValue (0);
    mAxisMap[getIDMouseAxis(1)]->setNewRawValue (-mHeight);	
    mAxisMap[getIDMouseAxis(1)]->setNewRawValue (0);
    
/*    DEBUG_LOG("Initializing keyboard axis");
    for (int i=SDLK_FIRST; i<SDLK_LAST; i++)
    {
        mAxisMap[getIDKeyboardKey (i)] = new Axis;
        mAxisMap[getIDKeyboardKey (i)]->setNewRawValue (1);
        mAxisMap[getIDKeyboardKey (i)]->setNewRawValue (0);
    }
*/    
    DEBUG_LOG("Initializing dummy joystick devices");
    mAxisMap[getIDJoyAxis(0,0)] = new Axis;
    mAxisMap[getIDJoyAxis(0,0)]->setNewRawValue (32767);
    mAxisMap[getIDJoyAxis(0,0)]->setNewRawValue (-32767);
    mAxisMap[getIDJoyAxis(0,0)]->setNewRawValue (0);
    mAxisMap[getIDJoyAxis(0,1)] = new Axis;
    mAxisMap[getIDJoyAxis(0,1)]->setNewRawValue (-32767);
    mAxisMap[getIDJoyAxis(0,1)]->setNewRawValue (32767);
    mAxisMap[getIDJoyAxis(0,2)] = new Axis;
    mAxisMap[getIDJoyAxis(0,2)]->setNewRawValue (-32767);
    mAxisMap[getIDJoyAxis(0,2)]->setNewRawValue (32767);
    
    DEBUG_LOG("Initializing real joystick devices");
    int nJoysticks = SDL_NumJoysticks();
    for (int joy = 0; joy < nJoysticks; joy++ ) 
    {
        SDL_Joystick * joystick = SDL_JoystickOpen(joy);
        int nAxis    = SDL_JoystickNumAxes(joystick);
        int nButtons = SDL_JoystickNumButtons(joystick);
        DEBUG_LOG("Found joystick "+Ogre::String(SDL_JoystickName(joy)));
        DEBUG_LOG("with "+ITOA(nAxis)+" axis and "+ITOA(nButtons)+" buttons.");
        for (int axis=0; axis<nAxis; axis++)
        {
            if ( (joy>0) || (axis>2) )
            //if (!( (joy==0) && ((axis==0)||(axis==1)||(axis==2)) ))
            {
				// (Joy #0 has already been declared, for axis 0-1-2)
                mAxisMap[getIDJoyAxis(joy, axis)] = new Axis;
            }
            if (axis==1 || axis==2)
            {
                mAxisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                mAxisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            } 
			else 
			{
                mAxisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (2);
                mAxisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                mAxisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            }
            DEBUG_LOG("Joystick axis "+ITOA(getIDJoyAxis(joy,axis))+" initialized.");
            DEBUG_LOG("with +ITOA(mAxisMap[getIDJoyAxis(joy, axis)]->getValue())");
        }
        for (int button=0; button<nButtons; button++)
        {
            mAxisMap[getIDJoyButton (joy, button)] = new Axis;
            mAxisMap[getIDJoyButton (joy, button)]->setNewRawValue (1);
            mAxisMap[getIDJoyButton (joy, button)]->setNewRawValue (0);
            DEBUG_LOG("Joystick initialized : button #"+ ITOA(getIDJoyButton(joy, button)));
        }
    }
    DEBUG_LOG(ITOA(nJoysticks)+" joystick(s) found.");

}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void InputEngine::clearLogicEventAxis()
{
/*
    mAxisMap[getIDKeyboardKey (SDLK_HOME)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_END)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_KP_MINUS)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_KP_PLUS)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_a)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_z)]->setNewRawValue(0);
*/
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void InputEngine::clearGraphicsEventAxis()
{
/*
    mAxisMap[getIDKeyboardKey (SDLK_PRINT)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_c)]->setNewRawValue(0);
    mAxisMap[getIDKeyboardKey (SDLK_v)]->setNewRawValue(0);
*/
}

/* -------------------------------------------------------------------------- */
/// Processes all user input events since last call.
/** Processes all user input events since last call, transforming them into values.*/
/* -------------------------------------------------------------------------- */
int InputEngine::computeStep(void)
{
	Ogre::String tmpString;
	int id;
	SDL_Event event;
	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			DEBUG_LOG("New SDL_QUIT event: User wants to exit. Notifying to stop mainLoop...");
//			disableMainLoop ();
            break;
		case SDL_KEYUP:
//			axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
//			DEBUG_LOG("New SDL_KEYUP event: "+tmpString);
//			mAxisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(0);
			break;
		case SDL_KEYDOWN:
//			axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
//			DEBUG_LOG("New SDL_KEYDOWN event: "+tmpString);
//			mAxisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(1);
			break;
		case SDL_MOUSEMOTION:
//			axisIDtoStr(getIDMouseAxis (0), tmpString);
//			DEBUG_LOG("New SDL_MOUSEMOTION event "+tmpString);
//			axisIDtoStr(getIDMouseAxis (1), tmpString);
//			DEBUG_LOG("New SDL_MOUSEMOTION event "+tmpString);
//			mAxisMap[getIDMouseAxis (0)]->setNewRawValue (event.motion.x);
//			mAxisMap[getIDMouseAxis (1)]->setNewRawValue (-event.motion.y);
//			DEBUG_LOG("Mouse movement: "+ITOA(event.motion.x)+" "+ITOA(-event.motion.y));
			break;
		case SDL_JOYAXISMOTION:
			id = getIDJoyAxis (event.jaxis.which, event.jaxis.axis);
			axisIDtoStr(id, tmpString);
			DEBUG_LOG("New SDL_JOYAXISMOTION event "+tmpString);
			mAxisMap[id]->setNewRawValue (event.jaxis.value);
			DEBUG_LOG("Joystick axis movement: "+ITOA(event.jaxis.value));
			break;
		case SDL_JOYBUTTONDOWN:
			id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
			axisIDtoStr(id, tmpString);
			DEBUG_LOG("New SDL_JOYBUTTONDOWN event "+tmpString);
			mAxisMap[id]->setNewRawValue (1);
			break;
		case SDL_JOYBUTTONUP:
			id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
			axisIDtoStr(id, tmpString);
			DEBUG_LOG("New SDL_JOYBUTTONUP event "+tmpString);
			mAxisMap[id]->setNewRawValue (0);
			break;
		default:
			break;
        }
    }


 /* Be careful with this debugging code!! you need to have created all the axis you're going to debug!
    log->telemetry (LOG_ENDUSER, "%5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f",
                                   mAxisMap[getIDJoyAxis(0,0)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,1)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,2)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,3)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,4)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,5)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,0)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,1)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,2)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,3)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,4)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,5)]->getValue());
    log->telemetry (LOG_ENDUSER, " A0    A1    A2    A3    A4    A5    B0    B1    B2    B3    B4    B5");*/

    return (0);
}

/* -------------------------------------------------------------------------- */
/// Destructor. Deletes the input engine.
/* -------------------------------------------------------------------------- */
InputEngine::~InputEngine(void)
{
	SDL_Quit();
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void InputEngine::initialise(int width, int height)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE | SDL_INIT_JOYSTICK) < 0)
    {
      DEBUG_LOG("Couldn't initialize SDL:");
      DEBUG_LOG(SDL_GetError());
    }

   if (width && height)
   {
      // if width = 0 and height = 0, the window is fullscreen
      // This is necessary to allow the window to move on WIN32 systems. 
	  // Without this, the window resets  to the smallest possible size after moving.
      SDL_SetVideoMode(width, height, 0, 0); // first 0: BitPerPixel, 
                                             // second 0: flags (fullscreen/...)
                                             // neither are needed as Ogre sets these
   }

   static SDL_SysWMinfo pInfo;
   SDL_VERSION(&pInfo.version);
   SDL_GetWMInfo(&pInfo);

   // Also, SDL keeps an internal record of the window size
   //  and position. Because SDL does not own the window, it
   //  missed the WM_POSCHANGED message and has no record of
   //  either size or position. It defaults to {0, 0, 0, 0},
   //  which is then used to trap the mouse "inside the 
   //  window". We have to fake a window-move to allow SDL
   //  to catch up, after which we can safely grab input.
   RECT r;
   GetWindowRect(pInfo.window, &r);
   SetWindowPos(pInfo.window, 0, r.left, r.top, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

   // Grab means: lock the mouse inside our window!
   SDL_ShowCursor(SDL_DISABLE);   // SDL_ENABLE to show the mouse cursor (default)
   SDL_WM_GrabInput(SDL_GRAB_ON); // SDL_GRAB_OFF to not grab input (default)
}