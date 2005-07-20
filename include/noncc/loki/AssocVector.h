///////////////////////////////////
// Generated header: AssocVector.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
///////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/AssocVector.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/AssocVector.h"
#	elif (__MWERKS__)
#		include "../../loki/AssocVector.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/AssocVector.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/AssocVector.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/AssocVector.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/AssocVector.h"
#	else
#		include "../../loki/AssocVector.h"
#	endif
#endif
