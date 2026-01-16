/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#ifndef _cgi_JSONAlgorithm_h_
#define _cgi_JSONAlgorithm_h_

#include "JSON.h"
#include <algorithm>

namespace json
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
      for( Array::const_iterator it = array.begin(); it !=  array.end(); ++it )
      {
        if( it->isObject() )
        {
          ForEachRecursive<Op,false>()( *it, op );
        }
        else if( it->isArray() )
        {
          ForEachRecursiveArray<Op,false>()( *it, op );
        }
      }
    }
  };

  template <typename Op>
  struct ForEachRecursive<Op,false>
  {
    void operator()( const Object& object, Op op ) const
    {
      for( Object::const_iterator it = object.begin(); it !=  object.end(); ++it )
      {
        op( *it );

        if( it->second.isObject() )
        {
          ForEachRecursive<Op,false>()( it->second, op );
        }
        else if( it->second.isArray() )
        {
          ForEachRecursiveArray<Op,false>()( it->second, op );
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
      for( Array::const_iterator it = array.begin(); it !=  array.end(); ++it )
      {
        if( it->isObject() )
        {
          ForEachRecursive<Op,true> fe;

          fe( *it, op );

          found |= fe.found;
        }
        else if( it->isArray() )
        {
          ForEachRecursiveArray<Op,true> fe;

          fe( *it, op );

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
      for( Object::const_iterator it = object.begin(); it !=  object.end(); ++it )
      {
        found |= op( *it );

        if( !found && it->second.isObject() )
        {
          ForEachRecursive<Op,true> fe;

          fe( it->second, op );

          found |= fe.found;
        }
        else if( !found && it->second.isArray() )
        {
          ForEachRecursiveArray<Op,true> fe;

          fe( it->second, op );

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
      ForEachRecursiveArray<Op,Op::bounded>()( object, op );
    }
    return op;
  }

  template <typename Op>
  Object findRecursiveArray( const Array& array, Op op )
  {
    for( Array::const_iterator it = array.begin(); it !=  array.end(); ++it )
    {
      if( op( *it ) )
      {
        return *it;
      }
      if( it->isObject() )
      {
        Object result = findRecursiveObject( *it, op );

        if( !result.empty() )
        {
          return result;
        }
      }
      else if( it->isArray() )
      {
        Object result = findRecursiveArray( *it, op );

        if( !result.empty() )
        {
          return result;
        }
      }
    }

    return Object();
  }

  template <typename Op>
  Object findRecursiveObject( const Object& object, Op op )
  {
    for( Object::const_iterator it = object.begin(); it != object.end(); ++it )
    {
      if( op( *it ) )
      {
        return *it;
      }
      else if( it->second.isObject() )
      {
        Object result = findRecursiveObject( it->second, op );

        if( !result.empty() )
        {
          return result;
        }
      }
      else if( it->second.isArray() )
      {
        Object result = findRecursiveArray( it->second, op );

        if( !result.empty() )
        {
          return result;
        }
      }
    }

    return Object();
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
      return findRecursiveArray( object, op );
    }
    else
    {
      return Object();
    }
  }

  template <typename Other>
  void convertTo( const Object& object, Other& other )
  {
    for( Object::const_iterator i = object.begin(); i != object.end(); ++i )
    {
      if( i->second.is<int64_t>() )
      {
        other.add( i->first, int64_t(i->second) );
      }
      else if( i->second.is<std::string>() )
      {
        other.add( i->first, std::string(i->second) );
      }
      else if( i->second.is<bool>() )
      {
        other.add( i->first, bool(i->second) );
      }
      else if( i->second.is<json::Object>() )
      {
        other.add( i->first, json::Object(i->second) );
      }
    }
  }

  struct ForEachOperator { enum { bounded = false }; };

  struct GetObjectsByName : ForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetObjectsByName( const std::string& name ) : name(name) {}

    void operator()( const Pair& object ) const
    {
      if( object.first == name )
      {
        matches << object.second;
      }
    }
  };

  struct BoundedForEachOperator { enum { bounded = true }; };

  struct GetParentObjectsByName : BoundedForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetParentObjectsByName( const std::string& name ) : name(name) {}

    bool operator()( const Pair& object ) const
    {
      if( object.first == name )
      {
        matches << object.second;
        return true;
      }
      return false;
    }
  };

  struct GetObjectsContainingName : BoundedForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetObjectsContainingName( const std::string& name ) : name(name) {}

    bool operator()( const Pair& object ) const
    {
      if( object.second.isObject() )
      {
        const Object obj(object.second);

        if( obj.has(name))
        {
          matches << object.second;
          return true;
        }
      }
      return false;
    }
  };

  struct GetAllObjectsContainingName : ForEachOperator
  {
    const std::string name;

    mutable Array matches;

    GetAllObjectsContainingName( const std::string& name ) : name(name) {}

    void operator()( const Pair& object ) const
    {
      if( object.second.isObject() )
      {
        const Object obj(object.second);

        if( obj.has(name))
        {
          matches << object.second;
        }
      }
    }
  };

  struct FindObjectByName
  {
    const std::string name;

    FindObjectByName( const std::string& name ) : name(name) {}

    bool operator()( const Value& value ) const
    {
      if( value.isObject() )
      {
        const Object object(value);

        return object.size() == 1 && object.has(name);
      }
      return false;
    }

    bool operator()( const Pair& object ) const
    {
      return object.first == name;
    }
  };

  template <typename T>
  struct GetObjectByItem : ForEachOperator
  {
    const std::string name;
    const T           value;

    mutable Array matches;

    GetObjectByItem( const std::string& name, const T value )
      : name(name), value(value) {}

    void operator()( const Pair& object ) const
    {
      if( object.second.isObject() )
      {
        const Object& obj(object.second);

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
    return GetObjectByItem<T>( name, value );
  }

  inline GetObjectByItem<std::string>
  makeGetObjectByItem( const std::string& name, const char* value )
  {
    return GetObjectByItem<std::string>( name, value );
  }


  template <typename T>
  struct FindObjectByItem
  {
    const std::string name;
    const T           value;

    FindObjectByItem( const std::string& name, const T value )
      : name(name), value(value) {}

    bool operator()( const Object& object ) const
    {
      return (object.has(name) && object[name] == value);
    }
  };

  template <typename T> FindObjectByItem<T>
  makeFindObjectByItem( const std::string& name, const T value )
  {
    return FindObjectByItem<T>( name, value );
  }

  struct FindStringObjectWithValueContainig
  {
    const std::string name;
    const std::string match;

    bool check( const Object object ) const
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

    FindStringObjectWithValueContainig( const std::string& name,
                                        const std::string& match )
      : name(name), match(match) {}

    bool operator()( const Pair& pair ) const
    {
      return check(pair);
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

#endif
