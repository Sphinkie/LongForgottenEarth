/* ------------------------------------------------------------------------- */
// File       : Interface_XMLParser.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _INTERFACE_XMLPARSER_H_
#define _INTERFACE_XMLPARSER_H_


#include "Ogre.h"

#include "../LFE/XMLParser.h"

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// Parsing des fichiers XML (pour les interfaces de Robots, computers, etc)
/* ------------------------------------------------------------------------- */
class Interface_XMLParser : public XMLParser
{
public:
	Interface_XMLParser(String XMLfile);
	~Interface_XMLParser();
	String  getTitle();
	
	// Fonctions specifiques pour la gestion des menus - listbox
	String  getItemTextContent(String sMenuName, int index);
	String  getItemTextContent(int id);
	int     getMenuItemID(String sMenuName, int index);
	String  getItemStringAttribute(int id, String attribut);
	String  getAssociatedTextName (String ListboxName);
};

#endif
