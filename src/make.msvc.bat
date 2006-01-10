if not exist ..\lib\ mkdir ..\lib

if not exist tmp\ mkdir tmp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W4 -I"." -I"..\include" -Fo.\tmp\ SmallObj.cpp Singleton.cpp OrderedStatic.cpp SafeFormat.cpp

link /lib /NOLOGO /OUT:"..\lib\loki.lib" tmp\SmallObj.obj tmp\Singleton.obj tmp\OrderedStatic.obj tmp\SafeFormat.obj

