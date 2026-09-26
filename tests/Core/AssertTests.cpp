#include "Core/Assert.h"

#include <gtest/gtest.h>

// The suite builds in Debug, so assertions are enabled here. AssertDisabledTests.cpp covers the
// disabled behaviour.

TEST(Assert, PassingAssertionDoesNothing)
{
	MYRIAS_ASSERT(1 + 1 == 2, "arithmetic works");
	SUCCEED();
}

TEST(Assert, ExpressionIsEvaluatedExactlyOnce)
{
	int evaluations = 0;
	MYRIAS_ASSERT(++evaluations == 1, "evaluated once");
	EXPECT_EQ(evaluations, 1);
}

// One death test only: they spawn a process and are slow on Windows.
TEST(AssertDeathTest, FailingAssertionAbortsWithMessage)
{
	EXPECT_DEBUG_DEATH(MYRIAS_ASSERT(false, "expected failure"), "expected failure");
}
