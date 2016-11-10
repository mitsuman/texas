#include <cppunit/extensions/HelperMacros.h>
#include <string.h>

#include "poker.h"

class Test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( Test );

  CPPUNIT_TEST(highcard_0);
  CPPUNIT_TEST(highcard_1);

  CPPUNIT_TEST(pair_0);
  CPPUNIT_TEST(pair_1);

  CPPUNIT_TEST(twopair_0);
  CPPUNIT_TEST(twopair_1);
  CPPUNIT_TEST(twopair_2);
  CPPUNIT_TEST(twopair_3);

  CPPUNIT_TEST(threecard_0);
  CPPUNIT_TEST(threecard_1);
  CPPUNIT_TEST(threecard_2);

  CPPUNIT_TEST(fourcard_0);

  CPPUNIT_TEST(full_house_0);
  CPPUNIT_TEST(full_house_1);
  CPPUNIT_TEST(full_house_2);

  CPPUNIT_TEST(straight_0);
  CPPUNIT_TEST(straight_1);
  CPPUNIT_TEST(straight_2);
  CPPUNIT_TEST(straight_3);
  CPPUNIT_TEST(straight_4);

  CPPUNIT_TEST(flush_0);
  CPPUNIT_TEST(flush_1);

  CPPUNIT_TEST(straight_flush_0);
  CPPUNIT_TEST(straight_flush_1);

  CPPUNIT_TEST(compare_0);

  CPPUNIT_TEST(hole_0);
  CPPUNIT_TEST(hole_1);

  CPPUNIT_TEST(deck_0);
  CPPUNIT_TEST(deck_1);

  CPPUNIT_TEST_SUITE_END();

public:
  void setup() {
  }

  void tearDown() {
  }

  void highcard_0() {
    Hand a;
    // Ace pair
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 3));
    a.put(MAKE_CARD(3, 2));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)HICARD, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL((1 << (12 - 3)) | (1 << (10 - 3)) | (1 << (5 - 3))
                         | (1 << (3 - 3)) | (1 << (14 - 3)), GET_HICARD_KICKER(r));
  }

  void highcard_1() {
    Hand a;
    // Ace pair
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(1, 3));
    a.put(MAKE_CARD(3, 2));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)HICARD, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL((1 << (12 - 3)) | (1 << (10 - 3)) | (1 << (5 - 3))
                         | (1 << (9 - 3)) | (1 << (3 - 3)), GET_HICARD_KICKER(r));
  }

  void pair_0() {
    Hand a;
    // Ace pair
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 2));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL(10, GET_RANK3(r));
    CPPUNIT_ASSERT_EQUAL(5, GET_RANK4(r));
  }

  void pair_1() {
    Hand a;
    // ace kicker
    a.put(MAKE_CARD(0, 12));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 13));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 13));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK3(r));
    CPPUNIT_ASSERT_EQUAL(10, GET_RANK4(r));
  }

  void twopair_0() {
    Hand a;
    // twopair
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 5));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 10));
    a.put(MAKE_CARD(0, 12));
    a.put(MAKE_CARD(0, 3));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)TWO_PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 5, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK3(r));
  }

  void twopair_1() {
    Hand a;
    // twopair
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 5));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 12));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)TWO_PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL( 5, GET_RANK3(r));
  }

  void twopair_2() {
    Hand a;
    // twopair
    a.put(MAKE_CARD(0, 13));
    a.put(MAKE_CARD(1, 13));
    a.put(MAKE_CARD(3, 10));
    a.put(MAKE_CARD(1, 11));
    a.put(MAKE_CARD(1, 12));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)TWO_PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL(11, GET_RANK3(r));
  }

  void twopair_3() {
    Hand a;
    // two pair
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(1, 3));
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(2, 2));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(3, 5));
    a.put(MAKE_CARD(3, 5));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)TWO_PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 5, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 2, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK3(r));
  }

  void threecard_0() {
    Hand a;
    // three card
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 1));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(0, 13));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)THREE_CARD, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL(12, GET_RANK3(r));
  }

  void threecard_1() {
    Hand a;
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(1, 2));
    a.put(MAKE_CARD(3, 3));
    a.put(MAKE_CARD(0, 3));
    a.put(MAKE_CARD(0, 3));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)THREE_CARD, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 3, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL( 2, GET_RANK3(r));
  }

  void threecard_2() {
    Hand a;
    // three card
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 1));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 2));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(0, 9));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)TWO_PAIR, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 9, GET_RANK2(r));
    CPPUNIT_ASSERT_EQUAL( 5, GET_RANK3(r));
  }

  void fourcard_0() {
    Hand a;
    // four card
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 5));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)FOUR_CARD, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    //CPPUNIT_ASSERT_EQUAL( 9, GET_RANK2(r));
  }

  void full_house_0() {
    Hand a;
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 1));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(0, 5));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)FULL_HOUSE, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 5, GET_RANK2(r));
  }

  void full_house_1() {
    Hand a;
    // full house, not flush
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 5));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)FULL_HOUSE, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 5, GET_RANK2(r));
  }

  void full_house_2() {
    Hand a;
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(0, 9));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0,13));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)FULL_HOUSE, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL( 1, GET_RANK1(r));
    CPPUNIT_ASSERT_EQUAL( 9, GET_RANK2(r));
  }

  void straight_0() {
    Hand a;
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(1, 3));
    a.put(MAKE_CARD(3, 4));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(0, 6));
    a.put(MAKE_CARD(1, 9));
    a.put(MAKE_CARD(0, 10));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(6, GET_RANK1(r));
  }

  void straight_1() {
    Hand a;
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(1, 3));
    a.put(MAKE_CARD(3, 4));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 6));
    a.put(MAKE_CARD(0, 7));
    a.put(MAKE_CARD(0, 8));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(8, GET_RANK1(r));
  }

  void straight_2() {
    Hand a;
    a.put(MAKE_CARD(0, 8));
    a.put(MAKE_CARD(1, 9));
    a.put(MAKE_CARD(3, 11));
    a.put(MAKE_CARD(0, 10));
    a.put(MAKE_CARD(1, 12));
    a.put(MAKE_CARD(0, 13));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK1(r));
  }

  void straight_3() {
    Hand a;
    a.put(MAKE_CARD(0, 8));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 11));
    a.put(MAKE_CARD(0, 10));
    a.put(MAKE_CARD(1, 12));
    a.put(MAKE_CARD(0, 13));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(1, GET_RANK1(r));
  }

  void straight_4() {
    Hand a;
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 3));
    a.put(MAKE_CARD(0, 4));
    a.put(MAKE_CARD(1, 5));
    a.put(MAKE_CARD(0, 13));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(5, GET_RANK1(r));
  }

  void flush_0() {
    Hand a;
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 3));
    a.put(MAKE_CARD(0, 4));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(0, 13));
    a.put(MAKE_CARD(0, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)FLUSH, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(13, GET_RANK1(r));
  }

  void flush_1() {
    Hand a;
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(1, 1));
    a.put(MAKE_CARD(3, 3));
    a.put(MAKE_CARD(1, 4));
    a.put(MAKE_CARD(1, 5));
    a.put(MAKE_CARD(1, 13));
    a.put(MAKE_CARD(1, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)FLUSH, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(1, GET_RANK1(r));
  }

  void straight_flush_0() {
    Hand a;
    a.put(MAKE_CARD(0, 2));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0, 3));
    a.put(MAKE_CARD(0, 4));
    a.put(MAKE_CARD(0, 5));
    a.put(MAKE_CARD(1, 13));
    a.put(MAKE_CARD(1, 12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT_FLUSH, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(5, GET_RANK1(r));
  }

  void straight_flush_1() {
    Hand a;
    a.put(MAKE_CARD(1, 2));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0,10));
    a.put(MAKE_CARD(2, 4));
    a.put(MAKE_CARD(0,11));
    a.put(MAKE_CARD(0,13));
    a.put(MAKE_CARD(0,12));
    int r = a.calc();
    CPPUNIT_ASSERT_EQUAL((int)STRAIGHT_FLUSH, GET_HAND(r));
    CPPUNIT_ASSERT_EQUAL(1, GET_RANK1(r));
  }

  void compare_0() {
    // hicard vs hicard
    CPPUNIT_ASSERT(Hand::calcHand("AdKh 8c5c4d QsJd") >  Hand::calcHand("Ad2h 8c5c4d QsJd"));
    CPPUNIT_ASSERT(Hand::calcHand("TsKh 8c5c4d QsJd") <  Hand::calcHand("As2h 8c5c3d QsJd"));
    CPPUNIT_ASSERT(Hand::calcHand("As3h 8c5c4d QsJd") == Hand::calcHand("As2h 8c5c4d QsJd"));

    // hicard vs one pair
    CPPUNIT_ASSERT(Hand::calcHand("AsKh 8c5c4d QsJd") <  Hand::calcHand("As2h 8c5c2d QsJd"));
    CPPUNIT_ASSERT(Hand::calcHand("AsKh 8c5c4d QsKd") >  Hand::calcHand("As2h 8c5c3d QsJd"));

    // 2 pair vs one pair
    CPPUNIT_ASSERT(Hand::calcHand("8s5h 8c5c4d QsJd") >  Hand::calcHand("As2h 8c5c2d QsJd"));

    // 2 pair vs 2 pair (kicker)
    CPPUNIT_ASSERT(Hand::calcHand("5s9h KcKc9d Qs3d") == Hand::calcHand("9s8h KcKc9d Qs3d"));
    CPPUNIT_ASSERT(Hand::calcHand("As9h KcKc9d Qs3d") >  Hand::calcHand("9s8h KcKc9d Qs3d"));

    // 2 pair vs trips
    CPPUNIT_ASSERT(Hand::calcHand("AsKh Ac5c4d QsKd") <  Hand::calcHand("As2h 8c2c2d QsJd"));

    // set vs straight
    CPPUNIT_ASSERT(Hand::calcHand("KsKh KcJc3d Qs2d") <  Hand::calcHand("Tc9d KcJc3d Qs2d"));

    // straight vs flash
    CPPUNIT_ASSERT(Hand::calcHand("As3s Js5c4s 3sKd") >  Hand::calcHand("7c6c Js5c4s 3sKd"));

    // nuts flash vs flash
    CPPUNIT_ASSERT(Hand::calcHand("As3s Js5c4s 3sKd") >  Hand::calcHand("7s6s Js5c4s 3sKd"));

    // nuts flash vs full house
    CPPUNIT_ASSERT(Hand::calcHand("As3s Js5c4s 3sJd") <  Hand::calcHand("5h5s Js5c4s 3sJd"));
  }

  void hole_0() {
    Hand a;
    a.put(MAKE_CARD(1, 2));
    a.put(MAKE_CARD(0, 1));
    a.put(MAKE_CARD(0,10));
    CPPUNIT_ASSERT_EQUAL(GET_HOLE(1, 2, false), a.getHole());

    a.clear();
    a.put(MAKE_CARD(1, 2));
    a.put(MAKE_CARD(1, 1));
    CPPUNIT_ASSERT_EQUAL(GET_HOLE(1, 2, true), a.getHole());

    a.clear();
    a.put(MAKE_CARD(1, 9));
    a.put(MAKE_CARD(0, 2));
    CPPUNIT_ASSERT_EQUAL(GET_HOLE(9, 2, false), a.getHole());

    a.clear();
    a.put(MAKE_CARD(1, 9));
    a.put(MAKE_CARD(1, 13));
    CPPUNIT_ASSERT_EQUAL(GET_HOLE(13, 9, true), a.getHole());
  }

  void hole_1() {
    int hole[Hand::holeSize];
    CLEAR(hole);

    int cnt = 0;

    // assign
    for (int suite = 0; suite < 2; suite++) {
      for (int i = 1; i <= 13; i++) {
        for (int j = i; j <= 13; j++) {
          if (suite == 0 && i == j) continue;
          hole[GET_HOLE(j, i, suite == 0)] = cnt;
          cnt++;
        }
      }
    }

    // check
    cnt = 0;
    for (int suite = 0; suite < 2; suite++) {
      for (int i = 1; i <= 13; i++) {
        for (int j = i; j <= 13; j++) {
          if (suite == 0 && i == j) continue;
          int p = GET_HOLE(j, i, suite == 0);
          CPPUNIT_ASSERT(p >=0 && p < Hand::holeSize);
          CPPUNIT_ASSERT_EQUAL(hole[p], cnt);
          cnt++;
        }
      }
    }
  }

  bool check_deck(Deck& a) {
    bool ret = true;

    for (int i = 0; i < 4; i++) {
      for (int j = 1; j <= 13; j++) {
        bool found = false;
        Card c = MAKE_CARD(i, j);
        for (int k = 0; k < 52; k++) {
          if (a.cards[k] == c) {
            found = true;
            break;
          }
        }

        if (!found) {
          ret = false;
          break;
        }
      }
    }

    return ret;
  }

  void deck_0() {
    Deck a;
    CPPUNIT_ASSERT(check_deck(a));
    a.shuffle();
    CPPUNIT_ASSERT(check_deck(a));
  }

  void deck_1() {
    Deck a;
    CPPUNIT_ASSERT(check_deck(a));
    a.shuffle();
    CPPUNIT_ASSERT(check_deck(a));

    CPPUNIT_ASSERT(a.remove(MAKE_CARD(0, 1)));
    CPPUNIT_ASSERT(!a.remove(MAKE_CARD(0, 1)));

    CPPUNIT_ASSERT(a.remove(MAKE_CARD(2, 9)));
    CPPUNIT_ASSERT(!a.remove(MAKE_CARD(2, 9)));
  }
};

CppUnit::TestSuite* handTestSuite() { return Test::suite(); }
