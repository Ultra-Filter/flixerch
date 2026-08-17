@echo off 

setLocal EnableDelayedExpansion

if not exist "build" mkdir "build"
if not exist "build\debug" mkdir "build\debug"
if not exist "build\release" mkdir "build\release"

set sources=
set cwd=%CD%\

for /r %%f in (*.c) do (
    set abs=%%f
    set "rel=!abs:%cwd%=!"
    set sources=!sources! !rel!
)

set compiler=gcc

set deps_include=-I"deps\include" 
set deps_libs=-L"deps\libs"

set include=%deps_include% -Isrc
set libs=%deps_libs%
set libs_flags=-lglfw3 -lgdi32 -lopengl32 -lcurl 

if  "%1"=="d"  (
    echo ==============================================
    echo                  DEBUG MODE
    echo ==============================================
    set flags=-Wall -Wextra -Wno-unused-function -Wno-int-to-pointer-cast
    set defines=-D_DEBUG
    set output=build\debug\main.exe
    set build_flags=-ggdb
) 
if "%1"=="r" (
    echo ==============================================
    echo                 RELEASE MODE
    echo ==============================================
    set flags=
    set defines=-DNDEBUG
    set output=build\release\main.exe
    set build_flags=
)

call %compiler% -o %output% %build_flags% %sources% %include% %libs% %libs_flags% %defines% %flags%

if %ERRORLEVEL% neq 0 (
    echo Build failed
) else (
    echo Build succeed
    echo    executable generated at %output%
)
exit
