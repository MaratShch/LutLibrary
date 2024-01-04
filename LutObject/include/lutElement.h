#ifndef __LUT_LIBRARY_LUT_ELEMENT__
#define __LUT_LIBRARY_LUT_ELEMENT__

#include <vector>
#include <string>

template <typename T>
struct tElement
{
	T r; /* RED	  */
	T g; /* GREEN */
	T b; /* BLUE  */
};

using fElement  = tElement<float>;
using dElement  = tElement<double>;
using ldElement = tElement<long double>;

namespace LutElement
{
	template <typename T>
	using lutTableRaw = std::vector<tElement<T>>;

	template <typename T>
	using lutTable1D = std::vector<lutTableRaw<T>>;

	template <typename T>
	using lutTable2D = std::vector<lutTable1D<T>>;

	template <typename T>
	using lutTable3D = std::vector<lutTable2D<T>>;

	using lutFileName = std::string;
}

#endif /* __LUT_LIBRARY_LUT_ELEMENT__ */
