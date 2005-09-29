if not exist tmp\ mkdir tmp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ Factory.cpp

if not defined LOKI_MSVC_NOLIB (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" ..\..\lib\loki.lib tmp\Factory.obj

) else (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" tmp\Factory.obj ..\..\lib\SmallObj.obj ..\..\lib\Singleton.obj 

)