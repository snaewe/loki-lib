if not exist tmp\ mkdir tmp


::  main.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ main.cpp

if not defined LOKI_MSVC_NOLIB (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" ..\..\lib\loki.lib tmp\main.obj 

) else (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" tmp\main.obj ..\..\lib\SmallObj.obj ..\..\lib\Singleton.obj 

)


::  main2.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ main2.cpp

if not defined LOKI_MSVC_NOLIB (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main2-msvc.exe" ..\..\lib\loki.lib tmp\main2.obj 

) else (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main2-msvc.exe" tmp\main2.obj ..\..\lib\SmallObj.obj ..\..\lib\Singleton.obj 

)
