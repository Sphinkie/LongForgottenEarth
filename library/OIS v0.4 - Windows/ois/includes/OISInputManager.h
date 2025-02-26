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
#ifndef OIS_InputManager_H
#define OIS_InputManager_H

#include "OISPrereqs.h"

namespace OIS
{
	/**
		This is a virtual class which allows specialised classes to be added easily. The aim
		of this project is to 1) Have a solid OO interface to IO which is typically very dirty
		and hacky; 2) Be multiplatform; 3) ??? 4) $$$
	*/
	class _OISExport InputManager
	{
	public:
		/**
		@remarks
			Creates appropriate input system dependent on platform. 
		@param paramList
			ParamList contains OS specific info (such as HWND and HINSTANCE for window apps),
			and access mode.
		@returns
			A pointer to the created manager, or raises Exception
		*/
		static InputManager* createInputSystem( ParamList &paramList );

		/**
		@remarks
			Gets the name of the current input system.. eg. "Win32", "Linux", "XBox", etc
		*/
		virtual const std::string& inputSystemName() = 0;

		/**
		@remarks
			Destroys the current InputManager
		*/
		static void destroyInputSystem();

		/**
		@remarks
			Returns the number of joysticks found
		*/
		virtual int numJoysticks() = 0;

		/**
		@remarks
			Returns the number of mice found
		*/
		virtual int numMice() = 0;

		/**
		@remarks
			Returns the number of keyboards found
		*/
		virtual int numKeyBoards() = 0;

		/**
		@remarks
			Returns the type of input requested or raises Exception
		*/
		virtual Object* createInputObject( Type iType, bool bufferMode ) = 0;

		/**
		@remarks
			Destroys Input Object
		*/
		virtual void destroyInputObject( Object* obj ) = 0;

		/**
		@remarks
			Get a pointer to the class.. or null if it has not been created yet
		*/
		static InputManager* getSingletonPtr();

	protected:
		//! Internal... Called from createInputSystem
		virtual void _initialize(ParamList &paramList) = 0;

		//These are hidden to prevent app from directly instantiating/deleting an InputManager class
		InputManager() {}
		virtual ~InputManager() {}
	};
}
#endif
