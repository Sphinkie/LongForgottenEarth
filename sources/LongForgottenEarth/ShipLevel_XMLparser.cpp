/* ------------------------------------------------------------------------- */
// File       : ShipLevel_XMLparser.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */


// Attention à respecter l'ordre de ces includes
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"


#include "ShipLevel_XMLparser.h"
#include "macros.h"


/* ------------------------------------------------------------------------- */
/// Constructeur (on execute le Constructeur de la surclasse)
/* ------------------------------------------------------------------------- */
ShipLevel_XMLparser::ShipLevel_XMLparser(String XMLfile) : XMLParser(XMLfile)
{
}

/* ------------------------------------------------------------------------- */
/// Destructeur (on execute le destructeur de la surclasse)
/* ------------------------------------------------------------------------- */
ShipLevel_XMLparser::~ShipLevel_XMLparser()
{
}


/* ------------------------------------------------------------------------- */
/// On construit la liste des <room ... > et des <door ... >
/* ------------------------------------------------------------------------- */
void ShipLevel_XMLparser::gotoTopOfFile()
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100] ;

	mRoomIndex = 0;
	mDoorIndex = 0;
	mSceneIndex= 0;

	// On recupere la liste des elements <room> du document
	XMLString::transcode("room",tempStr,99);
	mListRoom= ((DOMDocument*)mDocument)->getElementsByTagName(tempStr);

	// On recupere la liste des elements <door> du document
	XMLString::transcode("door",tempStr,99);
	mListDoor= ((DOMDocument*)mDocument)->getElementsByTagName(tempStr);

	// On recupere la liste des elements <scene> du document
	XMLString::transcode("scene",tempStr,99);
	mListScene= ((DOMDocument*)mDocument)->getElementsByTagName(tempStr);

}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag <room ... > suivant.
/* ------------------------------------------------------------------------- */
bool ShipLevel_XMLparser::getNextRoom()
{
	XERCES_CPP_NAMESPACE_USE

	XMLCh  tempStr[100] ;

	mCurrentNode = ((DOMNodeList*)mListRoom)->item(mRoomIndex);
	/// On renvoie FALSE s'il n'y a plus de <room...> dans la suite du fichier XML.
	if (!mCurrentNode) {DEBUG_LOG("XML Tag <room> with index "+ITOA(mRoomIndex)+": not found"); return false;}

	/// On recupere la liste des <objet...> de cette room
	XMLString::transcode("object",tempStr,99);
	mObjectSubList= ((DOMElement*)mCurrentNode)->getElementsByTagName(tempStr);
	mObjectIndex=0;

	/// On recupere la liste des <light...> de cette room
	XMLString::transcode("light",tempStr,99);
	mLightSubList= ((DOMElement*)mCurrentNode)->getElementsByTagName(tempStr);
	mLightIndex=0;

	mRoomIndex++;
	return true;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag <door ... > suivant.
/* ------------------------------------------------------------------------- */
bool ShipLevel_XMLparser::getNextDoor()
{
	XERCES_CPP_NAMESPACE_USE

	mCurrentNode = ((DOMNodeList*)mListDoor)->item(mDoorIndex);
	/// On renvoie FALSE s'il n'y a plus de <door...> dans la suite du fichier XML.
	if (!mCurrentNode) {DEBUG_LOG("XML Tag <door> with index "+ITOA(mDoorIndex)+": not found"); return false;}
	mDoorIndex++;

	return true;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag <scene ... > suivant.
/* ------------------------------------------------------------------------- */
bool ShipLevel_XMLparser::getNextScene()
{
	XERCES_CPP_NAMESPACE_USE

	if (!mDocument) return false;
	mCurrentNode = ((DOMNodeList*)mListScene)->item(mSceneIndex);
	/// On renvoie FALSE s'il n'y a plus de <scene...> dans la suite du fichier XML.
	if (!mCurrentNode) {DEBUG_LOG("XML Tag <scene> with index "+ITOA(mSceneIndex)+": not found"); return false;}
	mSceneIndex++;
	return true;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag <object ... > suivant, appartenant à la Room en cours
/* ------------------------------------------------------------------------- */
bool ShipLevel_XMLparser::getNextSubObject()
{
	XERCES_CPP_NAMESPACE_USE

	mCurrentNode = ((DOMNodeList*)mObjectSubList)->item(mObjectIndex);
	/// On renvoie FALSE s'il n'y a plus de <object...> dans cette liste.
	if (!mCurrentNode) 
	{
		// DEBUG_LOG("XML Tag <object> with index "+ITOA(mObjectIndex)+": not found"); 
		return false;
	}
	mObjectIndex++;

	return true;
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le tag <light ... > suivant, appartenant à la Room en cours
/* ------------------------------------------------------------------------- */
bool ShipLevel_XMLparser::getNextSubLight()
{
	XERCES_CPP_NAMESPACE_USE

	mCurrentNode = ((DOMNodeList*)mLightSubList)->item(mLightIndex);
	/// On renvoie FALSE s'il n'y a plus de <light...> dans cette liste.
	if (!mCurrentNode) 
	{
		// DEBUG_LOG("XML Tag <light> with index "+ITOA(mLightIndex)+": not found"); 
		return false;
	}
	mLightIndex++;

	return true;
}

