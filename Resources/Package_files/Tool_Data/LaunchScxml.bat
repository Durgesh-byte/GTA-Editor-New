@ECHO OFF
REM #emo test exe file path to be executed
SET EMOEXEPATH=C:\Users\tmseggar\Desktop\GTA\External_Tools\eMoTest_Scheduler_v1.2.8.2\eMoTest_Scheduler

REM #emo conf file path to be used
SET EMOCONFFILE=%EMOEXEPATH%\conf\default.conf

REM # execute the emo test
START %EMOEXEPATH%\eMoTest.exe -c %EMOCONFFILE% -l %1 %2 
