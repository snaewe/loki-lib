if not exist tmp\ mkdir tmp


::  DeletableSingleton.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ DeletableSingleton.cpp

if not defined LOKI_MSVC_NOLIB (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"DeletableSingleton-msvc.exe" ..\..\lib\loki.lib tmp\DeletableSingleton.obj 

) else (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"DeletableSingleton-msvc.exe" tmp\DeletableSingleton.obj ..\..\lib\SmallObj.obj ..\..\lib\Singleton.obj 

)


::  Phoenix.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ Phoenix.cpp

if not defined LOKI_MSVC_NOLIB (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"Phoenix-msvc.exe" ..\..\lib\loki.lib tmp\Phoenix.obj 

) else (

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"Phoenix-msvc.exe" tmp\Phoenix.obj ..\..\lib\SmallObj.obj ..\..\lib\Singleton.obj 

)
