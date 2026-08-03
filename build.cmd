@echo off 

setLocal EnableDelayedExpansion

set sources=
set cwd=%CD%\

for /r %%f in (*.c) do (
    set abs=%%f
    set "rel=!abs:%cwd%=!"
    set sources=!sources! !rel!
)

set include_dirs=-Isrc

pushd src

for /r /d %%d in (*) do (
    set abs=%%d
    set "rel=!abs:%cwd%=!"
    set include_dirs=!include_dirs! -I!rel!
) 

popd

@REM echo sources: %sources%
@REM echo dirs: %include_dirs%


:: ====================================================================================
:: =======================================================
:: Change those path to where ever it is on your computer
:: =======================================================


:: TODO: replace with your compiler
set compielr=C:\msys64\new_install\mingw64\bin\gcc.exe 
:: ------------------------ GLFW ----------------------------------
set include_glfw=-I"C:\Libraries\glfw-3.4.bin.WIN64 (1)\glfw-3.4.bin.WIN64\include" 
set lib_glfw=-L"C:\Libraries\glfw-3.4.bin.WIN64 (1)\glfw-3.4.bin.WIN64\lib-mingw-w64"
:: ------------------------ libcurl ----------------------------------
set include_curl=-I"C:\Libraries\curl-8.21.0_4-win64-mingw\include"
set lib_curl=-L"C:\Libraries\curl-8.21.0_4-win64-mingw\lib"

:: ====================================================================================

set include=%include_glfw% %include_curl%
set libs=%lib_glfw% %lib_curl%
set libs_flags=-lglfw3 -lgdi32 -lopengl32 -lcurl
set defines=-D_DEBUG
set flags=-Wall -Wextra

if  "%1"=="d"  (
    call %compielr% -o build\debug\main.exe -ggdb %sources% %include% %include_dirs% %libs% %libs_flags% %defines% %flags% 
    if %ERRORLEVEL% neq 0 (
        echo Build failed!
    ) else (
        echo build succeed!
    )
    exit
) 

if "%1"=="r" (
    call %compielr% -o build\release\main.exe %sources% %include% %include_dirs% %libs% %libs_flags% %defines% %flags%
    if %ERRORLEVEL% neq 0 (
        echo Build failed!
    ) else (
        echo build succeed!
    )
    exit
)