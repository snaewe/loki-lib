//////////////////////////////////////
//Generated header: AbstractFactory.h
//Forwards to the appropriate code
// that works on the detected compiler


#ifdef LOKI_USE_REFERENCE
#	include ".\Reference\AbstractFactory.h"
#else
#	if (_MSC_VER >= 1300)
#		include ".\MSVC\1300\AbstractFactory.h"
#elif (_MSC_VER >= 1200)
#		include ".\MSVC\1200\AbstractFactory.h"
#elif ( (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) )
#		include ".\Reference\AbstractFactory.h"
#elif (__BORLANDC__)
#		include ".\Borland\AbstractFactory.h"
#	else
		//Define LOKI_USE_REFERENCE and get back to us on the results
#		error Compiler not tested with Loki, #define LOKI_USE_REFERENCE
#	endif
#endif
