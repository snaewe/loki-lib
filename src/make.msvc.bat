if not exist ..\lib\ mkdir ..\lib

cl -c -Zm200 -O2 -DNDEBUG -MT -EHsc -GR -W4 -I"." -I"..\include" -Fo..\lib\ SmallObj.cpp Singleton.cpp
