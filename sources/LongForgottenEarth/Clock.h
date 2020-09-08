/* -------------------------------------------------------------------------- */
// File       : Clock.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* -------------------------------------------------------------------------- */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Ogre.h"


using namespace Ogre;


// Déroulement normal du temps jeu:
// N secondes         = 1 heure de jeu
// cad 24*N secondes  = 1 jour de jeu
// cad facteur=0.100 pour N=10 (10sec->1heure) et ( 240sec= 4min -> 1jour)
// cad facteur=0.016 pour N=60 (60sec->1heure) et (1440sec=24min -> 1jour)
#define GAME_FACTOR  0.016    // facteur = 1/N


/* -------------------------------------------------------------------------- */
/// Gestion d'une horloge pour le jeu.  C'est un singleton.
/* -------------------------------------------------------------------------- */
class Clock : public FrameListener, public Singleton <Clock>
{

public:
	Clock::Clock(RenderWindow* win);
	~Clock();

	// Fonction de gestion du temps
	long       getCurrentDate();
	long       getFutureDate(int dateToAdd);
	int        getSpeed();
	void       setSpeed(int speed);
	void       setSpeedHalf();
	void       setSpeedDouble();
	void       shrinkTime(long TargetDate = 0);

	// Fonctions de gestion de l'overlay "Messages"
	void       showMessageOverlay(String texte);
	void       hideMessageOverlay();
	void       updateMessageOverlay();

	// Pour le singleton
	static Clock& getSingleton(void);
	static Clock* getSingletonPtr(void);
	RenderWindow* getOgreWindow();

protected:
    bool       frameStarted(const FrameEvent& evt);

	long     mCurrentDate;      /// Date  courante: valeur max = 2.000.000.000
	Real     mCurrentHour;      /// Heure courante: reel de précision = 7 chiffres
	int      mSpeed;
	long     mTargetDate;

	// Membres pour la gestion de l'overlay "Messages"
	RenderWindow* mWindow;                      /// Fenetre où afficher le message
	Real          mTimeUntilMessageOff ;	    /// Timeout (en secondes) avant effacement du message
	bool          mTimeOutRunning;              /// VRAI si un timeout est en cours
	
	long mPreviousDate;	// debug
	int  mPreviousHour; // debug
};

#endif
