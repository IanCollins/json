/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      cgi
 *
 *******************************************************************************/

#include "JSONValue.h"
#include "JSONAlgorithm.h"

namespace masuma::json
{
  bool
  Value::operator==( const Value& other ) const
  {
    if( !sameTypeAs( other ) )
    {
      return false;
    }

    if( is<int64_t>() )
    {
      return as<int64_t>() == other.as<int64_t>();
    }
    else if( is<double>() )
    {
      return as<double>() == other.as<double>();
    }
    else if( is<bool>() )
    {
      return as<bool>() == other.as<bool>();
    }
    else if( is<std::string>() )
    {
      return as<std::string>() == other.as<std::string>();
    }
    else if( is<Object>() )
    {
      return as<Object>() == other.as<Object>();
    }
    else if( is<Array>() )
    {
      return as<Array>() == other.as<Array>();
    }
    else if( is<Null>() )
    {
      return other.is<Null>();
    }

    return false;
  }

  bool
  Value::operator==( const ValueProxy& vp ) const
  {
    const Value& value = vp;
    return operator==(value);
  }

  bool
  Value::operator<( const Value& other ) const
  {
    if( !sameTypeAs( other ) )
    {
      return false;
    }

    if( is<int64_t>() )
    {
      return as<int64_t>() < other.as<int64_t>();
    }
    else if( is<double>() )
    {
      return as<int64_t>() < other.as<int64_t>();
    }
    else if( is<std::string>() )
    {
      return as<std::string>() < other.as<std::string>();
    }
    else if( is<Object>() )
    {
      return false;
      //return as<Object>() < other.as<Object>();
    }
    else if( is<Array>() )
    {
      return as<Array>()[0] < other.as<Array>()[0];
    }

    return false;
  }

  bool
  Value::operator>( const Value& other ) const
  {
    if( !sameTypeAs( other ) )
    {
      return false;
    }

    if( is<int64_t>() )
    {
      return as<int64_t>() > other.as<int64_t>();
    }
    else if( is<double>() )
    {
      return as<int64_t>() > other.as<int64_t>();
    }
    else if( is<std::string>() )
    {
      return as<std::string>() > other.as<std::string>();
    }
    else if( is<Object>() )
    {
      return false;
      //return as<Object>() < other.as<Object>();
    }
    else if( is<Array>() )
    {
      return as<Array>()[0] > other.as<Array>()[0];
    }

    return false;
  }

  Value::Value( const Object& v )
    : item( new Holder<Object>(v) )
  {
  }

  Value::operator Object() const
  {
    return as<Object>();
  }
}
