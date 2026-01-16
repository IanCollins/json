/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#include "ValueProxy.h"
#include "JSON.h"

#include <cstdlib>
#include <iostream>
#include <utility>

namespace masuma::json
{
  ValueProxy::ValueProxy( Object*      object,
                          Value        value,
                          size_t       index,
                          std::string  name )
    : Value( std::move(value) ), object( object ), name(std::move( name )), index( index )
  {
  }

  ValueProxy&
  ValueProxy::operator=( const Value& v )
  {
    if( index >= object->data->size() )
    {
      object->add( name, v );
    }
    else
    {
      object->data->at( index ).value = v;
    }
    return *this;
  }

  ValueProxy&
  ValueProxy::operator=( Value&& v )
  {
    if( index >= object->data->size() )
    {
      object->add( name, std::move(v) );
    }
    else
    {
      object->data->at( index ).value = v;
    }
    return *this;
  }

  ValueProxy&
  ValueProxy::operator=(const ValueProxy& v)
  {
    return operator=(Value( v ));
  }

  bool
  ValueProxy::has( const std::string& s ) const
  {
    return as<Object>().has( s );
  }

  ValueProxy
  ValueProxy::operator[](const char* s) const
  {
    return as<Object>().operator[](std::string( s ));
  }

  ValueProxy
  ValueProxy::operator[](const char* s)
  {
    return addToObjectIfNull( s );
  }

  ValueProxy
  ValueProxy::operator[]( const std::string& s ) const
  {
    return as<Object>().operator[](s);
  }

  ValueProxy
  ValueProxy::operator[]( const std::string& s )
  {
    return addToObjectIfNull( s );
  }

  Array::Proxy
  ValueProxy::operator[](int n) const
  {
    return as<Array>()[n];
  }

  Array::Proxy
  ValueProxy::operator[](int n)
  {
    return as<Array>()[n];
  }

  int64_t
  ValueProxy::operator++()
  {
    int64_t n = *this;
    *this = ++n;
    return n;
  }

  ValueProxy
  ValueProxy::operator+=(int64_t i)
  {
    int64_t n = *this;
    *this = n + i;
    return *this;
  }

  ValueProxy
  ValueProxy::addToObjectIfNull( const std::string& s )
  {
    if( isNull() )
    {
      setItem<Object>();

      object->add( name, *this );

      object = &as<Object>();

      clearItem();
      index  = 0;
      name   = s;
      return *this;
    }
    else
    {
      return as<Object>()[s];
    }
  }

  Array
  ValueProxy::addArrayObjectIfNull( const std::string& )
  {
    if( isNull() )
    {
      setItem<Array>();

      object->add( name, as<Array>() );
    }

    return as<Array>();
  }

  bool
  ValueProxy::erase( const std::string& bit )
  {
    return as<Object>().erase( bit );
  }

  bool
  ValueProxy::erase( size_t n )
  {
    return as<Array>().erase( n );
  }

  size_t
  ValueProxy::objects() const
  {
    return as<Object>().objects();
  }

  std::string
  operator+( const std::string& s, const ValueProxy& v )
  {
    return s + v.as<std::string>();
  }
}
