/* ------------------------------------------------------------------------- */
// File       : TextList.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _TEXT_LIST_H_
#define _TEXT_LIST_H_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "ogre.h"
#include "XMLParser_T.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Gestion d'une liste d'entrées de type texte (par exemple: un journal de bord...)
/* ------------------------------------------------------------------------- */
class TextList  
{
public:
	TextList(String filename);
	virtual ~TextList();
	void    addItem(String title, String text);
	String  readDocumentName();
	String  readCurrentItemText();
	String  readCurrentItemTitle();
	bool    gotoPreviousItem();
	bool    gotoNextItem();
	bool    gotoFirstItem();
	bool    gotoLastItem();
	void    removeAllItems();
	void    removeCurrentItem();
	void    saveObject();

protected:
	void    loadObject();

	struct tEntry 
	{
		String  Title;
		String  Text;
		tEntry* Next;
		tEntry* Previous;
	};

	String        mFilename;
	String        mName;
	tEntry*       mEntryList;
	tEntry*       mLastEntry;
	tEntry*       mCurrentEntry;
	XMLParser_T*  mParser;
};

#endif 
