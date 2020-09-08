/* ------------------------------------------------------------------------- */
// File       : Hibernation_Interface.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _HIBERNATION_INTERFACE_H_
#define _HIBERNATION_INTERFACE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Interface.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI de commande des caissons d'hibernation.
/* ------------------------------------------------------------------------- */
class Hibernation_Interface : public Interface
{
public:
	Hibernation_Interface(void* pFriend, String InterfaceFile) ;
	virtual ~Hibernation_Interface();
	void    swapFlash();

protected:
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order);
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order) {};
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order);
	void     showScrollingImage    (CEGUI::ScrollablePane* pPane,     int scroll);
	void     executeOrder(String Order);

	void*    mHibernation;
	Real     mFlash;
	bool     mFlash_fast;
	bool     mFlash_slow;
	int      mDurationValue;
	String   mScanType;
	Real     mDurationAdjustValue;
};

#endif
