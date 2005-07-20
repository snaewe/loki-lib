////////////////////////////////////
// Generated header: static_check.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
////////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/static_check.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/static_check.h"
#	elif (__MWERKS__)
#		include "../../loki/static_check.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/static_check.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/static_check.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/static_check.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/static_check.h"
#	else
#		include "../../loki/static_check.h"
#	endif
#endif
