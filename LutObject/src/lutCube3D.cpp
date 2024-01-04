#include "lutCube3D.h"

void tst (void)
{
	CCubeLut3D<float>  fLut3D;
	CCubeLut3D<double> dLut3D;

	fLut3D.LoadCubeFile("C:\\TEMP\\lut.cube");

	return;
}
