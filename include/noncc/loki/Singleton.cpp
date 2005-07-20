///////////////////////////////////
// Generated header: Singleton.cpp
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
///////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#   include "../../loki/Singleton.cpp"
#else
#   if (__INTEL_COMPILER)
#       include "../../loki/Singleton.cpp"
#   elif (__MWERKS__)
#       include "../../loki/Singleton.cpp"
#   elif (__BORLANDC__ >= 0x560)
#       include "../Borland/Singleton.cpp"
#   elif (_MSC_VER >= 1301)
#       include "../../loki/Singleton.cpp"
#   elif (_MSC_VER >= 1300)
#       include "../MSVC/1300/Singleton.cpp"
#   elif (_MSC_VER >= 1200)
#       include "../MSVC/1200/Singleton.cpp"
#   else
#       include "../../loki/Singleton.cpp"
#   endif
#endif
