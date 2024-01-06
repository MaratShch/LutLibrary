#include "lutCube3D.h"

#ifdef CUBE_3D_LUT_FOLDER
	const std::string dbgLutsFolder{ CUBE_3D_LUT_FOLDER };
#else
	const std::string dbgLutsFolder{ "LUT/CUBE/3D" };
#endif

/* basic check & DBG */
int main(void)
{
	std::cout << "Start parse CUBE 3D lut " << dbgLutsFolder << std::endl;

	CCubeLut3D<float>  fLut3D;
	CCubeLut3D<double> dLut3D;
	CCubeLut3D<long double> ldLut3D;

	const std::string tinyLut = dbgLutsFolder + "/" + "Tiny_with_Blob.cube";
	std::cout << "LUT file name for DEBUG: " << tinyLut << std::endl;

	fLut3D.LoadCubeFile (tinyLut);
	dLut3D.LoadCubeFile (tinyLut);
	ldLut3D.LoadCubeFile(tinyLut);

	return 0;
}
