/* ------------------------------------------------------------------------- */
//  File       : ShipElement.h
//  Project    : Long Forgotten Earth
//  Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SHIP_ELEMENT_H_
#define _SHIP_ELEMENT_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ogre.h"
#include "spares.h"

using namespace Ogre;


#define MAX_RESOURCES 3		// Nombre max de resources que peut consommer un ShipElement

/* ------------------------------------------------------------------------- */
/// Objets du Vaisseau ayant un contenu variable avec le temps (par exemple: r�serve d'energie)
/* ------------------------------------------------------------------------- */
/**
	Cet objet est un peu complexe car il peut servir � simuler des �l�ments vari�s 
du vaisseau spacial. 

  L'utilisation basique est le g�n�rateur d'energie. L'�l�ment a:
 - une valeur courante qui repr�sente le niveau d'�nergie pr�sent dans la r�serve (les batteries)
 - des ressources exterieures, (par exemple, de l'hydrog�ne de l'espace)
 - une valeur d'increment: normalement, on emmagazine 300Watt par jour.
 - Pour cela, on a besoin d'une certaine quantit� de ressources, par exemple: 300 moles d'H2.

  Si l'Element est endommag�, ou s'il ne trouve pas assez de resources, ou si on lmui demande de travailler � 50%
  (activity), son rendement baisse.

*/
/* ------------------------------------------------------------------------- */
class ShipElement  
{
public:
	ShipElement(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement);
	virtual ~ShipElement();
	void    setValue(Real value);
	Real    getValue();
	String  getName();
	String  getUnit();
	Real    getPourcentageValue();
	void    updateValue_Day(int number_of_days = 1);
	void    updateValue_Hour(Real ElapsedTime);
	Real    addValue(Real addedValue);
	Real    subValue(Real subtractedValue);
	void    setResourceElement(ShipElement* ShipResource, Real QuantityPerHour);
	void    setDamage(Real DamagePercentage);
	void    addDamage(Real DamagePercentage);
	Real    getDamage();
	bool    isBroken();
	void    defineSpare(tSpare spare);
	void    setPosition(Vector3 pos);
	String  getStatus();
	void    setDayIncrementValue (Real dayIncrement);
	void    setHourIncrementValue(Real HourIncrement);
	void    setActivity(Real pourcentage);
	Real    getActivity();
	Real    getMaxValue();
	Real    getRealMaxValue();

protected:
	Real    consumeResource(int ResourceIndex, Real ElapsedHours = 24);

	String mName;                 /// Nom de la r�serve
	String mUnit;                 /// Unit� de la r�serve
	Real   mActivity;             /// Pourcentage d'activit� demand�: entre 0 et 1
	Real   mMinValue;             /// Valeur minimale de la r�serve
	Real   mMaxValue;             /// Valeur maximale de la r�serve
	Real   mCurrentValue;         /// Valeur courante de la r�serve
	Real   mDayIncrement;         /// Augmentation normale de la r�serve durant 1 journ�e
	Real   mHourIncrement;        /// Augmentation normale de la r�serve durant 1 heure
	Real   mBFpercentage;         /// Pourcentage de bon fonctionnement: entre 0 et 1
	tSpare mSpare;                /// liste des spareparts n�c�ssaires en cas de panne

	ShipElement* mResourcePtr[MAX_RESOURCES]; /// Tableau des ressources n�c�ssaires � cette r�serve
	Real         mResourceQty[MAX_RESOURCES]; /// Tableau des quantit�s de ressources n�cessaires

};

#endif 
