@ECHO OFF
set LOKI_TMP=%1
if defined LOKI_TMP (
	set PATH=%~1;%PATH%
)
set LOKI_TMP=
@ECHO ON

if not exist lib\ mkdir lib

mingw32-make 
