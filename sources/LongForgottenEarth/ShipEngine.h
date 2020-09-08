/* ------------------------------------------------------------------------- */
// File       : ShipEngine.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIP_ENGINE_H_
#define _SHIP_ENGINE_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ShipElement.h"


#define MAX_ENGINE_TEMP 10000	/// Au dela de cette temperature, les moteurs s'abiment
#define MIN_ENGINE_TEMP 800	    /// Au dessous de cette temperature, les moteurs ne d�marrent pas

using namespace Ogre;

/* ------------------------------------------------------------------------- */
/// ShipEngine est un ShipElement particulier du vaisseau: le Moteur 
/* ------------------------------------------------------------------------- */
/** Il a quelques propri�t�s en plus par rapport aux elements habituels: 
 - la gestion de la temp�rature, et du pr�chauffage,
 - la gestion d'un coefficient mod�rateur de l'activit�: le mode
 - La possibilit� d'�tre arr�t� (=en mode STOPPED)
 - La possibilit� de valeurs n�gatives (retro pouss�e)
 - Pour ce qui est des propri�tes h�rit�es de ShipElement:
 - Son Activity correspond � la puissance demand�e au moteur
 - Il consomme des moles d'hydrog�ne
 - Sa Value correspond � de la pouss�e (kN) qui est convertie en acc�l�ration par le Ship lui-m�me. */
/* ------------------------------------------------------------------------- */
class ShipEngine : public ShipElement  
{
public:
	enum tMode {STOPPED_ENGINE, IDLE_ENGINE, HEATING_ENGINE, ASTEROID_MODE, PLANETARY_MODE, SOLAR_MODE, FULL_ENGINE };

	ShipEngine(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement);
	virtual ~ShipEngine();
	void    updateTemperature(Real ElapsedHours);
	void    updateValue_Hour(Real ElapsedTime);	
	void    updatePower(Real ElapsedTime);
	void    setTemperature(Real temperature);
	void    setHeatingEngine(bool On);
	void    setActivity(Real pourcentage);
	void    setPushDirection(int dir);
	bool    isEngineHeating();
	bool    isEngineStopped();
	Real    getTemperature();
	Real    getValue();
	void    setMode(tMode mode);
	tMode   getMode();
	String  getModeName();
	String  getModeName(tMode mode);
	Real    getModeCoeff();
	Real    getModeCoeff(tMode mode);
	Real    getRealActivity();
	void    switchActivity();
	void    switchHeating();

protected:
	Real    mEngineTemperature;      /// Temperature du Moteur  en degres K
	tMode   mMode;                   /// Mode g�n�ral du Moteur (short range travel, long range travel, etc)
	Real    mModeCoeff;              /// Coefficient de r�duction de la puissance du moteur en fonction de son mode.
	int     mEngineSound;            /// Channel du son du moteur
	int     mForward;                /// Pous�e=1  Retropouss�e=-1
};

#endif 
