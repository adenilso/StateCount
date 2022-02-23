#include <iostream>
#include <values.h>

#include "BigNumber-test.h"

CPPUNIT_TEST_SUITE_REGISTRATION (BigNumberTest);
 // Datei: btest.cc
void BigNumberTest::setUp (void) {
}

void BigNumberTest::tearDown (void) {
}

void BigNumberTest::init (void) {
  BigNumber a;
  BigNumber b = 1234;
  BigNumber c(b);
  BigNumber d;
  d = b;
}

void BigNumberTest::print (void) {
  BigNumber b = 1234;
  std::cout << b << std::endl;
}

void BigNumberTest::inc (void) {
  BigNumber b = 9998;
  std::cout << b << std::endl;
  b++;
  std::cout << b << std::endl;
  b++;
  std::cout << b << std::endl;
}

void BigNumberTest::comparison (void) {
  BigNumber b = 9998;
  BigNumber c(b);
  BigNumber d(b);
  c ++;
  d ++;
  d ++;
  CPPUNIT_ASSERT_EQUAL(b, BigNumber(9998));
  CPPUNIT_ASSERT_EQUAL(d, BigNumber(10000));
  CPPUNIT_ASSERT(c < d);
}

void BigNumberTest::sum (void) {
  BigNumber b = 9998;
  BigNumber a = 9997;
  BigNumber c = a + b;
  CPPUNIT_ASSERT_EQUAL(c, BigNumber(9998 + 9997));
}

void BigNumberTest::mult (void) {
  BigNumber b = 9998;
  BigNumber a = 9997;
  BigNumber c = a * b;
  CPPUNIT_ASSERT_EQUAL(c, BigNumber(9998 * 9997));
}

void BigNumberTest::power (void) {
  for (long i = 1; i < 20; i++) {
    std::cout << i << "^" << i << " = " << Power(BigNumber(i), i) << std::endl;
  }
  std::cout << Power(35, 10) / Power(13, 8) << std::endl;
}

