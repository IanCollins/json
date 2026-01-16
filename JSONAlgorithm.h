/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#pragma once

#include "JSON.h"
#include <algorithm>
#include <utility>

namespace masuma::json
{
  bool operator==( const Object&, const Object& );

  inline bool operator!=( const Object& lhs, const Object& rhs )
  {
    return !(lhs == rhs);
  }

  bool operator==( const Array&, const Array& );

  inline bool operator!=( const Array& lhs, const Array& rhs )
  {
    return !(lhs == rhs);
  }

  Object normalise( Object, bool sortArrays = false );

  bool haveSameItems( const Object&, const Object& );

  Array arrayIntersection( const Array&, const Array& );
  Array arrayDifference( const Array&, const Array& );

  Object setDifference( const Object&, const Object& );

  template <typename Op>
  Op forEach( const Object& object, Op op )
  {
    return std::for_each( object.begin(), object.end(), op );
  }

  template <typename Op, int I> struct ForEachRecursive;
  template <typename Op, int I> struct ForEachRecursiveArray;

  template <typename Op>
  struct ForEachRecursiveArray<Op,false>
  {
    void operator()( const Array& array, Op op ) const
    {
      for( auto value : array )
      {
        if( value.isObject() )
        {
          ForEachRecursive<Op,false>()( value, op );
        }
        else if( value.isArray() )
        {
          ForEachRecursiveArray<Op,false>()( value, op );
        }
      }
    }
  };

  template <typename Op>
  struct ForEachRecursive<Op,false>
  {
    void operator()( const Object& object, Op op ) const
    {
      for( auto pair : object )
      {
        op( pair );

        if( pair.isObject() )
        {
          ForEachRecursive<Op,false>()( pair.value, op );
        }
        else if( pair.isArray() )
        {
          ForEachRecursiveArray<Op,false>()( pair.value, op );
        }
      }
    }
  };

  template <typename Op>
  struct ForEachRecursiveArray<Op,true>
  {
    mutable bool found;

    ForEachRecursiveArray() : found() {}

    void operator()( const Array& array, Op op ) const
    {
      for( auto value : array )
      {
        if( value.isObject() )
        {
          ForEachRecursive<Op,true> fe;

          fe( value, op );

          found |= fe.found;
        }
        else if( value.isArray() )
        {
          ForEachRecursiveArray<Op,true> fe;

          fe( value, op );

          found |= fe.found;
        }
      }
    }
  };


  template <typename Op>
  struct ForEachRecursive<Op,true>
  {
    mutable bool found;

    ForEachRecursive() : found() {}

    void operator()( const Object& object, Op op ) const
    {
      if( (found = op( object )) )
      {
        return;
      }
      for( auto pair : object )
      {
        found = op( {pair} );

        if( !found && pair.isObject() )
        {
          ForEachRecursive<Op,true> fe;

          fe( pair.value, op );

          found |= fe.found;
        }
        else if( !found && pair.isArray() )
        {
          ForEachRecursiveArray<Op,true> fe;

          fe( pair.value, op );

          found |= fe.found;
        }
      }
    }
  };

  template <typename Op>
  Op forEachRecursive( const Value& object, Op op )
  {
    if( object.isObject() )
    {
      ForEachRecursive<Op,Op::bounded>()( object, op );
    }
    else if( object.isArray() )
    {
      ForEachRecursiveArray<Op,Op::bounded>()( Array {object}, op );
    }
    return op;
  }

  template <typename Op>
  Object findRecursiveArray( const Array& array, Op op )
  {
    for( auto value : array )
    {
      if( op( value ) )
      {
        return value;
      }
      if( value.isObject() )
      {
        Object result = findRecursiveObject( value, op );

        if( !result.empty() )
        {
          return result;
        }
      }
      else if( value.isArray() )
      {
        Object result = findRecursiveArray( value, op );

        if( !result.empty() )
        {
          return result;
        }
      }
    }

    return {};
  }

  template <typename Op>
  Object findRecursiveObject( const Object& object, Op op )
  {
    for( auto pair : object )
    {
      if( op( pair ) )
      {
        return {pair};
      }
      else if( pair.isObject() )
      {
        Object result = findRecursiveObject( pair.value, op );

        if( !result.empty() )
        {
          return result;
        }
      }
      else if( pair.isArray() )
      {
        Object result = findRecursiveArray( pair.value, op );

        if( !result.empty() )
        {
          return result;
        }
      }
    }

    return {};
  }

  template <typename Op>
  Object findRecursive( const Value& object, Op op )
  {
    if( object.isObject() )
    {
      return findRecursiveObject( object, op );
    }
    else if( object.isArray() )
    {
      return findRecursiveArray( Array {object}, op );
    }
    else
    {
      return {};
    }
  }

  template <typename Other>
  void convertTo( const Object& object, Other& other )
  {
    for( const auto& pair : object )
    {
      if( pair.is<int64_t>() )
      {
        other.add( pair.name(), int64_t(pair.value) );
      }
      else if( pair.is<std::string>() )
      {
        other.add( pair.name(), std::string(pair.value) );
      }
      else if( pair.is<bool>() )
      {
        other.add( pair.name(), bool(pair.value) );
      }
      else if( pair.is<json::Object>() )
      {
        other.add( pair.name(), json::Object(pair.value) );
      }
    }
  }

  struct ForEachOperator { enum { bounded = false }; };

  struct GetObjectsByName : ForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetObjectsByName( std::string name ) : name(std::move(name)) {}

    void operator()( const NameValuePair& object ) const
    {
      if( object.name() == name )
      {
        matches << object.value;
      }
    }
  };

  struct BoundedForEachOperator { enum { bounded = true }; };

  struct GetParentObjectsByName : BoundedForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetParentObjectsByName( std::string name ) : name(std::move(name)) {}

    bool operator()( const Object& object ) const
    {
      if( object.has( name ) )
      {
        matches << object[name];
        return true;
      }
      return false;
    }
  };

  struct GetObjectsContainingName : BoundedForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetObjectsContainingName( std::string name ) : name(std::move(name)) {}

    bool operator()( const NameValuePair& object ) const
    {
      if( object.isObject() )
      {
        const Object obj = object.value;

        if( obj.has(name))
        {
          matches << object.value;
          return true;
        }
      }
      return false;
    }

    bool operator()( const Object& object ) const
    {
      if( object.has(name))
      {
        matches << object;
        return true;
      }
      return false;
    }
  };

  struct GetAllObjectsContainingName : ForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetAllObjectsContainingName( std::string name ) : name(std::move(name)) {}

    void operator()( const NameValuePair& object ) const
    {
      if( object.isObject() )
      {
        const Object obj = object.value;

        if( obj.has(name))
        {
          matches << object.value;
        }
      }
    }
  };

  struct FindObjectByName
  {
    const std::string name;

    FindObjectByName( std::string name ) : name(std::move(name)) {}

    bool operator()( const Value& value ) const
    {
      if( value.isObject() )
      {
        const Object object = value;

        return object.size() == 1 && object.has(name);
      }
      return false;
    }

    bool operator()( const NameValuePair& object ) const
    {
      return object.name() == name;
    }
  };

  template <typename T>
  struct GetObjectByItem : ForEachOperator
  {
    const std::string name;
    const T           value;

    mutable Array matches;

    GetObjectByItem( std::string name, const T value )
      : name(std::move(name)), value(value) {}

    void operator()( const NameValuePair& object ) const
    {
      if( object.isObject() )
      {
        const Object& obj(object.value);

        if( obj.has(name) && obj[name] == value )
        {
          matches << obj;
        }
      }
    }
  };

  template <typename T> GetObjectByItem<T>
  makeGetObjectByItem( const std::string& name, const T value )
  {
    return {name, value};
  }

  inline GetObjectByItem<std::string>
  makeGetObjectByItem( const std::string& name, const char* value )
  {
    return {name, value};
  }


  template <typename T>
  struct FindObjectByItem
  {
    const std::string name;
    const T           value;

    FindObjectByItem( std::string  name, const T value )
      : name(std::move(name)), value(value) {}

    bool operator()( const Object& object ) const
    {
      return (object.has(name) && object[name] == value);
    }
  };

  template <typename T> FindObjectByItem<T>
  makeFindObjectByItem( const std::string& name, const T value )
  {
    return {name, value};
  }

  struct FindStringObjectWithValueContainig
  {
    const std::string name;
    const std::string match;

    [[nodiscard]] bool check( Object object ) const
    {
      if( object.has(name) )
      {
        if( object[name].is<std::string>() )
        {
          const std::string& objectValue = object[name];

          return (objectValue.find(match) != std::string::npos);
        }
      }
      return false;
    }

    FindStringObjectWithValueContainig( std::string name, std::string match )
      : name(std::move(name)), match(std::move(match)) {}

    bool operator()( const NameValuePair& pair ) const
    {
      return check({pair});
    }

    bool operator()( const Value& value ) const
    {
      if( value.isObject() )
      {
        return check(value);
      }
      return false;
    }
  };
}
