# Microsoft Developer Studio Project File - Name="LFE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LFE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LFE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LFE.mak" CFG="LFE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LFE - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LFE - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LFE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "E:\sphinx\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "E:\SDK\OgreSDK\include" /I "E:\SDK\fmod\api\inc" /I "E:\SDK\xerces-c_2_6_0\include" /I "E:\SDK\OgreSDK\include\CEGUI" /I "E:\sphinx\libnoise\include" /I "E:\sphinx\LFE\lib_ois\includes" /I "E:\SDK\OgreOpcode\include" /I "E:\SDK\OgreOpcode\opcode132" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ogreMain.lib fmodvc.lib CEGUIbase.lib OgreGUIRenderer.lib xerces-c_2.lib libnoise.lib dinput8.lib dxguid.lib OgreOpcode.lib Opcode.lib /nologo /subsystem:windows /machine:I386 /libpath:"E:\sdk\OgreSDK\lib" /libpath:"E:\sdk\OgreOpcode\lib" /libpath:"E:\sphinx\libnoise\lib" /libpath:"E:\SDK\OgreOpcode\Opcode132\lib" /libpath:"E:\SDK\fmod\api\lib" /libpath:"E:\SDK\xerces-c_2_6_0\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "LFE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "E:\sphinx\debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "E:\SDK\OgreSDK\include" /I "E:\SDK\fmod\api\inc" /I "E:\SDK\xerces-c_2_6_0\include" /I "E:\SDK\OgreSDK\include\CEGUI" /I "E:\sphinx\libnoise\include" /I "E:\sdk\ois\includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ogremain_d.lib fmodvc.lib ceguibase_d.lib OgreGUIRenderer_d.lib CEGUITaharezLook_d.lib DirectX9GUIRenderer_d.lib DirectX81GUIRenderer_d.lib xerces-c_2D.lib ois_d.lib dinput8.lib dxguid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"E:\sdk\OgreSDK\lib" /libpath:"E:\SDK\fmod\api\lib" /libpath:"E:\SDK\cegui_mk2\lib" /libpath:"E:\SDK\cegui_mk2\dependencies\lib" /libpath:"E:\SDK\OIS\lib" /libpath:"E:\sphinx\libnoise\lib"

!ENDIF 

# Begin Target

# Name "LFE - Win32 Release"
# Name "LFE - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "StarGen"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib_stargen\SG_Atmosphere.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Atmosphere.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_const.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_File.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_File.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_File_TXT.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_File_TXT.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_File_XML.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_File_XML.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_FileWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_FileWriter.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Gas.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Gas.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Planet.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_planet.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_SolarSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_SolarSystem.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Star.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Star.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Stardust.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_Stardust.h
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_stargen\SG_utils.h
# End Source File
# End Group
# Begin Group "celestia"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\bytes.h
# End Source File
# Begin Source File

SOURCE=.\CatalogStar.cpp
# End Source File
# Begin Source File

SOURCE=.\CatalogStar.h
# End Source File
# Begin Source File

SOURCE=.\constellation.cpp
# End Source File
# Begin Source File

SOURCE=.\constellation.h
# End Source File
# Begin Source File

SOURCE=.\Stargen_XMLparser.h
# End Source File
# Begin Source File

SOURCE=.\StarsDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\StarsDatabase.h
# End Source File
# End Group
# Begin Group "TinyXML"

# PROP Default_Filter "*.cpp,*.h"
# Begin Source File

SOURCE=.\lib_tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=.\lib_tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\lib_tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "OpenIS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib_OIS\includes\OISConfig.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\OISEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISEffect.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISEvents.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISException.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\OISForceFeedback.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISForceFeedback.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\OISInputManager.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISInputManager.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISInterface.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISJoyStick.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\OISKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISMouse.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\OISObject.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISObject.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\OISPrereqs.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\win32\Win32ForceFeedback.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\win32\Win32ForceFeedback.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\win32\Win32InputManager.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\win32\Win32InputManager.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\win32\Win32JoyStick.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\win32\Win32JoyStick.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\win32\Win32KeyBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\win32\Win32KeyBoard.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\src\win32\Win32Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\win32\Win32Mouse.h
# End Source File
# Begin Source File

SOURCE=.\lib_OIS\includes\win32\Win32Prereqs.h
# End Source File
# End Group
# Begin Group "opCode"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\AnimatedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Body.cpp
# End Source File
# Begin Source File

SOURCE=.\Button.cpp
# End Source File
# Begin Source File

SOURCE=.\Circle3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Clock.cpp
# End Source File
# Begin Source File

SOURCE=.\Computer.cpp
# End Source File
# Begin Source File

SOURCE=.\Computer_Base.cpp
# End Source File
# Begin Source File

SOURCE=.\Computer_Control.cpp
# End Source File
# Begin Source File

SOURCE=.\Computer_logs.cpp
# End Source File
# Begin Source File

SOURCE=.\Computer_Maintenance.cpp
# End Source File
# Begin Source File

SOURCE=.\Computer_Navigation.cpp
# End Source File
# Begin Source File

SOURCE=.\Door.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicLines.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicRenderable.cpp
# End Source File
# Begin Source File

SOURCE=.\GalaxyBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Gradient.cpp
# End Source File
# Begin Source File

SOURCE=.\Hibernation.cpp
# End Source File
# Begin Source File

SOURCE=.\Hibernation_Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface_XMLparser.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyboardTranslator.cpp
# End Source File
# Begin Source File

SOURCE=.\LensFlare.cpp
# End Source File
# Begin Source File

SOURCE=.\Line3d.cpp
# End Source File
# Begin Source File

SOURCE=.\mainApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\mainApplicationListener.cpp
# End Source File
# Begin Source File

SOURCE=.\MovableText.cpp
# End Source File
# Begin Source File

SOURCE=.\lib_noiseutils\noiseutils.cpp
# End Source File
# Begin Source File

SOURCE=.\NPC_Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Complex.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Gas.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Granit.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_ice.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Rock.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Venusian.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\Robot.cpp
# End Source File
# Begin Source File

SOURCE=.\Robot_Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Room.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene_XMLparser.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipElement.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipLevel_XMLparser.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipLight.cpp
# End Source File
# Begin Source File

SOURCE=.\Simple_Interface.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SpaceShip.cpp
# End Source File
# Begin Source File

SOURCE=.\specialFX.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_Archipel.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_Asteroide.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_Planet.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_SolarSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\ss_Star.cpp
# End Source File
# Begin Source File

SOURCE=.\Stargen_XMLparser.cpp
# End Source File
# Begin Source File

SOURCE=.\TextList.cpp
# End Source File
# Begin Source File

SOURCE=.\TheShip.cpp
# End Source File
# Begin Source File

SOURCE=.\WaypointNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\WinMain.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLparser.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLparser_T.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AnimatedObject.h
# End Source File
# Begin Source File

SOURCE=.\Body.h
# End Source File
# Begin Source File

SOURCE=.\Button.h
# End Source File
# Begin Source File

SOURCE=.\Circle3D.h
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\Clock.h
# End Source File
# Begin Source File

SOURCE=.\Computer.h
# End Source File
# Begin Source File

SOURCE=.\Computer_Base.h
# End Source File
# Begin Source File

SOURCE=.\Computer_Control.h
# End Source File
# Begin Source File

SOURCE=.\Computer_logs.h
# End Source File
# Begin Source File

SOURCE=.\Computer_Maintenance.h
# End Source File
# Begin Source File

SOURCE=.\Computer_Navigation.h
# End Source File
# Begin Source File

SOURCE=.\const.h
# End Source File
# Begin Source File

SOURCE=.\Door.h
# End Source File
# Begin Source File

SOURCE=.\DynamicLines.h
# End Source File
# Begin Source File

SOURCE=.\DynamicRenderable.h
# End Source File
# Begin Source File

SOURCE=.\GalaxyBox.h
# End Source File
# Begin Source File

SOURCE=.\Gradient.h
# End Source File
# Begin Source File

SOURCE=.\Hibernation.h
# End Source File
# Begin Source File

SOURCE=.\Hibernation_Interface.h
# End Source File
# Begin Source File

SOURCE=.\Interface.h
# End Source File
# Begin Source File

SOURCE=.\Interface_XMLparser.h
# End Source File
# Begin Source File

SOURCE=.\KeyboardTranslator.h
# End Source File
# Begin Source File

SOURCE=.\LensFlare.h
# End Source File
# Begin Source File

SOURCE=.\Line3D.h
# End Source File
# Begin Source File

SOURCE=.\macros.h
# End Source File
# Begin Source File

SOURCE=.\mainApplication.h
# End Source File
# Begin Source File

SOURCE=.\mainApplicationListener.h
# End Source File
# Begin Source File

SOURCE=.\MovableText.h
# End Source File
# Begin Source File

SOURCE=.\lib_noiseutils\noiseutils.h
# End Source File
# Begin Source File

SOURCE=.\NPC_Manager.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Complex.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Gas.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Granit.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_ice.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Rock.h
# End Source File
# Begin Source File

SOURCE=.\PlanetTexturer_Venusian.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\Robot.h
# End Source File
# Begin Source File

SOURCE=.\Robot_Interface.h
# End Source File
# Begin Source File

SOURCE=.\Room.h
# End Source File
# Begin Source File

SOURCE=.\Scene_XMLparser.h
# End Source File
# Begin Source File

SOURCE=.\ShipElement.h
# End Source File
# Begin Source File

SOURCE=.\ShipEngine.h
# End Source File
# Begin Source File

SOURCE=.\ShipLevel.h
# End Source File
# Begin Source File

SOURCE=.\ShipLevel_XMLparser.h
# End Source File
# Begin Source File

SOURCE=.\ShipLight.h
# End Source File
# Begin Source File

SOURCE=.\Simple_Interface.h
# End Source File
# Begin Source File

SOURCE=.\SoundManager.h
# End Source File
# Begin Source File

SOURCE=.\SpaceShip.h
# End Source File
# Begin Source File

SOURCE=.\spares.h
# End Source File
# Begin Source File

SOURCE=.\specialFX.h
# End Source File
# Begin Source File

SOURCE=.\ss_Archipel.h
# End Source File
# Begin Source File

SOURCE=.\ss_Asteroide.h
# End Source File
# Begin Source File

SOURCE=.\ss_Planet.h
# End Source File
# Begin Source File

SOURCE=.\ss_SolarSystem.h
# End Source File
# Begin Source File

SOURCE=.\ss_Star.h
# End Source File
# Begin Source File

SOURCE=.\TextList.h
# End Source File
# Begin Source File

SOURCE=.\TheShip.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\WaypointNetwork.h
# End Source File
# Begin Source File

SOURCE=.\XMLparser.h
# End Source File
# Begin Source File

SOURCE=.\XMLparser_T.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\C8.ico
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\cursor4.cur
# End Source File
# Begin Source File

SOURCE=.\cursor5.cur
# End Source File
# Begin Source File

SOURCE=.\cursor6.cur
# End Source File
# Begin Source File

SOURCE=.\cursor7.cur
# End Source File
# Begin Source File

SOURCE=.\cursor8.cur
# End Source File
# Begin Source File

SOURCE=.\cursor9.cur
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\lfe.rc
# End Source File
# End Group
# End Target
# End Project
