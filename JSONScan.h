/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2014.  All rights reserved.
 *
 *  PROJECT:     JSON parser
 *
 *  MODULE:      json
 *
 ******************************************************************************/

#pragma once

#include "JSON.h"

namespace masuma::json
{
  struct State
  {
    virtual ~State() = default;

    virtual Value scan( std::istream& ) = 0;

    static bool isStructural( char );
    bool isSpecial( char c );

    static Value processSpecial( char, std::istream& );
  };

  struct InObject : State
  {
    Value scan( std::istream& in ) override;
  };

  struct Nowhere : State
  {
    Value scan( std::istream& in );
  };

  struct InString : State
  {
    Value scan( std::istream& ) override;
  };

  struct InName : State
  {
    Value scan( std::istream& ) override;
  };

  struct InValue : State
  {
    const char end;

    char c {};

    InValue( char end ) : end(end) {}

    Value scan( std::istream& ) override;

    bool atEnd() const { return c == end; }
  };

  struct InArray : State
  {
    Value scan( std::istream& ) override;
  };
}
