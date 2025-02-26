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
#include "Win32/Win32InputManager.h"
#include "Win32/Win32Keyboard.h"
#include "Win32/Win32Mouse.h"
#include "Win32/Win32JoyStick.h"
#include "OISException.h"

using namespace OIS;

const std::string Win32InputManager::iName = "Win32";

//--------------------------------------------------------------------------------//
Win32InputManager::Win32InputManager()
{
	hWnd = 0;
	mDirectInput = 0;

	kbSettings    = 0;
	mouseSettings = 0;
	joySettings   = 0;

	joySticks = 0;
}

//--------------------------------------------------------------------------------//
Win32InputManager::~Win32InputManager()
{
	if( mDirectInput )
	{
		mDirectInput->Release();
		mDirectInput = 0;
	}
}

//--------------------------------------------------------------------------------//
void Win32InputManager::_initialize( ParamList &paramList )
{
	HINSTANCE hInst = 0;
	HRESULT hr;

	//TODO 64 bit proof this little conversion xxx wip
	//First of all, get the Windows Handle and Instance
	ParamList::iterator i = paramList.find("WINDOW");
	if( i == paramList.end() ) 
		OIS_EXCEPT( E_InvalidParam, "Win32InputManager::Win32InputManager >> No HWND found!" );

	hWnd  = (HWND)strtoul(i->second.c_str(), 0, 10);

	hInst = GetModuleHandle(0);

	//Create the device
	hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&mDirectInput, NULL );
    if (FAILED(hr))	
		OIS_EXCEPT( E_General, "Win32InputManager::Win32InputManager >> Not able to init DirectX8 Input");

	//Ok, now we have DirectInput, parse whatever extra settings were sent to us
	_parseConfigSettings( paramList );
	_enumerateDevices();
}

//--------------------------------------------------------------------------------//
void Win32InputManager::_parseConfigSettings( ParamList &paramList )
{
	//Here we pick up settings such as a device's cooperation mode
	std::map<std::string, DWORD> temp;
	temp["DISCL_BACKGROUND"]	= DISCL_BACKGROUND;
	temp["DISCL_EXCLUSIVE"]		= DISCL_EXCLUSIVE;
	temp["DISCL_FOREGROUND"]	= DISCL_FOREGROUND;
	temp["DISCL_NONEXCLUSIVE"]	= DISCL_NONEXCLUSIVE;
	temp["DISCL_NOWINKEY"]		= DISCL_NOWINKEY;

	//Check for pairs: ie. ("w32_keyboard","DISCL_NOWINKEY")("w32_keyboard","DISCL_FOREGROUND")
	ParamList::iterator i = paramList.begin(), e = paramList.end();
	for( ; i != e; ++i ) 
	{
		if( i->first == "w32_keyboard" )
				kbSettings |= temp[i->second];
		else if( i->first == "w32_mouse" )
				mouseSettings |= temp[i->second];
		else if( i->first == "w32_joystick" )
				joySettings |= temp[i->second];
	}
	if( kbSettings == 0 ) kbSettings = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
	if( mouseSettings == 0 ) mouseSettings = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
	if( joySettings == 0 ) joySettings = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
}

//--------------------------------------------------------------------------------//
void Win32InputManager::_enumerateDevices()
{
	//Enumerate all attached devices
	mDirectInput->EnumDevices(NULL, _DIEnumKbdCallback, this, DIEDFL_ATTACHEDONLY); 
}

//--------------------------------------------------------------------------------//
BOOL CALLBACK Win32InputManager::_DIEnumKbdCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	Win32InputManager *_this_ = static_cast<Win32InputManager*>(pvRef);
	if( GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_JOYSTICK ||
		GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_GAMEPAD )
	{
		JoyStickInfo jsInfo;
		jsInfo.deviceID = lpddi->guidInstance;
		jsInfo.vendor = lpddi->tszInstanceName;
		jsInfo.devId = _this_->joySticks;

		_this_->joySticks++;
		
		_this_->unusedJoyStickList.push_back( jsInfo );
	}

	return DIENUM_CONTINUE;
}

//--------------------------------------------------------------------------------//
int Win32InputManager::numJoysticks()
{
	return joySticks;
}

//--------------------------------------------------------------------------------//
int Win32InputManager::numMice()
{
	return 1;
}

//--------------------------------------------------------------------------------//
int Win32InputManager::numKeyBoards()
{
	return 1;
}

//----------------------------------------------------------------------------//
Object* Win32InputManager::createInputObject( Type iType, bool bufferMode )
{
	Object* obj = 0;
	
	switch( iType )
	{
		case OISKeyboard: obj = new Win32Keyboard( mDirectInput, bufferMode, kbSettings ); break;
		case OISMouse: obj = new Win32Mouse( mDirectInput, bufferMode, mouseSettings ); break;
		case OISJoyStick: 
		{
			//Find a JoyStick not in use
			JoyStickInfoList::iterator i = unusedJoyStickList.begin();
			if( i != unusedJoyStickList.end() )
			{
				obj = new Win32JoyStick( mDirectInput, bufferMode, joySettings, (*i) );
				unusedJoyStickList.erase(i);
				break;
			}
			OIS_EXCEPT(E_InputDeviceNonExistant, "No Unused JoyStick could be found!");
		}
		default: OIS_EXCEPT( E_InputDeviceNotSupported, "Type not implemented");
	}

	try	{
		obj->_initialize();
	}
	catch(...) {
		delete obj;
		throw; //rethrow
	}

	return obj;
}

//----------------------------------------------------------------------------//
void Win32InputManager::destroyInputObject( Object* obj )
{
	if( obj == 0 )
		return;

	//If it was a numbered device... add it back to availiable list
	if( obj->type() == OISJoyStick )
	{
		unusedJoyStickList.push_back(((Win32JoyStick*)obj)->_getJoyInfo());
	}

	delete obj;
}
