@echo off
setlocal

:: Go to project root (parent directory of scripts)
cd /d "%~dp0.."

set "buildDir=build"

if exist "%buildDir%" (
    echo Deleted previous build folder
    rmdir /s /q "%buildDir%"
)

echo.
echo ========================
echo Building project...
echo ========================
:: Uses VS 2022 and Qt 6.11 located in C:/ by default.
:: Change parameters to use a different Visual Studio/Qt version.
cmake -B "%buildDir%" -S . -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.12.0\msvc2022_64" || exit /b

echo.
echo ========================
echo Compiling project...
echo ========================
cmake --build "%buildDir%" || exit /b
echo.

pause
endlocal