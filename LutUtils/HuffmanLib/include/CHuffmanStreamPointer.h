#ifndef __C_HUFFMAN_STREAM_POINTER_OBJECT_DEFINITION__
#define __C_HUFFMAN_STREAM_POINTER_OBJECT_DEFINITION__

#include <cstdint>
#include <iostream>
#include <utility>
#include <type_traits>

namespace HuffmanUtils
{

    class CStreamPointer final
    {
      public:
       // default constructor
          constexpr CStreamPointer (void) noexcept : m_StreamPointer {0ll} {};

      // class constructors with parameters	
          constexpr CStreamPointer (const int64_t& _offset) noexcept : m_StreamPointer { _offset > 0ll ? _offset : 0ll } {};

          constexpr CStreamPointer (const uint32_t& _byte, const uint32_t& _bit) noexcept : m_StreamPointer {(static_cast<const int64_t>(_byte) << 3) | static_cast<const int64_t>(0x7u & _bit)} {};

          explicit constexpr CStreamPointer (const std::pair<const uint32_t, const uint32_t>& _pair) noexcept : m_StreamPointer {(static_cast<const int64_t>(_pair.first) << 3) | (0x7u & _pair.second)} {};

       // copy and move constructors 
          constexpr CStreamPointer (const CStreamPointer& other_sp) noexcept : m_StreamPointer { other_sp.m_StreamPointer } {};
          constexpr CStreamPointer (CStreamPointer&& other_sp)      noexcept = default;

      // class destructor
          ~CStreamPointer (void) noexcept = default;

       // copy and move operators
          constexpr CStreamPointer& operator= (const std::pair<const uint32_t, const uint32_t>& other) noexcept
          {
              m_StreamPointer = ((static_cast<const int64_t>(other.first) << 3) | static_cast<const int64_t>(other.second & 0x07u));
              return *this;
          }

          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
          constexpr CStreamPointer& operator= (const T& other) noexcept
          {
              m_StreamPointer = (other > static_cast<T>(0) ? static_cast<const int64_t>(other) : 0ll);
              return *this;
          }

          constexpr CStreamPointer& operator= (const CStreamPointer& other) noexcept
          {
              if (this == &other)
                 return *this;
              m_StreamPointer = other.m_StreamPointer;
                 return *this;
          }

          CStreamPointer& operator= (CStreamPointer&& other) noexcept {swap(other); return *this;}

       // member access API
          constexpr int64_t get (void) const noexcept { return m_StreamPointer; }
          constexpr int64_t byte(void) const noexcept { return (get() >> 3); }
          constexpr int64_t bit (void) const noexcept { return (get() & 0x7);}
          const std::pair<const int64_t, const int64_t> byte_bit(void) const noexcept { return {byte(), bit()}; }
        
       // swap
          void swap (CStreamPointer& other) noexcept {std::swap(m_StreamPointer, other.m_StreamPointer);}

       // set / reset
           const int64_t set (int64_t newOffset) noexcept { const int64_t old = m_StreamPointer; m_StreamPointer = newOffset; return old; }
           const int64_t set (const CStreamPointer& newOffset) noexcept { return set(newOffset.m_StreamPointer); }
           const int64_t reset (void) noexcept { return set(0ll); }
           const int64_t set_byte(uint32_t byte) noexcept { const int64_t old = m_StreamPointer; m_StreamPointer = static_cast<int64_t>(byte) << 3; return old; }

       // forward / backward
          const int64_t forward  (int64_t add) noexcept { return (m_StreamPointer += add); }
          const int64_t backward (int64_t sub) noexcept { const int64_t _sub {sub}; return (_sub < m_StreamPointer ?  (m_StreamPointer -= _sub) : 0ll); }
          const int64_t step_forward  (void) noexcept { return m_StreamPointer++; }
          const int64_t step_backward (void) noexcept { return (m_StreamPointer > 0ll ? m_StreamPointer-- : 0ll); }
          const int64_t to_next_byte(void) noexcept   { return m_StreamPointer += 8ll; }
          const int64_t to_prev_byte(void) noexcept   { return m_StreamPointer < 8ll ? 0ll : m_StreamPointer - 8ll; }

       // distance
          constexpr int64_t distance (const int64_t v) const noexcept { return m_StreamPointer - v; }
          constexpr int64_t distance (const CStreamPointer& v) const noexcept { return distance (v.m_StreamPointer); }
      
       // compare
          const int compare (const int64_t& other_sp) const noexcept
          {
             if (m_StreamPointer == other_sp) return 0;
             else if (m_StreamPointer < other_sp) return -1;
             else return 1;
          }
 
          const int compare (const CStreamPointer& other_sp) const noexcept {return compare (other_sp.m_StreamPointer);}  

       // alignment 
          void align2byte (void) noexcept { (m_StreamPointer += 7ll) &= 0x7FFFFFFFFFFFFFF8; }


       // Typecast operators overloading to convert the object to an int64_t and uint64_t
          constexpr operator int64_t () const noexcept {return m_StreamPointer;}
          constexpr operator uint64_t() const noexcept {return static_cast<uint64_t>(m_StreamPointer);}

       // Prefix increment operator
	      CStreamPointer operator ++ () noexcept { ++m_StreamPointer; return *this;}
       // Postfix increment operator
	      CStreamPointer operator ++ (int) noexcept { CStreamPointer tmp(*this); step_forward(); return tmp;}

       // Prefix decrement operator
          CStreamPointer operator -- () noexcept {m_StreamPointer > 0ll ? --m_StreamPointer : 0ll; return *this;}
       // Postfix decrement operator
          CStreamPointer operator -- (int) noexcept {CStreamPointer tmp(*this); step_backward(); return tmp;}

       // operators +=
          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
	      inline CStreamPointer& operator += (const T& other) noexcept {forward(static_cast<int64_t>(other)); return *this;}
	      inline CStreamPointer& operator += (const CStreamPointer& other) noexcept {forward(other.m_StreamPointer); return *this;}
 
       // operators -=
          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
	      inline CStreamPointer& operator -= (const T& other) noexcept {backward(static_cast<int64_t>(other)); return *this;}
	      inline CStreamPointer& operator -= (const CStreamPointer& other) noexcept {backward(other.m_StreamPointer); return *this;}
   
       // operator +
          constexpr friend inline CStreamPointer operator + (const CStreamPointer& sp1, const CStreamPointer& sp2) noexcept
          { return CStreamPointer (sp1.m_StreamPointer + sp2.m_StreamPointer); }

          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
          constexpr friend inline CStreamPointer operator + (const CStreamPointer& sp1, const T& sp2) noexcept
          { return CStreamPointer (sp1.forward(sp2)); }

          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
          constexpr friend inline CStreamPointer operator + (const T& sp1, const CStreamPointer& sp2) noexcept
          { return CStreamPointer (sp2.forward(sp1)); }

       // operator -
          constexpr friend inline CStreamPointer operator - (const CStreamPointer& sp1, const CStreamPointer& sp2) noexcept
          {  
             return CStreamPointer (sp1.m_StreamPointer - sp2.m_StreamPointer > 0ll ? sp1.m_StreamPointer - sp2.m_StreamPointer : 0ll);
          }

          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
          constexpr friend inline CStreamPointer operator - (const CStreamPointer& sp1, const T& sp2) noexcept
          {  
             const int64_t diff = sp1.m_StreamPointer - static_cast<int64_t>(sp2);
             return CStreamPointer (diff > 0ll ? diff : 0ll);
         }

          template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
          constexpr friend inline CStreamPointer operator - (const T& sp1, const CStreamPointer& sp2) noexcept
          {  
             const int64_t diff = static_cast<int64_t>(sp1) - sp2.m_StreamPointer;
             return CStreamPointer (diff > 0ll ? diff : 0ll);
          }

       // out stream operator
  	      friend std::ostream& operator << (std::ostream& os, const CStreamPointer& sp) noexcept { os << (sp.byte()) << "." << (sp.bit()); return os; }

       // in stream operator 
	      friend inline std::istream& operator >> (std::istream& is, CStreamPointer& sp) noexcept { is >> sp.m_StreamPointer; return is; }


      private:
          int64_t m_StreamPointer;

    }; // class CStreamPointer


    // operator !=
    constexpr bool operator != (const CStreamPointer& l, const CStreamPointer& r) { return l.get() != r.get(); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator != (const CStreamPointer& l, const T& r) { return l.get() != static_cast<int64_t>(r); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator != (const T& l, const CStreamPointer& r) { return static_cast<int64_t>(l) != r.get(); }


    // operator ==
    constexpr bool operator == (const CStreamPointer& l, const CStreamPointer& r) { return l.get() == r.get(); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator == (const CStreamPointer& l, const T& r) { return l.get() == static_cast<int64_t>(r); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator == (const int64_t& l, const CStreamPointer& r) { return static_cast<int64_t>(l) ==  r.get();}


    // operator <
    constexpr bool operator <  (const CStreamPointer& l, const CStreamPointer& r) { return l.get() < r.get(); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator <  (const CStreamPointer& l, const T& r) { return l.get() < static_cast<int64_t>(r); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator <  (const T& l, const CStreamPointer& r) { return static_cast<int64_t>(l) < r.get(); }


    // operator >
    constexpr bool operator >  (const CStreamPointer& l, const CStreamPointer& r) { return l.get() > r.get(); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator >  (const CStreamPointer& l, const T& r) { return l.get() > static_cast<int64_t>(r); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator >  (const T& l, const CStreamPointer& r) { return static_cast<int64_t>(l) > r.get(); }


    // operator <=
    constexpr bool operator <=  (const CStreamPointer& l, const CStreamPointer& r) { return l.get() <= r.get(); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator <=  (const CStreamPointer& l, const T& r) { return l.get() <= static_cast<int64_t>(r); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator <=  (const int64_t& l, const CStreamPointer& r) { return static_cast<int64_t>(l) <= r.get(); }


    // operator >=
    constexpr bool operator >=  (const CStreamPointer& l, const CStreamPointer& r) { return l.get() >= r.get(); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator >=  (const CStreamPointer& l, const T& r) { return l.get() >= static_cast<int64_t>(r); }

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
    constexpr bool operator >=  (const T& l, const CStreamPointer& r) { return static_cast<int64_t>(l) >= r.get(); }

    // conversion API's (not class member)
    constexpr CStreamPointer byte2sp (const uint32_t& byte) noexcept { return CStreamPointer(byte, 0u); }
    constexpr int64_t sp2byte (const CStreamPointer& sp) noexcept { return sp.byte(); }

}; // namespace HuffmanUtils

#endif // __C_HUFFMAN_STREAM_POINTER_OBJECT_DEFINITION__
