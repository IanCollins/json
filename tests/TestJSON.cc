/******************************* C++ Source File *******************************
 *
 *  PROJECT:     CGI bits
 *
 *  MODULE:      cgi
 *
 ******************************************************************************/

#include "JSON.h"

#include <gtest/gtest.h>

#include <sstream>

namespace
{
  const char* const data = "{\"object\":\"Upload\",\"method\":\"poll\","
    "\"params\":[\"1234\"],\"id\":0}";

  class TestJSON: public testing::Test
  {
    protected:

      TestJSON() = default;
  };
}

using masuma::json::Object;
using masuma::json::Array;
using masuma::json::Value;

TEST_F(TestJSON, testBooleanNvPairCorrectlyConstructed)
{
  Object::value_type val {"bool", true};

  ASSERT_TRUE( val.is<bool>() );
}


TEST_F(TestJSON, testConstructionFromSingleItemInitialiserList)
{
  Object object { {"object", "Upload"} };

  ASSERT_EQ("Upload", object["object"] );
}


TEST_F(TestJSON, testConstructionFromManyItemInitialiserList)
{
  Object object { {"object", "Upload"}, {"first", 21}, {"second", true} };

  ASSERT_EQ( "Upload", object["object"] );
  ASSERT_EQ( 21,       object["first"] );
  ASSERT_EQ( true,     object["second"] );
}


TEST_F(TestJSON, testConstructionFromArrayItemInitialiserList)
{
  Object object {{"array", {1,2,"three"} }};

  ASSERT_EQ( 1,       object["array"][0] );
  ASSERT_EQ( 2,       object["array"][1] );
  ASSERT_EQ( "three", object["array"][2] );
}


TEST_F(TestJSON, testConstructionFromNameValue)
{
  Object object {"object", "Upload"};

  ASSERT_TRUE( object.has("object"));
  ASSERT_EQ("Upload", object["object"] );
}


TEST_F(TestJSON, testConstructionFromNameValueList)
{
  Object object {"object", "Upload", "number", 10};

  ASSERT_TRUE( object.has("object"));
  ASSERT_EQ("Upload", object["object"] );
  
  ASSERT_TRUE( object.has("number"));
  ASSERT_EQ(10, object["number"] );
}


TEST_F(TestJSON, testSingleObjectParsed)
{
  std::istringstream in(R"({"object":"Upload"})" );

  Object object = Object::scan( in );

  ASSERT_EQ("Upload", object["object"] );
}


TEST_F(TestJSON, testPairOfObjectsParsed)
{
  std::istringstream in(R"({"object":"Upload","method":"poll"})" );

  Object object = Object::scan( in );

  ASSERT_EQ( "Upload", object["object"] );
  ASSERT_EQ( "poll",   object["method"] );
}


TEST_F(TestJSON, testSingleElementArrayObjectParsed)
{
  std::istringstream in(R"({"params":["one"]})");

  Object object = Object::scan( in );

  ASSERT_EQ( "one", object["params"][0] );
}


TEST_F(TestJSON, testSingleObjectArrayParsed)
{
  std::istringstream in(R"([{"object":"Upload","method":"poll"}])");

  auto result = Value::scan( in );

  ASSERT_TRUE( result.isArray() );
  
  const Array array {result};
  
  ASSERT_EQ( 1, array.size() );
}


TEST_F(TestJSON, testSingleObjectArrayObjectParsed)
{
  std::istringstream in(R"([{"object":"Upload","method":"poll"}])");
  
  const Array array {Value::scan( in ).as<Array>()};
        
  ASSERT_EQ( 1, array.size() );
  
  ASSERT_TRUE( array[0].isObject() );
  
  const Object object {array[0]};
  
  ASSERT_EQ( "Upload", object["object"] );
  ASSERT_EQ( "poll",   object["method"] );      
}


TEST_F(TestJSON, testMultiElementArrayObjectParsed)
{
  std::istringstream in(R"({"params":["one","two","three"]})");

  Object object = Object::scan( in );

  ASSERT_EQ( "one",   object["params"][0] );
  ASSERT_EQ( "two",   object["params"][1] );
  ASSERT_EQ( "three", object["params"][2] );
}


TEST_F(TestJSON, testMixedElementArrayObjectParsed)
{
  std::istringstream in(R"({"params":["one",2,"three"]})");

  Object object = Object::scan( in );

  ASSERT_EQ( "one",   object["params"][0] );
  ASSERT_EQ( 2,       object["params"][1] );
  ASSERT_EQ( "three", object["params"][2] );
}


TEST_F(TestJSON, testSingleParameterFunctionObjectParsed)
{
  std::istringstream in(data);

  Object object = Object::scan( in );

  ASSERT_EQ( "Upload", object["object"] );
  ASSERT_EQ( "poll",   object["method"] );
  ASSERT_EQ( "1234",   object["params"][0] );
  ASSERT_EQ( 0,        object["id"] );
}


TEST_F(TestJSON, testObjectInObjectParsed)
{
  Object object = Object::scan( R"({"result":{"entries":2},"id":0})" );

  ASSERT_EQ( 0, object["id"] );
  ASSERT_EQ( 2, object["result"]["entries"] );
}


TEST_F(TestJSON, testBoolValueParsed)
{
  Object object = Object::scan( R"({"result":true,"other":false})" );

  ASSERT_EQ( true,  bool(object["result"]) );
  ASSERT_EQ( false, bool(object["other"]) );
}


TEST_F(TestJSON, testNullValueParsed)
{
  std::istringstream in("{\"result\":null}");

  Object object = Object::scan( in );

  const Value expected(nullptr);

  Value result = object["result"];

  ASSERT_EQ( expected, result );
}


TEST_F(TestJSON, testStringWithEscapedQuotesParsed)
{
  std::istringstream in(R"({"result":"this is \"quoted\""})");

  Object object = Object::scan( in );

  ASSERT_EQ( "this is \"quoted\"", object["result"] );
}


TEST_F(TestJSON, testStringWithCommaParsed)
{
  std::istringstream in(R"({"result":"this, that"})");

  Object object = Object::scan( in );

  ASSERT_EQ( "this, that", object["result"] );
}


TEST_F(TestJSON, testAssignmentToWrongTypeChucks)
{
  std::istringstream in("{\"value\":1}" );

  Object object = Object::scan( in );

  EXPECT_THROW( {std::string s = object["value"];}, std::runtime_error );
}


TEST_F(TestJSON, testAssignmentFromNullObjectChucks)
{
  std::istringstream in("{\"value\":1}" );

  Object object = Object::scan( in );

  EXPECT_THROW( {[[maybe_unused]] int n = object["values"];}, std::runtime_error );
}

void
checkObjectStream( const std::string& source )
{
  std::istringstream in( source );

  Object object = Object::scan( in );

  std::ostringstream out;

  out << object;

  ASSERT_EQ( source, out.str() );
}


TEST_F(TestJSON, testSingleObjectStreamed)
{
  checkObjectStream(R"({"object":"Upload"})");
}


TEST_F(TestJSON, testPairOfObjectsStreamed)
{
  checkObjectStream(R"({"object":"Upload","method":"poll"})");
}


TEST_F(TestJSON, testSingleElementArrayObjectStreamed)
{
  checkObjectStream(R"({"params":["one"]})");
}


TEST_F(TestJSON, testMultiElementArrayObjectStreamed)
{
  checkObjectStream(R"({"params":["one","two","three"]})");
}


TEST_F(TestJSON, testMixedElementArrayObjectStreamed)
{
  checkObjectStream(R"({"params":["one",2,"three"]})");
}


TEST_F(TestJSON, testSingleParameterFunctionObjectStreamed)
{
  checkObjectStream(data);
}


TEST_F(TestJSON, testObjectInObjectStreams)
{
  checkObjectStream(R"({"result":{"entries":2},"id":0})");
}


TEST_F(TestJSON, testBoolValueStreams)
{
  checkObjectStream(R"({"result":true,"other":false})");
}


TEST_F(TestJSON, testNullValueStreams)
{
  checkObjectStream("{\"result\":null}");
}


TEST_F(TestJSON, testStringValueStreams)
{
  Object object = Object::scan(R"({"string":"value,1"})");

  std::ostringstream out;

  out << object["string"];

  ASSERT_EQ( "value,1", out.str() );
}


TEST_F(TestJSON, testStringValueWithQuotesStreamsWithEscape)
{
  Object object = Object::scan(R"({"string":"a \"value\""})");

  std::ostringstream out;

  out << object["string"];

  ASSERT_EQ( "a \\\"value\\\"", out.str() );
}

// Construction tests.
//


TEST_F(TestJSON, testHandBuiltObjectStreams)
{
  Object object;

  object["string"] = "a string";
  object["int"]    = 1;

  ASSERT_EQ( "a string", object["string"] );

  std::ostringstream out;

  out << object;

  ASSERT_EQ( "{\"string\":\"a string\",\"int\":1}", out.str() );
}


TEST_F(TestJSON, testAddingToNewObjectAddsReferencedOject)
{
  Object object;

  object["attributes"]["name"] = "child";

  ASSERT_EQ( "child", object["attributes"]["name"] );

  object["attributes"]["sub"]["name"] = "child";

  ASSERT_EQ( "child", object["attributes"]["sub"]["name"] );
}

TEST_F(TestJSON, testArrayEraseByIndexDoesWahtItSaysOnTheTin)
{
  Array a;

  a << "1" << "2";

  ASSERT_EQ(2, a.size());

  a.erase(0);

  ASSERT_EQ(1, a.size());
  ASSERT_EQ("2", a[0]);
}

TEST_F(TestJSON, testArrayEraseByNameDoesWahtItSaysOnTheTin)
{
  Array a;

  a << "1" << "2";

  ASSERT_EQ(2, a.size());

  a.erase("1");

  ASSERT_EQ(1, a.size());
  ASSERT_EQ("2", a[0]);
}

TEST_F(TestJSON, testArrayEraseNamedObjectDoesWahtItSaysOnTheTin)
{
  Array a;

  a << Object {"name", "one"} << Object {"name", "two"};

  ASSERT_EQ(2, a.size());

  a.eraseNamedObject("one");

  ASSERT_EQ(1, a.size());
  ASSERT_EQ("two", a[0]["name"]);
}

TEST_F(TestJSON, testAddingToNewArrayAddsReferencedArray)
{
  Object object;

  object["attributes"] << "child";

  ASSERT_EQ( "child", object["attributes"][0] );
}

TEST_F(TestJSON, testAddingObjectToArrayAddsNewObjectToArray)
{
  Object object;

  object["attributes"] << Object();

  object["attributes"][0]["member"] = "child";

  Object a = object["attributes"][0];

  ASSERT_TRUE( a.has("member") );
}

TEST_F(TestJSON, testAddingObjectToArrayAddsNewObjectToArrayWithCorrectValue)
{
  Object object;

  object["attributes"] << Object();

  object["attributes"][0]["member"] = "child";

  ASSERT_EQ( "child", object["attributes"][0]["member"] );
}


TEST_F(TestJSON, testDeepObject)
{
  Object object;

  object["one"]["two"]["three"]["four"] = 42;

  ASSERT_TRUE( object.has("one") );
  ASSERT_TRUE( object["one"].has("two") );
  ASSERT_TRUE( object["one"]["two"].has("three") );
  ASSERT_TRUE( object["one"]["two"]["three"].has("four") );

  ASSERT_EQ( 42, object["one"]["two"]["three"]["four"] );
}


TEST_F(TestJSON, testNestedArrayIndex)
{
  Object item;

  item["dn"] = "stuff";

  Object object;

  object["attributes"] << item;

  ASSERT_EQ( "stuff", object["attributes"][0]["dn"] );
}


TEST_F(TestJSON, testNullObjectStructureScans)
{
  std::istringstream in("{\"a\":null}");

  Object object;

  Object::scan( in );
}


TEST_F(TestJSON, testMissingQuoteOnObjectNameChucks)
{
  std::istringstream in("{\"a:null}");

  Object object;

  EXPECT_THROW( object.scan( in ),  std::runtime_error );
}


TEST_F(TestJSON, testMissingColonInObjectChucks)
{
  std::istringstream in("{\"a\" null}");

  Object object;

  EXPECT_THROW( object.scan( in ), std::runtime_error );
}


TEST_F(TestJSON, testAcceptableWhitespaceIgnored)
{
  std::istringstream in( "{\"a\" : null }");

  Object object;

  Object::scan( in );
}


TEST_F(TestJSON, testWhitespaceInStringNotIgnored)
{
  std::istringstream in( R"({"a":"some stuff" })");

  Object object = Object::scan( in );

  ASSERT_EQ( "some stuff", object["a"] );
}


TEST_F(TestJSON, testArrayWithValueScans)
{
  std::istringstream in( "{\"a\" : [true] }");

  Object object = Object::scan( in );

  ASSERT_EQ( true, bool(object["a"][0]) );
}


TEST_F(TestJSON, testObjectWithValueScans)
{
  std::istringstream in( R"({"a" : {"b":true} })");

  Object object = Object::scan( in );

  ASSERT_EQ( true, bool(object["a"]["b"]) );
}


TEST_F(TestJSON, testOutOfSequenceDelimitersChuck)
{
  std::istringstream in( "{\"a\" : [true} ]");

  Object object;

  EXPECT_THROW( object.scan( in ), std::runtime_error );
}


TEST_F(TestJSON, testIsObjectTrueCase)
{
  Object object {"object", Object{"a", "b"}};

  ASSERT_TRUE( object["object"].isObject() );
}


TEST_F(TestJSON, testIsObjectFalseCase)
{
  Object object {"object", Array{"a", "b"}};

  ASSERT_TRUE( !object["object"].isObject() );
}


TEST_F(TestJSON, testIsArrayTrueCase)
{
  Object object {"object", Array{"a", "b"}};
  
  ASSERT_TRUE( object["object"].isArray() );
}


TEST_F(TestJSON, testIsArrayFalseCase)
{
  Object object {"object", Object{"a", "b"}};
  
  ASSERT_TRUE( !object["object"].isArray() );
}


TEST_F(TestJSON, testIdenticalIntegerObjectValuesAreEqual)
{
  Object a {"a", 42};
  Object b {"b", 42};

  ASSERT_EQ( a["a"], b["b"] );
}


TEST_F(TestJSON, testIdenticalDoubleObjectValuesAreEqual)
{
  Object a {"a", 42.0};
  Object b {"b", 42.0};

  ASSERT_DOUBLE_EQ( a["a"], b["b"] );
}


TEST_F(TestJSON, testIdenticalDoubleAndIntegerObjectValuesAreNotEqual)
{
  Object a {"a", 42};
  Object b {"b", 42.0};

  ASSERT_TRUE( a["a"] != b["b"] );
}


TEST_F(TestJSON, testObjectConstructedFromIteratorCopiesOriginal)
{
  Object object;

  object["first"] = 64;

  Object first(*object.begin());

  ASSERT_EQ( 64, first["first"] );

  first["first"] = 21;

  ASSERT_EQ( 21, first["first"] );
  ASSERT_EQ( 64, object["first"] );
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv); 
  
  return RUN_ALL_TESTS();
}
