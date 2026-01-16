/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2014.  All rights reserved.
 *
 *  PROJECT:     JSON parser
 *
 *  MODULE:      json
 *
 ******************************************************************************/

#ifndef _json_Scan_h_
#define _json_Scan_h_

#include <cstdlib>
#include <iostream>
#include <utils/String.h>
#include "JSON.h"

namespace masuma
{
  namespace json
  {
    struct State
    {
      virtual Value scan( std::istream& ) = 0;

      static bool isStructural( char );
      bool isSpecial( char c );

      static Value processSpecial( char, std::istream& );
    };

    struct InObject : State
    {
      Value scan( std::istream& in );
    };

    struct Nowhere : State
    {
      Value scan( std::istream& in );
    };

    struct InString : State
    {
      Value scan( std::istream& );
    };

    struct InName : State
    {
      Value scan( std::istream& );
    };

    struct InValue : State
    {
      const char end;

      char c;

      InValue( char end ) : end(end) {}

      Value scan( std::istream& );

      bool atEnd() const { return c == end; }
    };

    struct InArray : State
    {
      Value scan( std::istream& );
    };
  }
}

#endif
