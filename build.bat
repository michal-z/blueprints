@echo off
setlocal enableextensions enabledelayedexpansion

set "NAME=blueprints"

:: (D)ebug, (R)elease
set CONFIG=D

set CFLAGS=/MP /Wall /std:c++20 /GR- /EHsc /fp:except- /fp:precise /nologo /I"src/precomp" ^
/wd4711 /wd4061 /wd4710 /wd4365 /wd4255 /wd4505 /wd4820 /wd4191 /wd5045 /wd5039

if %CONFIG%==D set CFLAGS=%CFLAGS% /GS /Zi /Od /D"_DEBUG" /MTd /RTCs
if %CONFIG%==R set CFLAGS=%CFLAGS% /GS- /O2 /Oi /Ot /Gy /MT /D"NDEBUG"

set LFLAGS=/INCREMENTAL:NO /NOLOGO /NOIMPLIB /NOEXP
if %CONFIG%==D SET LFLAGS=%LFLAGS% /DEBUG:FULL
if %CONFIG%==R SET LFLAGS=%LFLAGS%

if exist "%NAME%.exe" del "%NAME%.exe"

if not "%1"=="clean" goto clean_end
if exist "*.pch" del "*.pch"
if exist "*.obj" del "*.obj"
if exist "*.pdb" del "*.pdb"
if exist "*.lib" del "*.lib"
:clean_end

::
:: precomp
::
if exist precomp.lib goto precomp_end
cl.exe ^
%CFLAGS% /c "src/precomp/precomp.cpp" ^
/Fo:"precomp.lib" /Fp:"precomp.pch" /Fd:"precomp.pdb" /Yc"precomp.h"
if errorlevel 1 goto error
:precomp_end

::
:: imgui
::
if exist imgui.lib goto imgui_end
cl.exe ^
%CFLAGS% /c "src/precomp/imgui/*.cpp" /Fd:"imgui.pdb" ^
/wd5262 /wd4668 /wd4189 /wd4774 /wd5219 /wd4514 /wd4582
if errorlevel 1 goto error
lib.exe ^
/NOLOGO *.obj /OUT:"imgui.lib"
if exist *.obj del *.obj
:imgui_end

::
:: game
::
cl.exe ^
%CFLAGS% /Fp:"precomp.pch" /Fd:"precomp.pdb" /Fe:"%NAME%.exe" /Yu"precomp.h" "src\*.cpp" ^
/link ^
%LFLAGS% kernel32.lib user32.lib imgui.lib precomp.lib /subsystem:CONSOLE
if errorlevel 1 goto error

if exist *.obj del *.obj

if "%1"=="run" if exist "%NAME%.exe" "%NAME%.exe"

goto end

:error

if exist *.obj del *.obj

echo ---------------
echo error
echo ---------------

:end
