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
#ifndef _LINUX_KEYBOARD_H_EADER_
#define _LINUX_KEYBOARD_H_EADER_

#include "OISKeyboard.h"
#include "linux/LinuxPrereqs.h"

#include <X11/Xlib.h>

namespace OIS
{
	class LinuxKeyboard : public Keyboard
	{
	public:
		LinuxKeyboard(bool buffered, bool grab, bool useXRepeat );
		virtual ~LinuxKeyboard();

		/** @copydoc Object::setBuffered */
		virtual void setBuffered(bool buffered);
		
		/** @copydoc Keyboard::isKeyDown */
		virtual bool isKeyDown( KeyCode key );

		/** 
		@remarks
			Note: Calling this will also update the mouse (X11 updates in a single
			event queue). Updates State and/or raises event for buffered mode..
		*/
		virtual void capture();

		/** @copydoc Object::queryInterface */
		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		/** @copydoc Object::_initialize */
		virtual void _initialize();

		bool _injectKeyDown( KeySym key, unsigned int time );
		bool _injectKeyUp( KeySym key, unsigned int time );

	protected:

		//! 1:1 Conversion Map between X Key Events and OIS KeyCodes
		typedef std::map<KeySym, KeyCode> XtoOIS_KeyMap;
		XtoOIS_KeyMap keyConversion;

		//! Depressed Key List
		char KeyBuffer[256];

		//! X11 Stuff
		Window window;
		Display *display;
		bool grabKeyboard;
		bool keyFocusLost;

		bool xAutoRepeat;
		bool oldXAutoRepeat;
	};
}

#endif //_LINUX_KEYBOARD_H_EADER_
