#ifndef LUTUTILS_STRING_VIEW_PORTING
#define LUTUTILS_STRING_VIEW_PORTING

#if __cplusplus >= 201703L /* C++17 and later standards only features */
#include <string_view>
using StringView = std::basic_string_view;
#else

#include <string> 
#include <ostream>
#include <cstddef>
    
/* lets implement this class by itself */
namespace lututils_std
{ 

  template <typename CharT, typename Traits = std::char_traits<CharT>>
  class basic_string_view final
  {
    public:
      using char_type   = CharT;
      using traits_type = Traits;
      using size_type   = std::size_t;

    private:
      const char_type* m_str;
      size_type        m_size; 
  }; /* basic_string_view */

} /* namespace lututils_std */

template <typename T>
using StringView = lututils_std::basic_string_view<T>;

#endif

#endif /* LUTUTILS_STRING_VIEW_PORTING */
