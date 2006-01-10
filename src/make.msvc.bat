
cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W4 -I"." -I"..\include" SmallObj.cpp Singleton.cpp OrderedStatic.cpp SafeFormat.cpp

link /lib /NOLOGO /OUT:"..\lib\loki.lib" SmallObj.obj Singleton.obj OrderedStatic.obj SafeFormat.obj

del *.obj

