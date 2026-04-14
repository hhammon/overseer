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

clang++ -c -o overseer.o -I../src %THIRD_PARTY_INCLUDES% ../src/main.cpp -O0 -g -Wall -Wextra -Werror
link /nologo /OUT:overseer.exe overseer.o %THIRD_PARTY_OBJ% /SUBSYSTEM:WINDOWS /DEBUG

popd