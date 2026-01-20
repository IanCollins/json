/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2013.  All rights reserved.
 *
 *  PROJECT:     JSON parser
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#pragma once

#if !defined HAVE_JSON
# define HAVE_JSON
#endif

#include <stdexcept>
#include <iterator>
#include <sstream>
#include <initializer_list>

#include "JSONValue.h"
#include "JSONArray.h"
#include "ValueProxy.h"

namespace masuma::json
{
  class Object
  {
    friend struct ValueProxy;

    using Base     = std::vector<NameValuePair>;
    using DataType = std::shared_ptr<Base>;

    DataType data;

    // One type that can add to these.
    //
    friend struct InObject;

    template <typename T>
    void assemble( std::string name, T value )
    {
      data->emplace_back(std::move(name),value);
    }

    template <typename T, typename... Args>
    void assemble( std::string name, T value, Args&&... args )
    {
      data->emplace_back(std::move(name),value);

      assemble(args...);
    }

  protected:

    void add( std::string name, Value value )
    {
      data->emplace_back( std::move(name), std::move(value) );
    }

    void add( std::string&& name, Value&& value )
    {
      data->emplace_back( std::move(name), std::move(value) );
    }

 public:

    using iterator       = Base::iterator;
    using const_iterator = Base::const_iterator;
    using value_type     = Base::value_type;

    [[nodiscard]] auto size() const { return data->size(); }
    [[nodiscard]] bool empty() const { return data->empty(); }

    Object() : data {std::make_shared<Base>()} {}

    template <typename T>
    Object( std::string name, T value ) : Object()
    {
      assemble(name, value);
    }

    template <typename T, typename... Args>
    Object( std::string name, T value, Args&&... args ) : Object()
    {
      assemble(name, value, args...);
    }

    // Be very careful here, brace initialisation will use the next constructor,
    // not this one!
    //
    explicit Object( const value_type& v )
      : data {std::make_shared<Base>(1,v)} {}

    Object( std::initializer_list<value_type> args)
      : data {std::make_shared<Base>(args)} {}

    explicit Object( std::istream& );

    Object( const Object& ) = default;
    Object( Object&& ) = default;

    explicit Object( const Array::Proxy& item )
      : Object {Value(item)} {}

    Object& operator=( const Object& ) = default;
    Object& operator=( Object&& ) = default;

    ~Object() = default;

    [[nodiscard]] bool has( const std::string& ) const;

    ValueProxy operator[]( const std::string& ) const;
    ValueProxy operator[]( const std::string& );

    ValueProxy operator[]( const char* ) const;
    ValueProxy operator[]( const char* );

    static Object scan( std::istream& );

    static Object scan( const std::string& s )
    {
      std::istringstream in(s);
      return scan( in );
    }

    static Value getValue( const std::string& );

    [[nodiscard]] size_t objects() const { return data->size(); }

    iterator erase( iterator it ) { return data->erase(it); }

    bool erase( const std::string& );

    [[nodiscard]] Object clone() const;

    [[nodiscard]] std::string asString() const;

    friend std::ostream& operator<<( std::ostream&, const Object& );

    [[nodiscard]] iterator begin() const { return data->begin(); }
    [[nodiscard]] iterator end() const { return data->end(); }
  };

  struct MatchName
  {
    const std::string& name;

    explicit MatchName( const std::string& name ) : name(name) {}

    bool operator()( const NameValuePair& p );
  };

  template <> inline void
  Value::Holder<Object>::stream( std::ostream& out ) const
  {
    out << value;
  }
}
