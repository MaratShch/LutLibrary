#include "gtest/gtest.h"
#include <iostream>
#include "StreamPointer.h"

TEST(operator_equal, help_option)
{
	constexpr uint64_t zer0 = 0ull;
	constexpr uint64_t offset1 = (100ull << 32) | 7ull;
	const CStreamPointer streamA (0, 0);
	const CStreamPointer streamB (0, 0);
	const CStreamPointer streamC (1, 0);
	const CStreamPointer streamD (0, 1);
	const CStreamPointer streamE (100, 7);
	const CStreamPointer streamF (1, 0);

	const auto compare1 = (streamA == streamB);
	EXPECT_EQ(true, compare1);
	const auto compare2 = (streamA == streamC);
	EXPECT_EQ(false, compare2);
	const auto compare3 = (streamC == streamD);
	EXPECT_EQ(false, compare3);
	const auto compare4 = (streamD == streamE);
	EXPECT_EQ(false, compare4);
	const auto compare5 = (streamE == streamF);
	EXPECT_EQ(false, compare5);
	const auto compare6 = (streamC == streamF);
	EXPECT_EQ(true, compare6);
	const auto compare7 = (streamA == zer0);
	EXPECT_EQ(true, compare7);
	const auto compare8 = (zer0 == streamC);
	EXPECT_EQ(false, compare8);
	const auto compare9 = (offset1 == streamE);
	EXPECT_EQ(true, compare9);
	const auto compareA = (streamE == offset1);
	EXPECT_EQ(true, compareA);

	return;
}


TEST(operator_not_equal, help_option)
{
	constexpr uint64_t zer0 = 0ull;
	constexpr uint64_t offset1 = (100ull << 32) | 7ull;
	const CStreamPointer streamA (0, 0);
	const CStreamPointer streamB (0, 0);
	const CStreamPointer streamC (1, 0);
	const CStreamPointer streamD (0, 1);
	const CStreamPointer streamE (100, 7);
	const CStreamPointer streamF (1, 0);

	const auto compare1 = (streamA != streamB);
	EXPECT_EQ(false, compare1);
	const auto compare2 = (streamA != streamC);
	EXPECT_EQ(true, compare2);
	const auto compare3 = (streamC != streamD);
	EXPECT_EQ(true, compare3);
	const auto compare4 = (streamD != streamE);
	EXPECT_EQ(true, compare4);
	const auto compare5 = (streamE != streamF);
	EXPECT_EQ(true, compare5);
	const auto compare6 = (streamC != streamF);
	EXPECT_EQ(false, compare6);
	const auto compare7 = (streamA != zer0);
	EXPECT_EQ(false, compare7);
	const auto compare8 = (zer0 != streamC);
	EXPECT_EQ(true, compare8);
	const auto compare9 = (offset1 != streamE);
	EXPECT_EQ(false, compare9);
	const auto compareA = (streamE != offset1);
	EXPECT_EQ(false, compareA);


	return;
}


int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();	
}
