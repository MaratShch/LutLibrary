#include "gtest/gtest.h"
#include <iostream>
#include <algorithm>
#include "StreamPointer.h"

TEST (operator_equal, help_option)
{
	constexpr uint64_t zer0 = make_stream_pointer_offset(0u, 0u);
	constexpr uint64_t offset1 = make_stream_pointer_offset(100u, 7u);
	const CStreamPointer streamA (0u, 0u);
	const CStreamPointer streamB (0u, 0u);
	const CStreamPointer streamC (1u, 0u);
	const CStreamPointer streamD (0u, 1u);
	const CStreamPointer streamE (100u, 7u);
	const CStreamPointer streamF (1u, 0u);

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


TEST (operator_not_equal, help_option)
{
	constexpr uint64_t zer0 = make_stream_pointer_offset(0u, 0u);
	constexpr uint64_t offset1 = make_stream_pointer_offset(100u, 7u);
	const CStreamPointer streamA (0u, 0u);
	const CStreamPointer streamB (0u, 0u);
	const CStreamPointer streamC (1u, 0u);
	const CStreamPointer streamD (0u, 1u);
	const CStreamPointer streamE (100u, 7u);
	const CStreamPointer streamF (1u, 0u);

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


TEST (operator_plus_plus, help_option)
{
	CStreamPointer streamA (0u, 7u);
	CStreamPointer streamB (10u, 0u);

        std::cout << streamA << " prefix ++ " << " will be equal to " << ++streamA << std::endl; 
        EXPECT_EQ(make_stream_pointer_offset(1u, 0u), streamA.get());

        std::cout << streamB << " prefix ++ " << " will be equal to " << ++streamB << std::endl; 
        EXPECT_EQ(make_stream_pointer_offset(10u, 1u), streamB.get());
	return;
}


TEST (operator_plus_plus_postfix, help_option)
{
	CStreamPointer streamA (0u, 7u);

        std::cout << streamA << " postfix ++ " << " will be equal to " << streamA++ << std::endl; 
        std::cout << "But at now stream pointer equal to " << streamA << std::endl; 
        EXPECT_EQ(make_stream_pointer_offset(1u, 0u), streamA.get());

 	return;
}


TEST (operator_minus_minus, help_option)
{
	CStreamPointer streamA (0u, 0u);
	CStreamPointer streamB (1u, 0u);
	
	int32_t i;
	constexpr int32_t loopCnt = 9;
	for (i = 0; i < loopCnt; i++)
        {
   	      std::cout << streamA << " prefix -- " << " will be equal to " << --streamA << std::endl; 
          EXPECT_EQ(0u, streamA.get());
        }

        int32_t idx = 7;
	for (i = 0; i < loopCnt; i++)
        {
   	      std::cout << streamB << " prefix -- " << " will be equal to " << --streamB << std::endl; 
          EXPECT_EQ(make_stream_pointer_offset(0, std::max(0, idx)), streamB.get());
          idx--;
        }
	return;
}

TEST (operator_minus_equal, help_option)
{
	constexpr uint64_t zer0 = make_stream_pointer_offset(0u, 0u);
	constexpr uint64_t offset1 = make_stream_pointer_offset(100u, 7u);
	CStreamPointer streamA (0u, 0u);
	CStreamPointer streamB (0u, 0u);
	CStreamPointer streamC (1u, 0u);
	CStreamPointer streamD (0u, 1u);
	CStreamPointer streamE (100u, 7u);
	CStreamPointer streamF (0u, 7u);
	CStreamPointer streamG (99u, 1u);
	
	std::cout << streamA << " -= " << streamB << " will be equal to " << (streamA -= streamB) << std::endl; 
	EXPECT_EQ(0u, streamA.get());
	std::cout << streamB << " -= " << streamC << " will be equal to " << (streamB -= streamC) << std::endl; 
	EXPECT_EQ(0u, streamB.get());
	std::cout << streamC << " -= " << streamB << " will be equal to " << (streamC -= streamB) << std::endl; 
	EXPECT_EQ(streamC.get(), make_stream_pointer_offset(1u, 0u));
	std::cout << streamE << " -= " << streamD << " will be equal to " << (streamE -= streamD) << std::endl; 
	EXPECT_EQ(streamE.get(), make_stream_pointer_offset(100u, 6u));
	std::cout << streamE << " -= " << streamF << " will be equal to " << (streamE -= streamF) << std::endl; 
	EXPECT_EQ(streamE.get(), make_stream_pointer_offset(99u, 7u));
	std::cout << streamE << " -= " << streamG << " will be equal to " << (streamE -= streamG) << std::endl; 
	EXPECT_EQ(streamE.get(), make_stream_pointer_offset(0u, 6u));
	return;
}


TEST (operator_plus, help_option)
{
	const CStreamPointer streamA (0u, 3u);
	const CStreamPointer streamB (1u, 0u);
	const CStreamPointer streamC (10u, 2u);
	const CStreamPointer streamD (100u, 7u);

	const CStreamPointer AsumB = streamA + streamB;
	std::cout << streamA << " + " << streamB << " will be equal to " << AsumB << std::endl; 
	EXPECT_EQ(AsumB.get(), make_stream_pointer_offset(1u, 3u));

	const CStreamPointer CsumD = streamC + streamD;
	std::cout << streamC << " + " << streamD << " will be equal to " << CsumD << std::endl; 
	EXPECT_EQ(CsumD.get(), make_stream_pointer_offset(111u, 1u));

	return;
}


TEST (operator_minus, help_option)
{
	const CStreamPointer streamA (0u, 3u);
	const CStreamPointer streamB (1u, 0u);
	const CStreamPointer streamC (10u, 2u);
	const CStreamPointer streamD (100u, 7u);

	const CStreamPointer AsubB = streamB - streamA;
	std::cout << streamB << " - " << streamA << " will be equal to " << AsubB << std::endl; 
	EXPECT_EQ(AsubB.get(), make_stream_pointer_offset(0u, 5u));

	const CStreamPointer BsubB = streamB - streamB;
	std::cout << streamB << " - " << streamB << " will be equal to " << BsubB << std::endl; 
	EXPECT_EQ(BsubB.get(), 0ull);

	const CStreamPointer CsubA = streamC - streamA;
	std::cout << streamC << " - " << streamA << " will be equal to " << CsubA << std::endl; 
	EXPECT_EQ(CsubA.get(), make_stream_pointer_offset(9u, 7u));

	const CStreamPointer DsubC = streamD - streamC;
	std::cout << streamD << " - " << streamC << " will be equal to " << DsubC << std::endl; 
	EXPECT_EQ(DsubC.get(), make_stream_pointer_offset(90u, 5u));

	constexpr uint64_t val1 = 6ull; /* absolute value in bits [actually equal to 0 bytes and 6 bits] */
	const CStreamPointer DsubVal1 = streamD - val1;
	std::cout << streamD << " - " << val1 << " bits, will be equal to " << DsubVal1 << std::endl; 
	EXPECT_EQ(DsubVal1.get(), make_stream_pointer_offset(100u, 1u));

	constexpr uint64_t val2 = 81ull; /* absolute value in bits [actually equal to 10 bytes and 1 bit] */
	const CStreamPointer DsubVal2 = streamD - val2;
	std::cout << streamD << " - " << val2 << " bits, will be equal to " << DsubVal2 << std::endl; 
	EXPECT_EQ(DsubVal2.get(), make_stream_pointer_offset(90u, 6u));

	const CStreamPointer Val2subB = val2 - streamB;
	std::cout << val2 << " bits - " << streamB << " will be equal to " << Val2subB << std::endl; 

	return;
}


int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();	
}
