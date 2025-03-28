echo off
cls

echo ----------------------------------------------
echo.
echo This tool:
echo - Create VENV for Python development
echo - Add PyInstaller and other required modules
echo.
echo ----------------------------------------------

echo Creating VENV
CALL C:\Users\romamont\AppData\Local\Programs\Python\Python38\python.exe -m venv ./

echo ----------------------------------------------
echo Activating VENV
CALL Scripts\activate.bat

cd %~dp0
echo ----------------------------------------------
echo Installing project modules
pip install pyinstaller
pip install  --upgrade pywin32
pip install chardet

echo ----------------------------------------------
echo Switching to main env
CALL Scripts\deactivate.bat

echo.
echo ----------------------------------------------
echo Creating Python VENV environment DONE
echo ----------------------------------------------
echo.

pause
echo on
