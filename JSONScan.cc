/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON syntax scanner
 *
 *  MODULE:      cgi
 *
 ******************************************************************************/

#include "JSONScan.h"

namespace masuma::json
{
  bool
  State::isStructural( char c )
  {
    static const std::string structuralCharacters("[]{}:,");

    return structuralCharacters.find(c) != std::string::npos;
  }

  bool
  State::isSpecial( char c )
  {
    static const std::string speciallCharacters("\\\"");

    return (isStructural( c ) ||
              speciallCharacters.find(c) != std::string::npos);
  }

  Value
  State::processSpecial( char c, std::istream& in )
  {
    switch( c )
    {
      case '"':
        return InString().scan( in );

      case '[':
        return InArray().scan( in );

      case '{':
        return InObject().scan( in );

      default:
        throw std::runtime_error( std::string("Unexpected ")+c );
    }
  }

  Value
  InString::scan( std::istream& in )
  {
    std::string out;

    char c = in.get();

    while( in && c != '"' )
    {
      if( c == '\\' )
      {
        c = in.get();
      }

      out += c;
      c = in.get();
    }

    if( c == '"' )
    {
      return out;
    }
    else
      throw std::runtime_error( "Bad string" );
  }

  Value
  InName::scan( std::istream& in )
  {
    char c;

    in >> c;

    if( c == '}' )
    {
      return Object();
    }

    if( c != '"' )
      throw std::runtime_error( "Member name not a string" );

    return InString().scan( in );
  }

  Value
  InValue::scan( std::istream& in )
  {
    in >> c;

    std::string token;

    while( in && c != end && c != ',' )
    {
      if( isSpecial( c ) )
      {
        Value result = processSpecial( c, in );
        in >> c;
        return result;
      }
      else
      {
        token += c;
        in >> c;
      }
    }

    if( token.empty() )
      throw std::runtime_error( "Empty value" );

    return Object::getValue( token );
  }

  Value
  InArray::scan( std::istream& in )
  {
    Array  array;

    if( in.peek() != ']' ) // Arrays can be empty
    {
      InValue inValue(']');
      do
      {
        array << inValue.scan( in );

      } while( !inValue.atEnd() );
    }
    else
    {
      in.get();
    }
    return array;
  }

  Value
  InObject::scan( std::istream& in )
  {
    Object  object;
    InValue inValue('}');

    do
    {
      Value value = InName().scan( in );

      if( value.isObject() )
      {
        return value; // Empty object.
      }

      const std::string name = value;

      //std::cerr << "Parsing " << name << std::endl;
      char c;

      in >> c;

      if( c != ':' )
        throw std::runtime_error( name+": Member delimiter missing - "+c );

      try
      {
        object.add( name, inValue.scan( in ) );
      }
      catch( const std::runtime_error& e )
      {
        std::cerr << name << std::endl;
        throw e;
      }

    } while( !inValue.atEnd() );

    return object;
  }

  Value
  Nowhere::scan( std::istream& in )
  {
    char c = in.get();

    while( in && c != '{' && c != '[' )
    {
      c = in.get();
    }

    if( in )
    {
      if( c == '{' )
      {
        return InObject().scan( in );
      }
      else
      {
        return InArray().scan( in );
      }
    }
    else
      throw std::runtime_error( "No input!" );
  }

  Object
  Object::scan( std::istream& in )
  {
    return Nowhere().scan( in );
  }

  Value
  Value::scan( std::istream& in )
  {
    return Nowhere().scan( in );
  }
}
