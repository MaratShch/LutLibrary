#include "gtest/gtest.h"
#include "lutHald.h"


#ifdef PROJECT_GIT_BRANCH
const std::string dbgBranchName = {PROJECT_GIT_BRANCH};
#else
const std::string dbgBranchName = "Not defined";
#endif

const std::string dbgLutsFolder = { HALD_LUT_FOLDER };

TEST(ParseHald, Parse_Contrast_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/contrast.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_DarkBlue_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Dark_blue.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_DeepDarkBlue_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Deep_dark_blue.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_DesaturationDark_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Desat_dark.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_DesaturationEdge_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Desat_edge.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_Desaturate_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/desaturate.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_IdentifyLevel8_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Identity_level_8.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 64);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_IdentifyLevel16_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Identity_level_16.HCLUT.png" };
    CHaldLut<float> lutFileF32;
    auto const result  = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 256);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_AnalogFX_Sepia_Color_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/AnalogFX-Sepia-Color.png" };
    CHaldLut<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 144);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_ZilverFX_Vintage_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/ZilverFX-Vintage-B&W.png" };
    CHaldLut<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 144);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_Kodak_Ektar_100_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Kodak_Ektar_100.png" };
    CHaldLut<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 256);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

// MONO LUT Not supported yet!!!
//TEST(ParseHald, Parse_PolaroidMono_HCLUT)
//{
//    const std::string lutName{ dbgLutsFolder + "/PolaroidMono.png" };
//    CHaldLut<float> lutFileF32;
//    auto const result = lutFileF32.LoadFile(lutName);
//    auto const lutSize = lutFileF32.getLutSize();
//    EXPECT_EQ(result, LutErrorCode::LutState::OK);
//}

TEST(ParseHald, Dark_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/Dark.png" };
    CHaldLut<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    EXPECT_EQ(lutSize, 144);
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseHald, Parse_Neutral_HCLUT)
{
    const std::string lutName{ dbgLutsFolder + "/neutral_hald_512.png" };
    CHaldLut<float> lutFileF32;
    auto const result = lutFileF32.LoadFile(lutName);
    auto const lutSize = lutFileF32.getLutSize();
    std::cout << "Size = " << lutSize << std::endl;
    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Parse from : " << dbgLutsFolder << std::endl;
    std::cout << "Branch name: " << dbgBranchName << std::endl;
    return RUN_ALL_TESTS();	
}
