REM @ECHO OFF

SET SCXML_ABSOLUTE_FILE=%~2
SET SCXML_ABSOLUTE_PATH_FILTERED=%SCXML_ABSOLUTE_FILE:/=\%
SET SCXML_ABSOLUTE_PATH_FILTERED=%SCXML_ABSOLUTE_PATH_FILTERED: =_%

REM #procedure name recovering
SET CAR=%SCXML_ABSOLUTE_PATH_FILTERED:~-1%
:loop
IF "%CAR%"=="\" GOTO endloop
SET SCXML_ABSOLUTE_PATH_FILTERED=%SCXML_ABSOLUTE_PATH_FILTERED:~0,-1%
SET PROC_NAME=%CAR%%PROC_NAME%
SET CAR=%SCXML_ABSOLUTE_PATH_FILTERED:~-1%
GOTO loop
:endloop

REM keep all lines commented for GTA editor local installation without Autolaunch (for testers)


REM uncomment and modifify relevant Autolaunch watch path (for GTA DEV/Support Team and bench installation)

REM # GTA editor local installation (GTA DEV/Support Team) - standalone mode with Autolaunch 
REM COPY "%SCXML_ABSOLUTE_FILE:/=\%" "C:\Users\ffacchetti\__GTA__\__TOOLS__\GTA_Autolaunch\watch\%PROC_NAME%"

REM # GTA editor for bench installation
REM COPY "%SCXML_ABSOLUTE_FILE:/=\%" "\\TODA300067685\SYSTeamData\TEST_AUTO\GTA_Autolaunch\watch\%PROC_NAME%"
REM COPY "%SCXML_ABSOLUTE_FILE:/=\%" "C:\ProgramData\NEXEYA\SYSTeam\TEST_AUTO\GTA_Autolaunch\watch\%PROC_NAME%"