//////////////////////////////////
//Generated header: Singleton.cpp
//Forwards to the appropriate code
// that works on the detected compiler
//Generated on Sun Sep 08 18:42:42 2002


#ifdef LOKI_USE_REFERENCE
#	include "./Reference/Singleton.cpp"
#else
#	if (_MSC_VER >= 1300)
#		include "./MSVC/1300/Singleton.cpp"
#	elif (_MSC_VER >= 1200)
#		include "./MSVC/1200/Singleton.cpp"
#	elif (__BORLANDC__)
#		include "./Borland/Singleton.cpp"
#	elif (__MWERKS__)
#		include "./Reference/Singleton.cpp"
#	elif ( (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 95)) )
#		include "./Reference/Singleton.cpp"
#	else
		//Define LOKI_USE_REFERENCE and get back to us on the results
#		error Compiler not tested with Loki, #define LOKI_USE_REFERENCE
#	endif
#endif
