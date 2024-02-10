#ifndef __LUT_LIBRARY_LUT_ELEMENT__
#define __LUT_LIBRARY_LUT_ELEMENT__

#include <vector>
#include <string>

namespace LutElement
{
	template <typename T>
	using lutTableRaw = std::vector<T>;

	template <typename T>
	using lutTable1D = std::vector<lutTableRaw<T>>;

	template <typename T>
	using lutTable2D = std::vector<lutTable1D<T>>;

	template <typename T>
	using lutTable3D = std::vector<lutTable2D<T>>;

	using lutFileName = std::string;
	using lutTitle    = std::string;
	using lutSize     = size_t;
	
	enum class LutComponent
	{
		Red = 0,
		Green,
		Blue
	};
}

#endif /* __LUT_LIBRARY_LUT_ELEMENT__ */
