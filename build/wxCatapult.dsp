# Microsoft Developer Studio Project File - Name="wxCatapult" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wxCatapult - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxCatapult.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxCatapult.mak" CFG="wxCatapult - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxCatapult - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wxCatapult - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "wxCatapult - Win32 Debug wx24" (based on "Win32 (x86) Console Application")
!MESSAGE "wxCatapult - Win32 Release wx24" (based on "Win32 (x86) Console Application")
!MESSAGE "wxCatapult - Win32 Release Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxCatapult - Win32 Debug Unicode" (based on "Win32 (x86) Console Application")
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
# PROP Output_Dir "..\derived\bin"
# PROP Intermediate_Dir "..\derived\bin\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /I "..\derived\config" /I "d:\programming\wx25\lib\vc_lib\msw" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /FR /FD /c
# SUBTRACT CPP /WX /X /YX
# ADD BASE RSC /l 0x3009 /d "NDEBUG"
# ADD RSC /l 0x3009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wxmsw25_xrc.lib wxmsw25_html.lib wxmsw25_adv.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxexpat.lib wxregex.lib kernel32.lib user32.lib gdi32.lib winspool.lib wsock32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib libxml2.lib winmm.lib shell32.lib /nologo /version:0.1 /subsystem:windows /machine:I386 /nodefaultlib:"wsock32.dll" /out:"..\derived\bin\catapult.exe" /libpath:"c:\windows\system32" /WARN:1
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\derived\bin"
# PROP Intermediate_Dir "..\derived\bin\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\derived\config" /I "d:\programming\wx25\lib\vc_lib\mswd" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x3009 /d "_DEBUG"
# ADD RSC /l 0x3009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw25d_xrc.lib wxmsw25d_html.lib wxmsw25d_adv.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxexpatd.lib wxregexd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib libxml2.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"..\derived\bin\catapultd.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\derived\bin"
# PROP Intermediate_Dir "..\derived\bin\Debug24"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib zlibd.lib regexd.lib pngd.lib jpegd.lib tiffd.lib wxmswd.lib wxxrcds.lib libxml2.lib winmm.lib shell32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"..\derived\bin\catapultd.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\derived\bin"
# PROP Intermediate_Dir "..\derived\bin\Release24"
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib wsock32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib zlib.lib regex.lib png.lib jpeg.lib tiff.lib wxmsw.lib wxxrcs.lib libxml2.lib winmm.lib shell32.lib /nologo /version:0.1 /subsystem:windows /machine:I386 /nodefaultlib:"wsock32.dll" /out:"..\derived\bin\catapult.exe" /libpath:"c:\windows\system32" /WARN:1
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wxCatapult___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "wxCatapult___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\derived\bin"
# PROP Intermediate_Dir "..\derived\bin\ReleaseUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Od /I "..\derived\config" /I "d:\programming\wx25\lib\vc_lib\msw" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /FR /FD /c
# SUBTRACT BASE CPP /WX /X /YX
# ADD CPP /nologo /MD /W3 /GX /Od /I "..\derived\config" /I "d:\programming\wx25\lib\vc_lib\mswu" /D "_UNICODE" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /FR /FD /c
# SUBTRACT CPP /WX /X /YX
# ADD BASE RSC /l 0x3009 /d "NDEBUG"
# ADD RSC /l 0x3009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_xrc.lib wxmsw25_html.lib wxmsw25_adv.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxexpat.lib wxregex.lib kernel32.lib user32.lib gdi32.lib winspool.lib wsock32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib libxml2.lib winmm.lib shell32.lib /nologo /version:0.1 /subsystem:windows /machine:I386 /nodefaultlib:"wsock32.dll" /out:"..\derived\bin\catapult.exe" /libpath:"c:\windows\system32" /WARN:1
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wxmsw25u_xrc.lib wxmsw25u_html.lib wxmsw25u_adv.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib libxml2.lib /nologo /subsystem:windows /machine:I386 /out:"..\derived\bin\catapult.exe" /libpath:"c:\windows\system32"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wxCatapult___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "wxCatapult___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\derived\bin"
# PROP Intermediate_Dir "..\derived\bin\DebugUnicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\derived\config" /I "d:\programming\wx25\lib\vc_lib\mswd" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FR /YX /FD /GZ /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\derived\config" /I "d:\programming\wx25\lib\vc_lib\mswd" /D "_UNICODE" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x3009 /d "_DEBUG"
# ADD RSC /l 0x3009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_xrc.lib wxmsw25d_html.lib wxmsw25d_adv.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxexpatd.lib wxregexd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib libxml2.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"..\derived\bin\catapultd.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wxmsw25ud_xrc.lib wxmsw25ud_html.lib wxmsw25ud_adv.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxexpatd.lib wxregexud.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib libxml2.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"..\derived\bin\catapultd.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "wxCatapult - Win32 Release"
# Name "wxCatapult - Win32 Debug"
# Name "wxCatapult - Win32 Debug wx24"
# Name "wxCatapult - Win32 Release wx24"
# Name "wxCatapult - Win32 Release Unicode"
# Name "wxCatapult - Win32 Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Dialogs"

# PROP Default_Filter "wxg"
# Begin Source File

SOURCE=..\dialogs\about.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\about.wxg
InputName=about

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\audiocontrols.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\audiocontrols.wxg
InputName=audiocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\catapult.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\catapult.wxg
InputName=catapult

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\config.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\config.wxg
InputName=config

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\fullscreen.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\fullscreen.wxg
InputName=fullscreen

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\input.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\input.wxg
InputName=input

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\input.wxg
InputName=input

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\input.wxg
InputName=input

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\input.wxg
InputName=input

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\input.wxg
InputName=input

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\input.wxg
InputName=input

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\ipsselect.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\ipsselect.wxg
InputName=ipsselect

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\ipsselect.wxg
InputName=ipsselect

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\ipsselect.wxg
InputName=ipsselect

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\ipsselect.wxg
InputName=ipsselect

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\ipsselect.wxg
InputName=ipsselect

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\ipsselect.wxg
InputName=ipsselect

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\misccontrols.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\misccontrols.wxg
InputName=misccontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\romtype.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\romtype.wxg
InputName=romtype

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\romtype.wxg
InputName=romtype

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\romtype.wxg
InputName=romtype

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\romtype.wxg
InputName=romtype

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\romtype.wxg
InputName=romtype

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\romtype.wxg
InputName=romtype

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\screenshot.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\screenshot.wxg
InputName=screenshot

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\screenshot.wxg
InputName=screenshot

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\screenshot.wxg
InputName=screenshot

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\screenshot.wxg
InputName=screenshot

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\screenshot.wxg
InputName=screenshot

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\screenshot.wxg
InputName=screenshot

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\session.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\session.wxg
InputName=session

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\status.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\status.wxg
InputName=status

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dialogs\videocontrols.wxg

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Translating XRC Resource: $(InputName)
ProjDir=.
InputPath=..\dialogs\videocontrols.wxg
InputName=videocontrols

"$(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	sed -f $(ProjDir)\wxg2xrc.sed $(InputPath) > $(ProjDir)\..\derived\resources\dialogs\$(InputName).xrc

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Version"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\version.mk

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# PROP Ignore_Default_Tool 1
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

# PROP Ignore_Default_Tool 1
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

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
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

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
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

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
USERDEP__VERSI="$(InputDir)\..\ChangeLog"	
# Begin Custom Build - Creating Version Information
InputDir=.
InputPath=.\version.mk

"$(InputDir)\..\derived\config\version.ii" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir ..\derived 2> nul 
	mkdir ..\derived\config 2> nul 
	sed -ne "s/\$Id: ChangeLog,v \([^ ]*\).*/const wxString Version::CHANGELOG_REVISION = wxT(\"\1\");/p" $(InputDir)\..\ChangeLog > $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/RELEASE_FLAG:= *\([^ ]*\).*/const bool Version::RELEASE = \1;/p" $(InputDir)/version.mk >> $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/PACKAGE_VERSION:= *\([^ ]*\).*/const wxString Version::VERSION =  wxT(\"\1\");/p" $(InputDir)\version.mk >> $(InputDir)\..\derived\config\version.ii 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
USERDEP__VERSI="$(InputDir)\..\ChangeLog"	
# Begin Custom Build - Creating Version Information
InputDir=.
InputPath=.\version.mk

"$(InputDir)\..\derived\config\version.ii" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mkdir ..\derived 2> nul 
	mkdir ..\derived\config 2> nul 
	sed -ne "s/\$Id: ChangeLog,v \([^ ]*\).*/const wxString Version::CHANGELOG_REVISION = wxT(\"\1\");/p" $(InputDir)\..\ChangeLog > $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/RELEASE_FLAG:= *\([^ ]*\).*/const bool Version::RELEASE = \1;/p" $(InputDir)/version.mk >> $(InputDir)\..\derived\config\version.ii 
	sed -ne "s/PACKAGE_VERSION:= *\([^ ]*\).*/const wxString Version::VERSION =  wxT(\"\1\");/p" $(InputDir)\version.mk >> $(InputDir)\..\derived\config\version.ii 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Bitmaps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\resources\bitmaps\about.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\about.png
InputName=about

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\diskimage.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\diskimage.png
InputName=diskimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\diskimage.png
InputName=diskimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\diskimage.png
InputName=diskimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\diskimage.png
InputName=diskimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\diskimage.png
InputName=diskimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\diskimage.png
InputName=diskimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\eject.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\eject.png
InputName=eject

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\eject.png
InputName=eject

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\eject.png
InputName=eject

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\eject.png
InputName=eject

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\eject.png
InputName=eject

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\eject.png
InputName=eject

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\file.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\file.png
InputName=file

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\file.png
InputName=file

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\file.png
InputName=file

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\file.png
InputName=file

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\file.png
InputName=file

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\file.png
InputName=file

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\ledoff.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledoff.png
InputName=ledoff

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledoff.png
InputName=ledoff

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledoff.png
InputName=ledoff

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledoff.png
InputName=ledoff

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledoff.png
InputName=ledoff

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledoff.png
InputName=ledoff

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\ledon.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledon.png
InputName=ledon

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledon.png
InputName=ledon

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledon.png
InputName=ledon

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledon.png
InputName=ledon

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledon.png
InputName=ledon

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\ledon.png
InputName=ledon

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\romimage.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\romimage.png
InputName=romimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\romimage.png
InputName=romimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\romimage.png
InputName=romimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\romimage.png
InputName=romimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\romimage.png
InputName=romimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\romimage.png
InputName=romimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\resources\bitmaps\tapeimage.png

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\tapeimage.png
InputName=tapeimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\tapeimage.png
InputName=tapeimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\tapeimage.png
InputName=tapeimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\tapeimage.png
InputName=tapeimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\tapeimage.png
InputName=tapeimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# Begin Custom Build - Copying Bitmap: $(InputName)
ProjDir=.
InputPath=..\resources\bitmaps\tapeimage.png
InputName=tapeimage

"$(ProjDir)\..\derived\resources\bitmaps\$(InputName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) $(ProjDir)\..\derived\resources\bitmaps > NUL 
	touch -m $(ProjDir)\..\derived\resources\bitmaps\$(InputName).png 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\src\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\AboutDlg.h
# End Source File
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

SOURCE=..\src\InputPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\IPSSelectionDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MiscControlPage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\openMSXController.cpp
# End Source File
# Begin Source File

SOURCE=..\src\openMSXLinuxController.cpp

!IF  "$(CFG)" == "wxCatapult - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug wx24"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release wx24"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "wxCatapult - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

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

SOURCE=..\src\RomTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ScreenShotDlg.cpp
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

SOURCE=..\src\InputPage.h
# End Source File
# Begin Source File

SOURCE=..\src\IPSSelectionDlg.h
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

SOURCE=..\src\RomTypeDlg.h
# End Source File
# Begin Source File

SOURCE=..\src\ScreenshotDlg.h
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
