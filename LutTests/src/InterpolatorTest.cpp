#include "gtest/gtest.h"
#include "lutCube3D.h"
#include <array>

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };

using RGBf32 = std::array<float, 3>;
using RGBf64 = std::array<double,3>;

// Test points
constexpr std::array<RGBf32, 20> TestPointF32 = 
{{
    {0.f,     0.f,     0.f},          // Corner
    {1.f,     1.f,     1.f},          // Opposite corner
    {1.f,     0.f,     0.f},          // pure R
    {0.f,     1.f,     0.f},          // pure G
    {0.f,     0.f,     1.f},          // pure B
    {1.f,     1.f,     0.f},          // pure Yellow
    {1.f,     0.f,     1.f},          // pure Purple
    {0.1f,    0.5f,    0.9f},         // Random interior point 1
    {0.15f,   0.15f,   0.15f},        // Dark Gray
    {0.5f,    0.5f,    0.5f},         // Center point
    {0.75f,   0.75f,   0.75f},        // Brigth Gray
    {0.8f,    0.2f,    0.4f},         // Random interior point 2
    {0.8f,    0.21f,   0.4f},         // Random interior point 3
    {0.335f,  0.127f,  0.023f},       // Random interior point 4
    {0.997f,  0.782f,  0.901f},       // Random interior point 5
    {1.f/3.f, 1.f/7.f, 1.f/6.f},      // Random interior point 6
    {0.25f,   0.5f,    0.75f},        // Point between grid lines
    {0.251f,  0.51f,   0.751f},       // Point closely to grid lines
    {-0.1f,   0.5f,    1.1f},         // Point 1 outside bounds (will be clamped)
    {-0.01f,  0.62f,   1.01f}         // Point 2 outside bounds (will be clamped)
}};


// Test points
constexpr std::array<RGBf64, 20> TestPointF64 = 
{{
    {0.0,     0.0,     0.0},          // Corner
    {1.0,     1.0,     1.0},          // Opposite corner
    {1.0,     0.0,     0.0},          // pure R
    {0.0,     1.0,     0.0},          // pure G
    {0.0,     0.0,     1.0},          // pure B
    {1.0,     1.0,     0.0},          // pure Yellow
    {1.0,     0.0,     1.0},          // pure Purple
    {0.10,    0.50,    0.90},         // Random interior point 1
    {0.150,   0.150,   0.150},        // Dark Gray
    {0.50,    0.50,    0.50},         // Center point
    {0.750,   0.750,   0.750},        // Brigth Gray
    {0.80,    0.20,    0.40},         // Random interior point 2
    {0.80,    0.210,   0.40},         // Random interior point 3
    {0.3350,  0.1270,  0.0230},       // Random interior point 4
    {0.9970,  0.7820,  0.9010},       // Random interior point 5
    {1.0/3.0, 1.0/7.0, 1.0/6.0},      // Random interior point 6
    {0.250,   0.50,    0.750},        // Point between grid lines
    {0.2510,  0.510,   0.7510},       // Point closely to grid lines
    {-0.10,   0.50,    1.10},         // Point 1 outside bounds (will be clamped)
    {-0.010,  0.620,   1.010}         // Point 2 outside bounds (will be clamped)
}};



TEST (InterpolatorTest, Interpolator_MagicHour_f32)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    CCubeLut3D<float> lutFileF32;
    if (LutErrorCode::LutState::OK == lutFileF32.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<float>, LutElement::lutTableRaw<float>> lutDomain = lutFileF32.getMinMaxDomain();
        const LutElement::lutTable3D<float>  lutData = lutFileF32.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}


TEST (InterpolatorTest, Interpolator_MagicHour_f64)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    CCubeLut3D<double> lutFileF64;
    if (LutErrorCode::LutState::OK == lutFileF64.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<double>, LutElement::lutTableRaw<double>> lutDomain = lutFileF64.getMinMaxDomain();
        const LutElement::lutTable3D<double>  lutData = lutFileF64.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}


TEST (InterpolatorTest, Interpolator_Large65_f32)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/Large65.cube" };
    CCubeLut3D<float> lutFileF32;
    if (LutErrorCode::LutState::OK == lutFileF32.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<float>, LutElement::lutTableRaw<float>> lutDomain = lutFileF32.getMinMaxDomain();
        const LutElement::lutTable3D<float>  lutData = lutFileF32.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}


TEST (InterpolatorTest, Interpolator_Large65_f64)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/Large65.cube" };
    CCubeLut3D<double> lutFileF64;
    if (LutErrorCode::LutState::OK == lutFileF64.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<double>, LutElement::lutTableRaw<double>> lutDomain = lutFileF64.getMinMaxDomain();
        const LutElement::lutTable3D<double>  lutData = lutFileF64.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}


TEST (InterpolatorTest, Interpolator_FiLMiC_deHLG_V3_f32)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/FiLMiC_deHLG_V3.cube" };
    CCubeLut3D<float> lutFileF32;
    if (LutErrorCode::LutState::OK == lutFileF32.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<float>, LutElement::lutTableRaw<float>> lutDomain = lutFileF32.getMinMaxDomain();
        const LutElement::lutTable3D<float>  lutData = lutFileF32.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}


TEST (InterpolatorTest, Interpolator_FiLMiC_deHLG_V3_f64)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/FiLMiC_deHLG_V3.cube" };
    CCubeLut3D<double> lutFileF64;
    if (LutErrorCode::LutState::OK == lutFileF64.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<double>, LutElement::lutTableRaw<double>> lutDomain = lutFileF64.getMinMaxDomain();
        const LutElement::lutTable3D<double>  lutData = lutFileF64.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}

TEST (InterpolatorTest, Interpolator_Huge_negative_lut_128_f32)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/Huge_negative_lut_128.cube" };
    CCubeLut3D<float> lutFileF32;
    if (LutErrorCode::LutState::OK == lutFileF32.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<float>, LutElement::lutTableRaw<float>> lutDomain = lutFileF32.getMinMaxDomain();
        const LutElement::lutTable3D<float>  lutData = lutFileF32.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}


TEST (InterpolatorTest, Interpolator_Huge_negative_lut_128_f64)
{
    bool bResult = false;
    const std::string lutName{ dbgLutsFolder + "/Huge_negative_lut_128.cube" };
    CCubeLut3D<double> lutFileF64;
    if (LutErrorCode::LutState::OK == lutFileF64.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<double>, LutElement::lutTableRaw<double>> lutDomain = lutFileF64.getMinMaxDomain();
        const LutElement::lutTable3D<double>  lutData = lutFileF64.get_data();
        bResult = true;
    }
    EXPECT_EQ(bResult, true);
}



int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Parse from: " << dbgLutsFolder << std::endl;
    return RUN_ALL_TESTS();	
}
