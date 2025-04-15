#include "gtest/gtest.h"
#include "lutCube3D.h"

#ifdef PROJECT_GIT_BRANCH
const std::string dbgBranchName = { PROJECT_GIT_BRANCH };
#else
const std::string dbgBranchName = "Not defined";
#endif

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };

TEST (ParseCube3d, Parse_Tiny_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Tiny_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Tiny_With_Blob_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny_with_Blob.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Tiny_With_Blob_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny_with_Blob.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}


TEST(ParseCube3d, Parse_Small25_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Small25.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Small25.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_With_Blob_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Small25_with_Blob.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_With_Blob_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Small25_with_Blob.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_With_Domain_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Small25_with_Domain.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.1f  == domain.first[0]  && 0.1f  == domain.first [1] && 0.1f  == domain.first[2] &&
                0.95f == domain.second[0] && 0.95f == domain.second[1] && 0.95f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Small25_With_Domain_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Small25_with_Domain.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 25);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.1  == domain.first[0]  && 0.1  == domain.first [1] && 0.1  == domain.first[2] &&
                0.95 == domain.second[0] && 0.95 == domain.second[1] && 0.95 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Medium_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Medium.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 32);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Medium_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Medium.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 32);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Large.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 64);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Large.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 64);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large65_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Large65.cube" };
	CCubeLut3D<float> lutFileF32;
        // load and parse file
	auto const result  = lutFileF32.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 65);
        // identify MIN/MAX domain 
        auto const domain = lutFileF32.getMinMaxDomain();
        const bool domainValid = 
               (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
                1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Large65_Cube3D_f64)
{
	const std::string lutName{ dbgLutsFolder + "/Large65.cube" };
	CCubeLut3D<double> lutFileF64;
        // load and parse file
	auto const result  = lutFileF64.LoadFile(lutName);
        // identify LUT size
	auto const lutSize = lutFileF64.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 65);
        // identify MIN/MAX domain 
        auto const domain = lutFileF64.getMinMaxDomain();
        const bool domainValid = 
               (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
                1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
	GTEST_EXPECT_TRUE(domainValid == true);

	EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_FiLMiC_deHLG_V3_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/FiLMiC_deHLG_V3.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 64);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_FiLMiC_deHLG_V3_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/FiLMiC_deHLG_V3.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 64);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_MagicHour_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 33);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_MagicHour_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 33);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Huge_negative_lut_96_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/Huge_negative_lut_96.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 96);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Huge_negative_lut_96_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/Huge_negative_lut_96.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 96);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Huge_negative_lut_128_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/Huge_negative_lut_128.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 128);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Huge_negative_lut_128_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/Huge_negative_lut_128.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 128);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Identify_8_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/Identify_8.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 8);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Identify_8_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/Identify_8.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 8);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Identify_33_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/Identify_33.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 33);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Identify_33_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/Identify_33.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 33);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Identify_65_Cube3D_f32)
{
    const std::string lutName{ dbgLutsFolder + "/Identify_65.cube" };
    CCubeLut3D<float> lutFileF32;
    // load and parse file
    auto const result = lutFileF32.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF32.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 65);
    // identify MIN/MAX domain 
    auto const domain = lutFileF32.getMinMaxDomain();
    const bool domainValid = 
           (0.f == domain.first[0]  && 0.f == domain.first [1] && 0.f == domain.first[2] &&
            1.f == domain.second[0] && 1.f == domain.second[1] && 1.f == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}

TEST(ParseCube3d, Parse_Identify_65_Cube3D_f64)
{
    const std::string lutName{ dbgLutsFolder + "/Identify_65.cube" };
    CCubeLut3D<double> lutFileF64;
    // load and parse file
    auto const result = lutFileF64.LoadFile(lutName);
    // identify LUT size
    auto const lutSize = lutFileF64.getLutSize();
    GTEST_EXPECT_TRUE(lutSize == 65);
    // identify MIN/MAX domain 
    auto const domain = lutFileF64.getMinMaxDomain();
    const bool domainValid = 
           (0.0 == domain.first[0]  && 0.0 == domain.first [1] && 0.0 == domain.first[2] &&
            1.0 == domain.second[0] && 1.0 == domain.second[1] && 1.0 == domain.second[2]);
    GTEST_EXPECT_TRUE(domainValid == true);

    EXPECT_EQ(result, LutErrorCode::LutState::OK);
}


int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
	std::cout << "Parse from: " << dbgLutsFolder << std::endl;
    std::cout << "Branch name: " << dbgBranchName << std::endl;
    return RUN_ALL_TESTS();
}
