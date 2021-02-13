# Microsoft Developer Studio Project File - Name="combine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=combine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "combine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "combine.mak" CFG="combine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "combine - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "combine - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "combine - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 gdiplus.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "combine - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdiplus.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "combine - Win32 Release"
# Name "combine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Alignment.cpp
# End Source File
# Begin Source File

SOURCE=.\Average.cpp
# End Source File
# Begin Source File

SOURCE=.\Balance.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseMacro.cpp
# End Source File
# Begin Source File

SOURCE=.\Clear.cpp
# End Source File
# Begin Source File

SOURCE=.\combine.cpp
# End Source File
# Begin Source File

SOURCE=.\combine.rc
# End Source File
# Begin Source File

SOURCE=.\combineDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\combineView.cpp
# End Source File
# Begin Source File

SOURCE=.\Copy.cpp
# End Source File
# Begin Source File

SOURCE=.\Deapth.cpp
# End Source File
# Begin Source File

SOURCE=.\DecimateDia.cpp
# End Source File
# Begin Source File

SOURCE=.\depthflow.cpp
# End Source File
# Begin Source File

SOURCE=.\detail.cpp
# End Source File
# Begin Source File

SOURCE=.\Dft.cpp
# End Source File
# Begin Source File

SOURCE=.\DftInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Display.cpp
# End Source File
# Begin Source File

SOURCE=.\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Files.cpp
# End Source File
# Begin Source File

SOURCE=.\FixRectangles.cpp
# End Source File
# Begin Source File

SOURCE=.\FramePicture.cpp
# End Source File
# Begin Source File

SOURCE=.\Getvalue.cpp
# End Source File
# Begin Source File

SOURCE=.\GotoFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Hide.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportDM.cpp
# End Source File
# Begin Source File

SOURCE=.\Info.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\Islands.cpp
# End Source File
# Begin Source File

SOURCE=.\Keys.cpp
# End Source File
# Begin Source File

SOURCE=.\LightDark.cpp
# End Source File
# Begin Source File

SOURCE=.\macro.cpp
# End Source File
# Begin Source File

SOURCE=.\Macro1.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuUI.cpp
# End Source File
# Begin Source File

SOURCE=.\Monochrome.cpp
# End Source File
# Begin Source File

SOURCE=.\Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\PaintDia.cpp
# End Source File
# Begin Source File

SOURCE=.\patch.cpp
# End Source File
# Begin Source File

SOURCE=.\Pixels.cpp
# End Source File
# Begin Source File

SOURCE=.\Planes.cpp
# End Source File
# Begin Source File

SOURCE=.\Point1.cpp
# End Source File
# Begin Source File

SOURCE=.\Progress.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisterDia.cpp
# End Source File
# Begin Source File

SOURCE=.\removesteps.cpp
# End Source File
# Begin Source File

SOURCE=.\RR.cpp
# End Source File
# Begin Source File

SOURCE=.\SeedFill.cpp
# End Source File
# Begin Source File

SOURCE=.\Size11.cpp
# End Source File
# Begin Source File

SOURCE=.\SizeDia.cpp
# End Source File
# Begin Source File

SOURCE=.\Skew.cpp
# End Source File
# Begin Source File

SOURCE=.\Special1.cpp
# End Source File
# Begin Source File

SOURCE=.\Special2.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Transparent.cpp
# End Source File
# Begin Source File

SOURCE=.\Undo.cpp
# End Source File
# Begin Source File

SOURCE=.\Values.cpp
# End Source File
# Begin Source File

SOURCE=.\Zoom.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\choosemacro.h
# End Source File
# Begin Source File

SOURCE=.\combine.h
# End Source File
# Begin Source File

SOURCE=.\combineDoc.h
# End Source File
# Begin Source File

SOURCE=.\combineView.h
# End Source File
# Begin Source File

SOURCE=.\DecimateDia.h
# End Source File
# Begin Source File

SOURCE=.\Dft.h
# End Source File
# Begin Source File

SOURCE=.\Getvalue.h
# End Source File
# Begin Source File

SOURCE=.\GotoFrame.h
# End Source File
# Begin Source File

SOURCE=.\ImportDM.h
# End Source File
# Begin Source File

SOURCE=.\Info.h
# End Source File
# Begin Source File

SOURCE=.\Macro1.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\PaintDia.h
# End Source File
# Begin Source File

SOURCE=.\Point1.h
# End Source File
# Begin Source File

SOURCE=.\Progress.h
# End Source File
# Begin Source File

SOURCE=.\RegisterDia.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RR.h
# End Source File
# Begin Source File

SOURCE=.\Size1.h
# End Source File
# Begin Source File

SOURCE=.\Size11.h
# End Source File
# Begin Source File

SOURCE=.\SizeDia.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Values.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\combine.ico
# End Source File
# Begin Source File

SOURCE=.\res\combine.rc2
# End Source File
# Begin Source File

SOURCE=.\res\combineDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\progress.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
