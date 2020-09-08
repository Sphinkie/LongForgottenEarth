/* ------------------------------------------------------------------------- */
// File       : macros.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _MACROS_H_
#define _MACROS_H_


#define ITOA(X)      Ogre::StringConverter::toString(X)
#define DEBUG_LOG(X) Ogre::LogManager::getSingleton().logMessage(X)
#define TRACE(X)     Ogre::LogManager::getSingleton().logMessage(X)
#define DEBUG_WIN(X) Ogre::Root::getSingleton().getRenderTarget("OGRE Render Window")->setDebugText(X)

#define NULL_CHANNEL    -1

#endif
