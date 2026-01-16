/******************************* C++ Source File *******************************
 *
 *  PROJECT:     JSON Algorithm tests
 *
 *  MODULE:      JSON
 *
 ******************************************************************************/

#include "JSON.h"
#include "JSONAlgorithm.h"

#include <gtest/gtest.h>
#include <sstream>

namespace
{
  class TestAlgorithm: public testing::Test
  {
    protected:

      TestAlgorithm() = default;
  };
}

using masuma::json::Object;
using masuma::json::Array;
using masuma::json::Value;

TEST_F(TestAlgorithm, testSimpleObjectIdenticalToSelf)
{
  Object object = Object::scan( "{\"object\":42}" );

  Object other = object.clone();

  ASSERT_TRUE( object == other );
}

TEST_F(TestAlgorithm, testSimpleObjectNotIdenticalToMoifiedSelf)
{
  Object object = Object::scan( "{\"value\":42}" );

  Object other = object.clone();

  other["value"] = 33;

  ASSERT_TRUE( object != other );
}

TEST_F(TestAlgorithm, testSimpleStringObjectIdenticalToSelf)
{
  Object object = Object::scan( R"({"object":"value"})" );

  Object other = object.clone();

  ASSERT_TRUE( object == other );
}

TEST_F(TestAlgorithm, testSimpleStringObjectNotIdenticalToMoifiedSelf)
{
  Object object = Object::scan( R"({"object":"value"})" );

  Object other = object.clone();

  other["object"] = "Different";

  ASSERT_TRUE( object != other );
}

TEST_F(TestAlgorithm, testSimpleDifferntTypeObjectsNotIdentical)
{
  Object object = Object::scan( R"({"object":"value"})" );

  Object other = object.clone();

  other["object"] = 2;

  ASSERT_TRUE( object != other );
}

TEST_F(TestAlgorithm, testObjectWithNestedObjectIdenticalToSelf)
{
  Object object = Object::scan( R"({"object":{"value":42}})" );

  Object other = object.clone();

  ASSERT_TRUE( object == other );
}

TEST_F(TestAlgorithm, testObjectWithNestedObjectNotIdenticalToMoifiedSelf)
{
  Object object = Object::scan( R"({"object":{"value":42}})" );

  Object other = object.clone();

  other["object"]["value"] = 2;

  ASSERT_TRUE( object != other );
}

TEST_F(TestAlgorithm, testArrayIdenticalToSelf)
{
  Array array;
  array << 1 << 2 << 3;

  Array other = array.clone();

  ASSERT_TRUE( array == other );
}

TEST_F(TestAlgorithm, testArrayNotIdenticalToMoifiedSelf)
{
  Array array;
  array << 1 << 2 << 3;

  Array other = array.clone();

  other[1] = 5;

  ASSERT_TRUE( array != other );
}

TEST_F(TestAlgorithm, testObjectWithNestedArrayIdenticalToSelf)
{
  Object object = Object::scan( "{\"object\":[1,2,3]}" );

  Object other = object.clone();

  ASSERT_TRUE( object == other );
}

TEST_F(TestAlgorithm, testObjectWithNestedArrayNotIdenticalToMoifiedSelf)
{
  Object object = Object::scan( "{\"object\":[1,2,3]}" );

  Object other = object.clone();

  other["object"][1] = 5;

  ASSERT_TRUE( object != other );
}

TEST_F(TestAlgorithm, testNornaliseSortsMembersofSimpleObject)
{
  Object object;
  object["c"] = "last";
  object["a"] = 1;
  object["b"] = 2;

  Object other;
  other["a"] = 1;
  other["b"] = 2;
  other["c"] = "last";

  ASSERT_TRUE( object != other );

  ASSERT_TRUE( normalise( object ) == other );
}

TEST_F(TestAlgorithm, testNornaliseSortsMembersofWithNestedObjects)
{
  Object object;
  object["c"]["d"] = "last";
  object["c"]["a"] = 1;
  object["b"] = 2;

  Object other;
  other["b"] = 2;
  other["c"]["a"] = 1;
  other["c"]["d"] = "last";

  ASSERT_TRUE( object != other );

  ASSERT_TRUE( normalise( object ) == other );
}

TEST_F(TestAlgorithm, testNornaliseDefaultDoesNotSortArrays)
{
  Object object;
  object["a"] << 1 << 2 << 5 << 3;

  Object other = object.clone();

  ASSERT_TRUE( normalise( object ) == other );
}

TEST_F(TestAlgorithm, testNornaliseDefaultDoesSortArraysIfRequested)
{
  Object object;
  object["a"] << 1 << 2 << 5 << 3;

  Object other;
  other["a"] << 1 << 2 << 3 << 5;

  ASSERT_TRUE( normalise( object, true ) == other );
}

TEST_F(TestAlgorithm, testSimpleObjectHasSameItemsAsSelf)
{
  Object object;
  object["a"] = 1;
  object["b"] = 2;

  Object other = object.clone();

  ASSERT_TRUE( haveSameItems( object, other ) );
}

TEST_F(TestAlgorithm, testSimpleObjectDoesNotHaveSameItemsAsDifferentObject)
{
  Object object;
  object["a"] = 1;
  object["b"] = 2;

  Object other;
  other["a"] = 1;
  other["c"] = 2;

  ASSERT_TRUE( !haveSameItems( object, other ) );
}

TEST_F(TestAlgorithm, testObjectWithNestedObjectsHasSameItemsAsSelf)
{
  Object object;
  object["a"] = 1;
  object["e"]["b"] = 2;
  object["e"]["c"] = 2;

  Object other = object.clone();

  ASSERT_TRUE( haveSameItems( object, other ) );
}

using masuma::json::GetObjectsByName;

TEST_F(TestAlgorithm, testForEachRecursiveWorksForFlatObject)
{
  Object object;
  object["a"] = 1;
  object["b"] = 2;
  object["c"] = 3;

  Array result = forEachRecursive( object, GetObjectsByName("b") ).matches;

  ASSERT_EQ( 1, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["b"], value );
}

TEST_F(TestAlgorithm, testForEachRecursiveWorksForNestedObject)
{
  Object object;
  object["a"] = 1;

  Object child;
  child["c"] = 2;

  object["b"] = child;
  object["d"] = 3;

  Array result = forEachRecursive( object, GetObjectsByName("c") ).matches;

  ASSERT_EQ( 1, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["b"]["c"], value );
}

TEST_F(TestAlgorithm, testForEachRecursiveWorksForObjectWithChild)
{
  Object object;
  object["target"] = 1;

  Object child;
  child["c"] = 2;

  Object target;
  target["target"] = 42;
  child["a"] = target;

  object["c"] = child;
  object["d"] = 3;

  Array result = forEachRecursive( object, GetObjectsByName("target") ).matches;

  ASSERT_EQ( 2, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["target"], value );

  value = result[1];

  ASSERT_EQ( object["c"]["a"]["target"], value );
}

using masuma::json::GetParentObjectsByName;

TEST_F(TestAlgorithm, testForEachRecursiveStopsLookingIfOperationReturnsFalse)
{
  Object object;
  object["a"] = 1;

  Object child;
  child["c"] = 2;

  object["c"] = child;
  object["d"] = 3;

  Array result = forEachRecursive( object, GetParentObjectsByName("c") ).matches;

  ASSERT_EQ( 1, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["c"], value );
}

TEST_F(TestAlgorithm, testForEachRecursiveWorksForObjectInArray)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] << target;
  object["c"] = 3;

  Array result = forEachRecursive( object, GetObjectsByName("target") ).matches;

  ASSERT_EQ( 1, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["b"][0]["target"], value );
}

TEST_F(TestAlgorithm, testForEachRecursiveBoundedWorksForObjectInArray)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] << target;
  object["c"] = 3;

  Array result = forEachRecursive( object, GetParentObjectsByName("target") ).matches;

  ASSERT_EQ( 1, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["b"][0]["target"], value );
}

TEST_F(TestAlgorithm, testForEachRecursiveBoundedFindsOnlyParentObjectInArray)
{
  Object object;
  Object target;
  Object nested;

  nested["target"] = 42;
  target["target"] = nested;

  object["a"] = 1;
  object["b"] << target;
  object["c"] = 3;

  Array result = forEachRecursive( object, GetParentObjectsByName("target") ).matches;

  ASSERT_EQ( 1, result.size() );

  Value value {result[0]};

  ASSERT_EQ( object["b"][0]["target"], value );

  ASSERT_EQ( 2,
                        forEachRecursive( object,
                                          GetObjectsByName("target") ).matches.size() );
}

using masuma::json::FindObjectByName;

TEST_F(TestAlgorithm, testFindRecursiveReturnsEndForMissingObject)
{
  Object object;
  object["a"] = 1;
  object["b"] = 2;
  object["c"] = 3;

  Object result = findRecursive( object, FindObjectByName("d") );

  ASSERT_TRUE( result.empty() );
}

TEST_F(TestAlgorithm, testFindRecursiveWorksForSingleObject)
{
  Object object;
  object["b"] = 2;

  Object result = findRecursive( object, FindObjectByName("b") );

  ASSERT_TRUE( !result.empty() );
  ASSERT_TRUE( result.has("b") );

  int value = result["b"];

  ASSERT_EQ( object["b"], value );
}

TEST_F(TestAlgorithm, testFindRecursiveWorksForFlatObject)
{
  Object object;
  object["a"] = 1;
  object["b"] = 2;
  object["c"] = 3;

  Object result = findRecursive( object, FindObjectByName("b") );

  ASSERT_TRUE( !result.empty() );
  ASSERT_TRUE( result.has("b") );

  int value = result["b"];

  ASSERT_EQ( object["b"], value );
}

TEST_F(TestAlgorithm, testFindRecursiveWorksForNestedObject)
{
  Object object;
  object["a"] = 1;

  Object child;
  child["c"] = 2;

  Object target;
  target["target"] = 42;
  child["a"] = target;

  object["b"] = child;
  object["d"] = 3;

  Object result = findRecursive( object, FindObjectByName("target") );

  ASSERT_TRUE( !result.empty() );

  ASSERT_EQ( Object {object["b"]["a"]}, result );
}

TEST_F(TestAlgorithm, testFindRecursiveWorksForObjectWithChild)
{
  Object object;
  object["a"] = 1;

  Object child;
  child["c"] = 2;

  object["b"] = child;
  object["d"] = 3;

  Object result = findRecursive( object, FindObjectByName("b") );

  ASSERT_TRUE( !result.empty() );
  ASSERT_TRUE( result.has("b") );

  const int expected = object["b"]["c"];
  const int value    = result["b"]["c"];

  ASSERT_EQ( expected, value );
}

TEST_F(TestAlgorithm, testFindRecursiveWorksForObjectInArray)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] << target;
  object["c"] = 3;

  Object result = findRecursive( object, FindObjectByName("target") );

  ASSERT_TRUE( !result.empty() );
  ASSERT_TRUE( result.has("target") );

  ASSERT_EQ( target, result );
}

TEST_F(TestAlgorithm, testFindRecursiveWorksForArray)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] << target;
  object["c"] = 3;

  Object result = findRecursive( object["b"], FindObjectByName("target") );

  ASSERT_TRUE( !result.empty() );
  ASSERT_TRUE( result.has("target") );

  ASSERT_EQ( target, result );
}

using masuma::json::GetObjectsContainingName;

TEST_F(TestAlgorithm, testGetObjectsContainingNameWorksForSimpleObject)
{
  Object target;

  target["target"] = 42;

  Array results = forEachRecursive( target,
                                    GetObjectsContainingName("target") ).matches;

  ASSERT_TRUE( !results.empty() );

  const Object result = results[0];

  ASSERT_EQ( result, target );
}

TEST_F(TestAlgorithm, testGetObjectsContainingNameWorksForObjectWithOneMatch)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] = target;
  object["c"] = 3;

  Array results = forEachRecursive( object,
                                    GetObjectsContainingName("target") ).matches;

  ASSERT_TRUE( !results.empty() );

  const Object result = results[0];

  ASSERT_EQ( result, Object {object["b"]} );
}

TEST_F(TestAlgorithm, testGetObjectsContainingNameWorksForObjectWithTwoMatches)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] = target;
  object["c"] = 3;

  Object target1;

  target1["target"] = 421;
  object["d"] = target1;

  Array results = forEachRecursive( object,
                                    GetObjectsContainingName("target") ).matches;

  ASSERT_TRUE( !results.empty() );
  ASSERT_EQ( 2, results.size() );

  ASSERT_EQ( Object{results[0]}, Object{object["b"]} );
  ASSERT_EQ( Object{results[1]}, Object{object["d"]} );
}

TEST_F(TestAlgorithm, testGetObjectsContainingNameWorksForMultiLevelMatches)
{
  Object object;
  Object target;

  target["target"] = 42;

  object["a"] = 1;
  object["b"] = target;
  object["c"] = 3;

  Object target1;

  target1["target"] = 421;
  Object nested;

  nested["a"] = target1;
  object["d"] = nested;

  Array results = forEachRecursive( object,
                                    GetObjectsContainingName("target") ).matches;

  ASSERT_TRUE( !results.empty() );
  ASSERT_EQ( 2, results.size() );
  ASSERT_EQ( Object{results[0]}, Object{object["b"]} );
  ASSERT_EQ( Object{results[1]}, Object{object["d"]["a"]} );
}

namespace test
{
  struct Other
  {
    Object data;

    void add( const std::string& name, int64_t value )
    {
      data[name] = value;
    }

    void add( const std::string& name, bool value )
    {
      data[name] = value;
    }

    void add( const std::string& name, const std::string& value )
    {
      data[name] = value;
    }

    void add( const std::string& name, const Object& value )
    {
      data[name] = value;
    }
  };
}

TEST_F(TestAlgorithm, testConvertToWorksForIntType)
{
  Object object;
  object["x"] = 42;

  test::Other other;

  convertTo( object, other );

  ASSERT_TRUE( other.data.has("x") );
  ASSERT_TRUE( other.data["x"].is<int64_t>() );
  ASSERT_EQ( other.data["x"], 42 );
}

TEST_F(TestAlgorithm, testConvertToWorksForBoolType)
{
  Object object;
  object["x"] = true;

  test::Other other;

  convertTo( object, other );

  ASSERT_TRUE( other.data.has("x") );
  ASSERT_TRUE( other.data["x"].is<bool>() );
  ASSERT_EQ( other.data["x"], true );
}

TEST_F(TestAlgorithm, testConvertToWorksForStringType)
{
  Object object;
  object["x"] = "hello";

  test::Other other;

  convertTo( object, other );

  ASSERT_TRUE( other.data.has("x") );
  ASSERT_TRUE( other.data["x"].is<std::string>() );
  ASSERT_EQ( other.data["x"], "hello" );
}

namespace
{
  class TestSetDifference : public testing::Test
  {
    protected:

      TestSetDifference() = default;
  };
}

TEST_F(TestSetDifference, testEmptyObjectReturnedWithEmptyInputs)
{
  Object object;
  Object other;

  Object diff = setDifference( object, other );

  ASSERT_TRUE( diff.empty() );
}

TEST_F(TestSetDifference, testEmptyObjectReturnedWithSimpleIdenticalInputs)
{
  Object object = Object::scan( "{\"x\":42}" );
  Object other  = Object::scan( "{\"x\":42}" );

  Object diff = setDifference( object, other );

  ASSERT_TRUE( diff.empty() );
}

TEST_F(TestSetDifference, testSimpleFirstObjectReturnedIfSecondObjectEmpty)
{
  Object object = Object::scan( "{\"x\":42}" );
  Object other;

  Object diff = setDifference( object, other );

  ASSERT_TRUE( diff == object );
}

TEST_F(TestSetDifference, testObjectWithItemsMissingFromSecondInputReturned)
{
  const Object object = Object::scan( R"({"x":42,"y":2})" );
  const Object other  = Object::scan( "{\"x\":42}" );

  Object diff = setDifference( object, other );

  const Object expected = Object::scan( "{\"y\":2}" );

  ASSERT_TRUE( expected == diff );
}

TEST_F(TestSetDifference, testObjectWithItemsMissingFromNestedObjectInSecondInputReturned)
{
  Object object;
  object["a"] = 1;
  object["e"]["b"] = 2;
  object["e"]["c"] = 2;

  Object other;
  other["a"] = 1;
  other["e"]["b"] = 2;

  Object diff = setDifference( object, other );

  Object expected;
  expected["e"]["c"] = 2;

  ASSERT_EQ( expected.asString(), diff.asString() );
}

TEST_F(TestSetDifference, testObjectWithItemsMissingFromNestedArrayInSecondInputReturned)
{
  Object object;
  object["a"] = 1;
  object["e"] << 1 << 2 << 3;

  Object other;
  other["a"] = 1;
  other["e"] << 2;

  Object diff = setDifference( object, other );

  Object expected;
  expected["e"] << 1 << 3;

  ASSERT_EQ( expected.asString(), diff.asString() );
}

TEST_F(TestSetDifference, testObjectReturnedWithItemsPresentButDiffernetInNestedArray)
{
  Object object;
  object["a"] = 1;

  Object one;
  one["1"] = 1;
  one["2"] = 2;
  object["e"] << 1 << 2 << one;

  Object other;
  other["a"] = 1;

  Object two;
  two["1"] = 1;
  other["e"] << 1 << 2 << two;

  Object diff = setDifference( object, other );

  Object expected;
  expected["e"] << one;

  ASSERT_EQ( expected.asString(), diff.asString() );
}
