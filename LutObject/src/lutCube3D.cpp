#include "lutCube3D.h"

void tst (void)
{
	CCubeLut3D<float>  fLut3D;
	CCubeLut3D<double> dLut3D;
	CCubeLut3D<long double> ldLut3D;

	fLut3D.LoadCubeFile("C:\\TEMP\\lut.cube");
	dLut3D.LoadCubeFile("C:\\TEMP\\lut.cube");
	ldLut3D.LoadCubeFile("C:\\TEMP\\lut.cube");

	return;
}
