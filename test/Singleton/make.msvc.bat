if not exist tmp\ mkdir tmp



::  DeletableSingleton.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ DeletableSingleton.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"DeletableSingleton-msvc.exe" ..\..\lib\loki.lib tmp\DeletableSingleton.obj 




::  Phoenix.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ Phoenix.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"Phoenix-msvc.exe" ..\..\lib\loki.lib tmp\Phoenix.obj 




::  Dependencies.cpp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W0 -wd4710 -I"." -I"..\..\include" -Fotmp\ Dependencies.cpp

link /NOLOGO /SUBSYSTEM:CONSOLE /incremental:no /OUT:"Dependencies-msvc.exe" ..\..\lib\loki.lib tmp\Dependencies.obj

