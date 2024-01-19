#ifndef LUTUTILS_STRING_VIEW_PORTING
#define LUTUTILS_STRING_VIEW_PORTING

#if __cplusplus >= 201703L /* C++17 and later standards only features */

#include <string_view>

#else /* C++11 and C++14 standards */

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
      using char_type       = CharT;
      using traits_type     = Traits;
      using value_type      = CharT;
      using size_type       = std::size_t;

      using reference       = value_type&;
      using pointer         = value_type*;
      using const_reference = const value_type&;
      using const_pointer   = const value_type*;

      using iterator        = const CharT*;
      using const_iterator  = const CharT*;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
      static constexpr size_type npos = static_cast<size_type>(-1);
      
      /* default constructor */
      constexpr basic_string_view () noexcept {m_string = nullptr; m_size = 0;}
      
      /* copy and move constructors */
      constexpr basic_string_view (const basic_string_view& other_sw) noexcept = default;
      constexpr basic_string_view (basic_string_view&& other_sw) noexcept = default;
      
      /* construct basic_string_view from ANSI string with given size */
      constexpr basic_string_view (const char_type* ansi_str, size_type size) noexcept {m_string{ansi_str}, m_size{size};}
      /* construct basic_string_view from ANSI string with zero termination */
      constexpr basic_string_view (const char_type* ansi_str) noexcept {basic_string_view(ansi_str, traits_type::length(ansi_str));}
      
      /* construct basic_string_view from std::string */
      template <typename tAllocator>
      basic_string_view (const std::basic_string<CharT,Traits,tAllocator>& std_str) noexcept {m_string{std_str.c_str()}, m_size{std_str.size()};}
      
      /* capacity, size and size validation */
      constexpr size_type size()     const noexcept {return m_size;}
      constexpr size_type length()   const noexcept {return size();}
      constexpr size_type max_size() const noexcept {return (npos - static_cast<size_type>(1));}
      constexpr bool      empty()    const noexcept {return (static_cast<size_type>(0) == m_size);}
      
      /* string and string elements access */
      constexpr char_type* data() const noexcept {return m_string;}
      constexpr const_reference operator[] (const size_type position) const noexcept {return m_string[position];}
      constexpr const_reference at (const size_type position) const
     {
          return (position < m_size) ? m_string[position] : 
         (throw std::out_of_range{"Position is out of range in basic_string_view::at"}), m_string[position];
      }
      
      constexpr const_reference front () const noexcept {return *m_string;}
      constexpr const_reference back  () const noexcept {return m_string[m_size - static_cast<size_type>(1)]};
      
  //    void remove_prefix (size_type prefix_pos) noexcept {m_string += prefix_pos; m_size += prefix_pos;}
      
      
    private:
      const char_type* m_string;
      size_type        m_size; 

  }; /* basic_string_view */

} /* namespace lututils_std */

  using string_view    = lututils_std::basic_string_view<char>;
  using wstring_view   = lututils_std::basic_string_view<wchar_t>;
  using u16string_view = lututils_std::basic_string_view<char16_t>;
  using u32string_view = lututils_std::basic_string_view<char32_t>;


#endif /*  __cplusplus >= 201703L  */ 

#endif /* LUTUTILS_STRING_VIEW_PORTING */
