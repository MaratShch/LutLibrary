#include "gtest/gtest.h"
#include "lutCube3D.h"

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };


TEST (SaveCube3d, Parse_Tiny_Cube3D_f32)
{
	const std::string lutName{ dbgLutsFolder + "/Tiny.cube" };
	const std::string savedLutName{ dbgLutsFolder + "/Tiny_saved.cube" };
	CCubeLut3D<float> lutFileF32;
	auto const result  = lutFileF32.LoadFile(lutName);
	auto const lutSize = lutFileF32.getLutSize();
	GTEST_EXPECT_TRUE(lutSize == 2);
	GTEST_EXPECT_TRUE(result == LutErrorCode::LutState::OK);
	auto const saveResult = lutFileF32.SaveFile (savedLutName);
	EXPECT_EQ(saveResult, LutErrorCode::LutState::OK);
}



int main (int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Parse from: " << dbgLutsFolder << std::endl;
    return RUN_ALL_TESTS();	
}
