#ifndef FEMLinear_Test
#define FEMLinear_Test

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "BigNumber.h"

class BigNumberTest : public CPPUNIT_NS :: TestFixture
{
    CPPUNIT_TEST_SUITE (BigNumberTest);
    CPPUNIT_TEST (init);
    CPPUNIT_TEST (print);
    CPPUNIT_TEST (inc);
    CPPUNIT_TEST (comparison);
    CPPUNIT_TEST (sum);
    CPPUNIT_TEST (mult);
    CPPUNIT_TEST (power);
    CPPUNIT_TEST_SUITE_END ();

    public:
        void setUp (void);
        void tearDown (void);

    protected:
        void init (void);
        void print (void);
        void inc (void);
        void comparison (void);
        void sum (void);
        void mult (void);
        void power (void);

    private:
};

#endif
