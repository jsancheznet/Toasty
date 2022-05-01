@echo off

REM TODO(Jorge): Make sure we compile with all libraries in release mode

pushd build

set SDL_INCLUDE="..\vendor\SDL2-2.0.22\include"
set SDL_LIB="..\vendor\SDL2-2.0.22\lib\x64"

set GLAD_INCLUDE="..\vendor\glad\include"
set GLAD_SRC="..\vendor\glad\src"

REM set ASSIMP_INCLUDE="..\vendor\include"
REM set ASSIMP_LIB="..\vendor\lib\assimp"

set IncludeDirectories=-I%SDL_INCLUDE% -I%GLAD_INCLUDE% -I%GLAD_SRC%
set LibDirectories=-LIBPATH:%SDL_LIB%

REM set CompilerFlags= -DDEBUG -nologo -W4 -WX -Ot -FS %IncludeDirectories% -Zi -EHsc -MD /D "_WINDOWS"
set CompilerFlags= -nologo -W4 -WX -Od -FS %IncludeDirectories% -Zi -EHsc -MD /D "_WINDOWS"
set LinkerFlags=-nologo -DEBUG %LibDirectories%

cl ..\toasty.cpp %CompilerFlags% /link %LinkerFlags% -SUBSYSTEM:CONSOLE SDL2.lib SDL2main.lib shell32.lib

popd