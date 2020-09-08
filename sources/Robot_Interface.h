/* ------------------------------------------------------------------------- */
// File       : Robot_Interface.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _ROBOT_INTERFACE_H_
#define _ROBOT_INTERFACE_H_


#if _MSC_VER > 1000
#pragma once
#endif


#include "Interface.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère l'interface GUI de commande du robot (ou droid)
/* ------------------------------------------------------------------------- */
class Robot_Interface : public Interface  
{
	
public:
	Robot_Interface(void* pFriend, String InterfaceFile) ;
	virtual ~Robot_Interface();

protected:
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order);
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order);
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};
	void     executeOrder(String Order);

	void*    mRobot;

};

#endif 
