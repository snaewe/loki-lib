///////////////////////////////
// Generated header: Factory.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
///////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/Factory.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/Factory.h"
#	elif (__MWERKS__)
#		include "../../loki/Factory.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/Factory.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/Factory.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/Factory.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/Factory.h"
#	else
#		include "../../loki/Factory.h"
#	endif
#endif
