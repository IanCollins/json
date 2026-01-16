/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      cgi
 *
 *******************************************************************************/

#include "JSONAlgorithm.h"

namespace masuma::json
{
  bool
  operator==( const Object& lhs, const Object& rhs )
  {
    if( lhs.size() != rhs.size() )
    {
      return false;
    }

    auto ref   = lhs.begin();
    auto other = rhs.begin();

    for( ; ref != lhs.end(); ++ref, ++other )
    {
      if( ref->name() != other->name() || ref->value != other->value )
      {
        return false;
      }
    }

    return true;
  }

  bool
  operator==( const Array& lhs, const Array& rhs )
  {
    if( lhs.size() != rhs.size() )
    {
      return false;
    }

    auto ref   = lhs.begin();
    auto other = rhs.begin();

    for( ; ref != lhs.end(); ++ref, ++other )
    {
      if( *ref != *other )
      {
        return false;
      }
    }

    return true;
  }

  bool
  orderObjectByValue( const NameValuePair& lhs, const NameValuePair& rhs )
  {
    return lhs.value < rhs.value;
  }

  Array
  normaliseArray( Array array )
  {
    for( const auto& value : array )
    {
      if( value.isObject() )
      {
        normalise( value );
      }
      else if( value.isArray() )
      {
        normaliseArray( value );
      }
    }

    std::sort( array.begin(), array.end() );

    return array;
  }

  bool
  orderObjectByName( const NameValuePair& lhs, const NameValuePair& rhs )
  {
    return lhs.name() < rhs.name();
  }

  Object
  normalise( Object object, bool sortArrays )
  {
    for( const auto& pair : object )
    {
      if( pair.isObject() )
      {
        normalise( pair.value, sortArrays );
      }
      if( sortArrays && pair.isArray() )
      {
        normaliseArray( pair.value );
      }
    }

    std::sort( object.begin(), object.end(), orderObjectByName );

    return object;
  }

  bool
  haveSameItems( const Object& lhs, const Object& rhs )
  {
    if( lhs.size() != rhs.size() )
    {
      return false;
    }

    auto ref   = lhs.begin();
    auto other = rhs.begin();

    for( ; ref != lhs.end(); ++ref, ++other )
    {
      if( ref->name() != other->name() )
      {
        return false;
      }
    }

    return true;
  }

  Array
  arrayIntersection( const Array& first, const Array& second )
  {
    Array result;

    for( const auto& value : first )
    {
      if( std::find( second.begin(), second.end(), value ) != second.end() )
      {
        result << value;
      }
    }

    return result;
  }

  Array
  arrayDifference( const Array& first, const Array& second )
  {
    Array result;

    for( const auto& value : first )
    {
      if( std::find( second.begin(), second.end(), value ) == second.end() )
      {
        result << value;
      }
    }

    return result;
  }

  Object
  setDifference( const Object& first, const Object& second )
  {
    Object result;

    for( const auto& ref : first )
    {
      if( !second.has( ref.name() ) )
      {
        result[ref.name()] = ref.value;
      }
      else if( ref.isObject() )
      {
        const Object temp = setDifference( ref.value, second[ref.name()] );

        if( !temp.empty() )
        {
          result[ref.name()] = temp;
        }
      }
      else if( ref.isArray() )
      {
        const Array temp = arrayDifference( ref.value, second[ref.name()] );

        if( !temp.empty() )
        {
          result[ref.name()] = temp;
        }
      }
    }

    return result;
  }
}
