#include "gtest/gtest.h"
#include "lutCube3D.h"

const std::string dbgLutsFolder = { CUBE_3D_LUT_FOLDER };


TEST (Parse_Tiny_Cube3D, help_option)
{
   int result = 0;
   EXPECT_EQ(result, 0);	
}


TEST (Parse_Small_Cube3D, help_option)
{
   int result = 0;
   EXPECT_EQ(result, 0);	
}


int main (int argc, char** argv)
{
    	::testing::InitGoogleTest(&argc, argv);

	std::cout << "Parse from: " << dbgLutsFolder << std::endl;
	return RUN_ALL_TESTS();	
}
