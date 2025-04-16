#ifndef __LUTUTILS_STRING_VIEW_PORTING__
#define __LUTUTILS_STRING_VIEW_PORTING__

#if __cplusplus >= 201703L /* C++17 and later standards only features */

#include <string_view>

using string_view    = std::basic_string_view<char>;
using wstring_view   = std::basic_string_view<wchar_t>;
using u16string_view = std::basic_string_view<char16_t>;
using u32string_view = std::basic_string_view<char32_t>;

#else /* C++11 and C++14 standards */

#include <algorithm>
#include <string> 
#include <ostream>
#include <cstddef>

   
/* lets implement this class by itself */
namespace lututils_std
{ 

  template <typename CharT, typename Traits = std::char_traits<CharT>>
  class basic_string_view
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
    
      static constexpr size_type npos {static_cast<size_type>(-1)};
      
      /* default constructor */
      constexpr basic_string_view () noexcept : m_string {nullptr}, m_size{0} {};
      
      /* copy and move constructors */
      constexpr basic_string_view (const basic_string_view& other_strv) noexcept = default;
      constexpr basic_string_view (basic_string_view&& other_strv) noexcept = default;
      
      /* construct basic_string_view from ANSI string with zero termination */
      constexpr basic_string_view (const char_type* str) noexcept : m_string{str}, m_size{traits_type::length(str)} {};
      /* construct basic_string_view from ANSI string with given size */
      constexpr basic_string_view (const char_type* str, const size_type count) noexcept : m_string{str}, m_size{count} {};
      
      /* construct basic_string_view from std::string */
      template <typename tAllocator>
      basic_string_view (const std::basic_string<CharT,Traits,tAllocator>& std_str) noexcept : m_string{std_str.c_str()}, m_size{std_str.size()} {}
      
      /* assignment / copy / move operators */
      basic_string_view& operator=(const basic_string_view& other) = default;
      basic_string_view& operator=(basic_string_view&& other) noexcept {swap(other); return *this;}
      
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
         (throw std::out_of_range{"Position index is out of range in basic_string_view::at()"}), m_string[position];
      }
      constexpr const_reference front () const noexcept {return *m_string;}
      constexpr const_reference back  () const noexcept {return m_string[m_size - static_cast<size_type>(1)];}
      
      /* simple modifiers */
      void remove_prefix (size_type prefix_pos)     noexcept {m_string += prefix_pos; m_size += prefix_pos;}
      void remove_suffix (size_type suffix_pos)     noexcept {m_size -= suffix_pos;}
      void swap          (basic_string_view& other) noexcept {std::swap(m_string, other.m_string), std::swap(m_size, other.m_size);}
      
      /* sub-strings */
      basic_string_view substr (size_t position = 0, size_t length = npos) const
      {
        auto const max_length {position > m_size ? 0 : m_size - position};
        if (position > size()) {throw std::out_of_range("Position index is out of range in basic_string_view::substr()");}
        return basic_string_view (m_string + position, std::min(length, max_length));
      }

      /* compare API's */
      int compare (basic_string_view s) const noexcept
      {
          auto const rlen = std::min(m_size, s.m_size);
          auto const compare = Traits::compare (m_string, s.m_string, rlen);
          if (compare != 0) return compare;
          if (m_size < s.m_size) return -1;
          if (m_size > s.m_size) return 1;
          return 0;
      }
      
      int compare (size_type position, size_type count, basic_string_view stringw) const {return substr(position,count).compare(stringw);}

      int compare (size_type position1, size_type count1, basic_string_view str, size_type position2, size_type count2) const
      {
         return substr(position1,count1).compare(str.substr(position2,count2));
      }

      int compare (const CharT* str) const {return compare(basic_string_view<CharT,Traits>(str));}
      int compare (size_type position, size_type count, const CharT* str) const {return substr(position, count).compare(basic_string_view<CharT,Traits>(str));}
      
      int compare (size_type position, size_type count1, const CharT* str, size_type count2) const
      {
          return substr(position, count1).compare(basic_string_view<CharT,Traits>(str, count2));
      }

      template <typename tAllocator>
      int compare (const std::basic_string<CharT,Traits,tAllocator>& str) const {return compare(basic_string_view<CharT,Traits>(str));}


      /* Iterators */
      const_iterator begin () const noexcept {return m_string;}
      const_iterator cbegin() const noexcept {return begin();}
      const_iterator end   () const noexcept {return m_string + m_size;}
      const_iterator cend  () const noexcept {return end();}

      /* Reverse iterators */
      const_reverse_iterator rbegin () const noexcept {const_reverse_iterator{end()};}
      const_reverse_iterator crbegin() const noexcept {return rbegin();}
      const_reverse_iterator rend   () const noexcept {const_reverse_iterator{begin()};}
      const_reverse_iterator crend  () const noexcept {return rend();}


      template <typename tAllocator>
      explicit constexpr operator std::basic_string<CharT, Traits, tAllocator>() const;

    private:
      const char_type* m_string;
      size_type        m_size; 

  }; /* basic_string_view */


  /* conversion from std::string */
  template <typename CharT, typename Traits>
  template <class tAllocator>
  inline constexpr basic_string_view<CharT,Traits>::operator std::basic_string<CharT, Traits, tAllocator>() const {return std::basic_string<CharT,Traits,tAllocator>(m_string, m_size);}

  /* swap */
  template <typename CharT, typename Traits>
  inline void swap (basic_string_view<CharT,Traits>& l, basic_string_view<CharT,Traits>& r) noexcept {l.swap(r);}

  /* output stream output operator */
  template <typename CharT, typename Traits>
  inline std::basic_ostream<CharT,Traits>& operator << (std::basic_ostream<CharT,Traits>& o, const basic_string_view<CharT,Traits>& s)
  { o.write(s.m_string(),static_cast<std::streamsize>(s.m_size())); return o;}

  /* logical operators != */
  template <typename CharT, typename Traits>
  inline bool operator != (const basic_string_view<CharT,Traits>& l, const basic_string_view<CharT,Traits>& r) noexcept {return 0 != l.compare(r);}
      
  template <typename CharT, typename Traits>
  inline bool operator != (const basic_string_view<CharT,Traits>& l, const CharT* r) noexcept {return l != basic_string_view<CharT,Traits>(r);}

  template <typename CharT, typename Traits>
  inline bool operator != (const CharT* l, const basic_string_view<CharT,Traits>& r) noexcept {return basic_string_view<CharT,Traits>(l) != r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator != (const std::basic_string<CharT,Traits,Allocator>& l, const basic_string_view<CharT,Traits>& r) noexcept {return basic_string_view<CharT,Traits>(l) != r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator != (const basic_string_view<CharT,Traits>& l, const std::basic_string<CharT,Traits,Allocator>& r) noexcept {return l != basic_string_view<CharT,Traits>(r);}
 
  /* logical operators == */
  template <typename CharT, typename Traits>
  bool operator == (const basic_string_view<CharT,Traits>& l, const basic_string_view<CharT,Traits>& r) noexcept {return 0 == l.compare(r);}

  template <typename CharT, typename Traits>
  inline bool operator == (const basic_string_view<CharT,Traits> l, const CharT* r)  noexcept {return l == basic_string_view<CharT,Traits>(r);}

  template <typename CharT, typename Traits>
  inline bool operator == (const CharT* l, const basic_string_view<CharT,Traits> r) noexcept {return basic_string_view<CharT,Traits>(l) == r;}
	
  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator == (const std::basic_string<CharT,Traits,Allocator>& l, const basic_string_view<CharT,Traits>& r) noexcept {return basic_string_view<CharT,Traits>(l) == r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator == (const basic_string_view<CharT,Traits>& l, const std::basic_string<CharT,Traits,Allocator>& r) noexcept {return l == basic_string_view<CharT,Traits>(r);}
	
  /* logical operators < */  
  template <typename CharT, typename Traits>
  inline bool operator < (const basic_string_view<CharT,Traits>& l, const basic_string_view<CharT,Traits>& r) noexcept {return l.compare(r) < 0;}

  template <typename CharT, typename Traits>
  inline bool operator < (const basic_string_view<CharT,Traits>& l, const CharT* r) noexcept {return l < basic_string_view<CharT,Traits>(r);}

  template <typename CharT, typename Traits>
  inline bool operator < (const CharT* l, const basic_string_view<CharT,Traits>& r) noexcept {return  basic_string_view<CharT,Traits>(l) < r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator < (const std::basic_string<CharT,Traits,Allocator>& l, const basic_string_view<CharT,Traits>& r) {return basic_string_view<CharT,Traits>(l) < r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator < (const basic_string_view<CharT,Traits>& l, const std::basic_string<CharT,Traits,Allocator>& r) {return l < basic_string_view<CharT,Traits>(r);}

  /* logical operators > */  
  template <typename CharT, typename Traits>
  inline bool operator > (const basic_string_view<CharT,Traits>& l, const basic_string_view<CharT,Traits>& r) noexcept {return l.compare(r) > 0;}

  template <typename CharT, typename Traits>
  inline bool operator > (const basic_string_view<CharT,Traits>& l, const CharT* r) noexcept {return l > basic_string_view<CharT,Traits>(r);}

  template <typename CharT, typename Traits>
  inline bool operator > (const CharT* l, const basic_string_view<CharT,Traits>& r) noexcept {return  basic_string_view<CharT,Traits>(l) > r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator > (const std::basic_string<CharT,Traits,Allocator>& l, const basic_string_view<CharT,Traits>& r) {return basic_string_view<CharT,Traits>(l) > r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator > (const basic_string_view<CharT,Traits>& l, const std::basic_string<CharT,Traits,Allocator>& r) {return l > basic_string_view<CharT,Traits>(r);}

  /* logical operators <= */  
  template <typename CharT, typename Traits>
  inline bool operator <= (const basic_string_view<CharT,Traits>& l, const basic_string_view<CharT,Traits>& r) noexcept {return l.compare(r) <= 0;}

  template <typename CharT, typename Traits>
  inline bool operator <= (const basic_string_view<CharT,Traits>& l, const CharT* r) noexcept {return l <= basic_string_view<CharT,Traits>(r);}

  template <typename CharT, typename Traits>
  inline bool operator <= (const CharT* l, const basic_string_view<CharT,Traits>& r) noexcept {return  basic_string_view<CharT,Traits>(l) <= r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator <= (const std::basic_string<CharT,Traits,Allocator>& l, const basic_string_view<CharT,Traits>& r) {return basic_string_view<CharT,Traits>(l) <= r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator <= (const basic_string_view<CharT,Traits>& l, const std::basic_string<CharT,Traits,Allocator>& r) {return l <= basic_string_view<CharT,Traits>(r);}

  /* logical operators >= */  
  template <typename CharT, typename Traits>
  inline bool operator >= (const basic_string_view<CharT,Traits>& l, const basic_string_view<CharT,Traits>& r) noexcept {return l.compare(r) >= 0;}

  template <typename CharT, typename Traits>
  inline bool operator >= (const basic_string_view<CharT,Traits>& l, const CharT* r) noexcept {return l >= basic_string_view<CharT,Traits>(r);}

  template <typename CharT, typename Traits>
  inline bool operator >= (const CharT* l, const basic_string_view<CharT,Traits>& r) noexcept {return  basic_string_view<CharT,Traits>(l) >= r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator >= (const std::basic_string<CharT,Traits,Allocator>& l, const basic_string_view<CharT,Traits>& r) {return basic_string_view<CharT,Traits>(l) >= r;}

  template <typename CharT, typename Traits, typename Allocator>
  inline bool operator >= (const basic_string_view<CharT,Traits>& l, const std::basic_string<CharT,Traits,Allocator>& r) {return l >= basic_string_view<CharT,Traits>(r);}

      

} /* namespace lututils_std */

  using string_view    = lututils_std::basic_string_view<char>;
  using wstring_view   = lututils_std::basic_string_view<wchar_t>;
  using u16string_view = lututils_std::basic_string_view<char16_t>;
  using u32string_view = lututils_std::basic_string_view<char32_t>;

  constexpr string_view    operator ""_sv( const char*     str, std::size_t len ) noexcept {return string_view    {str, len};}
  constexpr wstring_view   operator ""_sv( const wchar_t*  str, std::size_t len ) noexcept {return wstring_view   {str, len};}
  constexpr u16string_view operator ""_sv( const char16_t* str, std::size_t len ) noexcept {return u16string_view {str, len};}
  constexpr u32string_view operator ""_sv( const char32_t* str, std::size_t len ) noexcept {return u32string_view {str, len};}


#endif /*  __cplusplus >= 201703L  */ 

#endif /* __LUTUTILS_STRING_VIEW_PORTING__ */
