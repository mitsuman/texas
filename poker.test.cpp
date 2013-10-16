#include <cppunit/extensions/HelperMacros.h>
#include <string.h>

#include "poker.h"

class Test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( Test );

  CPPUNIT_TEST(test_0);
  CPPUNIT_TEST(test_1);
  /*CPPUNIT_TEST(test_2);
  CPPUNIT_TEST(test_3);
  CPPUNIT_TEST(test_4);
  */

  CPPUNIT_TEST_SUITE_END();

public:
  void setup() {
  }

  void tearDown() {
  }

  void test_0() {
    Hand a;
    // Ace pair
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 2));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)Hand::PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK1(r)); // ACE
    CPPUNIT_ASSERT_EQUAL(11, GET_KICKER(r));
  }

  void test_1() {
    Hand a;
    // twopair
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 5));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)Hand::TWO_PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK1(r)); // ACE
    CPPUNIT_ASSERT_EQUAL( 4, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL(11, GET_KICKER(r));
  }

};

CppUnit::TestSuite* handTestSuite() { return Test::suite(); }
