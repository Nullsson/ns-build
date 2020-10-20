@echo off
if not exist build mkdir build
pushd build
cl -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 ..\code\ns_build_main.c /link /out:ns_build.exe
popd
