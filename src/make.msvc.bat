if not exist mkdir ..\lib\ mkdir ..\lib

cl -c -Zm200 -O1 -MT -EHsc -GR -W4 -I"." -I"..\include\loki" -Fo..\lib\ SmallObj.cpp Singleton.cpp
