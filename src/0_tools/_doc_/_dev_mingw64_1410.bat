@echo off
REM set ROOT_DIR=%~dp0
REM set TOOLCHAIN_DIR=%ROOT_DIR%sdk
REM set COMPILER_DIR=%TOOLCHAIN_DIR%\mingw64_1410-posix-seh-msvcrt-rt_v12-rev0\bin
set PATH=C:\dev\sdk\mingw64_1410-posix-seh-msvcrt-rt_v12-rev0\bin;%PATH%
set PATH=C:\dev\sdk\cmake-3.29.0\bin;%PATH%
rem set PATH=%TOOLCHAIN%\ninja\bin;%PATH%

rem "%TOOLCHAIN%\git\git-bash.exe"
rem usr\bin\mintty.exe --icon git-bash.exe,0 --exec "/usr/bin/bash" --login -i <other arguments>
rem "%TOOLCHAIN%\git\usr\bin\mintty.exe" --icon %TOOLCHAIN%\git\git-bash.exe,0 --exec "/usr/bin/bash" --login -i

rem  -c "echo 'g++.exe --version'"

"C:\dev\sdk\Git\usr\bin\mintty.exe" --icon C:\dev\sdk\Git\git-bash.exe,0 --window max --exec "/usr/bin/bash" --login -i

REM set CHERE_INVOKING=1 &
REM set "PATH=C:\msys64\mingw64\bin;%PATH%" &
REM set MSYS2_PATH_TYPE=inherit &
REM set MSYSTEM=mingw64 &
REM set MSYSCON=conemu64.exe &
REM "C:\msys64\usr\bin\zsh.exe" --login -i -new_console:C:"C:\msys64\mingw64.exe"