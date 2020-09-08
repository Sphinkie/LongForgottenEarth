/* ------------------------------------------------------------------------- */
// File       : KeyboardTranslator.h
// Project    : Management of keyboards of different languages
// Author     : http://www.ogre3d.org/wiki/index.php/KeyboardTranslator
/* ------------------------------------------------------------------------- */

#ifndef _KEYBOARD_TRANSLATOR_H
#define _KEYBOARD_TRANSLATOR_H

#include "Ogre.h"

using namespace Ogre;

/* -------------------------------------------------------------------------- */
/// Gestion d'une horloge pour le jeu.  C'est un singleton.
/* -------------------------------------------------------------------------- */
class KeyboardTranslator : public Singleton<KeyboardTranslator>
{
public:
	enum KeyMap
	{
		KEYMAP_GERMAN  = 1, 
		KEYMAP_ENGLISH = 2,
		KEYMAP_FRENCH  = 3
	};

	static KeyboardTranslator* getSingletonPtr(void);
	static KeyboardTranslator& getSingleton(void);

	KeyboardTranslator(KeyMap keymap = KEYMAP_ENGLISH);
	virtual ~KeyboardTranslator();

	void translate(unsigned char key);
	unsigned char getKey();


private:
	KeyMap mKeymap;
	unsigned char mKey;
	unsigned char mTranslatedKey;
};

#endif

