#include "gtest/gtest.h"
#include "lut3DL.h"

const std::string dbgLutsFolder = { TrDL_LUT_FOLDER };


TEST (Parse3DL, Custom_10bits_17nodes_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_10bits_17nodes.3dl" };
	CLut3DL<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_10bits_17nodes_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_10bits_17nodes.3dl" };
	CLut3DL<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_10bits_to_16bits_65nodes_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_10bits_to_16bits_65nodes.3dl" };
	CLut3DL<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_10bits_to_16bits_65nodes_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_10bits_to_16bits_65nodes.3dl" };
	CLut3DL<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_14bits_17nodes_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_14bits_17nodes.3dl" };
	CLut3DL<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_14bits_17nodes_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_14bits_17nodes.3dl" };
	CLut3DL<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_14bits_65nodes_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_14bits_65nodes.3dl" };
	CLut3DL<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Custom_14bits_65nodes_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Custom_14bits_65nodes.3dl" };
	CLut3DL<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Test_3DL_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Test.3dl" };
	CLut3DL<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST (Parse3DL, Test_3DL_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Test.3dl" };
	CLut3DL<double> lutFileF64;
	auto const result  = lutFileF64.LoadFile(lutName);
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse3DL, Fuji_Sepia_3DL_f32)
{
    const std::string lutName{ dbgLutsFolder + "/Fuji_XTrans_III-Sepia.3dl" };
    CLut3DL<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse3DL, Fuji_Sepia_3DL_f64)
{
    const std::string lutName{ dbgLutsFolder + "/Fuji_XTrans_III-Sepia.3dl" };
    CLut3DL<double> lutFileF64;
    auto const result = lutFileF64.LoadFile(lutName);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}


int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	std::cout << "Parse from: " << dbgLutsFolder << std::endl;
	return RUN_ALL_TESTS();	
}
