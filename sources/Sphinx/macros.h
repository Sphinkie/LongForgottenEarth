/* *********************************************************************************** */
/// Quelques macros utiles pour l'ensemble des fichiers
/* *********************************************************************************** */

#ifndef _MACROS_H_
#define _MACROS_H_



#define ITOA(X)      StringConverter::toString(X)
#define TRACE(X)     LogManager::getSingleton().logMessage(X)
#define DEBUG_LOG(X) LogManager::getSingleton().logMessage(X)
#define DEBUG_WIN(X) Root::getSingleton().getRenderTarget("OGRE Render Window")->setDebugText(X)




#endif

