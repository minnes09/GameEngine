// Checks the Release behaviour inside the Debug test build: force assertions off for this
// translation unit only, before the header is included.
#define MYRIAS_ASSERTS_ENABLED 0
#include "Core/Assert.h"

#include <gtest/gtest.h>

TEST(AssertDisabled, ExpressionIsNotEvaluated)
{
	int evaluations = 0;
	MYRIAS_ASSERT(++evaluations == 1, "must not run");
	EXPECT_EQ(evaluations, 0);
}

TEST(AssertDisabled, FalseAssertionDoesNotAbort)
{
	MYRIAS_ASSERT(false, "disabled");
	SUCCEED();
}
