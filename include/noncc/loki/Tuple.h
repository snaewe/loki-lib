/////////////////////////////
// Generated header: Tuple.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
/////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/Tuple.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/Tuple.h"
#	elif (__MWERKS__)
#		include "../../loki/Tuple.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/Tuple.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/Tuple.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/Tuple.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/Tuple.h"
#	else
#		include "../../loki/Tuple.h"
#	endif
#endif
