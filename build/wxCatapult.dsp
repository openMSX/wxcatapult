# Microsoft Developer Studio Project File - Name="wxCatapult" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wxCatapult - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxCatapult.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxCatapult.mak" CFG="wxCatapult - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxCatapult - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wxCatapult - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\bin\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /Od /I "..\derived\config" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /FR /FD /c
# SUBTRACT CPP /X /YX
# ADD BASE RSC /l 0x3009 /d "NDEBUG"
# ADD RSC /l 0x3009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib wsock32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib zlib.lib regex.lib png.lib jpeg.lib tiff.lib wxmsw.lib wxxrcs.lib libxml2.lib winmm.lib shell32.lib /nologo /version:0.1 /subsystem:windows /machine:I386 /nodefaultlib:"wsock32.dll" /out:"..\bin/Catapult.exe" /libpath:"c:\windows\system32" /WARN:1
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin"
# PROP Intermediate_Dir "..\Bin\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\derived\config" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x3009 /d "_DEBUG"
# ADD RSC /l 0x3009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlibd.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswd.lib wxxrcds.lib libxml2.lib winmm.lib shell32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"../Bin/Catapultd.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "wxCatapult - Win32 Release"
# Name "wxCatapult - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Dialogs"

# PROP Default_Filter "wxg"
# Begin Source File

SOURCE=..\dialogs\audiocontrols.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\catapult.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\config.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\fullscreen.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\misccontrols.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\session.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\status.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\videocontrols.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating resource $(InputPath)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Version"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\version.mk

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

USERDEP__VERSI="$(InputDir)\..\ChangeLog"	
# Begin Custom Build - Creating Version Information
InputDir=.
InputPath=.\version.mk

"$(InputDir)\..\derived\config\version.ii" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir ..\derived 2> nul 
	mkdir ..\derived\config 2> nul 
	sed -ne "s/\$Id: ChangeLog,v \([^ ]*\).*/const wxString Version::CHANGELOG_REVISION = \"\1\";/p" $(InputDir)\..\ChangeLog > $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/RELEASE_FLAG:= *\([^ ]*\).*/const bool Version::RELEASE = \1;/p" $(InputDir)/version.mk >> $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/PACKAGE_VERSION:= *\([^ ]*\).*/const wxString Version::VERSION =  \"\1\";/p" $(InputDir)\version.mk >> $(InputDir)\..\derived\config\version.ii 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

USERDEP__VERSI="$(InputDir)\..\ChangeLog"	
# Begin Custom Build - Creating Version Information
InputDir=.
InputPath=.\version.mk

"$(InputDir)\..\derived\config\version.ii" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir ..\derived 2> nul 
	mkdir ..\derived\config 2> nul 
	sed -ne "s/\$Id: ChangeLog,v \([^ ]*\).*/const wxString Version::CHANGELOG_REVISION = \"\1\";/p" $(InputDir)\..\ChangeLog > $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/RELEASE_FLAG:= *\([^ ]*\).*/const bool Version::RELEASE = \1;/p" $(InputDir)/version.mk >> $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/PACKAGE_VERSION:= *\([^ ]*\).*/const wxString Version::VERSION =  \"\1\";/p" $(InputDir)\version.mk >> $(InputDir)\..\derived\config\version.ii 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\src\AudioControlPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\catapult.rc
# End Source File
# Begin Source File

SOURCE=..\src\CatapultConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CatapultPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CatapultXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ConfigurationData.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FullScreenDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MiscControlPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\openMSXController.cpp
# End Source File
# Begin Source File

SOURCE=..\src\openMSXLinuxController.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\src\openMSXWindowsController.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PipeConnectThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PipeReadThread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SessionPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\StatusPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Version.cpp
# End Source File
# Begin Source File

SOURCE=..\src\VideoControlPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wxCatapultApp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wxCatapultFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\wxToggleButtonXmlHandler.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\AudioControlPage.h
# End Source File
# Begin Source File

SOURCE=..\src\CatapultConfigDlg.h
# End Source File
# Begin Source File

SOURCE=..\src\CatapultPage.h
# End Source File
# Begin Source File

SOURCE=..\src\CatapultXMLParser.h
# End Source File
# Begin Source File

SOURCE=..\src\ConfigurationData.h
# End Source File
# Begin Source File

SOURCE=..\src\FullScreenDlg.h
# End Source File
# Begin Source File

SOURCE=..\src\MiscControlPage.h
# End Source File
# Begin Source File

SOURCE=..\src\openMSXController.h
# End Source File
# Begin Source File

SOURCE=..\src\openMSXLinuxController.h
# End Source File
# Begin Source File

SOURCE=..\src\openMSXWindowsController.h
# End Source File
# Begin Source File

SOURCE=..\src\PipeConnectThread.h
# End Source File
# Begin Source File

SOURCE=..\src\PipeReadThread.h
# End Source File
# Begin Source File

SOURCE=..\src\SessionPage.h
# End Source File
# Begin Source File

SOURCE=..\src\StatusPage.h
# End Source File
# Begin Source File

SOURCE=..\src\Version.h
# End Source File
# Begin Source File

SOURCE=..\src\VideoControlPage.h
# End Source File
# Begin Source File

SOURCE=..\src\wxCatapultApp.h
# End Source File
# Begin Source File

SOURCE=..\src\wxCatapultFrm.h
# End Source File
# Begin Source File

SOURCE=..\src\wxToggleButtonXmlHandler.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\src\catapult.ico
# End Source File
# End Group
# End Target
# End Project
