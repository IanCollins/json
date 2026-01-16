/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      cgi
 *
 ******************************************************************************/

#include "JSONArray.h"
#include "JSONValue.h"
#include "JSON.h"
#include "ValueProxy.h"

#include <sstream>

namespace masuma::json
{
  Array::Proxy&
  Array::Proxy::operator=( const Value& v )
  {
    if( index >= array.size() )
    {
      array << v;
    }
    else
    {
      array.array->at(index) = v;
    }
    return *this;
  }

  Value
  Array::Proxy::operator[]( const std::string& s ) const
  {
    return as<Object>()[s];
  }

  ValueProxy
  Array::Proxy::operator[]( const std::string& s )
  {
    return as<Object>()[s];
  }

  ValueProxy
  Array::Proxy::operator[]( const char* s )
  {
    return (*this)[std::string(s)];
  }

  bool
  Array::Proxy::has( const std::string& s ) const
  {
    return as<Object>().has( s );
  }

  Array::Proxy
  Array::operator[]( unsigned n )
  {
    if( n >= size() )
      throw std::runtime_error( "Out of bounds" );

    return Proxy( *this, n );
  }

  Array::Proxy
  Array::operator[]( unsigned n ) const
  {
    return const_cast<Array*>(this)->operator[](n);
  }

  Array
  Array::clone() const
  {
    std::stringstream ss;

    ss << *this;

    return Value::scan(ss);
  }

  bool
  Array::erase( size_t index )
  {
    if( index < array->size() )
    {
      array->erase( array->begin()+index );
      return true;
    }
    return false;
  }

  bool
  Array::erase( const std::string& name )
  {
    return eraseIf([&](const std::string& s) {return s == name;});
  }

  Value::Value( const Array& v )
    : item( new Holder<Array>(v) ) {}

  Value::Value( std::initializer_list<Value> v )
    : item {new Holder<Array> {Array(v)}} {}

  Value::operator Array() const
  {
    return as<Array>();
  }

  std::ostream&
  operator<<( std::ostream& out, const Array& array )
  {
    out << '[';

    for( auto i = array.begin(); i != array.end(); ++i )
    {
      if( i != array.begin() )
      {
        out << ',';
      }

      if( i->is<std::string>() )
      {
        out << '"' << *i << '"';
      }
      else
      {
        out << *i;
      }
    }

    return out << ']';
  }

  bool
  Array::eraseNamedObject(const std::string& name)
  {
    return eraseIf([&]( const Object& o )
                   {
                     return o.has("name") && std::string (o["name"]) == name;
                   });
  }

  template <> void
  Value::Holder<Array>::stream( std::ostream& out ) const
  {
    out << value;
  }
}
