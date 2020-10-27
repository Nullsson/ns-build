@echo off
if not exist build mkdir build
pushd build
clang-cl -D_CRT_SECURE_NO_DEPRECATE -Wno-writable-strings -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 ..\code\ns_build_main.cpp /link /out:ns_build.exe
popd
