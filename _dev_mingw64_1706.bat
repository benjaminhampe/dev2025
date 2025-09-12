REM :: Silence auto echo of every command line
@echo off

REM set PATH=C:\sdk\StrawberryPerl\c\bin;%PATH%
REM set PATH=C:\sdk\StrawberryPerl\perl\site\bin;%PATH%
REM set PATH=C:\sdk\StrawberryPerl\perl\bin;%PATH%
set PATH=C:\sdk\nasm;%PATH%
set PATH=C:\sdk\ninja;%PATH%
set PATH=C:\sdk\cmake-3.29.0\bin;%PATH%
set PATH=C:\sdk\Qt\Tools\llvm-mingw1706_64\bin;%PATH%
REM set PATH=C:\sdk\Qt\Tools\llvm-mingw1706_64\python\bin;%PATH%
REM set PATH=C:\sdk\Qt\Tools\llvm-mingw1706_64\python\include;%PATH%
REM set PATH=C:\sdk\Qt\Tools\llvm-mingw1706_64\python\lib;%PATH%
REM set PATH=C:\sdk\Java\jdk-24\bin;%PATH%
set PATH=C:\sdk\Git;%PATH%
REM set PATH=C:\sdk\Graphviz-10.0.1-win64\bin;%PATH%

start /B "" "C:\sdk\ConEmu\ConEmu64.exe"
REM start /B "" "%TOOLCHAIN_DIR%\ConEmu\ConEmu64.exe"
REM start /B "" "%TOOLCHAIN_DIR%\ConEmu-23.07.24\ConEmu64.exe"

REM set PATH=C:\dev\sdk\Qt\Tools\mingw1310_64\bin;%PATH%
REM set PATH=C:\dev\sdk\cmake-3.29.0\bin;%PATH%
REM set PATH=C:\dev\sdk\Git;%PATH%

REM set ROOT_DIR=%~dp0
REM set TOOLCHAIN_DIR=%ROOT_DIR%sdk
REM set COMPILER_DIR=%TOOLCHAIN_DIR%\mingw64_1410-posix-seh-msvcrt-rt_v12-rev0\bin
REM set PATH=C:\dev\sdk\mingw64_1410-posix-seh-msvcrt-rt_v12-rev0\bin;%PATH%
REM set PATH=%TOOLCHAIN%\ninja\bin;%PATH%

REM "C:\dev\sdk\ConEmu-23.07.24\ConEmu64.exe"

REM start /B "" "C:\dev\sdk\ConEmu-23.07.24\ConEmu64.exe" -run {Git Bash}

REM cmd /c start "" "C:\dev\sdk\ConEmu-23.07.24\ConEmu64.exe" -run {Git Bash}

REM cmd /min /c start "" "C:\dev\sdk\ConEmu-23.07.24\ConEmu64.exe" -run {Git Bash}

REM cmd /min /c start "" "C:\dev\sdk\ConEmu-23.07.24\ConEmu64.exe"

REM start /B "" "C:\dev\sdk\ConEmu-23.07.24\ConEmu64.exe"



REM "C:\dev\sdk\Git\usr\bin\mintty.exe" --icon C:\dev\sdk\Git\git-bash.exe,0 --window max --exec "/usr/bin/bash" --login -i

rem "%TOOLCHAIN%\git\git-bash.exe"
rem usr\bin\mintty.exe --icon git-bash.exe,0 --exec "/usr/bin/bash" --login -i <other arguments>
rem "%TOOLCHAIN%\git\usr\bin\mintty.exe" --icon %TOOLCHAIN%\git\git-bash.exe,0 --exec "/usr/bin/bash" --login -i

rem  -c "echo 'g++.exe --version'"


REM set CHERE_INVOKING=1 &
REM set "PATH=C:\msys64\mingw64\bin;%PATH%" &
REM set MSYS2_PATH_TYPE=inherit &
REM set MSYSTEM=mingw64 &
REM set MSYSCON=conemu64.exe &
REM "C:\msys64\usr\bin\zsh.exe" --login -i -new_console:C:"C:\msys64\mingw64.exe"