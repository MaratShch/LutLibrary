#ifndef __LUT_LIBRARY_LUT_CUBE_1D__
#define __LUT_LIBRARY_LUT_CUBE_1D__

#include <string>
#include "lutElement.h"
#include "lutErrors.h"

template <typename T, typename = std::enable_if<std::is_floating_point<T>::value>::type>
class CCubeLut1D
{
public:
	
private:
	size_t                     m_lutSize;
	LutElement::lutFileName    m_lutName;
	LutElement::lutTableRaw<T> m_domainMin;
	LutElement::lutTableRaw<T> m_domainMax;
	LutElement::lutTable3D<T>  m_lutBody;

};

#endif /* __LUT_LIBRARY_LUT_CUBE_1D__ */