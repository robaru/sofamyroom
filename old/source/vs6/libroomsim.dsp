# Microsoft Developer Studio Project File - Name="libroomsim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libroomsim - Win32 Unittest
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libroomsim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libroomsim.mak" CFG="libroomsim - Win32 Unittest"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libroomsim - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libroomsim - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libroomsim - Win32 Unittest" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libroomsim - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\libsfmt" /I "..\libroomsim\include" /I "..\libfftw\win32\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libroomsim - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I ".\libsfmt" /I "..\libroomsim\include" /I "..\libfftw\win32\include" /I "..\libsfmt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libroomsim - Win32 Unittest"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unittest"
# PROP BASE Intermediate_Dir "Unittest"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unittest"
# PROP Intermediate_Dir "Unittest"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\libsfmt" /I "..\libroomsim\include" /I "..\libfftw\win32\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libroomsim - Win32 Release"
# Name "libroomsim - Win32 Debug"
# Name "libroomsim - Win32 Unittest"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\libroomsim\source\3D.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\dsp.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\interface.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\interp.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\rng.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\roomsim.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\sensor.c
# End Source File
# Begin Source File

SOURCE=..\libroomsim\source\setup.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\libroomsim\include\3D.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\defs.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\dsp.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\interface.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\interp.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\mem.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\msg.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\mstruct.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\rng.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\sensor.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\setup.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\types.h
# End Source File
# End Group
# End Target
# End Project
