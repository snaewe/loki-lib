////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 by Peter Kümmel
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_LOKIEXPORT_INC_
#define LOKI_LOKIEXPORT_INC_

// $Id$


#ifdef __GNUC__

#ifdef _HAVE_GCC_VISIBILITY
#define LOKI_EXPORT_SPEC __attribute__ ((visibility("default")))
#define LOKI_IMPORT_SPEC
#else
#define LOKI_EXPORT_SPEC
#define LOKI_IMPORT_SPEC
#endif

#else

#ifdef _WIN32
#define LOKI_EXPORT_SPEC __declspec(dllexport)
#define LOKI_IMPORT_SPEC __declspec(dllimport)
#else
#define LOKI_EXPORT_SPEC
#define LOKI_IMPORT_SPEC
#endif

#endif


#if (defined(LOKI_MAKE_DLL) && defined(LOKI_DLL)) || \
    (defined(LOKI_MAKE_DLL) && defined(LOKI_STATIC)) || \
    (defined(LOKI_DLL) && defined(LOKI_STATIC))
#error export macro error: you could not build AND use the library
#endif

#ifdef LOKI_MAKE_DLL
#define LOKI_EXPORT LOKI_EXPORT_SPEC
#endif

#ifdef LOKI_DLL
#define LOKI_EXPORT LOKI_IMPORT_SPEC
#endif

#ifdef LOKI_STATIC
#define LOKI_EXPORT
#endif

#if !defined(LOKI_EXPORT) && !defined(EXPLICIT_EXPORT)
#define LOKI_EXPORT
#endif

#ifndef LOKI_EXPORT
#error export macro error: LOKI_EXPORT was not defined, disable EXPLICIT_EXPORT or define a export specification
#endif


#endif // end file guardian

