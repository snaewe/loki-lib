///////////////////////////////////////////////////////////////////////////////
// Unit Test for Loki
//
// Copyright Terje Slettebø and Pavel Vozenilek 2002.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.
//
// Last update: September 16, 2002
///////////////////////////////////////////////////////////////////////////////

#ifdef __INTEL_COMPILER
# pragma warning(disable: 111 193 304 383 444 981 1418)
#endif

#include "LokiTest.h"

int main()
{
LokiTest test;

const int result=test.result();

#if __BORLANDC__ || __GNUC__
	while(true); // Stop console window from closing if run from IDE.
#endif

return result;
}
