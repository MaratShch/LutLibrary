#ifndef __LUT_LIBRARY_ENDIAN_CHANGE_UTILS__
#define __LUT_LIBRARY_ENDIAN_CHANGE_UTILS__

#include <type_traits>

template<typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
inline T endian_convert (const T& val) noexcept
{
	constexpr size_t bytes = sizeof(val);
	if (1u != bytes)
	{
		T out_val {0};
		for (size_t i = 0u; i < bytes; i++)
			out_val |= (((val >> (i * 8)) & 0xFF) << ((bytes - 1 - i) * 8));
		return out_val;
	}
	return val;
}
	
#endif /* __LUT_LIBRARY_ENDIAN_CHANGE_UTILS__ */ 