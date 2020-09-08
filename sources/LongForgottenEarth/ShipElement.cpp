/* ------------------------------------------------------------------------- */
//  File       : ShipElement.cpp
//  Project    : Long Forgotten Earth
//  Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "macros.h"
#include "ShipElement.h"

#undef  TRACE
#define TRACE(X)


/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
ShipElement::ShipElement(String name, Real minValue, Real maxValue, String Unit, Real HourIncrement)
{
	// On mémorise les paramètres généraux
	mName         = name;
	mMinValue     = minValue;
	mMaxValue     = maxValue;
	mDayIncrement = HourIncrement * 24;
	mHourIncrement= HourIncrement;
	mUnit         = " "+Unit;

	// On initialise les membres
	mBFpercentage = 1;		// entre 0 et 1
	mCurrentValue = 0;
//	mActivity     = 0.00;	// initialement au repos = 0%	/// DEBUG
	mActivity     = 1.00;	// initialement actif = 100%
	mSpare        = NOTHING;

	for (int i=0; i<MAX_RESOURCES; i++)
	{
		mResourceQty[i] = 0;
		mResourcePtr[i] = NULL;
	}
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
ShipElement::~ShipElement()
{
}


/* ------------------------------------------------------------------------- */
/// On met à jour la valeur en fonction d'un certain nombre de jours écoulés
/* ------------------------------------------------------------------------- */
void ShipElement::updateValue_Day(int number_of_days)
{
	Real RatioQty=1;		// Ration des ressources obtenues par rapport aux ressources nécéssaires

	/// Si le reservoir est plein, on ne produit plus
	if (mCurrentValue>=mMaxValue) return;

	if (this->isBroken())   
	{
		// Si completement cassé, on ne consomme rien
		RatioQty=0;
	}
	else 
	{
		// Sinon on cosomme des resources autres
		for (int ResourceIndex=0; ResourceIndex<MAX_RESOURCES; ResourceIndex++)
		{
			/// La production est aussi tempérée par la quantité de resources disponibles
			RatioQty *= this->consumeResource(ResourceIndex);
		}
	}

	/// Si l'élément est endommagé, l'évolution de la réserve est diminuée.
	this->addValue(number_of_days * mBFpercentage * mDayIncrement * RatioQty * mActivity);
	TRACE("   "+mName+"::updated value is "+StringConverter::toString(mCurrentValue) + mUnit);
}



/* ------------------------------------------------------------------------- */
/// On met à jour la valeur en fonction d'un certain nombre de fraction d'heures écoulées
/* ------------------------------------------------------------------------- */
void ShipElement::updateValue_Hour(Real ElapsedTime)
{
	Real RatioQty=1; // Ration des ressources obtenues par rapport aux ressources nécéssaires

	TRACE (">> "+mName+"::updateValue_Hour()");
	/// Si le reservoir est plein, on ne produit plus
	if (mCurrentValue>=mMaxValue) return;

	if (this->isBroken())   
	{
		// Si completement cassé, on ne consomme rien
		RatioQty=0;
	}
	else 
	{
		// Sinon on cosomme des resources autres
		for (int ResourceIndex=0; ResourceIndex<MAX_RESOURCES; ResourceIndex++)
		{
			/// La production est aussi tempérée par la quantité de resources disponibles
			RatioQty *= this->consumeResource(ResourceIndex, ElapsedTime);
		}
	}

	/// Si l'élément est endommagé, l'évolution de la réserve est diminuée.
	this->addValue(mHourIncrement * ElapsedTime * mBFpercentage * RatioQty * mActivity);
	// TRACE("   mHourIncrement ="+StringConverter::toString(mHourIncrement) + mUnit);
	// TRACE("   ElapsedTime    ="+StringConverter::toString(ElapsedTime) + " hours");
	TRACE("   RatioQty       ="+StringConverter::toString(100*RatioQty) + " %");
	TRACE("   mActivity      ="+StringConverter::toString(100*mActivity) + " %");
	TRACE("   "+mName+"::updated value is "+StringConverter::toString(mCurrentValue) + mUnit);
}


/* ------------------------------------------------------------------------- */
/// Gestion de la consommation des resources d'autres ShipElement
/* ------------------------------------------------------------------------- */
Real ShipElement::consumeResource(int ResourceIndex, Real ElapsedHours)
{
	Real Ratio = 0;

	if(!mResourcePtr[ResourceIndex])
	{
		// Si on ne dépend pas de cet autre element: on ne consomme rien
		Ratio =1;
	}
	else
	{
		// Sinon, on consomme une partie de cette resource
		// La consommation est proportionnelle à l'activité de l'élément
		// La consommation est proportionelle au temps écoulé depuis la dernière consommation
		// Les dommages ne réduisent pas la consommation.
		Real NeededResources   = mResourceQty[ResourceIndex] * mActivity * ElapsedHours;
		Real ObtainedResources = mResourcePtr[ResourceIndex]->subValue(NeededResources);

		if (NeededResources!=0) Ratio =  ObtainedResources / NeededResources;
		TRACE(">> "+mName+"::consumeResource: ask=" + StringConverter::toString(NeededResources) +" got="+StringConverter::toString(ObtainedResources) + mResourcePtr[ResourceIndex]->getUnit()+" from '"+mResourcePtr[ResourceIndex]->getName()+"'");
		TRACE("   ratio= "+StringConverter::toString(Ratio));
	}
	return Ratio;
}


/* ------------------------------------------------------------------------- */
/// On renvoie la valeur courante de cet élément (en valeur absolue)
/* ------------------------------------------------------------------------- */
Real ShipElement::getValue()
{
	return mCurrentValue;
}


/* ------------------------------------------------------------------------- */
/// Initialise la valeur courante de cet élément (en valeur absolue)
/* ------------------------------------------------------------------------- */
void ShipElement::setValue(Real value)
{
	mCurrentValue = value;
}

/* ------------------------------------------------------------------------- */
/// On renvoie la valeur courante de cet élément (de 0 à 1)
/* ------------------------------------------------------------------------- */
Real ShipElement::getPourcentageValue()
{
	return (Real) (mCurrentValue-mMinValue)/(mMaxValue-mMinValue) ;
}

/* ------------------------------------------------------------------------- */
/// Ajoute une partie de la valeur de cet élément. On renvoie la valeur réellement ajoutée.
/* ------------------------------------------------------------------------- */
Real ShipElement::addValue(Real addedValue)
{
	TRACE(">> "+mName+"::addValue "+StringConverter::toString(addedValue) + mUnit + " to " + StringConverter::toString(mCurrentValue));
	
	// Note: On doit faire de facon subtile en raison de la précision des Real du C++
	// (il y a trop de différences entre la Value, et la valeur à ajouter)
	Real PreviousValue = mCurrentValue;

	// En cas de dépassement de la limite supérieure
	if (mCurrentValue+addedValue > mMaxValue) 
	{
		mCurrentValue = mMaxValue;
		return (mMaxValue-PreviousValue);
	}

	// En cas de dépassement de la limite inférieure
	if (mCurrentValue+addedValue < mMinValue) 
	{
		mCurrentValue = mMinValue;
		return (mMinValue-PreviousValue);
	}

	// Dans les autres cas (pas de dépassement)
	mCurrentValue += addedValue;
	return (addedValue);
}


/* ------------------------------------------------------------------------- */
/// Soustrait une partie de la valeur de cet élément
/* ------------------------------------------------------------------------- */
Real ShipElement::subValue(Real substractedValue)
{
	return (-addValue(-substractedValue));
}

/* ------------------------------------------------------------------------- */
/// L'élément peut consommer des ressources produites par un (ou plusieurs) autres éléments
/* ------------------------------------------------------------------------- */
void ShipElement::setResourceElement(ShipElement* ShipResource, Real QuantityPerHour)
{
	int found = -1;

	// On trouve un emplacement libre dans le tableau pour stocker cette consommation
	for (int i=0; i<MAX_RESOURCES; i++)
	{
		// Si on a trouvé un emplacement libre, on l'utilise
		if (mResourcePtr[i]==NULL)         found=i;
		// Si on a trouvé que cette resource exitait déjà, on la met à jour
		if (mResourcePtr[i]==ShipResource) found=i;
	}

	if (found != -1)
	{
		// Si on a trouvé un emplacement: on l'utilise
		mResourcePtr[found] = ShipResource;
		mResourceQty[found] = QuantityPerHour;
	}
	else
	{
		// Si on n'a pas trouvé d'emplacement, on met un warning
		DEBUG_LOG("Warning: "+mName+"::mResourcePtr Array is to small to store this Resource");
	}
}

/* ------------------------------------------------------------------------- */
/// On mémorise le niveau de dégradation (entre 0 et 1) (1=100% damaged)
/* ------------------------------------------------------------------------- */
void ShipElement::setDamage (Real DamagePercentage)
{
	mBFpercentage = 1-DamagePercentage;
	if (mBFpercentage<0)  mBFpercentage=0;
	if (mBFpercentage>1)  mBFpercentage=1;
}

/* ------------------------------------------------------------------------- */
/// On augmente (ou diminue) le niveau de dégradation (entre 0 et 1) (1=100% damaged)
/* ------------------------------------------------------------------------- */
void ShipElement::addDamage(Real DamagePercentage)
{
	mBFpercentage -= DamagePercentage;
	if (mBFpercentage<0)  mBFpercentage=0;
	if (mBFpercentage>1)  mBFpercentage=1;
}


/* ------------------------------------------------------------------------- */
/// Renvoie le niveau de dégradation
/* ------------------------------------------------------------------------- */
Real ShipElement::getDamage()
{
	return (1-mBFpercentage);
}

/* ------------------------------------------------------------------------- */
/// Renvoie TRUE si l'element est completement détruit
/* ------------------------------------------------------------------------- */
bool ShipElement::isBroken()
{
	return (mBFpercentage == 0);
}


/* ------------------------------------------------------------------------- */
/// Memorise les spareparts nécessaires à la réparation de cet element quand il tombe en panne.
/* ------------------------------------------------------------------------- */
void ShipElement::defineSpare(tSpare spare)
{
	mSpare = spare;
}


/* ------------------------------------------------------------------------- */
/// TODO (Si l'élement a un mesh associé)
/* ------------------------------------------------------------------------- */
void ShipElement::setPosition(Vector3 pos)
{
}

/* ------------------------------------------------------------------------- */
/// Renvoie le Nom de cet Element
/* ------------------------------------------------------------------------- */
String ShipElement::getName()
{
	return mName;
}

/* ------------------------------------------------------------------------- */
/// Renvoie les Unités de cet Element
/* ------------------------------------------------------------------------- */
String ShipElement::getUnit()
{
	return mUnit;
}

/* ------------------------------------------------------------------------- */
/// TODO
/* ------------------------------------------------------------------------- */
String ShipElement::getStatus()
{
	TRACE (">> "+mName+"::getStatus()");
	String Xstatus = "";

	// On construit le status de l'élément 
	// (si le % de Bon Fonctionnement n'est pas 100%)
	if (mBFpercentage < 100)
	{
		Xstatus  = "<item";
		Xstatus += " id='"+ITOA((int)this)  +"' ";
		Xstatus += " location='to be defined' ";
		Xstatus += " spares='"  +ITOA(mSpare)    +"' ";
		Xstatus += " ETR='"+ITOA(100-mBFpercentage)+"' ";		// "Estimated Time for Repair" selon le BF
		Xstatus += " text='This element is damaged and does not work properly. It should be repaired.' ";
		Xstatus += ">";
		Xstatus += mName;
		Xstatus += "</item> \n";
	}
	return Xstatus;
}

/* ------------------------------------------------------------------------- */
/// Change la valeur d'évolution quotidienne de l'élément.
/* ------------------------------------------------------------------------- */
void ShipElement::setDayIncrementValue(Real dayIncrement)
{
	mDayIncrement = dayIncrement;
}

/* ------------------------------------------------------------------------- */
/// Change la valeur d'évolution continue de l'élément.
/* ------------------------------------------------------------------------- */
void ShipElement::setHourIncrementValue(Real HourIncrement)
{
	mHourIncrement = HourIncrement;
}

/* ------------------------------------------------------------------------- */
/// Détermine l'activite de l'Element (de 0 à 1) (1=100% actif)
/* ------------------------------------------------------------------------- */
void ShipElement::setActivity(Real pourcentage)
{
	mActivity = pourcentage;
	if (mActivity >1) mActivity=1;
	if (mActivity <0) mActivity=0;
}

/* ------------------------------------------------------------------------- */
/// Renvoie l'activité de l'Element (de 0 à 1) (1=100% actif)
/* ------------------------------------------------------------------------- */
Real ShipElement::getActivity()
{
	return mActivity;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la valeur maximum définie pour cet Element
/* ------------------------------------------------------------------------- */
Real ShipElement::getMaxValue()
{
	return mMaxValue;
}

/* ------------------------------------------------------------------------- */
/// Renvoie la valeur maximum que peut prendre cet Element (compte tenu des dommages)
/* ------------------------------------------------------------------------- */
Real ShipElement::getRealMaxValue()
{
	return (mMaxValue * mBFpercentage);
}

