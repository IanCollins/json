/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#include <system/String.h>

#include "JSON.h"
#include "ValueProxy.h"

#include <cstdlib>
#include <iostream>

namespace masuma::json
{
  const char* const Value::typeNames[Value::numberOfTypes] =
  {
    "Integer", "Boolean", "Number", "Null", "String", "Object", "Array"
  };

  Object::Object( std::istream& in )
    : Object()
  {
    *this = scan(in);
  }

  Value
  Object::getValue( const std::string& value )
  {
    if( value[0] == '"' )
    {
      return masuma::system::trimQuotes(value);
    }
    else if( value == "true" )
    {
      return true;
    }
    else if( value == "false" )
    {
      return false;
    }
    else if( value == "null" )
    {
      return null;
    }
    else
    {
      if( value.find('.') != std::string::npos )
      {
        return strtod( value.c_str(), nullptr );
      }
      else
      {
        int64_t l = strtoll( value.c_str(), nullptr, 10 );
        return l;
      }
    }
  }

  bool
  MatchName::operator()( const NameValuePair& p )
  {
    return p.name() == name;
  }

  bool
  Object::has( const std::string& name ) const
  {
    return (std::find_if( data->begin(),data->end(), MatchName(name) ) !=data->end());
  }

  bool
  Object::erase( const std::string& name )
  {
    auto i = std::find_if(data->begin(),data->end(), MatchName(name));

    if( i != data->end() )
    {
      data->erase(i);
      return true;
    }

    return false;
  }

  ValueProxy
  Object::operator[]( const std::string& name ) const
  {
    auto i = std::find_if(data->begin(),data->end(), MatchName(name) );

    if( i == data->end() )
      throw std::runtime_error( name+" missing" );

    Value v(i->value);
    return ValueProxy( const_cast<Object*>(this), v, i-data->begin(), name );
  }

  ValueProxy
  Object::operator[]( const std::string& name )
  {
    auto i = std::find_if(data->begin(),data->end(), MatchName(name) );

    if( i !=data->end() )
    {
      return ValueProxy( this, i->value, i-data->begin(), name );
    }
    else
    {
      return ValueProxy( this, Value(), data->size(), name );
    }
  }

  ValueProxy
  Object::operator[]( const char* s ) const
  {
    return (*this)[ std::string(s) ];
  }

  ValueProxy
  Object::operator[]( const char* s )
  {
    return (*this)[ std::string(s) ];
  }

  Object
  Object::clone() const
  {
    std::stringstream ss;

    ss << *this;

    return scan(ss);
  }

  std::string
  Object::asString() const
  {
    std::ostringstream out;

    out << *this;

    return out.str();
  }

  std::ostream&
  operator<<( std::ostream& out, const Object& object )
  {
    out << '{';

    for( auto i = object.data->begin(); i != object.data->end(); ++i )
    {
      if( i != object.data->begin() )
      {
        out << ',';
      }

      out << '"' << i->name() << '"' << ':';

      if( i->value.is<std::string>() )
      {
        out << '"' << i->value << '"';
      }
      else
      {
        out << i->value;
      }
    }

    return out << '}';
  }
}
