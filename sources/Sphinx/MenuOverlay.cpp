/* ------------------------------------------------------------------------- */
// File       : MenuOverlay.cpp
// Project    : Long Forgotten Earth - Menu Principal
// Auteur     : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "MenuOverlay.h"


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
#include "CEGUI/elements/CEGUIEditbox.h"
#include <OgreMemoryMacros.h>

#include <fstream>
#include "macros.h"
#include "fmod.h"
#include "fmod_errors.h"
#include "menuFrameListener.h"

#define NB_MAX_MEMORIES 5

	
/* ------------------------------------------------------------------------- */
/// Constructeur
/* ------------------------------------------------------------------------- */
MenuOverlay::MenuOverlay(void* pFriend, SceneManager* SceneMgr, String InterfaceFile) : Interface()
{
	// Le constructeur de la surclasse INTERFACE s'exécute automatiquement

	TRACE(">> MenuOverlay()");
	mSceneMgr     = SceneMgr;
	mMainListener = pFriend;
	mGUIname      = "mainMenu";
	mLangCode     = "eng";
	
	Interface::loadInterfaceFile(InterfaceFile);
	DEBUG_LOG("   loading "+InterfaceFile);
	
	// Creation de l'imageset pour le menu principal
	CEGUI::Imageset*   giset = CEGUI::ImagesetManager::getSingleton().createImageset("../media/gui/menuImagery.imageset");
	// MenuImagery est défini dans le mainmenu.imageset
	mImageSetName="menuImagery";

	mMemoriesNumber=1;
	// Le decor par defaut est celui du Memories 4
	mCurrentMem = new Memories (mWindow, mSceneMgr, "memories04");
	mCurrentMem->showMemories();
	mPleaseWait = false;
	mIntroOnly  = false;

	this->createMenuOverlay();

	// TODO: la suite n'est plus utile , je pense ...
	this->createMemoriesOverlay();
	this->createInfosOverlay();
	this->createCreditsOverlay();
	// Initialisation de l'écran "load game"
	this->removeAnnexWindow();
	this->addGUIWindow("loadingWin");
	this->hideInterface("loadingWin");
	// Initialisation de l'écran "formulaire d'enrolement"
	this->removeAnnexWindow();
	this->addGUIWindow("enlistForm");
	this->hideInterface("enlistForm");

	CEGUI::MouseCursor::getSingleton().draw();

	TRACE("<< MenuOverlay()\n");
}

/* ------------------------------------------------------------------------- */
/// Destructeur
/* ------------------------------------------------------------------------- */
MenuOverlay::~MenuOverlay()
{
	// Destruction de l'imageset 
	CEGUI::ImagesetManager::getSingleton().destroyImageset("MainmenuImagery");

	if (mAnnexWindow) CEGUI::WindowManager::getSingletonPtr()->destroyWindow(mAnnexWindow);

	// Le destructeur de la surclasse INTERFACE s'exécute automatiquement
}



/* ------------------------------------------------------------------------- */
/// Affiche ou Masque les overlays
/* ------------------------------------------------------------------------- */
void MenuOverlay::showOverlays(bool show)
{
	if (show)  
	{
		this->showInterface("mainWin");
	}
	else       
	{ 
		this->hideInterface("mainWin");
		this->hideInterface("memoriesWin");
		this->hideInterface("creditsWin");
		this->hideInterface("infosWin");
		this->hideInterface("enlistForm");
	}
}



/* ------------------------------------------------------------------------- */
/// Creation de l'overlay "MENU PRINCIPAL"
/* ------------------------------------------------------------------------- */
void MenuOverlay::createMenuOverlay()
{
	TRACE(">> createMenuOverlay()");

	this->createGUIWindows();
	this->setAlpha(0.75);
	this->createGUIWidgets("mainWin");

	TRACE("<< createMenuOverlay\n");
}

/* ------------------------------------------------------------------------- */
/// Creation de l'overlay "CREDITS"
/* ------------------------------------------------------------------------- */
void MenuOverlay::createCreditsOverlay()
{
	TRACE(">> createCreditsOverlay()");

	this->removeAnnexWindow();
	this->addGUIWindow("creditsWin");
	this->hideInterface("creditsWin");

	TRACE("<< createCreditsOverlay\n");
}

/* ------------------------------------------------------------------------- */
/// Creation de l'overlay "MEMORIES"
/* ------------------------------------------------------------------------- */
void MenuOverlay::createMemoriesOverlay()
{
	TRACE(">> createMemoriesOverlay()");

	this->removeAnnexWindow();
	this->addGUIWindow("memoriesWin");
	this->hideInterface("memoriesWin");

	TRACE("<< createMemoriesOverlay\n");
}


/* ------------------------------------------------------------------------- */
/// Creation de l'overlay "INFOS"
/* ------------------------------------------------------------------------- */
void MenuOverlay::createInfosOverlay()
{
	TRACE(">> createInfosOverlay()");
	this->removeAnnexWindow();
	this->addGUIWindow("infosWin");
	this->hideInterface("infosWin");
	TRACE("<< createInfosOverlay\n");
}



/* ------------------------------------------------------------------------- */
/// Executer l'instruction "call=" recus de l'interface GUI
/* ------------------------------------------------------------------------- */
void MenuOverlay::executeOrder(String Order)
{
	// On extrait les paramètres, de la chaine contenant les ordres
	StringVector FuncV = StringUtil::split(Order, "(,)");
	menuFrameListener* menuListener = static_cast<menuFrameListener*>(mMainListener);


	// L'appui sur un bouton fait disparaitre les informations de debug
	showDebugOverlay(false);

	// *****************************************
	// Clic sur le bouton EXIT
	// *****************************************
	if (FuncV.at(0)=="exit")
	{
		delete (mCurrentMem);
		// on quitte l'application avec le parametre -1
		menuListener->requestMenuExit(-1);
	}
	// *****************************************
	// Clic sur le bouton LOAD
	// *****************************************
	else if (FuncV.at(0)=="loadScreen")
	{
		this->hideInterface("mainWin");
		this->removeAnnexWindow();
		this->addGUIWindow("loadingWin");
		this->showInterface("loadingWin");
	}
	// *****************************************
	// Clic sur le bouton LOAD GAME
	// *****************************************
	else if (FuncV.at(0)=="loadGame")
	{
		// on quitte l'application avec le parametre fourni
		int value = StringUtil::toReal(FuncV.at(1));
		menuListener->requestMenuExit(value);
	}
	// *****************************************
	// Clic sur le bouton MEMORIES
	// *****************************************
	else if (FuncV.at(0)=="showMemories")
	{
		mIntroOnly  = true;
		mPleaseWait = true;
		this->refreshTexts("mainWin");
		delete (mCurrentMem);
		mCurrentMem = new Memories (mWindow, mSceneMgr, "memories01");
		mCurrentMem->showMemories();
		this->hideInterface("mainWin");
//		this->hideInterface("infosWin");
		this->removeAnnexWindow();
		this->addGUIWindow ("memoriesWin");
		this->showInterface("memoriesWin");
		this->refreshTexts ("memoriesWin");
	}
	// *****************************************
	// Clic sur le bouton NEW
	// *****************************************
	else if (FuncV.at(0)=="newGame")
	{
		mIntroOnly  = false;
		mPleaseWait = true;
		this->refreshTexts("mainWin");
		delete (mCurrentMem);
		mCurrentMem = new Memories (mWindow, mSceneMgr, "memories01");
		mCurrentMem->showMemories();
		this->hideInterface("mainWin");
		this->removeAnnexWindow();
//		this->hideInterface("infosWin");
		this->addGUIWindow ("memoriesWin");
		this->showInterface("memoriesWin");
		this->refreshTexts ("memoriesWin");
	}
	// *****************************************
	// Clic sur le bouton CREDIT
	// *****************************************
	else if (FuncV.at(0)=="showCredits")
	{
		mCurrentMem->yaw(Degree(215)); 
		this->hideInterface("mainWin");
//		this->hideInterface("infosWin");
//		this->hideInterface("memoriesWin");
		this->removeAnnexWindow();
		this->addGUIWindow("creditsWin");
		this->showInterface("creditsWin");
	}
	// *****************************************
	// Clic sur le bouton INFOS
	// *****************************************
	else if (FuncV.at(0)=="showInfos")
	{
		this->hideInterface("mainWin");
		this->removeAnnexWindow();
		this->addGUIWindow("infosWin");
		this->showInterface("infosWin");
		mMemoriesNumber=1;
	}
	// *****************************************
	// Clic sur le bouton LANGUAGE
	// *****************************************
	else if (FuncV.at(0)=="setLanguage")
	{
		mLangCode = FuncV.at(1);
	}
	// *****************************************
	// Clic sur le bouton BACK
	// *****************************************
	else if (FuncV.at(0)=="back")
	{
		mPleaseWait = false;
		mIntroOnly  = false;
		this->hideInterface("enlistForm");
		this->hideInterface("loadingWin");
		this->hideInterface("creditsWin");
		this->hideInterface("infosWin");
		this->hideInterface("memoriesWin");
		this->showInterface("mainWin");
		mMemoriesNumber=1;
	}
	// *****************************************
	// Clic sur le bouton NEXT MEMORIES
	// *****************************************
	else if (FuncV.at(0)=="showNextMemories")
	{
		switch(mMemoriesNumber) 
		{
		// Quand on a atteint la derniere memories....
		case NB_MAX_MEMORIES:
			// Si on est dans l'intro, on revient au menu principal
			if (mIntroOnly)
			{
				this->hideInterface("memoriesWin");
				this->removeAnnexWindow();
				this->showInterface("mainWin");
				mMemoriesNumber=1;
				mPleaseWait=false;
				mIntroOnly =false;
			}
			else
			// Si on est dans newGame, on lance le jeu
			{
				delete (mCurrentMem);
				// On quitte l'application MENU avec le paramètre 0.
				menuListener->requestMenuExit(0);
			}
			break;

		// Gestion du cas particulier du Memories02, où il y a un formulaire a remplir
		case 1:
			delete (mCurrentMem);
			mMemoriesNumber++;
			mCurrentMem = new Memories (mWindow, mSceneMgr, "memories02");
			if (!mIntroOnly) 
			{
				// Si on est dans NewGame: on affiche le formulaire
				this->hideInterface("memoriesWin");
				this->removeAnnexWindow();
				this->addGUIWindow ("enlistForm");
				this->showInterface("enlistForm");
				this->refreshTexts ("enlistForm");
			}
			mCurrentMem->showMemories();
			this->refreshTexts("memoriesWin");
			break;
		
		default:
			// sinon, on passe a la memories suivante
			delete (mCurrentMem);
			mMemoriesNumber++;
			// Par defaut, on cache le formulaire d'enrolement
			this->hideInterface("enlistForm");
			mCurrentMem = new Memories (mWindow, mSceneMgr, "memories0"+ITOA(mMemoriesNumber));
			mCurrentMem->showMemories();
			this->refreshTexts("memoriesWin");
			break;
		}
	}
	// *****************************************
	// Clic sur le bouton SUBMIT
	// *****************************************
	else if (FuncV.at(0)=="submit")
	{
		String PlayerName="";
		String PlayerCity="";
		String CRLF      ="&#x0A;";

		CEGUI::Editbox* eBox_0 = (CEGUI::Editbox*)mWinMgr->getWindow("mainMenu/enlistForm/edit/0");
		CEGUI::Editbox* eBox_1 = (CEGUI::Editbox*)mWinMgr->getWindow("mainMenu/enlistForm/edit/1");
		// On recupère les textes saisis
		PlayerName = eBox_0->getText().c_str();
		PlayerCity = eBox_1->getText().c_str();

		// On vérifie qu'ils n'ont pas une taille nulle
		if ((PlayerCity.length()>0) && (PlayerName.length()>0))
		{
			// On joue l'audio de la signature
			String SoundFile = "../media/sounds/writing.wav";
			// Loads and decodes a static soundfile into memory.
			FSOUND_SAMPLE* Sound=FSOUND_Sample_Load(FSOUND_FREE,SoundFile.c_str(),FSOUND_LOOP_OFF,0,0);
			if (!Sound)
			{
				DEBUG_LOG ("Error Loading SoundFile:"+SoundFile);
				DEBUG_LOG (FMOD_ErrorString(FSOUND_GetError()) );
				FSOUND_Sample_Free(Sound);
			}
			// play the sound
			int SoundChannel = FSOUND_PlaySound(FSOUND_FREE, Sound);
			// set the volume 
			FSOUND_SetVolume(SoundChannel, 125);
			// On memorise les données dans un fichier de config
			writeFile("../savedgames/playerID.cfg", "name="+PlayerName, false);
			writeFile("../savedgames/playerID.cfg", "city="+PlayerCity, true);
			// On complete le fichier XML CrewBook
			// 	<entry title = "PlayerName">Biographie</entry>
			writeFile("../savedgames/CrewBook.xml", "	<entry title = '"+PlayerName+"'>", true);
			writeFile("../savedgames/CrewBook.xml", "Soldier."+CRLF+" Engineering and Software Expert."+CRLF+" Born in "+PlayerCity+CRLF+" Age 34."+CRLF+" Graduate of the University of Bordeaux.</entry>", true);
			writeFile("../savedgames/CrewBook.xml", "</logbook>", true);
			// On efface le formulaire
			this->hideInterface("enlistForm");
			this->removeAnnexWindow();
			this->addGUIWindow ("memoriesWin");
			this->showInterface("memoriesWin");
		}
	}
}




/* ------------------------------------------------------------------------- */
/// On affiche le texte demandé (soit directement, soit via une fonction)
/* ------------------------------------------------------------------------- */
void MenuOverlay::showDynamicText(CEGUI::StaticText* pText, String Order)
{
	TRACE(">> showText("+ Order +")");

	// *****************************************
	// Affiche des credits
	// *****************************************
	if (Order=="@showCredits")
	{
		// On lit le contenu du fichier de credits.
		String creditsText = readFile("../media/texts/credits.txt");
		pText->setText(creditsText);
		pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
		pText->setHorizontalFormatting(CEGUI::StaticText::HorzCentred);
		pText->setVerticalScrollbarEnabled(true);
	}
	// *****************************************
	// Affiche les infos sur le jeu
	// *****************************************
	else if (Order=="@showInfos")
	{
		// On lit le contenu du fichier d'infos
		String creditsText = readFile("../media/texts/infos.txt");
		pText->setText(creditsText);
		pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
		pText->setHorizontalFormatting(CEGUI::StaticText::LeftAligned);
	}

	// *****************************************
	// Affiche le texte associé à un memories
	// *****************************************
	else if (Order=="@showMemoriesText")
	{
		// On lit le contenu du fichier du Memories
		String MemorieText = readFile("../media/memories/"+mLangCode+"/memories0"+ITOA(mMemoriesNumber)+".txt");
		pText->setText(MemorieText);
		pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
		pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapLeftAligned );
	}
	// *****************************************
	else if (Order=="@advice")
	{
		// En general, il y a le texte standard
		// Mais quelquefois, on met un petit message
//		if (mPleaseWait) pText->setText("please WAIT");
		// Fonctionnalité inhibée: quelque chose n'est pas synchronisé
		if (mPleaseWait) pText->setText("Use ARROWS to look around");
		else             pText->setText("Use ARROWS to look around");

	}
	// *****************************************
	else if (Order=="@showEnlist")
	{
		// On lit le contenu du fichier 
		String enlistText = readFile("../media/memories/enlistcondition.txt");
		pText->setText(enlistText);
		pText->setVerticalFormatting(CEGUI::StaticText::TopAligned);
		pText->setHorizontalFormatting(CEGUI::StaticText::WordWrapLeftAligned );
		pText->setVerticalScrollbarEnabled(false);
	}
	// *****************************************
	else
		pText->setText(Order);
}




/* ------------------------------------------------------------------------- */
/// Lecture d'un fichier texte
/* ------------------------------------------------------------------------- */
String MenuOverlay::readFile(String filename)
{
using namespace std;

	String s, FileContent;

	// On ouvre le fichier en lecture
	ifstream inputfile(filename.c_str()); 

	// On le lit ligne à ligne
	while(getline(inputfile, s)) 
	{
		// On ignore les lignes de commentaires commençant par #
		if (!StringUtil::startsWith(s,"#"))
		{
			if (s.length()==0) s=" ";
			// On le stocke dans la String
			FileContent += s;
			FileContent += "\n";
		}
	} 
	inputfile.close();
	return (FileContent);
}


/* ------------------------------------------------------------------------- */
/// Ecriture d'un fichier texte
/* ------------------------------------------------------------------------- */
void MenuOverlay::writeFile(String filename, String data, bool append)
{
using namespace std;

	fstream::openmode mode;

	// On ouvre le fichier en ecriture
	if (append)  mode = (fstream::out|fstream::app); 
	else         mode = (fstream::out|fstream::trunc); 

	ofstream outputfile(filename.c_str(), mode); 
	data += "\n";
	outputfile << data;	// On ecrit les données 
	outputfile.close();
}


/* ------------------------------------------------------------------------- */
/// Affichage des informations de debug
/* ------------------------------------------------------------------------- */
void MenuOverlay::showDebugOverlay(bool show)
{
	Overlay* OverlayPage = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	if (!OverlayPage)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find Core/DebugOverlay ","showDebugOverlay" );

	if (show)  { OverlayPage->show(); }
	else       { OverlayPage->hide(); }
}


/* ------------------------------------------------------------------------- */
/// Mise a jour de l'overlay "MESSAGES"
/* ------------------------------------------------------------------------- */
void MenuOverlay::updateMessageOverlay()
{
	// On met a jour la ligne de message
    OverlayElement* mMsgArea = OverlayManager::getSingleton().getOverlayElement("message/textline");
    mMsgArea->setCaption(mWindow->getDebugText());
}


/* ------------------------------------------------------------------------- */
/// Renvoie VRAI si on est dans les memories d'intro 
/* ------------------------------------------------------------------------- */
bool MenuOverlay::isIntroRunning()
{
	return (mIntroOnly);
}
