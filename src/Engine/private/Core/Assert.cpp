#include "Core/Assert.h"

#include <cstdio>
#include <cstdlib>

void Assert_Private::Fail(const char* expression, const char* message, const char* file, int line) noexcept
{
	// fprintf does not allocate on the C++ heap and cannot throw: safe even when the failure
	// comes from a broken allocator or a frame that must not allocate.
	std::fprintf(stderr, "MYRIAS_ASSERT failed: %s - %s (%s:%d)\n", expression, message, file, line);
	std::fflush(stderr);
	std::abort();
}
