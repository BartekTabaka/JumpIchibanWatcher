@echo off
setlocal

cd /d "%~dp0.."

set "appName=JumpIchibanWatcher"
set "buildDir=build"
set "qtDir=C:\Qt\6.12.0\msvc2022_64"
set "exeName=JumpIchibanWatcher.exe"
set "exePath=%buildDir%\%appName%\Debug\%exeName%"
set "deployTool=%qtDir%\bin\windeployqt.exe"

echo.
echo ========================
echo Building Debug...
echo ========================

cmake --build "%buildDir%" --config Debug
if errorlevel 1 exit /b 1

echo.
echo ========================
echo Deploying Qt Debug...
echo ========================

if not exist "%exePath%" (
    echo ERROR: Executable not found: %exePath%
    pause
    exit /b 1
)

"%deployTool%" --debug "%exePath%"
if errorlevel 1 exit /b 1

echo.
echo Debug build and deployment complete!
echo.

pause
endlocal