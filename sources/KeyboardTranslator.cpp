/* ------------------------------------------------------------------------- */
// File       : KeyboardTranslator.cpp
// Project    : Management of keyboards of different languages
// Author     : http://www.ogre3d.org/wiki/index.php/KeyboardTranslator
/* ------------------------------------------------------------------------- */

#include "KeyboardTranslator.h"

template<> KeyboardTranslator* Singleton<KeyboardTranslator>::ms_Singleton = 0;

/* -------------------------------------------------------- */
/// Singleton function
/* -------------------------------------------------------- */
KeyboardTranslator* KeyboardTranslator::getSingletonPtr(void)
{
	return ms_Singleton;
}


/* -------------------------------------------------------- */
/// Singleton function
/* -------------------------------------------------------- */
KeyboardTranslator& KeyboardTranslator::getSingleton(void)
{
	assert(ms_Singleton); 
	return (*ms_Singleton);
}


/* -------------------------------------------------------- */
/// Constructor
/* -------------------------------------------------------- */
KeyboardTranslator::KeyboardTranslator(KeyMap keymap)
{
	mKeymap = keymap;
}


/* -------------------------------------------------------- */
/// Destructor
/* -------------------------------------------------------- */
KeyboardTranslator::~KeyboardTranslator()
{}


/* -------------------------------------------------------- */
/// Translation function
/* -------------------------------------------------------- */
void KeyboardTranslator::translate(unsigned char key)
{
	mKey = key;

	/* ------------------------------------------------ */
	// English Keymap
	/* ------------------------------------------------ */
	if (mKeymap == KEYMAP_ENGLISH)
	{
		mTranslatedKey = mKey;
		return;
	}

	/* ------------------------------------------------ */
	// German Keymap
	/* ------------------------------------------------ */
	else if (mKeymap == KEYMAP_GERMAN)
	{
		const unsigned char KEYS_TO_CHANGE = 28;
		unsigned char array_german[KEYS_TO_CHANGE] = {'y', 'z', 246, 228, 252, ':', ';', '?', '_', ')', '=', 180, '(', '\'', '#', '/', '&', 167, '"', '^', 176, '+', 214, 196, '-', 220, '*', 223 };
		unsigned char array_english[KEYS_TO_CHANGE] = {'z', 'y', ';', '\'', '[', '>', '<', '_', '?', '(', ')', '+', '*', '|', '\\', '&', '^', '#', '@', '`', '~', ']', ':', ';', '/', '{', '}', '-'};
                
		mTranslatedKey = mKey;
		for (unsigned char charIterator = 0; charIterator < KEYS_TO_CHANGE; charIterator++)
		{
			if (mKey == array_english[charIterator])
			{
				mTranslatedKey = array_german[charIterator];
				return;
			}
		}
		return;
	}

	/* ------------------------------------------------ */
	// French Keymap (95% complete)
	/* ------------------------------------------------ */
	else if (mKeymap == KEYMAP_FRENCH)
	{
		const unsigned char KEYS_TO_CHANGE = 47;
		// When you press on .... (qwerty keyboard)
		unsigned char array_english[KEYS_TO_CHANGE] = 
{'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', 'q', 'Q', 'w', 'W', '}', ']', '1', '2', '3',  '4',  '5', '6', '7', '8', '9', '0', '-', 'a', 'A', ':', ';', '\"', '\'', 'z', 'Z', 'M', 'm', '<', ',', '>', '.', '?', '/','\\','|','}' };
		// You should get a ...
		unsigned char array_french[KEYS_TO_CHANGE] = 
{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '°', 'a', 'A', 'z', 'Z', '£', '$', '&', 'e', '\"', '\'', '(', '-', 'e', '_', 'c', 'a', ')', 'q', 'Q', 'M', 'm', '%',  'u',  'w', 'W', '?', ',', '.', ';', '/', ':', '§', '!','*','µ','£' };
                
		mTranslatedKey = mKey;
		for (unsigned char charIterator = 0; charIterator < KEYS_TO_CHANGE; charIterator++)
		{
			if (mKey == array_english[charIterator])
			{
				mTranslatedKey = array_french[charIterator];
				return;
			}
		}
		return;
	}

        /// Here you can add your own keymaps, just copy from the if(){} above
}


/* -------------------------------------------------------- */
/// 
/* -------------------------------------------------------- */
unsigned char KeyboardTranslator::getKey()
{
	return mTranslatedKey;
}

