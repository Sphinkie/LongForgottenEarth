/* ------------------------------------------------------------------------- */
// File       : SoundManager.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "SoundManager.h"

#include "macros.h"
#include "fmod.h"
#include "fmod_errors.h"



/* ------------------------------------------------------------------------- */
/// Contructeur
/* ------------------------------------------------------------------------- */
SoundManager::SoundManager()
{
	// Plus la valeur est élevée, plus le son porte loin
	FSOUND_3D_SetDistanceFactor(200);		// en théorie: 100 units per meter 

	mTimer_fading = 0;
	this->initSounds();
	Root::getSingletonPtr()->addFrameListener(this);
};

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
SoundManager::~SoundManager()
{
	Root::getSingletonPtr()->removeFrameListener(this);
}


/* ------------------------------------------------------------------------- */
/// Gestionnaire de son
/* ------------------------------------------------------------------------- */
bool SoundManager::frameStarted(const FrameEvent& evt)
{
	mTimer_fading -= evt.timeSinceLastFrame;	// en secondes

	// -------------------------------------------------------
	/// Gestion des fading audio
	// -------------------------------------------------------
	// Tous les 10eme de seconde, on baisse un peu le son
	if (mTimer_fading<0) 
	{
		int i;
		mTimer_fading = 0.10;
		for (i=0; i<MAX_CHANNELS; i++) 
		{
			int SoundChannel = mSoundChannel[i];
			if (SoundChannel != NULL_CHANNEL) 
			{
				mSoundChannel[i] = this->fadeSound(SoundChannel);  
			}
		}
	}

	return true;
}

/* ------------------------------------------------------------------------- */
/// Override the Singleton Class method
/* ------------------------------------------------------------------------- */
template<> SoundManager* Singleton<SoundManager>::ms_Singleton = 0;

SoundManager* SoundManager::getSingletonPtr(void)
{
	return ms_Singleton;
}




/* ------------------------------------------------------------------------- */
/// Loads and decodes a static soundfile into memory, and play it.
/**
Parametres:
* SoundFile: préciser le chemin complet
* Volume: c'est la distance, à partir de laquelle le son commence à décroitre.
* Range: c'est la distance à partir de laquelle le son a fini de décroitre et se stabilise.
* renvoie le No de channel du son.
Exemples d'utilisation:
* Pour le son d'ambiance d'une pièce de rayon 300: 300,10000: Le son est constant dans toute la pièce, puis il décroit au delà des murs.
* Pour un bip = 20,100: Le son est audible autour de l'émetteur et ne décroit quasiment pas.
* Pour un objet bruyant = 20,10000: Le son est audible autour de l'émetteur, et décroit rapidement quand on s'éloigne.
L'appel a cette fonction ciommence à jouer le son. Si on veut que la source du son bouge, il faut appeler SFX_updateSoundSource().
*/
/* ------------------------------------------------------------------------- */
int SoundManager::playSound(String SoundFile, int Volume, int Range, bool Loop)
{
	FSOUND_SAMPLE* Sound;
	int SoundChannel ;

	// Loads and decodes a static soundfile into memory.
	Sound=FSOUND_Sample_Load(FSOUND_FREE,SoundFile.c_str(),FSOUND_HW3D|FSOUND_FORCEMONO,0,0);

	if (!Sound)
	{
		DEBUG_LOG ("Error Loading SoundFile:"+SoundFile);
		DEBUG_LOG (FMOD_ErrorString(FSOUND_GetError()) );
		FSOUND_Sample_Free(Sound);
	}

	// set the volume and the range, and the mode
    // Note: Increasing min-distance makes it louder in 3d space
	FSOUND_Sample_SetMinMaxDistance(Sound, Volume, Range);

	if (Loop) FSOUND_Sample_SetMode(Sound, FSOUND_LOOP_NORMAL);
	else      FSOUND_Sample_SetMode(Sound, FSOUND_LOOP_OFF);

	// play the sound
	// Pour les sons 3D, la position par defaut est celle du listener
	SoundChannel = FSOUND_PlaySoundEx(FSOUND_FREE, Sound, NULL, true);
//	SoundChannel = FSOUND_PlaySound(FSOUND_FREE, Sound);
	// il doit sortir de pause sans erreur
	if (!FSOUND_SetPaused(SoundChannel, false))
	{
		DEBUG_LOG(FMOD_ErrorString(FSOUND_GetError()));
	}
	return SoundChannel;
}


/* ------------------------------------------------------------------------- */
/// Removes a sample from memory and makes its slot available again.
/* ------------------------------------------------------------------------- */
void SoundManager::freeSound(int soundchannel)
{
	if (soundchannel == NULL_CHANNEL) return;
	// On arrete le son 
	if (!FSOUND_SetPaused(soundchannel, true)) DEBUG_LOG(FMOD_ErrorString(FSOUND_GetError()));
	// On libere le channel
	FSOUND_Sample_Free(FSOUND_GetCurrentSample(soundchannel));
}


/* ------------------------------------------------------------------------- */
/// Définit (ou met à jour) de la position des sons 3D (moteur, etc)
/* ------------------------------------------------------------------------- */
void SoundManager::updateSoundSource(int Channel, Vector3 VectorPos)
{
	float pos[3] = { 0,0,0 };
    float vel[3] = { 0,0,0 };

	pos[0]=VectorPos.x;
	pos[1]=VectorPos.y;
	pos[2]=VectorPos.z;

	FSOUND_3D_SetAttributes(Channel, pos, vel);
}


/* -------------------------------------------------------------------------- */
/// Initialise le tableau des channels audio
/* -------------------------------------------------------------------------- */
void SoundManager::initSounds()
{
	// Initialisation des fading audio
	for (int i=0; i<MAX_CHANNELS; i++) 
	{
		mSoundChannel[i] = NULL_CHANNEL;
	}
}




/* -------------------------------------------------------------------------- */
/// Memorise un Channel devant subir un fading audio
/* -------------------------------------------------------------------------- */
void SoundManager::setFading(int soundchannel)
{
	int i;
	if (soundchannel!= NULL_CHANNEL) 
	{
		TRACE(">> fadeSound("+ITOA(soundchannel)+")");
		// Si ce channel n'est pas NULL_CHANNEL;
		// on cherche un emplacement libre pour memorise ce channel
		for (i=0; i<MAX_CHANNELS; i++) 
		{
			if (mSoundChannel[i] == NULL_CHANNEL) 
			{
				TRACE("   Channel registered with ID "+ITOA(i));
				mSoundChannel[i] = soundchannel;
				return;
			}
		}
	}
}


/* -------------------------------------------------------------------------- */
/// Effectue un fading progressif du channel audio. 
/** Cette fonction doit etre appelée plusieurs fois de suite à intervalles réguliers */
/* -------------------------------------------------------------------------- */
int SoundManager::fadeSound(int soundchannel)
{
//	TRACE(">> SFX_fadeSound()");

	// On baisse le volume (de 1%)
	int volume = FSOUND_GetVolume(soundchannel);
	volume = volume * 0.99;
	FSOUND_SetVolume(soundchannel, volume);

	// Si le son est devenu trop faible, on le coupe
	if (volume < 5)
	{
		FSOUND_StopSound(soundchannel);
		soundchannel = NULL_CHANNEL;
	}
	
	// On renvoie le No du channel ,ou NULL_CHANNEL si on l'a coupé.
	return (soundchannel);
}




/* -------------------------------------------------------------------------- */
/// Joue un son continu de type "ambiance sonore" audible dans une piece seulement. 
/** Le son est constant sur le rayon typique d'une piece (300 unités), puis décroit jusqu'à l'infini, 
 ou il devient inaudible. (l'infini est représenté ici par 10.000) */
/* -------------------------------------------------------------------------- */
int SoundManager::playSoundAmbiance(String SoundFile)
{
	return this->playSound(SoundFile,300,10000,true);
}


/* -------------------------------------------------------------------------- */
/// Joue un son de type "ambiance sonore" audible dans le vaisseau entier. 
/** Le son est constant sur le rayon typique du vaisseau (1.000 unités), puis décroit jusqu'à l'infini, 
 ou il devient inaudible. (l'infini est représenté ici par 10.000) */
/* -------------------------------------------------------------------------- */
int SoundManager::playSoundShip(String SoundFile, bool Loop)
{
	return this->playSound(SoundFile,1000,10000,Loop);
}



/* -------------------------------------------------------------------------- */
/// Joue un son continu de type "moving object".
/** Le son est constant sur une faible distance (15 unités) puis décroit jusqu'à l'infini
 ou il devient inaudible. (l'infini est représenté ici par 10.000) */
/* -------------------------------------------------------------------------- */
int SoundManager::playSoundMoving(String SoundFile)
{
	return this->playSound(SoundFile, 15, 10000, true);
}


/* -------------------------------------------------------------------------- */
/// Joue un son pour un objet.
/** Le son est constant sur une faible distance (15 unités) puis décroit jusqu'à l'infini
 ou il devient inaudible. (l'infini est représenté ici par 10.000) */
/* -------------------------------------------------------------------------- */
int SoundManager::playSoundObject(String SoundFile, bool Loop)
{
	return this->playSound(SoundFile, 15, 10000, Loop);
}


/* -------------------------------------------------------------------------- */
/// Joue un son bref de type "Beep".
/* -------------------------------------------------------------------------- */
int SoundManager::playSoundShort(String SoundFile, bool Loop)
{
	return this->playSound(SoundFile, 5, 100, Loop);
}

