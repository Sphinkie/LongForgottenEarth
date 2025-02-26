/*
The zlib/libpng License

Copyright (c) 2005 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#include "linux/LinuxInputManager.h"
#include "linux/LinuxKeyboard.h"
#include "OISException.h"
#include "OISEvents.h"

#include <X11/keysym.h>
#include <X11/Xutil.h>

using namespace OIS;

//-------------------------------------------------------------------//
LinuxKeyboard::LinuxKeyboard(bool buffered, bool grab, bool useXRepeat)
{
	mBuffered = buffered;
	mType = OISKeyboard;
	listener = 0;
	display = 0;
	window = 0;

	grabKeyboard = grab;
	keyFocusLost = false;
	
	xAutoRepeat = useXRepeat;
	oldXAutoRepeat = false;
	
	//Map idea borrowed from Ogre3d =) -- Need to verify all KeyCodes used...
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_1, KC_1));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_2, KC_2));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_3, KC_3));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_4, KC_4));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_5, KC_5));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_6, KC_6));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_7, KC_7));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_8, KC_8));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_9, KC_9));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_0, KC_0));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_BackSpace, KC_BACK));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_minus, KC_MINUS));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_equal, KC_EQUALS));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_space, KC_SPACE));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_comma, KC_COMMA));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_period, KC_PERIOD));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_backslash, KC_BACKSLASH));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_slash, KC_SLASH));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Escape,KC_ESCAPE));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Caps_Lock, KC_CAPITAL));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Tab, KC_TAB));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Return, KC_RETURN));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Control_L, KC_LCONTROL));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Control_R, KC_RCONTROL));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_colon, KC_COLON));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_semicolon, KC_SEMICOLON));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_apostrophe, KC_APOSTROPHE));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_grave, KC_GRAVE));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_b, KC_B));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_a, KC_A));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_c, KC_C));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_d, KC_D));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_e, KC_E));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_f, KC_F));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_g, KC_G));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_h, KC_H));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_i, KC_I));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_j, KC_J));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_k, KC_K));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_l, KC_L));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_m, KC_M));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_n, KC_N));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_o, KC_O));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_p, KC_P));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_q, KC_Q));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_r, KC_R));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_s, KC_S));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_t, KC_T));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_u, KC_U));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_v, KC_V));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_w, KC_W));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_x, KC_X));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_y, KC_Y));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_z, KC_Z));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F1, KC_F1));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F2, KC_F2));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F3, KC_F3));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F4, KC_F4));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F5, KC_F5));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F6, KC_F6));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F7, KC_F7));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F8, KC_F8));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F9, KC_F9));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F10, KC_F10));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F11, KC_F11));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F12, KC_F12));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F13, KC_F13));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F14, KC_F14));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_F15, KC_F15));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_0, KC_NUMPAD0));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_1, KC_NUMPAD1));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_2, KC_NUMPAD2));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_3, KC_NUMPAD3));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_4, KC_NUMPAD4));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_5, KC_NUMPAD5));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_6, KC_NUMPAD6));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_7, KC_NUMPAD7));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_8, KC_NUMPAD8));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_9, KC_NUMPAD9));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_Add, KC_ADD));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_Subtract, KC_SUBTRACT));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_Decimal, KC_DECIMAL));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_Equal, KC_NUMPADEQUALS));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_Divide, KC_DIVIDE));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_KP_Multiply, KC_MULTIPLY));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Print, KC_SYSRQ));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Up, KC_UP));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Down, KC_DOWN));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Left, KC_LEFT));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Right, KC_RIGHT));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Page_Up, KC_PGUP));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Page_Down, KC_PGDOWN));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Home, KC_HOME));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_End, KC_END));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Num_Lock, KC_NUMLOCK));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Scroll_Lock, KC_SCROLL));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Shift_R, KC_RSHIFT));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Shift_L, KC_LSHIFT));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Alt_R, KC_RMENU));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Alt_L, KC_LMENU));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Insert, KC_INSERT));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Delete, KC_DELETE));

	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Super_L, KC_LWIN));
	keyConversion.insert(XtoOIS_KeyMap::value_type(XK_Super_R, KC_RWIN));
}

//-------------------------------------------------------------------//
void LinuxKeyboard::_initialize()
{
	//Clear our keyboard state buffer
	memset( &KeyBuffer, 0, 256 );
	mModifiers = 0;

	if( display ) XCloseDisplay(display);
	display = 0;
	window = static_cast<LinuxInputManager*>(InputManager::getSingletonPtr())->_getWindow();

	//Create our local X listener connection
	if( !(display = XOpenDisplay(0)) )
		OIS_EXCEPT(E_General, "LinuxKeyboard::_initialize >> Error opening X!");

	//Set it to recieve Input events
	if( XSelectInput(display, window, KeyPressMask | KeyReleaseMask) == BadWindow )
		OIS_EXCEPT(E_General, "LinuxKeyboard::_initialize: X error!");

	if( grabKeyboard )
		XGrabKeyboard(display,window,True,GrabModeAsync,GrabModeAsync,CurrentTime);

	keyFocusLost = false;

	if( xAutoRepeat == false )
	{
		//We do not want to blindly turn on autorepeat later when quiting if
		//it was not on to begin with.. So, let us check and see first
		XKeyboardState old;
		XGetKeyboardControl( display, &old );
		oldXAutoRepeat = false;

		if( old.global_auto_repeat == AutoRepeatModeOn )
			oldXAutoRepeat = true;

		XAutoRepeatOff( display );
	}
}

//-------------------------------------------------------------------//
LinuxKeyboard::~LinuxKeyboard()
{
	if( display )
	{
		if( oldXAutoRepeat )
			XAutoRepeatOn(display);

		if( grabKeyboard )
			XUngrabKeyboard(display, CurrentTime);

		XCloseDisplay(display);
	}
}

//-------------------------------------------------------------------//
bool LinuxKeyboard::isKeyDown( KeyCode key )
{
	return (KeyBuffer[key]);
}

//-------------------------------------------------------------------//
void LinuxKeyboard::capture()
{
	KeySym key;
	XEvent event;
	LinuxInputManager* linMan = 
		static_cast<LinuxInputManager*>(InputManager::getSingletonPtr());

	while( XPending(display) > 0 ) 
	{
		XNextEvent(display, &event);

		if( KeyPress == event.type )
		{
			//Mask out the modifier states X sets.. or we will get improper values
			event.xkey.state &= ~ShiftMask;
			event.xkey.state &= ~LockMask;

			XLookupString(&event.xkey,NULL,0,&key,NULL);
			_injectKeyDown(key, event.xkey.time);

			//Check for Alt-Tab
			if( event.xkey.state & Mod1Mask && key == XK_Tab )
				linMan->_setGrabState(false);
		}
		else if( KeyRelease == event.type )
		{
			//Mask out the modifier states X sets.. or we will get improper values
			event.xkey.state &= ~ShiftMask;
			event.xkey.state &= ~LockMask;

			//Else, it is a valid key release
			XLookupString(&event.xkey,NULL,0,&key,NULL);
			_injectKeyUp(key, event.xkey.time);
		}
	}

	//If grabbing mode is on.. Handle focus lost/gained via Alt-Tab and mouse clicks
	if( grabKeyboard )
	{
		if( linMan->_getGrabState() == false )
		{
			// are no longer grabbing
			if( keyFocusLost == false )
			{
				//UnGrab KeyBoard
				XUngrabKeyboard(display, CurrentTime);
				keyFocusLost = true;
			}
		}
		else
		{
			//We are grabbing - and regained focus
			if( keyFocusLost == true )
			{
				//ReGrab KeyBoard
				XGrabKeyboard(display, window, True, GrabModeAsync,
					      GrabModeAsync, CurrentTime);
				keyFocusLost = false;
			}
		}
	}
}

//-------------------------------------------------------------------//
void LinuxKeyboard::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//-------------------------------------------------------------------//
bool LinuxKeyboard::_injectKeyDown( KeySym key, unsigned int time )
{
	KeyCode kc = keyConversion[key];
	KeyBuffer[kc] = 1;

	//Turn on modifier flags
	if( kc == KC_LCONTROL || kc == KC_RCONTROL)
		mModifiers |= Ctrl;
	else if( kc == KC_LSHIFT || kc == KC_RSHIFT )
		mModifiers |= Shift;
	else if( kc == KC_LMENU || kc == KC_RMENU )
		mModifiers |= Alt;

	if( mBuffered && listener ) 
		return listener->keyPressed(KeyEvent(this, time, kc));

	return true;
}

//-------------------------------------------------------------------//
bool LinuxKeyboard::_injectKeyUp( KeySym key, unsigned int time )
{
	KeyCode kc = keyConversion[key];
	KeyBuffer[kc] = 0;

	//Turn off modifier flags
	if( kc == KC_LCONTROL || kc == KC_RCONTROL)
		mModifiers &= ~Ctrl;
	else if( kc == KC_LSHIFT || kc == KC_RSHIFT )
		mModifiers &= ~Shift;
	else if( kc == KC_LMENU || kc == KC_RMENU )
		mModifiers &= ~Alt;

	if( mBuffered && listener )
		return listener->keyReleased(KeyEvent(this, time, kc));

	return true;
}
