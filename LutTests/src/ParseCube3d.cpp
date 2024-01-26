#include "gtest/gtest.h"
#include "lutCube3D.h"

#ifdef CUBE_3D_LUT_FOLDER
	const std::string dbgLutsFolder{ CUBE_3D_LUT_FOLDER };
#else
	const std::string dbgLutsFolder{ "LUT/CUBE/3D" };
#endif


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

	return RUN_ALL_TESTS();	
}
