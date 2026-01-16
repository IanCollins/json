/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2014.  All rights reserved.
 *
 *  PROJECT:     CGI script engine - JSON parser
 *
 *  MODULE:      cgi
 *
 ******************************************************************************/

#pragma once

#include <vector>
#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <type_traits>

#include <utils/Typelist.h>

namespace masuma::json
{
  class Object;
  class Array;
  struct ValueProxy;

  struct Null {
    template <typename T> bool operator==( T )const {return true;}
  };

  constexpr Null null {};

  class Value
  {
    using Types = std::tuple<int64_t,bool,double,Null,std::string,Object,Array>;

    static constexpr size_t numberOfTypes {std::tuple_size<Types>::value};

    static const char* const typeNames[numberOfTypes];

    struct BaseHolder
    {
      virtual ~BaseHolder() = default;
      virtual void stream( std::ostream& ) const = 0;
      [[nodiscard]] virtual bool typeIs(int) const = 0;
      virtual bool sameTypeAs( const BaseHolder* ) const = 0;
      [[nodiscard]] virtual std::string type() const = 0;
    };

    template <typename ValueType>
    struct Holder : BaseHolder
    {
      static constexpr auto valueType = masuma::system::IndexOfIn<ValueType,Types>::value;

      ValueType value;

      explicit Holder( const ValueType& v ) : value(v) {}
      explicit Holder( ValueType&& v ) : value(std::move(v)) {}

      void stream( std::ostream& out ) const override { out << value; }

      [[nodiscard]] bool typeIs( int type ) const override { return type == valueType; }

      bool sameTypeAs( const BaseHolder* other ) const override
      {
        return other->typeIs(valueType);
      }

      [[nodiscard]] std::string type() const override { return typeNames[valueType]; }
    };

    typedef std::shared_ptr<BaseHolder> ItemType;

    ItemType item;

    [[nodiscard]] bool sameTypeAs( const Value& other) const
    {
      return item->sameTypeAs(other.item.get());
    }

  protected:

    void clearItem() { item = ItemType(); }

  public:

    Value() = default;
    Value( const Value& v ) = default;
    Value( Value&& v )      = default;

    virtual Value& operator=( const Value& ) noexcept(false) = default;
    virtual Value& operator=( Value&& )      noexcept(false) = default;

    virtual ~Value() = default;

    // NOLINTBEGIN(google-explicit-constructor)
    //
    Value( const std::string& v ) : item( new Holder<std::string>(v) ) {}
    Value( std::string&& v )      : item( new Holder<std::string>(v) ) {}
    Value( const char* v )        : item( new Holder<std::string>(v) ) {}
    Value( long long v )          : item( new Holder<int64_t>(v) ) {}
    Value( unsigned long long v ) : item( new Holder<int64_t>(v) ) {}
    Value( long v )               : item( new Holder<int64_t>(v) ) {}
    Value( unsigned long v )      : item( new Holder<int64_t>(v) ) {}
    Value( int v )                : item( new Holder<int64_t>(v) ) {}
    Value( unsigned int v )       : item( new Holder<int64_t>(v) ) {}
    Value( short v )              : item( new Holder<int64_t>(v) ) {}
    Value( unsigned short v )     : item( new Holder<int64_t>(v) ) {}
    Value( char v )               : item( new Holder<int64_t>(v) ) {}
    Value( unsigned char v )      : item( new Holder<int64_t>(v) ) {}
    Value( bool v )               : item( new Holder<bool>(v) ) {}
    Value( Null v )               : item( new Holder<Null>(v) ) {}
    Value( std::nullptr_t )       : item( new Holder<Null>(Null{}) ) {}
    Value( float v )              : item( new Holder<double>(v) ) {}
    Value( double v )             : item( new Holder<double>(v) ) {}

    Value( std::initializer_list<Value> );
    Value( const Array& v );
    Value( const Object& v );

    [[nodiscard]] bool isValid() const { return item.get(); }
    [[nodiscard]] bool isNull() const  { return !item.get(); }

    template <typename T> void setItem()
    {
      item = ItemType( new Holder<T>( T() ) );
    }

    template <typename T> T setItemIfNull( )
    {
      if( isNull() ) setItem<T>();
      return *this;
    }

    template <typename T> [[nodiscard]] const T& as() const
    {
      static constexpr auto valueType = masuma::system::IndexOfIn<T,Types>::value;

      if( isNull() )
        throw std::runtime_error("[Value::as()] Can't covert item from null object to "+std::string (typeNames[valueType]));

      if( !item->typeIs(valueType) )
      {
        throw std::runtime_error("[Value::as()] Can't covert item of type "+item->type()+" to "+typeNames[valueType]);
      }

      return static_cast<Holder<T>*>(item.get())->value;
    }

    template <typename T> T& as()
    {
      return const_cast<T&>(const_cast<const Value*>(this)->as<T>());
    }

    [[nodiscard]] int64_t asInteger() const { return as<int64_t>(); }
    [[nodiscard]] double  asNumber()  const { return as<double>(); }

    operator const std::string&() const { return as<std::string>(); }
    operator long long() const { return as<int64_t>(); }
    operator unsigned long long() const { return as<int64_t>(); }
    operator long() const { return as<int64_t>(); }
    operator unsigned long() const { return as<int64_t>(); }
    operator int() const { return as<int64_t>(); }
    operator unsigned int() const { return as<int64_t>(); }
    operator short() const { return as<int64_t>(); }
    operator unsigned short() const { return as<int64_t>(); }
    operator unsigned char() const { return as<int64_t>(); }
    operator bool() const { return as<bool>(); }
    operator double() const { return as<double>(); }

    operator Object() const;
    operator Array() const;

    // NOLINTEND(google-explicit-constructor)

    template <typename T> [[nodiscard]] bool is() const
    {
      return (isValid() && item->typeIs( masuma::system::IndexOfIn<T,Types>::value) );
    }

    [[nodiscard]] bool isObject() const { return is<Object>(); }
    [[nodiscard]] bool isArray()  const { return is<Array>(); }

    static Value scan( std::istream& );

    bool operator==( const char* s ) const
    {
      return as<std::string>() == s;
    }

    bool operator==( bool b ) const
    {
      return as<bool>() == b;
    }

    bool operator==( const Value& ) const;
    bool operator!=( const Value& v) const {return !operator==(v);}
    bool operator==( const ValueProxy& ) const;
    bool operator!=( const ValueProxy& v) const {return !operator==(v);}

    template <typename ValueType,
              std::enable_if<!std::is_same<ValueType,Value>::value
                          && !std::is_same<ValueType,ValueProxy>::value> >
    bool operator==( const ValueType& v ) const
    {
      return as<ValueType>() == v;
    }

    template <typename ValueType,
              std::enable_if<!std::is_same<ValueType,Value>::value
                          && !std::is_same<ValueType,ValueProxy>::value> >
    bool operator!=( const ValueType& v ) const
    {
      return !operator==(v);
    }

    bool operator<( const Value& ) const;
    bool operator>( const Value& ) const;

    bool operator<=( const Value& v ) const
    {
      return (*this == v) || *this < v;
    }

    bool operator>=( const Value& v ) const
    {
      return (*this == v) || *this > v;
    }

    friend std::ostream& operator<<( std::ostream& out, const Value& val )
    {
      val.item->stream( out );
      return out;
    }

    friend bool operator==( const std::string& p, const Value& v )
    {
      return v.as<std::string>() == p;
    }

    friend bool operator==( const char* p, const Value& v )
    {
      return v.as<std::string>() == p;
    }

    friend bool operator==( int i, const Value& v )
    {
      return v.as<int64_t>() == i;
    }

    friend bool operator==( unsigned i, const Value& v )
    {
      return v.as<int64_t>() == i;
    }

    friend bool operator==( double i, const Value& v )
    {
      return v.as<double>() == i;
    }

    friend bool operator==( bool b, const Value& v )
    {
      return v.as<bool>() == b;
    }

    template <typename T> T convertedTo() const
    {
      std::stringstream ss;

      ss << *this;

      T t;

      ss >> t;

      return t;
    }
  };

  class NameValuePair
  {
     std::string theName;

  public:

    Value value;

    NameValuePair() = delete;

//      NameValuePair( std::string&& n, const Value& v )
//        : theName(std::move(n)), value(v) {}

    NameValuePair( std::string&& n, Value&& v )
      : theName(std::move(n)), value(std::move(v)) {}

    NameValuePair( std::string&& n, std::initializer_list<Value> v )
      : theName(std::move(n)), value(v) {}

    NameValuePair( const NameValuePair& ) = default;
    NameValuePair( NameValuePair&& ) = default;

    NameValuePair& operator=( const NameValuePair& ) = default;
    NameValuePair& operator=( NameValuePair&& ) = default;

    [[nodiscard]] std::string name() const { return theName; }

    template <typename T> [[nodiscard]] bool is() const { return value.is<T>(); }

    [[nodiscard]] bool isObject() const { return is<Object>(); }
    [[nodiscard]] bool isArray()  const { return is<Array>(); }
  };

  template <> inline const int&
  Value::as<int>() const
  {
    static int n;
    n = as<int64_t>();
    return n;
  }

  template <> inline void
  Value::Holder<bool>::stream( std::ostream& out ) const
  {
    out << (value ? "true" : "false");
  }

  template <> inline void
  Value::Holder<double>::stream( std::ostream& out ) const
  {
    out << std::fixed << value;
  }

  template <> inline void
  Value::Holder<Null>::stream( std::ostream& out ) const
  {
    out << "null";
  }

  template <> inline void
  Value::Holder<std::string>::stream( std::ostream& out ) const
  {
    static const std::string specialCharacters("\\\"");

    const std::string tmp(value);
    std::string result;

    for( auto c : tmp )
    {
      if( specialCharacters.find(c) != std::string::npos )
      {
        result += '\\';
      }
      result += c;
    }

    out << result;
  }
}
