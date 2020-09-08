/* ------------------------------------------------------------------------- */
// File       : Simple_Interface.h
// Project    : Long Forgotten Earth
// Author     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#ifndef _SIMPLE_INTERFACE_H_
#define _SIMPLE_INTERFACE_H_


#if _MSC_VER > 1000
#pragma once
#endif


#include "Interface.h"

/* ------------------------------------------------------------------------- */
/// Cette classe gère des interfaces GUI simples (du type "Help Screen"
/* ------------------------------------------------------------------------- */
class Simple_Interface : public Interface
{

public:
	Simple_Interface(String InterfaceFile) ;
	virtual ~Simple_Interface();

protected:
	void     showDynamicText       (CEGUI::StaticText* pText,         String Order);
	void     showDynamicImage      (CEGUI::StaticImage* pImage,       String Order);
	void     showDynamicListbox    (CEGUI::Listbox* pListbox,         String Order);
	void     showDynamicPushButton (CEGUI::PushButton* pPushButton,   String Order) {};
	void     showDynamicProgressBar(CEGUI::ProgressBar* pProgressBar, String Order) {};
	void     executeOrder(String Order);

};

#endif 
