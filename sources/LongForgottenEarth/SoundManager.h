/* ------------------------------------------------------------------------- */
// File       : SoundManager.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ogre.h"

using namespace Ogre;

#define MAX_CHANNELS 32	/// Nombre max de channels audio pouvant faire du fading en même temps

/* ------------------------------------------------------------------------- */
/// Bibliothèque de fonctions sonores.
/** Fait l'interface avec la librairie Audio (FMOD). */
/* ------------------------------------------------------------------------- */
class SoundManager: public FrameListener, public Singleton <SoundManager>	
{
public:
	SoundManager();
	~SoundManager();
	static SoundManager*  getSingletonPtr  (void);
	int        playSound        (String SoundFile, int Volume, int Range, bool Loop = false);
	void       freeSound        (int soundchannel);
	void       setFading        (int soundchannel);
	void       updateSoundSource(int Channel, Vector3 VectorPos);
	int        playSoundAmbiance(String SoundFile);
	int        playSoundMoving  (String SoundFile);
	int        playSoundShort   (String SoundFile, bool Loop = false);
	int        playSoundShip    (String SoundFile, bool Loop = false);
	int        playSoundObject  (String SoundFile, bool Loop = false);


protected:
	bool       frameStarted (const FrameEvent& evt);
	int        fadeSound    (int soundchannel);
	void       initSounds();
	

	Real     mTimer_fading;     /// Timer pour le fading des audios
	int      mSoundChannel[MAX_CHANNELS];
};

#endif 
