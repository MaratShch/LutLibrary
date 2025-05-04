#include "gtest/gtest.h"
#include "lutCube3D.h"
#include "lutInterpolator.hpp"
#include <array>
#include <iomanip>  // Required for std::setprecision

// Computation tolerance
// Because we use AVX2/AVX512 and FMA instructtions - let's temporary increase tolerance for compare
constexpr double tolerance = 1e-10;
//constexpr double tolerance = 1e-12; // investigate later

constexpr int32_t print_precision = 16;

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };

using RGBf64 = std::array<double, 3>;

template <typename T>
constexpr bool Equal (const T& a, const T& b, const T& tolerance) noexcept
{
    return std::abs(a - b) <= tolerance;
}

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
    {0.250,   0.50,    0.750},        // Point between grid lines
    {0.2510,  0.510,   0.7510},       // Point closely to grid lines
    {-0.10,   0.50,    1.10},         // Point 1 outside bounds (will be clamped)
    {-0.010,  0.620,   1.010},        // Point 2 outside bounds (will be clamped)
    { 0.4791666666666666, 0.40, 0.70 }// Make a lot of digits in output
}};


// Test points
constexpr std::array<RGBf64, 20> ReferencePointsLerpF64 =
{{
    { 0.0000000000000000, 0.0000010000000000, 0.0000010000000000 },          // Corner
    { 0.9999960000000000, 0.9999940000000000, 0.9999940000000000 },          // Opposite corner
    { 1.0000000000000000, 0.0000000000000000, 0.0000000000000000 },          // pure R
    { 0.0000000000000000, 1.0000000000000000, 0.0000000000000000 },          // pure G
    { 0.0000000000000000, 0.0000000000000000, 1.0000000000000000 },          // pure B
    { 1.0000000000000000, 0.9914140000000000, 0.0000000000000000 },          // pure Yellow
    { 0.9417990000000001, 0.0000000000000000, 0.8989850000000000 },          // pure Purple
    { 0.0000000000000000, 0.4641344000000001, 1.0000000000000000 },          // Random interior point 1
    { 0.0491930000000000, 0.0462410000000000, 0.0525178000000000 },          // Dark Gray
    { 0.4922720000000000, 0.4131400000000000, 0.4136880000000000 },          // Center point
    { 0.6837720000000000, 0.6130500000000000, 0.6170650000000000 },          // Brigth Gray
    { 1.0000000000000000, 0.0000000000000000, 0.2721128000000000 },          // Random interior point 2
    { 1.0000000000000000, 0.0000000000000000, 0.2662072000000000 },          // Random interior point 3
    { 0.3127563600000001, 0.0015338400000000, 0.0000000000000000 },          // Random interior point 4
    { 1.0000000000000000, 0.6411124240000000, 0.8490945120000000 },          // Random interior point 5
    { 0.0000000000000000, 0.4282320000000000, 0.9751450000000000 },          // Point between grid lines
    { 0.0000000000000000, 0.4278494080000000, 0.9744875600000000 },          // Point closely to grid lines
    { 0.0000000000000000, 0.5030950000000000, 1.0000000000000000 },          // Point 1 outside bounds (will be clamped)
    { 0.0000000000000000, 0.6639140000000000, 1.0000000000000000 },          // Point 2 outside bounds (will be clamped)
    { 0.4044693333333331, 0.2398833333333334, 0.7342823333333335 }           // Make a lot of digits in output  
}};




TEST (InterpolatorTest, Interpolator_Linear_MagicHour_f64)
{
    const std::string lutName{ dbgLutsFolder + "/MagicHour.cube" };
    int bResult = 0;
    CCubeLut3D<double> lutFileF64;

    if (LutErrorCode::LutState::OK == lutFileF64.LoadFile(lutName))
    {
        const std::pair<LutElement::lutTableRaw<double>, LutElement::lutTableRaw<double>> lutDomain = lutFileF64.getMinMaxDomain();
        const LutElement::lutTable3D<double> lutData = lutFileF64.get_data();
        
        std::cout << std::fixed << std::showpoint << std::setprecision(print_precision);

        constexpr size_t checkPoints = TestPointF64.size();
        for (size_t i = 0; i < checkPoints; i++)
        {
           const RGBf64 rgb = TestPointF64[i];
           const RGBf64 expected_rgb = ReferencePointsLerpF64[i];

           LutElement::lutTableRaw<double> out = Interpolator3D::linear_interpolation (lutData, rgb[0], rgb[1], rgb[2], lutDomain.first, lutDomain.second);

           if (
                (true != Equal(out[0], expected_rgb[0], tolerance)) || 
                (true != Equal(out[1], expected_rgb[1], tolerance)) ||
                (true != Equal(out[2], expected_rgb[2], tolerance))
              )
           { 
               std::cout << "LERP Checkpoint " << i << " has a mismatch in result: [tolerance = "  << tolerance << "]" << std::endl; 
               std::cout << std::fixed << std::showpoint << std::setprecision(16) << "INPUT:    R = " << rgb[0]          << " G = " << rgb[1]          << " B = " << rgb[2]          << std::endl;
               std::cout << std::fixed << std::showpoint << std::setprecision(16) << "OUTPUT:   R = " << out[0]          << " G = " << out[1]          << " B = " << out[2]          << std::endl;
               std::cout << std::fixed << std::showpoint << std::setprecision(16) << "EXPECTED: R = " << expected_rgb[0] << " G = " << expected_rgb[1] << " B = " << expected_rgb[2] << std::endl;
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
