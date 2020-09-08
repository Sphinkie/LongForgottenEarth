/* ------------------------------------------------------------------------- */
// File       : TextList.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "TextList.h"
#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
TextList::TextList(String filename)
{
	mName         = "";
	mFilename     = "../savedgames/"+filename+".xml";
	mEntryList    = NULL;
	mLastEntry    = NULL;
	mCurrentEntry = NULL;
	mParser       = NULL;
	
	this->loadObject();
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
TextList::~TextList()
{
	this->removeAllItems();
	if (mParser) delete (mParser);
}

/* ------------------------------------------------------------------------- */
/// Ajoute un item à la fin du journal
/* ------------------------------------------------------------------------- */
void TextList::addItem(String title, String text)
{
	// On crée une nouvelle Entry pour le book
	tEntry* NewEntry = new tEntry;

	// On mémorise l'Entry 
	NewEntry->Title    = title;
	NewEntry->Text     = text;
	NewEntry->Next     = NULL;
	NewEntry->Previous = NULL;

	// Si c'est la première Entry: on mémorise la tete 
	if (!mEntryList)		
	{
		// On met à jour le pointeur vers la premiere Entry
		mEntryList = NewEntry;	
		// On met à jour le pointeur vers la dernière Entry
		mLastEntry    = NewEntry;
		// On met à jour le pointeur vers l'Entry courante
		mCurrentEntry = NewEntry;
	}
	else
	{	
		// Sinon, il y a des Entry précédentes, et on s'y rattache
		mLastEntry->Next   = NewEntry;
		NewEntry->Previous = mLastEntry;
		// On met à jour le pointeur vers la dernière NewEntry
		mLastEntry    = NewEntry;
		// On met à jour le pointeur vers l'Entry courante
		mCurrentEntry = NewEntry;
	}
}

/* ------------------------------------------------------------------------- */
/// Supprime l'item courant. Le pointeur sur le Courant est décalé d'un élément.
/* ------------------------------------------------------------------------- */
void TextList::removeCurrentItem()
{
	// S'il n'y a pas de currentEntry: inutile de la supprimer.
	if (!mCurrentEntry) return;


	tEntry* the_previous = mCurrentEntry->Previous;
	tEntry* the_next     = mCurrentEntry->Next;
	tEntry* the_destroyed= mCurrentEntry;

	mCurrentEntry = NULL;

	if (the_next)     
	{
		the_next->Previous = the_previous;
		mCurrentEntry = the_next;
	}
	if (the_previous) 
	{
		the_previous->Next = the_next;
		mCurrentEntry = the_previous;
	}

	// On vide l'entry
	the_destroyed->Title = "";
	the_destroyed->Text = "";

	// S'il n'y a pas de suivant, Current était le dernier.
	// Previous devient donc le Last.
	if (!the_next)    
	{
		the_previous->Next = NULL;
		mLastEntry = the_previous;
	}

	// S'il n'y a pas de precedent, Current était le premier.
	// Next devient donc le First.
	if (!the_previous)    
	{
		the_next->Previous = NULL;
		mEntryList = the_next;
	}

	delete (the_destroyed);
}

/* ------------------------------------------------------------------------- */
/// Vide le journal
/* ------------------------------------------------------------------------- */
void TextList::removeAllItems()
{
	tEntry* entry = mEntryList;
	while (entry)
	{
		tEntry* toDestroy = entry;
		entry = entry->Next;
		delete (toDestroy);
	}
	mEntryList    = NULL;
	mLastEntry    = NULL;
	mCurrentEntry = NULL;
}


/* ------------------------------------------------------------------------- */
/// On se positionne sur le premier item
/* ------------------------------------------------------------------------- */
bool TextList::gotoFirstItem()
{
	mCurrentEntry = mEntryList;
	// Si le premier Entry existe, on renvoie TRUE
	return (mCurrentEntry != NULL);
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur le dernier item
/* ------------------------------------------------------------------------- */
bool TextList::gotoLastItem()
{
	mCurrentEntry = mLastEntry;
	// Si le dernier Entry existe, on renvoie TRUE
	return (mCurrentEntry != NULL);
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur l'item suivant. Renvoie TRUE si réussi.
/* ------------------------------------------------------------------------- */
bool TextList::gotoNextItem()
{
	if(!mCurrentEntry)       return false;
	if(!mCurrentEntry->Next) return false;
	mCurrentEntry = mCurrentEntry->Next;
	return true;
}

/* ------------------------------------------------------------------------- */
/// On se positionne sur l'item precedent. Renvoie TRUE si réussi.
/* ------------------------------------------------------------------------- */
bool TextList::gotoPreviousItem()
{
	if(!mCurrentEntry)           return false;
	if(!mCurrentEntry->Previous) return false;
	mCurrentEntry = mCurrentEntry->Previous;
	return true;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le texte associé à l'item courant
/* ------------------------------------------------------------------------- */
String TextList::readCurrentItemText()
{
	String text = "";
	if (mCurrentEntry) text=mCurrentEntry->Text;
	return text;
}

/* ------------------------------------------------------------------------- */
/// Renvoie de titre de l'item courant
/* ------------------------------------------------------------------------- */
String TextList::readCurrentItemTitle()
{
	String title = "";
	if (mCurrentEntry) title=mCurrentEntry->Title;
	return title;
}

/* ------------------------------------------------------------------------- */
/// Renvoie de titre du document
/* ------------------------------------------------------------------------- */
String  TextList::readDocumentName()
{
	return mName;
}

/* ------------------------------------------------------------------------- */
/// Charge le contenu du journal à partir d'un fichier XML
/* ------------------------------------------------------------------------- */
void TextList::loadObject()
{
	int index = 0;
	mParser = new XMLParser_T(mFilename);
	mParser->gotoRootElement("logbook");
	mName = mParser->getStringAttribute("doc");
	while (mParser->gotoSubElement("entry", index))
	{
		String title = mParser->getStringAttribute("title");
		String text  = mParser->getTextContent();
		this->addItem(title, text);
		index++;
	}
}

/* ------------------------------------------------------------------------- */
/// Sauve le contenu du journal dans un fichier XML
/* ------------------------------------------------------------------------- */
void TextList::saveObject()
{
	mParser->gotoRootElement("logbook");
	// On efface toutes les entry du book
	mParser->removeAllSubElements();
	// On se place sur le premier record
	this->gotoFirstItem();
	if (!mCurrentEntry) return;	// S'il n'existe pas: on sort

	do
	{
		// On recupère les données du record courant et on les met au format XML
		String XmlEntry = "<entry title = '";
		XmlEntry += this->readCurrentItemTitle();
		XmlEntry += "'>";
		XmlEntry += this->readCurrentItemText();
		XmlEntry += "</entry>";
		// On mémorise cette ligne XML
		mParser->addXmlData(XmlEntry,"entry", "logbook");
	}
	while (this->gotoNextItem());

	mParser->saveToFile(mFilename);
}
