# Microsoft Developer Studio Project File - Name="roomsim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=roomsim - Win32 Unittest
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "roomsim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "roomsim.mak" CFG="roomsim - Win32 Unittest"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "roomsim - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "roomsim - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "roomsim - Win32 Unittest" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "roomsim - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\libroomsim\include" /I "..\libsfmt" /I "..\libfftw\win32\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libfftw3-3.lib Release/build.obj /nologo /subsystem:console /machine:I386 /libpath:"..\libfftw\win32\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=cl /nologo /ML /c ..\build.c /Fo"Release/build.obj"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "roomsim - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\libroomsim\include" /I "..\libsfmt" /I "..\libfftw\win32\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libfftw3-3.lib Debug/build.obj /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\libfftw\win32\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=cl /nologo /MLd /c ..\build.c /Fo"Debug/build.obj"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "roomsim - Win32 Unittest"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unittest"
# PROP BASE Intermediate_Dir "Unittest"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Unittest"
# PROP Intermediate_Dir "Unittest"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "..\libroomsim\include" /I "..\libsfmt" /I "..\libfftw\win32\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "UNITTEST" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libfftw3-3.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\libfftw\win32\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libfftw3-3.lib Unittest\build.obj /nologo /subsystem:console /debug /machine:I386 /out:"Unittest/unittest.exe" /pdbtype:sept /libpath:"..\libfftw\win32\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=cl /nologo /MLd /c ..\build.c /Fo"Unittest/build.obj"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "roomsim - Win32 Release"
# Name "roomsim - Win32 Debug"
# Name "roomsim - Win32 Unittest"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=..\main.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\build.h
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

SOURCE=..\libroomsim.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\msg.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\mstruct.h
# End Source File
# Begin Source File

SOURCE=..\libroomsim\include\setup.h
# End Source File
# End Group
# End Target
# End Project
