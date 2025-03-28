echo off
cls

echo ----------------------------------------------
echo.
echo This tool:
echo - Build GTA-Editor in release configuration
echo - Create the package with needed files
echo - Zip the final folder to root folder
echo.
echo ----------------------------------------------


rem Current branch
rem set about=UAT
rem git branch --show-current > branch
rem set /p branch_name= < .\branch
rem del branch

rem Switch branch if needed
rem set branch_name=I04DAIVYTE-1813
rem echo 0/ CHECKOUT BRANCH %branch_name% 
rem git checkout %branch_name%

echo ----------------------------------------------
echo BUILDING
rem set package_name=GTA-Editor_%branch_name%_%about%
rem set package_name=GTA-Editor_commit-of-2024Oct01-17h04

set package_name=GTA-Editor_V31.05.20
set configuration=Release
set folder="C:\Users\fnaandrianony\Documents\GTA\GTA-Editor"

rem set build="C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
rem %build% %folder%\GTAApplication.sln /t:Rebuild /p:WarningLevel=0 /v:m /p:configuration=%configuration%

echo ----------------------------------------------
echo REMOVING lib AND exp FILES
set package_folder=%folder%\build\%configuration%
del "%package_folder%\*lib"
del "%package_folder%\*exp"

echo ----------------------------------------------
echo EXPORTING USERGUIDE INTO PDF
set userguide=%folder%\doc\UserGuide
rem pip install docx2pdf
rem docx2pdf %userguide%
copy %userguide%\*.pdf %package_folder%

echo ----------------------------------------------
echo EXPORTING MCDU FILE
copy  %folder%\src\GTAUI\MCDU_GTA.xml %package_folder%\MCDU_GTA.xml

echo ----------------------------------------------
echo IMPORTING PACKAGE FILES
set package_files_path=%folder%\Resources\Package_files
xcopy %package_files_path% %package_folder% /E /I /Y

echo ----------------------------------------------
echo UPDATING LaunchScxml.bat
move %package_folder%\LaunchScxml\LaunchScxml.bat %package_folder%\Tool_Data

echo ----------------------------------------------
echo CREATING THE ZIP
del .\%package_name%.zip
CALL powershell.exe Compress-Archive %package_folder%\* C:\Users\fnaandrianony\Documents\GTA\_GTA_Packages\_pkg_gta\%package_name%.zip

echo ----------------------------------------------
echo OUTPUT: %package_name%.zip

pause