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
#ifndef _WIN32_JOYSTICK_H_EADER_
#define _WIN32_JOYSTICK_H_EADER_

#include "OISJoyStick.h"
#include "Win32/Win32Prereqs.h"

namespace OIS
{
	class Win32JoyStick : public JoyStick
	{
	public:
		Win32JoyStick( IDirectInput8* pDI, bool buffered, DWORD coopSettings, const JoyStickInfo &info );
		virtual ~Win32JoyStick();
		
		/** @copydoc Object::setBuffered */
		virtual void setBuffered(bool buffered);
		
		/** @copydoc Object::capture */
		virtual void capture();

		/** @copydoc Object::queryInterface */
		virtual Interface* queryInterface(Interface::IType type);

		/** @copydoc Object::_initialize */
		virtual void _initialize();

		/**
		@remarks
			For internal use only... Returns a strucure to the manager, to make the device
			availiable for use again
		*/
		JoyStickInfo _getJoyInfo();
	protected:
		Win32JoyStick() {}
		//! Enumerates all things
		void _enumerate();
		//! Enumerate axis callback
		static BOOL CALLBACK DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
		//! Enumerate Force Feedback callback
		static BOOL CALLBACK DIEnumEffectsCallback(LPCDIEFFECTINFO pdei, LPVOID pvRef);

		bool _doButtonClick( int button, DIDEVICEOBJECTDATA& di );
		bool _changePOV( int pov, DIDEVICEOBJECTDATA& di );

		void _read();
		void _readBuffered();

		IDirectInput8* mDirectInput;
		IDirectInputDevice8* mJoyStick;
		DWORD coopSetting;
		GUID deviceGuid;

		//! A force feedback device
		Win32ForceFeedback* ff_device;
	};
}

#endif //_WIN32_JOYSTICK_H_EADER_
