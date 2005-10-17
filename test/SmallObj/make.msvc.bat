@ECHO OFF

echo -
echo - Use make.msvc.bat dll to link with dynamic runtile library
echo -

if not exist tmp\ mkdir tmp

if "%1"=="dll" (
	set MTMD=MD
	set OUT_EXE=main-dll-msvc.exe
	echo - using dynamic library: /MD
	echo -
) else (
	set MTMD=MT
	set OUT_EXE=main-msvc.exe
	echo - using static library: /MT
	echo -
)

@ECHO ON

:: main.cpp

cl -c -DNDEBUG -Zm200 -O2 -%MTMD% -EHsc -GR -W4 -wd4710 -I"." -I"..\..\include" -I"..\..\include\loki" -Fotmp\ SmallObjBench.cpp ..\..\src\SmallObj.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:%OUT_EXE% tmp\SmallObjBench.obj tmp\SmallObj.obj 


:: SmallSingleton.cpp

cl -c -DNDEBUG -Zm200 -O2 -%MTMD% -EHsc -GR -W4 -wd4710 -I"." -I"..\..\include" -I"..\..\include\loki" -Fotmp\ SmallSingleton.cpp ..\..\src\SmallObj.cpp ..\..\src\Singleton.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:SmallSingleton-msvc.exe tmp\SmallSingleton.obj tmp\SmallObj.obj tmp\Singleton.obj



@ECHO OFF
set MTMD=
set OUT_EXE=