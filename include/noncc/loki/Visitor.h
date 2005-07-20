///////////////////////////////
// Generated header: Visitor.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
///////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/Visitor.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/Visitor.h"
#	elif (__MWERKS__)
#		include "../../loki/Visitor.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/Visitor.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/Visitor.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/Visitor.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/Visitor.h"
#	else
#		include "../../loki/Visitor.h"
#	endif
#endif
