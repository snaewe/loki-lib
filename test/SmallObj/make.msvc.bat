if not exist tmp\ mkdir tmp

cl -c -DNDEBUG -Zm200 -O2 -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -I"..\..\include\loki" -Fotmp\ SmallObjBench.cpp ..\..\src\SmallObj.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" tmp\SmallObjBench.obj tmp\SmallObj.obj 