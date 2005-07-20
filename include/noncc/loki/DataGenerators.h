//////////////////////////////////////
// Generated header: DataGenerators.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
//////////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/DataGenerators.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/DataGenerators.h"
#	elif (__MWERKS__)
#		include "../../loki/DataGenerators.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/DataGenerators.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/DataGenerators.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/DataGenerators.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/DataGenerators.h"
#	else
#		include "../../loki/DataGenerators.h"
#	endif
#endif
