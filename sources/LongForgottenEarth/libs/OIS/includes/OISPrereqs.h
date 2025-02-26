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
#ifndef OIS_Prereqs_H
#define OIS_Prereqs_H
//----------------------------------------------------------------------------//
//* This Header File contains forward declared classes                       *//
//*	Also, common properties for the keyboard and mouse                       *//
//* There is also preprocessor defines to determine type of OS               *//
//----------------------------------------------------------------------------//

//-------------- Common STL Containers ---------------------------------------//
#include <vector>
#include <string>
#include <map>
#include "OISConfig.h"

// Default is blank for most OS's
#define _OISExport

//-------------- Determine Compiler ---------------------------------
#if defined( _MSC_VER )
#	define OIS_MSVC_COMPILER
#elif defined( __GNUC__ )
#	define OIS_GCC_COMPILER
#elif defined( __BORLANDC__ )
#	define OIS_BORLAND_COMPILER
#endif

// --------------- Determine Operating System Platform ---------------
#if defined( __WIN32__ ) || defined( _WIN32 ) // Windows 2000, XP, ETC
#	if defined ( _XBOX )
#		define OIS_XBOX_PLATFORM
#	else
#		define OIS_WIN32_PLATFORM
#		if defined( OIS_DYNAMIC_LIB )
#			undef _OISExport
			//Ignorable Dll interface warning...
#			pragma warning (disable : 4251)
#			if defined( OIS_NONCLIENT_BUILD )
#				define _OISExport __declspec( dllexport )
#			else
#				define _OISExport __declspec( dllimport )
#			endif
#		endif
#	endif
#elif defined( __APPLE_CC__ ) // Apple OS X
#	define OIS_APPLE_PLATFORM
#else //Probably Linux
#	define OIS_LINUX_PLATFORM
#endif

//Is Processor 32 or 64 bits...
#if defined(__x86_64__)
#	define OIS_ARCH_64
#else
#	define OIS_ARCH_32
#endif

//-------------- Common Classes, Enums, and Typdef's -------------------------//
namespace OIS
{
	//Device Types
	class Object;
	class Keyboard;
	class Mouse;
	class JoyStick;

	//EventListeners
	class KeyListener;
	class MouseListener;
	class JoyStickListener;

	//Device Output
	class Interface;
	class ForceFeedback;
	class Effect;

	//! Way to send OS nuetral parameters.. ie OS Window handles, modes, flags
	typedef _OISExport std::multimap<std::string, std::string> ParamList;

	//! Each Input class has a General Type variable, a form of RTTI
	enum _OISExport Type
	{
		OISUnknown = 0,
		OISKeyboard,
		OISMouse,
		OISJoyStick,
		OISTablet
	};
	
	//! Keyboard scan codes
	enum KeyCode
	{
		KC_ESCAPE      = 0x01,
		KC_1           = 0x02,
		KC_2           = 0x03,
		KC_3           = 0x04,
		KC_4           = 0x05,
		KC_5           = 0x06,
		KC_6           = 0x07,
		KC_7           = 0x08,
		KC_8           = 0x09,
		KC_9           = 0x0A,
		KC_0           = 0x0B,
		KC_MINUS       = 0x0C,    // - on main keyboard
		KC_EQUALS      = 0x0D,
		KC_BACK        = 0x0E,    // backspace
		KC_TAB         = 0x0F,
		KC_Q           = 0x10,
		KC_W           = 0x11,
		KC_E           = 0x12,
		KC_R           = 0x13,
		KC_T           = 0x14,
		KC_Y           = 0x15,
		KC_U           = 0x16,
		KC_I           = 0x17,
		KC_O           = 0x18,
		KC_P           = 0x19,
		KC_LBRACKET    = 0x1A,
		KC_RBRACKET    = 0x1B,
		KC_RETURN      = 0x1C,    // Enter on main keyboard
		KC_LCONTROL    = 0x1D,
		KC_A           = 0x1E,
		KC_S           = 0x1F,
		KC_D           = 0x20,
		KC_F           = 0x21,
		KC_G           = 0x22,
		KC_H           = 0x23,
		KC_J           = 0x24,
		KC_K           = 0x25,
		KC_L           = 0x26,
		KC_SEMICOLON   = 0x27,
		KC_APOSTROPHE  = 0x28,
		KC_GRAVE       = 0x29,    // accent
		KC_LSHIFT      = 0x2A,
		KC_BACKSLASH   = 0x2B,
		KC_Z           = 0x2C,
		KC_X           = 0x2D,
		KC_C           = 0x2E,
		KC_V           = 0x2F,
		KC_B           = 0x30,
		KC_N           = 0x31,
		KC_M           = 0x32,
		KC_COMMA       = 0x33,
		KC_PERIOD      = 0x34,    // . on main keyboard
		KC_SLASH       = 0x35,    // / on main keyboard
		KC_RSHIFT      = 0x36,
		KC_MULTIPLY    = 0x37,    // * on numeric keypad
		KC_LMENU       = 0x38,    // left Alt
		KC_SPACE       = 0x39,
		KC_CAPITAL     = 0x3A,
		KC_F1          = 0x3B,
		KC_F2          = 0x3C,
		KC_F3          = 0x3D,
		KC_F4          = 0x3E,
		KC_F5          = 0x3F,
		KC_F6          = 0x40,
		KC_F7          = 0x41,
		KC_F8          = 0x42,
		KC_F9          = 0x43,
		KC_F10         = 0x44,
		KC_NUMLOCK     = 0x45,
		KC_SCROLL      = 0x46,    // Scroll Lock
		KC_NUMPAD7     = 0x47,
		KC_NUMPAD8     = 0x48,
		KC_NUMPAD9     = 0x49,
		KC_SUBTRACT    = 0x4A,    // - on numeric keypad
		KC_NUMPAD4     = 0x4B,
		KC_NUMPAD5     = 0x4C,
		KC_NUMPAD6     = 0x4D,
		KC_ADD         = 0x4E,    // + on numeric keypad
		KC_NUMPAD1     = 0x4F,
		KC_NUMPAD2     = 0x50,
		KC_NUMPAD3     = 0x51,
		KC_NUMPAD0     = 0x52,
		KC_DECIMAL     = 0x53,    // . on numeric keypad
		KC_OEM_102     = 0x56,    // < > | on UK/Germany keyboards
		KC_F11         = 0x57,
		KC_F12         = 0x58,
		KC_F13         = 0x64,    //                     (NEC PC98)
		KC_F14         = 0x65,    //                     (NEC PC98)
		KC_F15         = 0x66,    //                     (NEC PC98)
		KC_KANA        = 0x70,    // (Japanese keyboard)
		KC_ABNT_C1     = 0x73,    // / ? on Portugese (Brazilian) keyboards
		KC_CONVERT     = 0x79,    // (Japanese keyboard)
		KC_NOCONVERT   = 0x7B,    // (Japanese keyboard)
		KC_YEN         = 0x7D,    // (Japanese keyboard)
		KC_ABNT_C2     = 0x7E,    // Numpad . on Portugese (Brazilian) keyboards
		KC_NUMPADEQUALS= 0x8D,    // = on numeric keypad (NEC PC98)
		KC_PREVTRACK   = 0x90,    // Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
		KC_AT          = 0x91,    //                     (NEC PC98)
		KC_COLON       = 0x92,    //                     (NEC PC98)
		KC_UNDERLINE   = 0x93,    //                     (NEC PC98)
		KC_KANJI       = 0x94,    // (Japanese keyboard)
		KC_STOP        = 0x95,    //                     (NEC PC98)
		KC_AX          = 0x96,    //                     (Japan AX)
		KC_UNLABELED   = 0x97,    //                        (J3100)
		KC_NEXTTRACK   = 0x99,    // Next Track
		KC_NUMPADENTER = 0x9C,    // Enter on numeric keypad
		KC_RCONTROL    = 0x9D,
		KC_MUTE        = 0xA0,    // Mute
		KC_CALCULATOR  = 0xA1,    // Calculator
		KC_PLAYPAUSE   = 0xA2,    // Play / Pause
		KC_MEDIASTOP   = 0xA4,    // Media Stop
		KC_VOLUMEDOWN  = 0xAE,    // Volume -
		KC_VOLUMEUP    = 0xB0,    // Volume +
		KC_WEBHOME     = 0xB2,    // Web home
		KC_NUMPADCOMMA = 0xB3,    // , on numeric keypad (NEC PC98)
		KC_DIVIDE      = 0xB5,    // / on numeric keypad
		KC_SYSRQ       = 0xB7,
		KC_RMENU       = 0xB8,    // right Alt
		KC_PAUSE       = 0xC5,    // Pause
		KC_HOME        = 0xC7,    // Home on arrow keypad
		KC_UP          = 0xC8,    // UpArrow on arrow keypad
		KC_PGUP        = 0xC9,    // PgUp on arrow keypad
		KC_LEFT        = 0xCB,    // LeftArrow on arrow keypad
		KC_RIGHT       = 0xCD,    // RightArrow on arrow keypad
		KC_END         = 0xCF,    // End on arrow keypad
		KC_DOWN        = 0xD0,    // DownArrow on arrow keypad
		KC_PGDOWN      = 0xD1,    // PgDn on arrow keypad
		KC_INSERT      = 0xD2,    // Insert on arrow keypad
		KC_DELETE      = 0xD3,    // Delete on arrow keypad
		KC_LWIN        = 0xDB,    // Left Windows key
		KC_RWIN        = 0xDC,    // Right Windows key
		KC_APPS        = 0xDD,    // AppMenu key
		KC_POWER       = 0xDE,    // System Power
		KC_SLEEP       = 0xDF,    // System Sleep
		KC_WAKE        = 0xE3,    // System Wake
		KC_WEBSEARCH   = 0xE5,    // Web Search
		KC_WEBFAVORITES= 0xE6,    // Web Favorites
		KC_WEBREFRESH  = 0xE7,    // Web Refresh
		KC_WEBSTOP     = 0xE8,    // Web Stop
		KC_WEBFORWARD  = 0xE9,    // Web Forward
		KC_WEBBACK     = 0xEA,    // Web Back
		KC_MYCOMPUTER  = 0xEB,    // My Computer
		KC_MAIL        = 0xEC,    // Mail
		KC_MEDIASELECT = 0xED     // Media Select
	};

	//! Base type for all device components (button, axis, etc)
	enum _OISExport ComponentType
	{
		OIS_Unknown,
		OIS_Button, //Key, mouse button, joy button, etc
		OIS_Axis,
		OIS_POV
	};
	
	//! Base of all device components (button, axis, etc)
	struct _OISExport Component
	{
		Component() : cType(OIS_Unknown) {};
		Component(ComponentType type) : cType(type) {};
		ComponentType cType;
	};
	
	//! Button can be a keyboard key, mouse button, etc
	struct _OISExport Button : Component
	{
		Button(bool bPushed) : Component(OIS_Button), pushed(bPushed) {};
		bool pushed;
	};

	//! POV / HAT Joystick component
	struct _OISExport Pov : Component
	{
		Pov(int iValue) : Component(OIS_POV), value(iValue) {};
		int value;
	};

	//! Axis component... xxx Remove Z component from here
	struct _OISExport Axis : Component
	{
		Axis() : Component(OIS_Axis), 
			     abX(0), abY(0), abZ(0), relX(0), relY(0), relZ(0),
				 absOnly(false) {};

		long abX, abY, abZ, relX, relY, relZ;

		bool absOnly;

		void setAbsValues( long x, long y, long z ) {abX=x; abY=y; abZ=z;}
		void setRelValues( long x, long y, long z ) {relX=x; relY=y; relZ=z;}

		void clear()
		{
			abX = abY = abZ = relX = relY = relZ = 0;
		}
	};

	//! Button ID for mouse devices
	enum MouseButtonID
	{
		MB_Left = 0, MB_Right, MB_Middle,
		MB_Button3, MB_Button4,	MB_Button5, MB_Button6,	MB_Button7
	};

	/**
		Represents the state of the mouse
		All members are valid for both buffered and non buffered mode
	*/
	struct _OISExport MouseState : public Axis
	{
		MouseState() : width(50), height(50), buttons(0) {};

		/** Represents the height/width of your display area.. used if mouse clipping 
		or mouse grabbed in case of X11 - defaults to 50.. Make sure to set this 
		and change when your size changes.. 
		*/
		mutable unsigned int width, height;

		//! represents all buttons
		long buttons;

		//! Button down test
		inline long buttonDown( MouseButtonID button ) const
		{
			return buttons & ( 1 << button );
		}

		//! Clear all the values
		void clear()
		{
			Axis::clear();
			buttons = 0;
		}
	};

	/**
		Represents the state of the joystick
		All members are valid for both buffered and non buffered mode
		Sticks with zero values are not present on the device
	*/
	struct _OISExport JoyStickState
	{
		JoyStickState() : buttons(0) { clear(); }

		enum AxisName
		{
			//! Typical Stick Axis
			Stick = 0,
			//! Rotational Axis
			Rotational,
			//! Velocity Axis
			Velocity,
			//! Angular Velocity Axis
			AngularVelocity,
			//! Acceleration Axis
			Acceleration,
			//! AngularAcceleration Axis
			AngularAcceleration,
			//! Force Axis
			Force,
			//! Torque Axis
			Torque
		};

		//! Represents the maximum number of axes
		Axis mAxes[8];

		//! Represents the value of a POV. Maximum of 4
		int mPOV[4];

		//! Represents All Buttons By Bit (max buttons = 32 for 32bit long)
		long buttons;

		//! Nice Button Bit Test
		inline long buttonDown( int button ) const {
			return buttons & ( 1 << button );
		}

		//! Do Not Call Directly! called internally
		void clear() {			
			buttons = 0;
			for( int i = 0; i < 8; ++i )
			{
				mAxes[i].absOnly = true;	//Currently, joysticks only report Abs
				mAxes[i].clear();
			}
		}
	};
}

#endif //end if prereq header defined
