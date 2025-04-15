#include "gtest/gtest.h"
#include "lutCube3D.h"
#include "lutInterpolator.hpp"
#include <array>
#include <iomanip>  // Required for std::setprecision

// Computation tolerance
constexpr float tolerance = 1e-7f;
constexpr int32_t print_precision = 7;

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };

using RGBf32 = std::array<float, 3>;

template <typename T>
constexpr bool Equal (const T& a, const T& b, const T& tolerance) noexcept
{
    return std::abs(a - b) <= tolerance;
}


// Test points
constexpr std::array<RGBf32, 19> TestPointF32 = 
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
    {0.25f,   0.5f,    0.75f},        // Point between grid lines
    {0.251f,  0.51f,   0.751f},       // Point closely to grid lines
    {-0.1f,   0.5f,    1.1f},         // Point 1 outside bounds (will be clamped)
    {-0.01f,  0.62f,   1.01f}         // Point 2 outside bounds (will be clamped)
}};


// Test points
constexpr std::array<RGBf32, 20> ReferencePointsLerpF32 =
{{
    { 0.0000000000f, 0.0000010000f, 0.0000010000f },          // Corner
    { 0.9999960065f, 0.9999939799f, 0.9999939799f },          // Opposite corner
    { 1.0000000000f, 0.0000000000f, 0.0000000000f },          // pure R
    { 0.0000000000f, 1.0000000000f, 0.0000000000f },          // pure G
    { 0.0000000000f, 0.0000000000f, 1.0000000000f },          // pure B
    { 1.0000000000f, 0.9914140105f, 0.0000000000f },          // pure Yellow
    { 0.9417989850f, 0.0000000000f, 0.8989850283f },          // pure Purple
    { 0.0000000000f, 0.4641344249f, 1.0000000000f },          // Random interior point 1
    { 0.0491930023f, 0.0462410003f, 0.0525178015f },          // Dark Gray
    { 0.4922719896f, 0.4131399989f, 0.4136880040f },          // Center point
    { 0.6837720275f, 0.6130499840f, 0.6170650125f },          // Brigth Gray
    { 1.0000000000f, 0.0000000000f, 0.2721128166f },          // Random interior point 2
    { 1.0000000000f, 0.0000000000f, 0.2662071884f },          // Random interior point 3
    { 0.3127563596f, 0.0015338401f, 0.0000000000f },          // Random interior point 4
    { 1.0000000000f, 0.6411124468f, 0.8490945101f },          // Random interior point 5
    { 0.0000000000f, 0.4282320142f, 0.9751449823f },          // Point between grid lines
    { 0.0000000000f, 0.4278494120f, 0.9744875431f },          // Point closely to grid lines
    { 0.0000000000f, 0.5030949712f, 1.0000000000f },          // Point 1 outside bounds (will be clamped)
    { 0.0000000000f, 0.6639140248f, 1.0000000000f }           // Point 2 outside bounds (will be clamped)
}};




TEST (InterpolatorTest, Interpolator_Linear_MagicHour_f32)
{
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    int bResult = 0;
    CCubeLut3D<float> lutFileF32;

    if (LutErrorCode::LutState::OK == lutFileF32.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<float>, LutElement::lutTableRaw<float>> lutDomain = lutFileF32.getMinMaxDomain();
        const LutElement::lutTable3D<float> lutData = lutFileF32.get_data();
        
        std::cout << std::fixed << std::showpoint << std::setprecision(print_precision);

        constexpr size_t checkPoints = TestPointF32.size();
        for (size_t i = 0; i < checkPoints; i++)
        {
           const RGBf32 rgb = TestPointF32[i];
           const RGBf32 expected_rgb = ReferencePointsLerpF32[i];

           LutElement::lutTableRaw<float> out = linear_interpolation (lutData, rgb[0], rgb[1], rgb[2], lutDomain.first, lutDomain.second);

           if (
                (true != Equal(out[0], expected_rgb[0], tolerance)) || 
                (true != Equal(out[1], expected_rgb[1], tolerance)) ||
                (true != Equal(out[2], expected_rgb[2], tolerance))
              )
           { 
               std::cout << "LERP Checkpoint " << i << " has a mismatch in result: [tolerance = "  << tolerance << "]" << std::endl; 
               std::cout << "INPUT:    R = " << rgb[0]          << " G = " << rgb[1]          << " B = " << rgb[2]          << std::endl;
               std::cout << "OUTPUT:   R = " << out[0]          << " G = " << out[1]          << " B = " << out[2]          << std::endl;
               std::cout << "EXPECTED: R = " << expected_rgb[0] << " G = " << expected_rgb[1] << " B = " << expected_rgb[2] << std::endl;
               std::cout << "DELTA:    R = " << std::abs(expected_rgb[0] - out[0])
                         <<          " G = " << std::abs(expected_rgb[1] - out[1])
                         <<          " B = " << std::abs(expected_rgb[2] - out[2]) << std::endl;
               std::cout << "========================================================" << std::endl;
               bResult++;
           }
        }
    }
    EXPECT_EQ(bResult, 0);
}



int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Parse from: " << dbgLutsFolder << std::endl;
#ifdef WITH_HIGH_ACCURACY_MODE
    std::cout << "Compilation with High Accuracy mode" << std::endl;
#endif
    return RUN_ALL_TESTS();	
}
