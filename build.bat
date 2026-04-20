@echo off
setlocal enabledelayedexpansion

if not exist build mkdir build
pushd build

set THIRD_PARTY_INCLUDES=
set THIRD_PARTY_OBJ=
for /D %%d in ("../src/third_party/*") do (
	set THIRD_PARTY_INCLUDES=!THIRD_PARTY_INCLUDES! -I../src/third_party/%%d
)

for /D %%d in ("../src/third_party/*") do (
	for %%f in ("../src/third_party/%%d/*.cpp") do (
		set "obj=%%~nf.o"
		set THIRD_PARTY_OBJ=!THIRD_PARTY_OBJ! !obj!
		if not exist !obj! (
			clang++ -c %THIRD_PARTY_INCLUDES% ../src/third_party/%%d/%%f -O3
		)
	)
)

set WARNINGS=-Wall -Wextra -Werror -Wno-unused-function -Wno-switch -Wno-gnu-alignof-expression -Wno-writable-strings ^
-Wno-missing-declarations -Wno-missing-field-initializers

clang++ -c -o overseer.o -I../src %THIRD_PARTY_INCLUDES% ../src/main.cpp -O0 -g -DBUILD_DEBUG=1 %WARNINGS%
@REM clang++ -c -o overseer.o -I../src %THIRD_PARTY_INCLUDES% ../src/main.cpp -O3 -DBUILD_DEBUG=0 %WARNINGS%
rc /nologo /fo logo.res ..\data\logo.rc
link /nologo /OUT:overseer.exe overseer.o logo.res %THIRD_PARTY_OBJ% /SUBSYSTEM:WINDOWS /DEBUG /OPT:REF

popd
