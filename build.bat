@echo off
setlocal enableextensions enabledelayedexpansion

set "NAME=blueprints"

:: (D)ebug, (R)elease
set CONFIG=D

set CFLAGS=/W4 /std:c++latest /GR- /EHsc /fp:except- /fp:precise /nologo

if %CONFIG%==D set CFLAGS=%CFLAGS% /GS /Zi /Od /D"_DEBUG" /MTd /RTCs
if %CONFIG%==R set CFLAGS=%CFLAGS% /GS- /O2 /Oi /Ot /Gy /MT /D"NDEBUG"

set LFLAGS=/INCREMENTAL:NO /NOLOGO /NOIMPLIB /NOEXP
if %CONFIG%==D SET LFLAGS=%LFLAGS% /DEBUG:FULL
if %CONFIG%==R SET LFLAGS=%LFLAGS%

if exist "%NAME%.exe" del "%NAME%.exe"

if not "%1"=="clean" goto clean_end
if exist "*.obj" del "*.obj"
if exist "*.pdb" del "*.pdb"
if exist "*.lib" del "*.lib"
if exist "*.ifc" del "*.ifc"
:clean_end

::
:: imgui
::
if exist imgui.lib goto imgui_end
cl %CFLAGS% /MP /c "src/external/imgui/*.cpp" /I "src/external/imgui" /Fd:"imgui.pdb"
lib.exe /NOLOGO *.obj /OUT:"imgui.lib"
if exist *.obj del *.obj
:imgui_end

::
:: std
::
if exist std.ifc goto std_end
cl %CFLAGS% /c "%VCToolsInstallDir%\modules\std.ixx" /Fd:"std.pdb"
:std_end

::
:: external
::
if exist external.h.ifc goto external_end
cl %CFLAGS% /exportHeader "src/external.h" /I "src/external" /Fd:"external.pdb"
:external_end

::
:: game
::
set SRC=^
src\gpu.ixx ^
src\main.cpp

cl ^
%CFLAGS% /Fe:"%NAME%.exe" /Fd:"%NAME%.pdb" /headerUnit "src/external.h=external.h.ifc" %SRC% ^
/link ^
%LFLAGS% kernel32.lib user32.lib dxgi.lib imgui.lib std.obj /subsystem:CONSOLE

:: Delete .obj and .ifc files (all except std.obj, std.ifc and external.h.ifc)
for %%f in (%SRC%) do if exist %%~nf.* del %%~nf.*

if "%1"=="run" if exist "%NAME%.exe" "%NAME%.exe"
