///////////////////////////////////////////
// Generated header: HierarchyGenerators.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
///////////////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "../../loki/HierarchyGenerators.h"
#else
#	if (__INTEL_COMPILER)
#		include "../../loki/HierarchyGenerators.h"
#	elif (__MWERKS__)
#		include "../../loki/HierarchyGenerators.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "../Borland/HierarchyGenerators.h"
#	elif (_MSC_VER >= 1301)
#		include "../../loki/HierarchyGenerators.h"
#	elif (_MSC_VER >= 1300)
#		include "../MSVC/1300/HierarchyGenerators.h"
#	elif (_MSC_VER >= 1200)
#		include "../MSVC/1200/HierarchyGenerators.h"
#	else
#		include "../../loki/HierarchyGenerators.h"
#	endif
#endif
