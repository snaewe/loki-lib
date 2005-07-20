Last update: July 20, 2005

Directions:

To use Loki, simply extract the files from the archive, 
give your compiler access to their include path:

- if you have a standard conforming compiler use "loki/include/loki" 
  or "loki/include" for "#include <loki/HeaderFile.h>" usage

- if you have a non-conforming compiler use "loki/include/noncc/loki" 
  or "loki/include/noncc" for "#include <loki/HeaderFile.h>" usage 


If you use the small object allocator directly or indirectly 
(through the Functor class) you must add SmallObj.cpp to your 
project/makefile.

If you use Singletons with longevity you must add Singleton.cpp 
to your project/makefile.


Compatibility:

Supported Compilers:
Gcc v2.95.3
Gcc v3.4
Microsoft Visual C++ v7.1
Microsoft Visual C++ v8.0

by special noncc files:
Borland C++ Builder v6.0
Microsoft Visual C++ v6.0
Microsoft Visual C++ v7.0
see also readme.txt files


Mostly Supported:
CodeWarrior 6.0

CodeWarrior has a problem with the Conversion template (see TypeManip.h) 
and, though it compiles it, it doesn't provide correct results. 
Consequently, the DerivedToFront algorithm in Typelist.h does not function. 
This affects the static dispatcher in Multimethods.h. As a fix, you must 
order the types (putting the most derived ones in the front) when providing 
the typelist argument to StaticDispatcher.



More info:

Andrei's page  : http://erdani.org 
Soureforge page: http://sourceforge.net/projects/loki-lib/



