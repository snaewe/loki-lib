if not exist tmp\ mkdir tmp


::  main.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ main.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main-msvc.exe" ..\..\lib\loki.lib tmp\main.obj



::  main2.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ main2.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"main2-msvc.exe" ..\..\lib\loki.lib tmp\main2.obj

