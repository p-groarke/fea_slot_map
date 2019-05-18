#include <algorithm>
#include <chrono>
#include <fea_slot_map/fea_slot_map.hpp>
#include <gtest/gtest.h>
#include <random>

namespace {


TEST(slot_map, generations) {
	fea::slot_map<size_t> m;

	for (size_t i = 0; i < 100; ++i) {
		m.insert(i);
	}

	m.erase(m.begin(), m.end());

	for (size_t i = 0; i < 100; ++i) {
		EXPECT_EQ(m.insert(i).second, 1);
	}

	m.erase(m.begin(), m.end());

	for (size_t i = 0; i < 100; ++i) {
		EXPECT_EQ(m.insert(i).second, 2);
	}
}

TEST(slot_map, erase_stable) {

	fea::slot_map<size_t> m;
	std::vector<std::pair<unsigned, unsigned>> keys;

	for (size_t i = 0; i < 100; ++i) {
		keys.push_back(m.insert(i));
	}
	while (!m.empty()) {
		m.erase_stable(keys.front());
		keys.erase(keys.begin());
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}

	for (size_t i = 0; i < 100; ++i) {
		keys.push_back(m.insert(i));
	}
	while (!m.empty()) {
		m.erase_stable(keys.back());
		keys.pop_back();
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}

	for (size_t i = 0; i < 100; ++i) {
		keys.push_back(m.insert(i));
	}
	while (!m.empty()) {
		m.erase_stable(keys.front());
		keys.erase(keys.begin());
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}

	for (size_t i = 0; i < 100; ++i) {
		keys.push_back(m.insert(i));
	}
	while (!m.empty()) {
		m.erase_stable(keys.back());
		keys.pop_back();
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}


	for (size_t i = 0; i < 100; ++i) {
		keys.push_back(m.insert(i));
	}
	srand(static_cast<unsigned int>(
			std::chrono::system_clock::now().time_since_epoch().count()));
	while (!m.empty()) {
		size_t i = rand() % m.size();
		m.erase_stable(keys[i]);
		keys.erase(keys.begin() + i);
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}

	// range erase_stable
	for (size_t i = 0; i < 1000; ++i) {
		m.insert(i);
	}
	while (!m.empty()) {
		m.erase_stable(m.data().begin(), m.data().begin() + 10);
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}

	for (size_t i = 0; i < 1000; ++i) {
		m.insert(i);
	}
	while (!m.empty()) {
		m.erase_stable(m.data().begin() + m.size() - 10, m.data().end());
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}

	for (size_t i = 0; i < 1000; ++i) {
		m.insert(i);
	}
	srand(static_cast<unsigned int>(
			std::chrono::system_clock::now().time_since_epoch().count()));
	while (!m.empty()) {
		size_t num = m.size() - 10;
		if (num == 0) {
			m.erase_stable(m.begin(), m.end());
			break;
		}

		size_t i = rand() % num;
		m.erase_stable(m.begin() + i, m.begin() + i + 10);
		EXPECT_TRUE(std::is_sorted(m.begin(), m.end()));
	}
}
} // namespace
