/* -------------------------------------------------------------------------- */
// File       : Clock.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#include "Clock.h"
#include "macros.h"
#include "TheShip.h"

#include "mainApplication.h"	// debug

#define MAX_SPEED    8192
#define INITIAL_DATE 0

/* -------------------------------------------------------------------------- */
/// Constructor. D�marrage de l'horloge
/* -------------------------------------------------------------------------- */
Clock::Clock(RenderWindow* win)
{
	mWindow = win;
	mCurrentDate = INITIAL_DATE;		// une date dans le futur					
	mSpeed =1;							// vitesse de d�roulement du temps normale
	// On d�marre l'horloge
	Root::getSingletonPtr()->addFrameListener(this);
}

/* -------------------------------------------------------------------------- */
/// Destructeur
/* -------------------------------------------------------------------------- */
Clock::~Clock()
{
}


template<> Clock* Singleton<Clock>::ms_Singleton = 0; 
/* ------------------------------------------------------------------------- */
/// Override the Singleton Class method
/* ------------------------------------------------------------------------- */
Clock* Clock::getSingletonPtr(void)
{
	return ms_Singleton;
}



/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
bool Clock::frameStarted(const FrameEvent& evt)
{
	
	/// TODO : � faire en fonction du temps et de l'acc�l�ration

	

	/// RealElapsedTime est le temps qui s'est r�ellement �coul� depuis la frame pr�c�dente (en secondes)
	/// CompressedElapsedTime est le temps de jeu qui s'est �coul� depuis la frame pr�c�dente (en heures)
	/// facteur est l'acc�l�ration normale (inverse en 1/N) du temps pour le jeu
	/// mSpeed est l'acc�l�ration exceptionnelle du temps (quand on hiberne, par exemple)
	Real RealElapsedTime = evt.timeSinceLastFrame;	
	Real CompressedElapsedTime = RealElapsedTime * GAME_FACTOR * mSpeed;	
	mCurrentHour += CompressedElapsedTime; 

	// -------------------------------------------------------
	// -------------------------------------------------------
	// On met � jour certains �l�ments du Ship
	/// TODO: est sa place ici ou dans TheShip ?(avec facteur de compression temporelle ?)
	TheShip::getSingletonPtr()->updateElements_Hour(CompressedElapsedTime);

	// On met a jour les messages OSD
	if (mTimeOutRunning)
	{
		if (mTimeUntilMessageOff > 0) 	
		{
			this->updateMessageOverlay();
			// On d�cremente le timeout des messages OSD
			mTimeUntilMessageOff -= evt.timeSinceLastFrame;
		}
		else	
		{
			// Quand le timeout est expir�, on efface le message.
			this->hideMessageOverlay();
		}
	}

	
	// Gestion du changement de jour
	if (mCurrentHour>24) 
	{
		mCurrentHour=0;
		mPreviousHour=0;
		mCurrentDate++;
		DEBUG_LOG ("--- new day ---"+ ITOA(mCurrentDate));
		DEBUG_LOG ("mSpeed="+ ITOA(mSpeed));
		TheShip::getSingletonPtr()->updateElements_Day();
	}

	// Gestion du changement d'heure
	int currentHour = int(mCurrentHour); // (mCurrentHour*100) % 100;
	if (currentHour > mPreviousHour) 
	{
		DEBUG_LOG ("--- new hour ---" + ITOA(currentHour));
		//TheShip::getSingletonPtr()->updateElements_Day(); /// DEBUG
		mPreviousHour = currentHour;
	}

	// Root::getSingletonPtr()->getAutoCreatedWindow()->setDebugText("Current Date= "+ITOA(mCurrentDate)+" "+ITOA(RealElapsedTime)+" "+ITOA(CompressedElapsedTime));

	// Pour les tests: A J+3 on lance la musique
	if ((mCurrentDate==INITIAL_DATE+3) && (mCurrentHour==0)) mainApplication::getSingletonPtr()->playMusic("../media/music/ambiance1.mp3");




	return true;
}


/* ------------------------------------------------------------------------- */
/// Renvoie la date courante (en nombre de jours)
/* ------------------------------------------------------------------------- */
long Clock::getCurrentDate()
{
	return mCurrentDate;
	// DATE = COleDateTime
}

/* ------------------------------------------------------------------------- */
/// Renvoie la date courante (incr�ment�e de la valeur fournie)
/* ------------------------------------------------------------------------- */
long Clock::getFutureDate(int DateToAdd)
{
	return mCurrentDate + DateToAdd;
}


/* ------------------------------------------------------------------------- */
/// Accelere le d�roulement du temps d'un facteur 2
/* ------------------------------------------------------------------------- */
void Clock::setSpeedDouble()
{
	// On accelere le d�roulement du temps d'un facteur 2
	// On ne monte pas au dessus de MAX_SPEED
	if (mSpeed<MAX_SPEED) mSpeed=mSpeed*2;
	else mSpeed=MAX_SPEED;
}

/* ------------------------------------------------------------------------- */
/// Acc�l�re (puis ralentit) le temps � chaque appel, de fa�on � s'approcher de TargetTime de la mani�re la plus rapide et la plus lisse possible.
/* ------------------------------------------------------------------------- */
void Clock::shrinkTime(long TargetDate)
{
	// Si elle n'est pas nulle, on m�morise la date cible.
	if (TargetDate>0) mTargetDate=TargetDate;

	// On determine la date � partir de laquelle, il faut commencer � ralentir
	long Step = mCurrentDate-mPreviousDate;
	if (Step <0) Step=0;

	// Si on est presque � la fin, on ralentit le temps
	if (mCurrentDate+(2*Step) > mTargetDate) this->setSpeedHalf();

	// Si on est loin de la fin, on acc�l�re le temps
	if (mCurrentDate+(4*Step) < mTargetDate) this->setSpeedDouble();


	// DEBUG_LOG(">> shrinkTime: curr="+ITOA(mCurrentDate) + " target="+ITOA(mTargetDate)+" Speed="+ITOA(mSpeed)+" step= "+ITOA(Step));
	mPreviousDate = mCurrentDate;
}

/* ------------------------------------------------------------------------- */
/// Ralenti le d�roulement du temps d'un facteur 2
/* ------------------------------------------------------------------------- */
void Clock::setSpeedHalf()
{
	// On ne descend pas en dessous de 1
	if (mSpeed>1) mSpeed=mSpeed/2;
	else mSpeed=1;
}

/* ------------------------------------------------------------------------- */
/// Accelere ou ralenti le d�roulement du temps
/* ------------------------------------------------------------------------- */
void Clock::setSpeed(int speed)
{
	mSpeed=speed;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le facteur d'acc�l�ration du temps
/* ------------------------------------------------------------------------- */
int Clock::getSpeed()
{
	return (mSpeed);
}



/* -------------------------------------------------------------------------- */
/// Affichage de l'overlay "MESSAGES"
/* -------------------------------------------------------------------------- */
void Clock::showMessageOverlay(String texte)
{
	TRACE("-->"+texte);
	mWindow->setDebugText(texte);
	Overlay* OverlayPage = (Overlay*)OverlayManager::getSingleton().getByName("sphinx/messages");
	if (!OverlayPage)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find overlay sphinx/messages","showMessageOverlay" );

	OverlayPage->show(); 
	mTimeUntilMessageOff=2; // en secondes
	mTimeOutRunning=true;
}

/* -------------------------------------------------------------------------- */
/// Masque l'overlay "MESSAGES"
/* -------------------------------------------------------------------------- */
void Clock::hideMessageOverlay()
{
	Overlay* OverlayPage = (Overlay*)OverlayManager::getSingleton().getByName("sphinx/messages");
	if (!OverlayPage)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find overlay sphinx/messages","hideMessageOverlay" );
	OverlayPage->hide(); 
	mTimeOutRunning=false;
}

/* -------------------------------------------------------------------------- */
/// Mise a jour de l'overlay "MESSAGES"
/* -------------------------------------------------------------------------- */
void Clock::updateMessageOverlay()
{
	// On met a jour la ligne de message
    OverlayElement* mMsgArea = OverlayManager::getSingleton().getOverlayElement("message/textline");
    mMsgArea->setCaption(mWindow->getDebugText());
}

/* -------------------------------------------------------------------------- */
/// Renvoie un pointeur sur la fenetre principale de Ogre.
/** Cela peut servir � differents endroits du programme */
/* -------------------------------------------------------------------------- */
RenderWindow* Clock::getOgreWindow()
{
	return mWindow;
}
