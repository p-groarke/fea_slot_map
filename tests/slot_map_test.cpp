#include <slot_map/slot_map.h> // Must be included before <algorithm>

#include <gtest/gtest.h>

namespace {
TEST(slot_map, iter_swap_experiment) {
	using id = std::pair<unsigned, unsigned>;
	fea::slot_map<int, id> sm;
	std::vector<id> keys;
	// keys.rbegin();

	for (int i = 0; i < 20; ++i) {
		keys.push_back(sm.insert(i));
	}

	std::iter_swap(sm.begin(), sm.begin() + sm.size() - 1);
	auto pivot = std::partition(
			sm.begin(), sm.end(), [](int elem) { return elem % 2 == 0; });

	for (auto it = sm.begin(); it != pivot; ++it) {
		EXPECT_TRUE(*it % 2 == 0);
		EXPECT_EQ(sm.key(it), keys[*it]);
	}

	// std::sort(sm.begin(), sm.end()); // Doesn't use iter_swap on MSVC
	// for (auto it = sm.begin(); it != sm.end(); ++it) {
	//	EXPECT_EQ(sm.key(it), keys[*it]);
	//}

	// sm.erase(std::remove_if(sm.begin(), sm.end(),
	//				 [](int elem) { return elem % 2 == 0; }),
	//		sm.end()); // Doesn't use iter_swap on MSVC, possible erase hack?

	// for (auto it = sm.begin(); it != sm.end(); ++it) {
	//	EXPECT_EQ(sm.key(it), keys[*it]);
	//}
}

TEST(slot_map, basics) {
}
} // namespace
