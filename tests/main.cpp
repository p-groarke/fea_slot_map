#include "SG14_test.h"

#include <gtest/gtest.h>

int main(int argc, char** argv) {
#if 0 // Times-out gtest discovery
	sg14_test::slot_map_test();
#endif

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
