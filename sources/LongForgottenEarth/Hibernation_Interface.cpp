/* ------------------------------------------------------------------------- */
// File       : Hibernation_Interface.cpp
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Hibernation_Interface.h"

// Pour CEGUI
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIBase.h> 
#include <CEGUI/CEGUISystem.h> 
#include <CEGUI/CEGUILogger.h> 
#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUIWindowManager.h> 
#include <CEGUI/CEGUIWindow.h> 
#include "CEGUI/elements/CEGUIStaticImage.h"
#include "CEGUI/elements/CEGUIStaticText.h"
#include "CEGUI/elements/CEGUIScrollablePane.h"
#include "CEGUI/elements/CEGUIScrolledContainer.h"
#include <OgreMemoryMacros.h>


#include "Hibernation.h"
#include "TheShip.h"
#include "Clock.h"
#include "macros.h"


#define THR_FLASH 3		/// Rythme du clignotement lent


/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Hibernation_Interface::Hibernation_Interface(void* pFriend, String InterfaceFile) : Interface()
{
	// Le constructeur de la surclasse INTERFACE s'exécute automatiquement

	TRACE(">> Hibernation_Interface()");
	mHibernation  = pFriend;
	mGUIname = static_cast<Hibernation*>(pFriend)->mHibernationName;
	mDurationValue=0;

	// Initialisaton de la saisie de la durée d'hibernation
	mSliderValue[0]=0;
	mSliderValue[1]=0;
	mDurationAdjustValue=0;

	// Chargement de la description de l'interface
	Interface::loadInterfaceFile(InterfaceFile);
}


/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Hibernation_Interface::~Hibernation_Interface()
{
	// Destruction de l'imageset pour le caisson d'hibernation
//	CEGUI::ImagesetManager::getSingleton().destroyImageset("HibernationImagery");
//	if (mAnnexWindow) CEGUI::WindowManager::getSingletonPtr()->destroyWindow(mAnnexWindow);

	// Le destructeur de la surclasse INTERFACE s'exécute automatiquement
}


/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void Hibernation_Interface::executeOrder(String Order)
{
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");
	Hibernation* Hiber = static_cast<Hibernation*>(mHibernation);

	// *****************************************
	if (FuncV.at(0)=="exitGUI")
	{
		// On quitte l'interface.GUI
		Hiber->requestExitInterface(0.3);
	}
	// *****************************************
	else if (FuncV.at(0)=="actionDoor")
	{
		if (Hiber->isClosed() && !Hiber->mBroken)   Hiber->openDoor();
		if (Hiber->isOpened() && !Hiber->mBroken)   Hiber->closeDoor();
		Hiber->requestExitInterface(0.3);
	}
	// *****************************************
	else if (FuncV.at(0)=="start")
	{
		if (   (Hiber->isClosed()) 
			&& (Hiber->mSubject) 
			&& (Hiber->mInitialDuration>0) 
			&& (Hiber->mPhase==Hibernation::IDLE)  
			&& (!Hiber->mBroken) 
			&& (TheShip::getSingletonPtr()->getAlertLevel()==0) )
		{
			// Si toutes les conditions sont bonnes, on lance la sequence d'hibernation
			Hiber->startSequence();
		}
	}
	// *****************************************
	else if (FuncV.at(0)=="wakeup")
	{
		if (   (Hiber->mPhase==Hibernation::OPERATING) 
			&& (!Hiber->mBroken) 
			&& (Hiber->mSubject->getStatus()!="Critical") )
		{
			// Si toutes les conditions sont bonnes, on arrete la sequence
			Hiber->endSequence();
		}
	}
	// *****************************************
	else if (FuncV.at(0)=="setBroken")
	{
		if (FuncV.at(1)=="true" ) Hiber->setBroken(true);
		if (FuncV.at(1)=="false") Hiber->setBroken(false);
	}
	// *****************************************
	// Creation d'une fenetre annexe
	// *****************************************
	else if (FuncV.at(0)=="addWindow")
	{
		// Conditions d'apparition:
		// - La fenetre durationWin n'apparait que si le caisson est idle
		// - La fenetre medicalWin  apparait dans tous les cas
		// - La fenetre annexe ne doit pas etre deja active
		if ((FuncV.at(1)!="durationWin") || (Hiber->mPhase==Hibernation::IDLE)) 
		{
			if (mAnnexWindow==NULL)
			{
				// On cree la fenetre demandée dans le premier parametre
				this->addGUIWindow(FuncV.at(1));
				// On memorise le second parametre
				mScanType = FuncV.at(2);
				// On rafraichit les textes et les images de la nouvelle fenetre
				this->refreshTexts(FuncV.at(1));
				this->refreshImages(FuncV.at(1));	
			}
		}
	}
	// *****************************************
	else if (FuncV.at(0)=="setDuration")
	{
		// On mémorise la durée de l'hibernation à effectuer
		Hiber->setDuration(mDurationValue);
		// On efface la fenetre annexe (mais on ne la détruit pas)
		this->removeAnnexWindow();
	}
	// *****************************************
	else if (FuncV.at(0)=="cancel")
	{
		// On efface la fenetre annexe (mais on ne la détruit pas)
		this->removeAnnexWindow();
	}
	// *****************************************
	else if (FuncV.at(0)=="changeDuration")
	{
		mDurationAdjustValue += StringUtil::toReal(FuncV.at(1));
		refreshTexts("durationWin");
	}
	// *****************************************
	else if (FuncV.at(0)=="climb")
	{
		if (Hiber->mStatus==Hibernation::OPENED)
		{
			Hiber->climb();
			Hiber->requestExitInterface(0.3);
		}
	}
}


/* ------------------------------------------------------------------------- */
/// On affiche l'image demandée (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Hibernation_Interface::showDynamicImage(CEGUI::StaticImage* pImage, String Order)
{
	Hibernation* Hiber  = static_cast<Hibernation*>(mHibernation);

	// *****************************************
	if (!Hiber->mSubject)
	{
		pImage->setImage("HibernationImagery","empty");
	}
	// *****************************************
	else if (Order=="@ScannedImage")
	{
		if (mScanType=="IR")   /* Image du corps entier en infra-rouge (fixe) */
		{
			short status = Hiber->mSubject->getNecrosedStatus();
			if (status==Body::DEAD)        pImage->setImage("HibernationImagery","IRdead");
			else if (status==Body::ALL_OK) pImage->setImage("HibernationImagery","IRok");
			else                           pImage->setImage("HibernationImagery","IRbad");
		}
		else if (mScanType=="XR")    /* Image du corps entier aux rayons-X (fixe) */
		{
			short status = Hiber->mSubject->getBoneStatus();
			if (status==Body::DEAD)        pImage->setImage("HibernationImagery","XRdead");
			else if (status==Body::ALL_OK) pImage->setImage("HibernationImagery","XRok");
			else                           pImage->setImage("HibernationImagery","XRbad");
		}
		else if (mScanType=="EEG")   /* Electro-Encephalo-Grame (scrolling) */
		{
			short status = Hiber->mSubject->getHeadStatus();
			if (status==Body::DEAD)        pImage->setImage("HibernationImagery","EEGdead");
			else if (status==4)            pImage->setImage("HibernationImagery","EEGok");
			else                           pImage->setImage("HibernationImagery","EEGbad");
		}
		else if (mScanType=="ECG")   /* Electro-Cardio-Grame (scrolling) */
		{
			short status = Hiber->mSubject->getTorsoStatus();
			if (status==Body::DEAD)        pImage->setImage("HibernationImagery","ECGdead");
			else if (status==4)            pImage->setImage("HibernationImagery","ECGok");
			else                           pImage->setImage("HibernationImagery","ECGbad");

		}
	}
}


/* ------------------------------------------------------------------------- */
/// On fait scroller l'image demandée.
/* ------------------------------------------------------------------------- */
void Hibernation_Interface::showScrollingImage(CEGUI::ScrollablePane* pPane, int scroll)
{
	CEGUI::Window* subwindow = pPane->getContentPane()->getChildAtIdx(0); 
	if (!subwindow) return; 
	subwindow->setPosition(CEGUI::Absolute, CEGUI::Point(scroll, 0));		
}

/* ------------------------------------------------------------------------- */
/// Gestion des ProgressBar de la GUI
/* ------------------------------------------------------------------------- */
void Hibernation_Interface::showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order)
{}


/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void Hibernation_Interface::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	Hibernation* Hiber  = static_cast<Hibernation*>(mHibernation);

	mDurationValue = mSliderValue[0] + mSliderValue[1] + mDurationAdjustValue;
	if (mDurationValue<0) 
	{
		mDurationValue=0;
		mDurationAdjustValue=0;
	}

	// *****************************************
	// Fenetre principale
	// *****************************************
	if (Order=="@showTemperature")
	{
		pText->setText(ITOA(Hiber->mTemperature)+" oC");
	}
	// *****************************************
	else if (Order=="@showAmbient")
	{
		if (Hiber->mPhase==Hibernation::STARTING)       pText->setText("Cooling");
		else if (Hiber->mPhase==Hibernation::OPERATING) pText->setText("Cryo. Op.");
		else if (Hiber->mPhase==Hibernation::ENDING)    pText->setText("Warming up");
		else if (Hiber->mPhase==Hibernation::IDLE)      pText->setText("Ambiant");
	}
	// *****************************************
	else if (Order=="@showAlert")
	{
		if (TheShip::getSingletonPtr()->getAlertLevel()>0)
		{
			pText->setBackgroundColours(mYellow);  
			if (mFlash_fast)    pText->setText("INTERRUPT"); 
			else                pText->setText("Ship Alert"); 
		}
		else
		{
			pText->setBackgroundColours(mGreen);  
			pText->setText("Clear"); 
		}
	}
	// *****************************************
	else if (Order=="@showCaution")
	{
		pText->setAlpha(1); 
		if (Hiber->mBroken)	
		{
			if (mFlash_fast) pText->setAlpha(0.5); 
			pText->setBackgroundColours(mRed);  
			pText->setText("WARNING"); 
		}
		else
		{
			pText->setBackgroundColours(mGreen);  
			pText->setText("status ok"); 
		}
	}
	// *****************************************
	else if (Order=="@total_days")
	{
		pText->setText(ITOA(mDurationValue)+" days");
	}
	// *****************************************
	else if (Order=="@showInitialDuration")
	{
		int days = Hiber->mInitialDuration;
		pText->setText("Initial");
		if (days==0)    
		{
			pText->setBackgroundColours(mWhite);
		}
		else 
		{
			pText->setBackgroundColours(mGreen);  
			if (mFlash_slow) pText->setText(ITOA(days)+" days"); 
		}
	}
	// *****************************************
	else if (Order=="@showElapsedDuration")
	{
		long days = 0;
		pText->setText("Elapsed");
		pText->setBackgroundColours(mGreen);
		if (Hiber->mPhase!=Hibernation::IDLE) 
			days=Clock::getSingletonPtr()->getCurrentDate() - Hiber->mInitialDate;

		if (Hiber->mBroken)                     {pText->setBackgroundColours(mYellow); pText->setText("0xFFFFFFFF");}
		else if (Hiber->mInitialDuration==0)    {pText->setBackgroundColours(mWhite);  }
		else if (Hiber->mPhase==Hibernation::OPERATING) {pText->setText(ITOA(days)+" days");}
		else if (Hiber->mPhase==Hibernation::ENDING)    {pText->setText(ITOA(days)+" days");}
		else if (mFlash_slow)                           {pText->setText(ITOA(days)+" days");   }
	}
	// *****************************************
	else if (Order=="@showRemainingDuration")
	{
		long days = 0;
		pText->setText("Remaining");
		pText->setBackgroundColours(mGreen);  
		if (Hiber->mPhase!=Hibernation::IDLE) 
			days = Hiber->mFinalDate - Clock::getSingletonPtr()->getCurrentDate();

		if (Hiber->mBroken)                  {pText->setBackgroundColours(mYellow); pText->setText("0xFFFFFFFF"); }
		else if (Hiber->mInitialDuration==0) {pText->setBackgroundColours(mWhite);  }
		else if (days<0)                     {pText->setBackgroundColours(mYellow); pText->setText("overrun"); }
		else if (Hiber->mPhase==Hibernation::OPERATING) {pText->setText(ITOA(days)+" days");}
		else if (Hiber->mPhase==Hibernation::ENDING)    {pText->setText(ITOA(days)+" days");}
		else if (mFlash_slow)                           {pText->setText(ITOA(days)+" days");   }
	}
	// *****************************************
	else if (Order=="@showCover")
	{
		if      (Hiber->mBroken)                       {pText->setText("Locked"); pText->setBackgroundColours(mRed);  }
		else if (Hiber->mStatus==Hibernation::CLOSED)  {pText->setText("Closed"); pText->setBackgroundColours(mGreen);}
		else if (Hiber->mStatus==Hibernation::OPENED)  {pText->setText("Open");   pText->setBackgroundColours(mYellow);}
		else                                           {pText->setText(" * * * ");pText->setBackgroundColours(mYellow);}
	}
	// *****************************************
	else if (Order=="@showLocked")
	{
		if      (Hiber->mBroken)  {pText->setText("Locked");  pText->setBackgroundColours(mRed);  }
		else if (Hiber->mLocked)  {pText->setText("Locked");  pText->setBackgroundColours(mYellow);}
		else                      {pText->setText("Unlocked");pText->setBackgroundColours(mGreen);}
	}	// *****************************************
	else if (Order=="@showPhase")
	{
		if      (Hiber->mBroken)                        {pText->setText("DEFAULT");            pText->setBackgroundColours(mRed);  }
		else if (Hiber->mPhase==Hibernation::IDLE)      {pText->setText("Idle");               pText->setBackgroundColours(mGreen);}
		else if (Hiber->mPhase==Hibernation::STARTING)  {pText->setText("Starting Sequence");  pText->setBackgroundColours(mYellow);}
		else if (Hiber->mPhase==Hibernation::OPERATING) {pText->setText("Hiberning");          pText->setBackgroundColours(mYellow);}
		else if (Hiber->mPhase==Hibernation::ENDING)    {pText->setText("Awaking Sequence");   pText->setBackgroundColours(mYellow);}
		else                              pText->setText(" ");
	}
	// *****************************************
	else if (Order=="@showSubjectStatus")
	{
		if (Hiber->mSubject)
		{
			pText->setText(Hiber->mSubject->getStatus());
			int vitality = Hiber->mSubject->getVitalite();
			if (vitality==0) pText->setBackgroundColours(mRed);
			else if (vitality<50) pText->setBackgroundColours(mYellow);
			else                  pText->setBackgroundColours(mGreen);
		}
		else
		{
			pText->setText("empty");
			pText->setBackgroundColours(mYellow);
		}
	}
	// *****************************************
	else if (Order=="@showSubjectName")
	{
		if (Hiber->mSubject)
		{
			pText->setText(Hiber->mSubject->getName());
		}
		else
		{
			pText->setText("Caisson Hibernation");
		}
	}
	// *****************************************
	// Fenetre Annexe DURATION
	// *****************************************
	else if (Order=="@showYears")
	{
		int y = mDurationValue/365;
		pText->setText(ITOA(y)+" years");
	}
	// *****************************************
	else if (Order=="@showMonthes")
	{
		int y = mDurationValue/365;
		int m = (mDurationValue - 365*y)/30;
		pText->setText(ITOA(m)+" monthes");
	}
	// *****************************************
	else if (Order=="@showDays")
	{
		int days = mDurationValue;
		int y = days/365;
		int m = (days - 365*y)/30;
		int d = days - 365*y - 30*m;
		pText->setText(ITOA(d)+" days");
	}
	// *****************************************
	// Fenetre Annexe DIAGNOSTIC
	// *****************************************
	else if (Order=="@showDiag")
	{
		if (Hiber->mSubject)
		{
			//Root::getSingletonPtr()->getAutoCreatedWindow()->setDebugText("Vitality= "+ITOA(Hiber->mSubject->getVitalite()));
			if (mScanType=="IR") 		// Scan IR: on fait un diagnostic sur l'état des nécroses
			{
				short status = Hiber->mSubject->getNecrosedStatus();
				if (status==Body::DEAD)        pText->setText("No internal body temperature detected. The subject is probably dead.");
				else if (status==Body::ALL_OK) pText->setText("The temperature of the body is uniform. Everything looks normal.");
				else                           pText->setText("Some members have an abnormal temperature profile. They can be necrosed and an amputation can be needed.");
			}
			if (mScanType=="XR")		// Scan XR: on fait un diagnostic sur l'état des os
			{
				short status = Hiber->mSubject->getBoneStatus();
				if (status==Body::DEAD)        pText->setText("All bones are totally decalcified. The subject is probably dead.");
				else if (status==Body::ALL_OK) pText->setText("Normal skeleton constitution. The subject looks in good shape.");
				else                           pText->setText("Some bones have a very low calcium level. A treatment against decalcification is strongly recommanded.");
			}
			if (mScanType=="ECG")		// ECG : on fait un diagnostic sur l'état du torse
			{
				short status = Hiber->mSubject->getTorsoStatus();
				if (status==Body::DEAD)        pText->setText("Elecro-CardioGram is flat. The subject may be dead.");
				else if (status==4)            pText->setText("The cardiac rythm is slow, due to hibernation, but is normal.");
				else                           pText->setText("The cardiac rythm is far below the expected level. Medic have been used without noticiable effect. A surgical operation may be needed.");
			}
			if (mScanType=="EEG")		// EEG : on fait un diagnostic sur l'état de la tête
			{
				short status = Hiber->mSubject->getHeadStatus();
				if (status==Body::DEAD)        pText->setText("Electro-EncephaloGram detects no cerebral activity. The subject can be considered dead.");
				else if (status==4)            pText->setText("The cerebral activity is low, due to hibernation, but normal. Alpha, Beta and Theta waves are normal.");
				else                           pText->setText("Alpha and Beta waves are flat. Theta waves are irregular. The subject is in a comatic state.");
			}
			// Bouton->setText("Unable to perform the requested medical scan.");
		}
		else
		{
			pText->setText("No subject present in the Hibernation Stasis Bed.");
		}
	}
	// *****************************************
	else
		pText->setText(Order);
}




/* ------------------------------------------------------------------------- */
/// Gere un signal de cligotement pour certains textes
/* ------------------------------------------------------------------------- */
void Hibernation_Interface::swapFlash()
{
	// Gestion du clignotement des alarmes
	mFlash_fast = !mFlash_fast;
	mFlash++;
	if (mFlash==THR_FLASH) 
	{
		mFlash=0; 
		mFlash_slow = !mFlash_slow;
	}
}
