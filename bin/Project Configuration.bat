ECHO OFF
CLS
SET	VERSION=9
SET CURPATH=%~dp0
TITLE KSDK %VERSION%
GOTO MENU

:MENU
CLS 
COLOR 47
ECHO.
ECHO `7MMF' `YMM'       db   MMP""MM""YMM   db      `7MN.   `7MF'     db      
ECHO   MM   .M'        ;MM:  P'   MM   `7  ;MM:       MMN.    M      ;MM:     
ECHO   MM .d"         ,V^MM.      MM      ,V^MM.      M YMb   M     ,V^MM.    
ECHO   MMMMM.        ,M  `MM      MM     ,M  `MM      M  `MN. M    ,M  `MM    
ECHO   MM  VMA       AbmmmqMA     MM     AbmmmqMA     M   `MM.M    AbmmmqMA   
ECHO   MM   `MM.    A'     VML    MM    A'     VML    M     YMM   A'     VML  
ECHO .JMML.   MMb..AMA.   .AMMA..JMML..AMA.   .AMMA..JML.    YM .AMA.   .AMMA.
ECHO.  
ECHO  Copyright (C) 2015 OldTimes Software
ECHO.
ECHO.
ECHO Current project path...
ECHO     %KSDK_PROJECTPATH%
ECHO.
ECHO.
ECHO Welcome to the KSDK! Please select an option below:
ECHO 1 - Create Project
ECHO 2 - Delete Project
echo 3 - Update Project Path
ECHO 4 - Exit
ECHO.
SET /P M=
IF %M%==1 GOTO MENU_CREATEPROJECT
IF %M%==2 GOTO MENU_DELETEPROJECT
IF %M%==3 GOTO MENU_UPDATEPATH
IF %M%==4 (
GOTO EXIT
) ELSE GOTO MENU

:MENU_CREATEPROJECT
ECHO MENU/CREATEPROJECT
ECHO.
PAUSE
GOTO MENU

:MENU_DELETEPROJECT
ECHO MENU/DELETEPROJECT
ECHO.
PAUSE
GOTO MENU

REM Update the current directory, either to the current or whatever.

:MENU_UPDATEPATH
ECHO MENU/UPDATEPATH
ECHO .
ECHO 1 - Enter directory path
ECHO 2 - Use current directory
SET /P M=
IF %M%==1 (
SET /P N=
setx KSDK_PROJECTPATH %N%
) ELSE IF %M%==2 (
setx KSDK_PROJECTPATH %CURPATH%
) ELSE GOTO MENU_UPDATEPATH
PAUSE
GOTO MENU

:EXIT
EXIT