//////////////////////////////
//Generated header: Visitor.h
//Forwards to the appropriate code
// that works on the detected compiler
//Generated on Sun Sep 08 18:42:42 2002


#ifdef LOKI_USE_REFERENCE
#	include "./Reference/Visitor.h"
#else
#	if (_MSC_VER >= 1300)
#		include "./MSVC/1300/Visitor.h"
#	elif (_MSC_VER >= 1200)
#		include "./MSVC/1200/Visitor.h"
#	elif (__BORLANDC__)
#		include "./Borland/Visitor.h"
#	elif (__MWERKS__)
#		include "./Reference/Visitor.h"
#	elif ( (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 95)) )
#		include "./Reference/Visitor.h"
#	else
		//Define LOKI_USE_REFERENCE and get back to us on the results
#		error Compiler not tested with Loki, #define LOKI_USE_REFERENCE
#	endif
#endif
