if not exist tmp\ mkdir tmp

cl -c -Zm200 -O2 -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\include" -I"..\include\loki" -Fotmp\ Factory.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" tmp\Factory.obj ..\lib\SmallObj.obj ..\lib\Singleton.obj 