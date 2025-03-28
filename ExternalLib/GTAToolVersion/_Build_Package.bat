echo off
cls

echo ----------------------------------------------
echo.
echo This tool:
echo - Clean DIST and BUILD folder
echo - Switch to VENV
echo - Build the package
echo - Zip the final folder to root folder
echo.
echo ----------------------------------------------

echo 'Move to folder'
cd %~dp0

echo 'Cleaning DIST'
rd /S dist /Q
echo ----------------------------------------------

echo 'Cleaning BUILD'
rd /S build /Q
echo ----------------------------------------------

echo Cleaning previous zip build
del /Q GTAToolVersion_exe.zip
echo ----------------------------------------------

echo 'Switching to venv'
CALL Scripts\activate.bat
echo ----------------------------------------------

echo 'Starting packing'
pyinstaller GTAToolVersion.spec
echo ----------------------------------------------

echo 'Generating zip file'
CALL powershell.exe Compress-Archive .\dist\* .\GTAToolVersion_exe.zip
echo ----------------------------------------------

echo 'Switching to main env'
CALL Scripts\deactivate.bat
echo ----------------------------------------------

echo.
echo ----------------------------------------------
echo 'Packing operation DONE'
echo ----------------------------------------------
echo.

pause
echo on