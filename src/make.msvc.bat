if not exist ..\lib\ mkdir ..\lib

if not defined LOKI_MSVC_NOLIB (

if not exist tmp\ mkdir tmp

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W4 -I"." -I"..\include" -Fo.\tmp\ SmallObj.cpp Singleton.cpp

lib /NOLOGO /OUT:"..\lib\loki.lib" tmp\SmallObj.obj tmp\Singleton.obj

) else (

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W4 -I"." -I"..\include" -Fo..\lib\ SmallObj.cpp Singleton.cpp

)