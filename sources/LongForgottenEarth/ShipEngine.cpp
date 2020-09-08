/* ------------------------------------------------------------------------- */
// File       : ShipEngine.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "ShipEngine.h"

#include "macros.h"
#include "TheShip.h"
#include "SoundManager.h"



#undef  TRACE
#define TRACE(X)


/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */

ShipEngine::ShipEngine(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement) : ShipElement(name, minValue, maxValue, Unit, HourIncrement)
{
	// Le constructeur de ShipElement est execut�

	// Initialisation des membres
	mMode        = STOPPED_ENGINE;
	mModeCoeff   = 0;
	mActivity    = 0;
	mEngineSound = NULL_CHANNEL;
	mForward     = 1;    // �tat initial = pouss�e
}


/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
ShipEngine::~ShipEngine()
{
}


/* ------------------------------------------------------------------------- */
/// Gestion de la temperature du moteur
/* ------------------------------------------------------------------------- */
void ShipEngine::updateTemperature(Real ElapsedHours)
{
	// ----------------------------------
	// Mise � jour de la temperature
	// ----------------------------------

	if (mMode==HEATING_ENGINE) 
		// En Mode Prechauffage, il se r�chauffe de 1600 degr�s par heure
		// cad: on passe de 0 � 800 degres en 0.5 heure
		mEngineTemperature += 1600 * ElapsedHours;

	else
		// En Mode Repos, le moteur ne chauffe pas et ne refroidit pas.
		if (mMode==IDLE_ENGINE) {}

	else
		// Si le moteur est a l'arret, il refroidit de 100 degr� par heure.
		// cad: on passe de 10.000 degres � 0 degres en 100 heures (4 jours)
		if (mMode==STOPPED_ENGINE) mEngineTemperature -= 100*ElapsedHours;

	else
		// Sinon, il se r�chauffe de 10 degr�s par heure (� pleine puissance)
		// cad: on passe de 0 � 10.000 degres en 1.000 heures (1 mois plein pots)
		mEngineTemperature += 10 * ElapsedHours * this->getRealActivity();


	// ----------------------------------
	// Controles
	// ----------------------------------

	// La temperature du moteur ne descend pas plus bas que la temperature ambiante externe
	if (mEngineTemperature < TheShip::getSingletonPtr()->mExternalTemp)   mEngineTemperature = TheShip::getSingletonPtr()->mExternalTemp;

	// Si on depasse le seuil autoris�, le moteur s'abime de 0.1% par heure.
	// cad: le moteur est foutu en 1.000 heures (1 mois)
	if (mEngineTemperature > MAX_ENGINE_TEMP)  this->addDamage(0.001);

	// Si on atteint la temperature de fonctionnement, le pr�chauffage s'arrete
	if ((mMode==HEATING_ENGINE) && (mEngineTemperature > MIN_ENGINE_TEMP+50))  mMode=IDLE_ENGINE;

	// TRACE (mName+" Temperature = "+ StringConverter::toString(mEngineTemperature));
}


/* ------------------------------------------------------------------------- */
/// Gestion de la valeur du Moteur (c'est une valeur instantann�e sans stockage) -- Override ShipElement Function
/* ------------------------------------------------------------------------- */
void ShipEngine::updateValue_Hour(Real ElapsedTime)
{
	TRACE (">> "+mName+"::updateValue_Hour()");
	TRACE ("   Initial Value ="+StringConverter::toString(mCurrentValue) + mUnit);

	Real RatioQty=1;		// Ration des ressources obtenues par rapport aux ressources n�c�ssaires

	/// Si le reservoir est plein, on ne produit plus.
	if (mCurrentValue>=mMaxValue) return;

	if (this->isBroken())   
	{
		// Si completement cass�, on ne consomme rien
		RatioQty=0;
	}
	else 
	{
		// Sinon on cosomme des resources autres
		for (int ResourceIndex=0; ResourceIndex<MAX_RESOURCES; ResourceIndex++)
		{
			/// La production est aussi temp�r�e par la quantit� de resources disponibles
			RatioQty *= this->consumeResource(ResourceIndex, ElapsedTime);
		}
	}

	// Pour le moteur, la Value est directement issue de l'activit� demand�e
	// et de l'�tat du moteur. Il n'y a pas de stockage.
	// Elle ne d�pend pas du temps �coul�.
	mCurrentValue = mHourIncrement * RatioQty * mActivity * mBFpercentage;	// * mModeCoeff; 

	// Il y a une temperature minimale pour d�marrer
	if (mEngineTemperature < MIN_ENGINE_TEMP) mCurrentValue = 0;

	TRACE("   mHourIncrement ="+StringConverter::toString(mHourIncrement)    + mUnit);
	TRACE("   RatioQty       ="+StringConverter::toString(100*RatioQty)      + " %");
	TRACE("   mActivity      ="+StringConverter::toString(100*mActivity)     + " %");
	TRACE("   mBFpercentage  ="+StringConverter::toString(100*mBFpercentage) + " %");
	TRACE("   Updated Value  ="+StringConverter::toString(mCurrentValue)     + mUnit);

}

/* ------------------------------------------------------------------------- */
/// Renvoie la temperature du moteur
/* ------------------------------------------------------------------------- */
Real ShipEngine::getTemperature()
{
	return mEngineTemperature;
}


/* ------------------------------------------------------------------------- */
/// Stocke la temperature du moteur
/* ------------------------------------------------------------------------- */
void ShipEngine::setTemperature(Real temperature)
{
	mEngineTemperature = temperature;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Mode du moteur
/* ------------------------------------------------------------------------- */
ShipEngine::tMode ShipEngine::getMode()
{
	return mMode;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le Coefficient d'efficat� du moteur (selon son mode de fonctionnement)
/* ------------------------------------------------------------------------- */
Real ShipEngine::getModeCoeff()
{
	return mModeCoeff;
}


/* ------------------------------------------------------------------------- */
/// Positionne le mode du moteur
/* ------------------------------------------------------------------------- */
void ShipEngine::setMode(tMode mode)
{
	// On reconstitue l'activit� normale du moteur (vaut 0 si mModeCoeff est nul)
	Real FullActivity = (mModeCoeff)? mActivity/mModeCoeff : 0.00 ;


	// On fixe le nouveau coefficient
	mModeCoeff = getModeCoeff(mode);

	// On d�termine l'activit� reelle du moteur (pond�r�e par le mode)
	mActivity = FullActivity * mModeCoeff;

	// On joue les sons associ�s au mode
	if ((mMode==STOPPED_ENGINE)&&(mode!=STOPPED_ENGINE))  SoundManager::getSingletonPtr()->playSoundShip("../media/sounds/sci-fi23.wav");  // sci-fi18
	if  (mode==HEATING_ENGINE)                            SoundManager::getSingletonPtr()->playSoundShip("../media/sounds/sci-fi05.wav");
	if  (mode!=STOPPED_ENGINE)                            mEngineSound = SoundManager::getSingletonPtr()->playSoundShip("../media/sounds/humming.wav", true);
	if ((mMode!=STOPPED_ENGINE)&&(mode==STOPPED_ENGINE))  
	{
		SoundManager::getSingletonPtr()->playSoundShip("../media/sounds/sci-fi21.wav");  // sci-fi16
		SoundManager::getSingletonPtr()->freeSound(mEngineSound);
	}

	// On fixe le nouveau mode
	mMode = mode;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le coefficient de r�duction de la puissance Moteur correspondant au mode demand�.
/* ------------------------------------------------------------------------- */
Real ShipEngine::getModeCoeff(tMode mode)
{
	Real Coeff;

	switch(mode) 
	{
	case STOPPED_ENGINE: Coeff = 0.00;	break;
	case IDLE_ENGINE   : Coeff = 0.00;	break;
	case HEATING_ENGINE: Coeff = 0.00;	break;
	case ASTEROID_MODE : Coeff = 0.15;	break;
	case PLANETARY_MODE: Coeff = 0.10;	break;
	case SOLAR_MODE    : Coeff = 0.40;	break;
	case FULL_ENGINE   : Coeff = 1.00;	break;
	}
	return Coeff;
}

/* ------------------------------------------------------------------------- */
/// Renvoie le mode du moteur (en ASCII)
/* ------------------------------------------------------------------------- */
String ShipEngine::getModeName()
{
	return getModeName(mMode);
}

/* ------------------------------------------------------------------------- */
/// Renvoie le nom d'un mode moteur (en ASCII)
/* ------------------------------------------------------------------------- */
String ShipEngine::getModeName(tMode mode)
{
	switch(mode) 
	{
	case STOPPED_ENGINE: return "Stopped";	    break;
	case IDLE_ENGINE   : return "Idle";	        break;
	case HEATING_ENGINE: return "Heating";	    break;
	case ASTEROID_MODE : return "Asteroid";     break;
	case PLANETARY_MODE: return "Planetary";	break;
	case SOLAR_MODE    : return "Solar System";	break;
	case FULL_ENGINE   : return "Full Engines";	break;
	default : return "";
	}
}


/* ------------------------------------------------------------------------- */
/// Demarre ou arr�te le moteur
/* ------------------------------------------------------------------------- */
void ShipEngine::switchActivity()
{
	if (mMode == STOPPED_ENGINE) 
		this->setMode(IDLE_ENGINE);
	else
		this->setMode(STOPPED_ENGINE);
}

/* ------------------------------------------------------------------------- */
/// Demarre ou arr�te le pr�chauffage moteur
/* ------------------------------------------------------------------------- */
void ShipEngine::switchHeating()
{
	switch(mMode) 
	{
	case HEATING_ENGINE:
		this->setMode(IDLE_ENGINE);
		break;
	case IDLE_ENGINE:
		this->setMode(HEATING_ENGINE);
		break;
	// Dans les autres modes, le prechauffage est inhib�
	}
}


/* ------------------------------------------------------------------------- */
/// Allume ou Eteint le pr�chauffage du moteur
/* ------------------------------------------------------------------------- */
void ShipEngine::setHeatingEngine(bool On)
{
	if (On)
	{
		if (mMode == IDLE_ENGINE) this->setMode(HEATING_ENGINE);
	}
	else
	{
		this->setMode(IDLE_ENGINE);
	}
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'�tat du pr�chauffage moteur
/* ------------------------------------------------------------------------- */
bool ShipEngine::isEngineHeating()
{
	return (mMode == HEATING_ENGINE);
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si le moteur est Stopp� (OFF).
/* ------------------------------------------------------------------------- */
bool ShipEngine::isEngineStopped()
{
	return (mMode == STOPPED_ENGINE);
}


/* ------------------------------------------------------------------------- */
/// M�morise l'activit� demand�e au moteur. Celle ci-est pond�r�e par le mode. 
/* ------------------------------------------------------------------------- */
void ShipEngine::setActivity(Real pourcentage)
{
	// Cette fonction surcharge ShipElement::setActivity()
	mActivity = pourcentage * mModeCoeff;
	// Activit� de 0 � 1   (1=100% actif)
	if (mActivity >1) mActivity=1;
	if (mActivity <0) mActivity=0;
}


/* ------------------------------------------------------------------------- */
/// Renvoie l'activit� r�elle du moteur [0..1]. 
/** Celle ci-est pond�r�e par les dommages. 
*/
/* ------------------------------------------------------------------------- */
Real ShipEngine::getRealActivity()
{
	return (mActivity * mBFpercentage);
}

/* ------------------------------------------------------------------------- */
/// On renvoie la valeur de la pouss�e (kN) g�n�r�e par le moteur.
/** Valeur sign�e, selon le sens de la pouss�e (n�gative pour la r�tropouss�e) */
/* ------------------------------------------------------------------------- */
Real ShipEngine::getValue()
{
	return mCurrentValue * mForward;
}


/* ------------------------------------------------------------------------- */
/// On fixe le sens de la pouss�e.
/** @param 1=Pouss�e vers l'avant -1=R�tropouss�e   */
/* ------------------------------------------------------------------------- */
void  ShipEngine::setPushDirection(int dir)
{
	mForward = dir;
}
