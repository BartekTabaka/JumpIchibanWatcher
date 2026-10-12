@echo off
setlocal

cd /d "%~dp0.."

set "buildDir=build"
set "qtDir=C:\Qt\6.12.0\msvc2022_64"

echo.
echo ========================
echo Generating solution...
echo ========================

cmake -B "%buildDir%" -S . -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="%qtDir%"

if errorlevel 1 exit /b 1

echo.
echo Solution generated successfully!
echo Location: %buildDir%
echo.

pause
endlocal