#include "gtest/gtest.h"
#include "lutCube3D.h"

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };


TEST (ParseCube3d, Parse_Tiny_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Tiny_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Tiny_With_Blob_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny_with_Blob.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Tiny_With_Blob_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny_with_Blob.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}


TEST(ParseCube3d, Parse_Small25_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Small25.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Small25.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_With_Blob_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Small25_with_Blob.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_With_Blob_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Small25_with_Blob.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Medium_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Medium.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 32);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Medium_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Medium.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 32);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Large.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 64);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Large.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 64);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large65_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Large65.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 65);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large65_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Large65.cube" };
	CCubeLut3D<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 65);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_FiLMiC_deHLG_V3_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/FiLMiC_deHLG_V3.cube" };
    CCubeLut3D<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_FiLMiC_deHLG_V3_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/FiLMiC_deHLG_V3.cube" };
    CCubeLut3D<double> lutFileF64;
    auto const result = lutFileF64.LoadFile(lutName);
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_MagicHour_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    CCubeLut3D<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 33);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_MagicHour_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    CCubeLut3D<double> lutFileF64;
    auto const result = lutFileF64.LoadFile(lutName);
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 33);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}
/*
 add huge LUT with 96 entries - where we may take lut like this ??? 
*/

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	std::cout << "Parse from: " << dbgLutsFolder << std::endl;
	return RUN_ALL_TESTS();	
}
