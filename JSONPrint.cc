/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2014.  All rights reserved.
 *
 *  PROJECT:     JSON
 *
 *  MODULE:      JSON printer
 *
 ******************************************************************************/

#include "JSONPrint.h"
#include <algorithm>

namespace masuma
{
  namespace json
  {
    PrettyPrint::PrettyPrint( std::ostream& out,
                              size_t        items,
                              bool          inArray,
                              const std::string tab )
      : indent(tab+"  "), items(items), item(0), printingArray(inArray), out(out)
    {
    }

    void
    PrettyPrint::printObject( const Object& object )
    {
      if( object.size() == 1 )
      {
        out << '{' << '"' << object.begin()->name() << "\": ";

        printValueValue( object.begin()->value );

        out << '}' << (isLastItem() ? "" : ",") << '\n';
      }
      else
      {
        out << (printingArray ? indent  : "") << "{\n";

        std::for_each( object.begin(), object.end(),
                       PrettyPrintObject( out, object.size(), printingArray, indent ) );

        out << indent << '}' << (isLastItem() ? "" : ",") << '\n';
      }
    }

    void
    PrettyPrint::printArray( const Array& array )
    {
      if( array.size() == 1 )
      {
        out << '[';

        printValueValue( *array.begin() );

        out << ']' << (isLastItem() ? "" : ",") << '\n';
      }
      else
      {
        const bool isPOD = (!array.begin()->isArray() &&
                            !array.begin()->isObject());

        out << '[' << (isPOD ? "" : "\n");

        std::for_each( array.begin(), array.end(),
                       PrettyPrintArray( out, array.size(), indent ) );

        out << (isPOD ? "" : indent) << ']' << (isLastItem() ? "" : ",") << '\n';
      }
    }

    void
    PrettyPrint::printValueValue( const Value& value )
    {
      if( value.is<std::string>() )
      {
        out << '"' << value << '"';
      }
      else
      {
        out << value;
      }
    }

    void
    PrettyPrint::printValue( const Value& value, bool newLineAfterPOD )
    {
      printValueValue( value );

      out << (isLastItem() ? "" : ",");

      if( newLineAfterPOD ) out << '\n';
    }

    void
    PrettyPrint::printInstance( const Value& instance, bool newLineAfterPOD )
    {
      if( instance.isObject() )
      {
        printObject( instance );
      }
      else if( instance.isArray() )
      {
        printArray( Array {instance} );
      }
      else
      {
        printValue( instance, newLineAfterPOD );
      }
    }

    PrettyPrintObject::PrettyPrintObject( std::ostream& out,
                                          size_t items,
                                          bool inArray,
                                          const std::string tab )
      : PrettyPrint(out, items, inArray, tab )
    {
    }

    void
    PrettyPrintObject::operator()( const NameValuePair& p )
    {
      out << indent << '"' << p.name() << "\": ";

      ++item;

      printInstance( p.value );
    }

    void
    PrettyPrintObject::print( std::ostream& out, const Object& object )
    {
      out << "{\n";

      std::for_each( object.begin(), object.end(),
                     PrettyPrintObject(out, object.size(), false) );

      out << "}\n";
    }

    PrettyPrintArray::PrettyPrintArray( std::ostream& out,
                                        size_t        items,
                                        const std::string tab )
      : PrettyPrint(out, items, true, tab )
    {
    }

    void
    PrettyPrintArray::operator()( const Value& value )
    {
      ++item;

      printInstance( value, false );
    }
  }
}
