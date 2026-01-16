/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      cgi
 *
 ******************************************************************************/

#pragma once

#include <vector>
#include <algorithm>

#include <memory>
#include "JSONValue.h"

namespace masuma::json
{
  class Value;
  struct ValueProxy;

  class Array
  {
    using Vector   = std::vector<Value>;
    using ItemType = std::shared_ptr<Vector>;

    ItemType array;

  public:

    struct Proxy : Value
    {
      using Value::operator=;

      Array& array;
      size_t index;

      Proxy( Array& array, size_t index )
        : Value( array.array->at(index) ), array(array), index( index ) {}

      Value operator[]( const std::string& ) const;
      ValueProxy operator[]( const std::string& );

      Value operator[]( const char* s ) const
      {
        return (*this)[std::string(s)];
      }

      ValueProxy operator[]( const char* );

      [[nodiscard]] size_t size() const { return array.size(); }

      Proxy& operator=( const Value& );

      //operator Value();

      [[nodiscard]] bool has( const std::string& ) const;
    };

    Array()
      : array {std::make_shared<Vector>()} {}

    Array( Vector::iterator first, Vector::iterator last )
      : array {std::make_shared<Vector>(first,last)} {}

    explicit Array( size_t n )
      : array {std::make_shared<Vector>(n)} {}

    // Be very careful here, brace initialisation will use the next constructor,
    // not this one!
    //
    explicit Array( const Value& value ) : Array {value.as<Array>()} {}

    Array( std::initializer_list<Value> args )
      : array {std::make_shared<Vector>(args)} {}

    Array( const Array& ) = default;
    Array( Array&& ) = default;

    Array& operator=( const Array& ) = default;
    Array& operator=( Array&& ) = default;

    ~Array() = default;

    typedef Vector::const_iterator   const_iterator;
    typedef Vector::iterator         iterator;
    typedef Vector::reverse_iterator reverse_iterator;
    typedef Vector::value_type       value_type;

    [[nodiscard]] const_iterator begin() const { return array->begin(); }
    [[nodiscard]] const_iterator end()   const { return array->end(); }
    iterator begin() { return array->begin(); }
    iterator end()   { return array->end(); }
    reverse_iterator rbegin() { return array->rbegin(); }
    reverse_iterator rend()   { return array->rend(); }

    Proxy operator[]( unsigned n ) const;
    Proxy operator[]( unsigned n );

    [[nodiscard]] size_t size() const { return array->size(); }
    [[nodiscard]] bool empty()  const { return array->empty(); }

    void reserve( size_t n ) { array->reserve( n ); }

    void push_back( const Value& v ) { array->push_back(v); }
    void emplace_back( const Value& v ) { array->emplace_back(v); }

    bool erase( const std::string& name );
    bool erase( size_t index );
    bool eraseNamedObject( const std::string& name );

    iterator erase( iterator it )
    {
      return array->erase( it );
    }

    void clear() { array->clear(); }

    template <typename Pred> bool eraseIf( const Pred& pred )
    {
      const auto n {std::erase_if(*array, pred )};
      return n == 1;
    }

    iterator insert( iterator position, const value_type& v )
    {
      return array->insert( position, v );
    }

    [[nodiscard]] Array clone() const;

    template <typename InputIterator>
    void insert( iterator position, InputIterator first, InputIterator last )
    {
      array->insert( position, first, last );
    }

    template <typename Iterator>
    void append( Iterator begin, Iterator end )
    {
      array->insert( array->end(), begin, end );
    }

    template <typename T>
    Array& operator<<( T v )
    {
      array->push_back(v);
      return *this;
    }

    Array& operator<<( const Array& other )
    {
      array->insert( array->end(), other.begin(), other.end() );
      return *this;
    }

    friend std::ostream& operator<<( std::ostream&, const Array& );

//    template <typename Op> Op forEach( Op& op )
//    {
//      for( const_iterator i = begin(); i != end(); ++i )
//      {
//        if( i->isObject() )
//        {
//          Object(*i).forEach( op );
//        }
//        else if( i->isArray() )
//        {
//          Array(*i).forEach( op );
//        }
//        else
//        {
//          op( *i );
//        }
//      }
//
//      return op;
//    }
  };
}
