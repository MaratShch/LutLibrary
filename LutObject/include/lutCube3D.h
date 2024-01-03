#ifndef __LUT_LIBRARY_LUT_CUBE_3D__
#define __LUT_LIBRARY_LUT_CUBE_3D__

#include <string>
#include "lutElement.h"
#include "lutErrors.h"

template <typename T>
class CCubeLut3D
{
public:
	CCubeLut3D(void) = default;
	virtual ~CCubeLut3D(void) = default;

private:
	size_t                     m_lutSize;
	LutElement::lutFileName    m_lutName;
	LutElement::lutTableRaw<T> m_domainMin;
	LutElement::lutTableRaw<T> m_domainMax;
	LutElement::lutTable3D<T>  m_lutBody;
};

#endif /* __LUT_LIBRARY_LUT_CUBE_1D__ */