#include "gtest/gtest.h"
#include "lutHald.h"

const std::string dbgLutsFolder = { HALD_LUT_FOLDER };

TEST(Parse_Contrast_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/contrast.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}


TEST(Parse_DarkBlue_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/Dark_blue.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse_DeepDarkBlue_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/Deep_dark_blue.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse_DesaturationDark_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/Desat_dark.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse_DesaturationEdge_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/Desat_edge.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse_Desaturate_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/desaturate.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse_IdentifyLevel8_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/Identity_level_8.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(Parse_IdentifyLevel16_HCLUT, help_option)
{
	const std::string lutName{ dbgLutsFolder + "/Identity_level_16.HCLUT.png" };
	CHaldLut<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	std::cout << "Parse from: " << dbgLutsFolder << std::endl;
	return RUN_ALL_TESTS();	
}
