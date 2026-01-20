/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#pragma once

#include "JSONValue.h"
#include "JSONArray.h"
#include "JSON.h"

namespace masuma::json
{
  class Object;

  struct ValueProxy : Value
  {
    Object*     object;
    std::string name;
    size_t      index;

    ValueProxy( Object*, Value, size_t, std::string  );

    ValueProxy addToObjectIfNull( const std::string& );
    Array addArrayObjectIfNull( const std::string & s );

    ValueProxy& operator=( const Value& ) override;
    ValueProxy& operator=( Value&& ) override;

    ValueProxy& operator=(const ValueProxy & v);

    template <typename T >
    Array operator<<(const T & obj)
    {
      Array array( addArrayObjectIfNull( name ) );

      return array << obj;
    }

    [[nodiscard]] const char* c_str() const
    {
      return as<std::string>().c_str();
    }

    [[nodiscard]] bool has( const std::string& ) const;

    ValueProxy operator[]( const char* ) const;

    ValueProxy operator[]( const char* );

    ValueProxy operator[]( const std::string& ) const;

    ValueProxy operator[](const std::string& );

    Array::Proxy operator[]( int ) const;

    Array::Proxy operator[]( int );

    int64_t operator++();

    ValueProxy operator+=(int64_t i);

    [[nodiscard]] size_t size() const
    {
      return as<Array>().size();
    }

    [[nodiscard]] size_t empty() const
    {
      return as<Array>().empty();
    }

    template <typename T >
    int find( T t ) const
    {
      return as<std::string>().find( t );
    }

    [[nodiscard]] Array::const_iterator begin() const
    {
      return as<Array>().begin();
    }

    [[nodiscard]] Array::const_iterator end() const
    {
      return as<Array>().end();
    }

    template <typename Iterator>
    void append( Iterator begin, Iterator end )
    {
      as<Array>().append( begin, end );
    }

    template <typename Container>
    void append( Container container )
    {
      as<Array>().append( container.begin(), container.end() );
    }

    bool erase( const std::string& );
    bool erase( size_t );

    [[nodiscard]] size_t objects() const;

    friend std::string operator+( const std::string&, const ValueProxy& );
  };

//    template <> inline void
//    Value::Holder<ValueProxy>::stream( std::ostream& out ) const
//    {
//      out << value;
//    }

  template <typename T> T
  operator/( const Value& lhs, T rhs )
  {
    const T t = lhs;
    return t/rhs;
  }
}

