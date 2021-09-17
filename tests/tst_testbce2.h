#ifndef TST_TESTBCE2_H
#define TST_TESTBCE2_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(TestSet, TestBCE2)
{
  EXPECT_EQ(1, 1);
  ASSERT_THAT(0, Eq(0));
}

#endif // TST_TESTBCE2_H
