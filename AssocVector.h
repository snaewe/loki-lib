//////////////////////////////////
//Generated header: AssocVector.h
//Forwards to the appropriate code
// that works on the detected compiler
//Generated on Thu Aug 22 21:04:30 2002


#ifdef LOKI_USE_REFERENCE
#	include ".\Reference\AssocVector.h"
#else
#	if (_MSC_VER >= 1300)
#		include ".\MSVC\1300\AssocVector.h"
#	elif (_MSC_VER >= 1200)
#		include ".\MSVC\1200\AssocVector.h"
#	elif (__BORLANDC__)
#		include ".\Borland\AssocVector.h"
#	elif (__MWERKS__)
#		include ".\Reference\AssocVector.h"
#	elif ( (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 95)) )
#		include ".\Reference\AssocVector.h"
#	else
		//Define LOKI_USE_REFERENCE and get back to us on the results
#		error Compiler not tested with Loki, #define LOKI_USE_REFERENCE
#	endif
#endif
