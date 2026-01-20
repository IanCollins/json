/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2014.  All rights reserved.
 *
 *  PROJECT:     JSON
 *
 *  MODULE:      JSON printer
 *
 ******************************************************************************/

#ifndef _cgi_JSONPrint_h_
#define _cgi_JSONPrint_h_

#include "JSON.h"

namespace masuma
{
  namespace json
  {
    class PrettyPrint
    {
      void printValueValue( const Value& );

    protected:

      const std::string indent;

      const size_t items;
      size_t item;

      const bool printingArray;

      std::ostream& out;

    public:

      PrettyPrint( std::ostream&, size_t, bool, const std::string = "" );

      bool isFirstItem() const { return item == 1; }
      bool isLastItem() const { return item == items; }

      void printObject( const Object& );
      void printArray( const Array& );
      void printValue( const Value&, bool newLineAfterPOD );

      void printInstance( const Value&, bool newLineAfterPOD = true );
    };

    struct PrettyPrintObject : PrettyPrint
    {
      PrettyPrintObject( std::ostream&, size_t, bool, const std::string = "" );

      void operator()( const NameValuePair& );

      static void print( std::ostream&, const Object& );
    };

    struct PrettyPrintArray : PrettyPrint
    {
      PrettyPrintArray( std::ostream&, size_t, const std::string = "" );

      void operator()( const Value& );
    };
  }
}

#endif
