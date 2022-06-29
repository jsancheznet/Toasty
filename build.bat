
@echo off

REM TODO(Jorge): Make sure we compile with all libraries in release mode

pushd build

set SDL_INCLUDE="..\vendor\SDL2-2.0.22\include"
set SDL_LIB="..\vendor\SDL2-2.0.22\lib\x64"

set GLAD_INCLUDE="..\vendor\glad\include"
set GLAD_SRC="..\vendor\glad\src"

set GLM_INCLUDE="..\vendor\glm"

set IMGUI_INCLUDE="..\vendor\imgui"

set STB_INCLUDE="..\vendor\stb"

set ASSIMP_INCLUDE="..\vendor\assimp\include"
set ASSIMP_LIB="..\vendor\assimp\lib\assimp"



set IncludeDirectories=-I%SDL_INCLUDE% -I%GLAD_INCLUDE% -I%GLAD_SRC% -I%GLM_INCLUDE% -I%IMGUI_INCLUDE% -I%STB_INCLUDE% -I%ASSIMP_INCLUDE%
set LibDirectories=-LIBPATH:%SDL_LIB% -LIBPATH:%ASSIMP_LIB%

set CompilerFlags= -nologo -W4 -WX -Od -FS /Fm %IncludeDirectories% -Zi -EHsc -MD /D "_WINDOWS"
set LinkerFlags=-nologo -DEBUG %LibDirectories%

cl ..\toasty.cpp %CompilerFlags% /link %LinkerFlags% -SUBSYSTEM:CONSOLE SDL2.lib SDL2main.lib shell32.lib assimp-vc142-mt.lib

popd
