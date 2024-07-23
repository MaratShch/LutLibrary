#ifndef __LUT_LIBRARY_DEFLATE_COMMON_ALGO_UTILS__
#define __LUT_LIBRARY_DEFLATE_COMMON_ALGO_UTILS__

#include <cstdint>
#include <vector>
#include <iostream>

class StreamPointer
{
public:
      /* default constructor */
      constexpr	StreamPointer (void) noexcept : bit {0u}, byte {0u} {};
      constexpr	StreamPointer (const uint32_t& _bit, const uint32_t& _byte) noexcept : bit {_bit}, byte {_byte} {};
      
      /* copy and move constructors */
      constexpr StreamPointer (const StreamPointer& other_sp) noexcept = default;
      constexpr StreamPointer (StreamPointer&& other_sp)      noexcept = default;

      /* class destructor */
      virtual ~StreamPointer (void) {;}

      /* copy and move operators */
      StreamPointer& operator=(const StreamPointer& other) = default;
      StreamPointer& operator=(StreamPointer&& other) noexcept {swap(other); return *this;}

      /* swap API */
      void swap (StreamPointer& other) noexcept {std::swap(bit, other.bit), std::swap(byte, other.byte);}

      void forward (const uint64_t add_offset) {;}
      void set (const uint64_t new_offset) {;}	
      void forward (const uint32_t bit_offset, uint32_t byte_offset) {;}
      void set (const uint32_t bit_offset, uint32_t byte_offset) {;}
      const uint64_t get () {return 0ul;}
      void reset () {bit = byte = 0u;}
		
private:
	uint32_t bit;
	uint32_t byte;	

friend inline std::ostream& operator << (std::ostream& os, const StreamPointer& sp) noexcept;
friend inline std::istream& operator >> (std::istream& in, StreamPointer& sp) noexcept;

};

/* conversion from uint64_t [high 32 bits - byte offset. low 32 bits - bit offset] */
/* conversion from uint32_t [byte offset] */

inline bool operator != (const StreamPointer& l, const StreamPointer& r) noexcept {return false;}
inline bool operator == (const StreamPointer& l, const StreamPointer& r) noexcept {return false;}
inline bool operator <  (const StreamPointer& l, const StreamPointer& r) noexcept {return false;}
inline bool operator >  (const StreamPointer& l, const StreamPointer& r) noexcept {return false;}
inline bool operator <= (const StreamPointer& l, const StreamPointer& r) noexcept {return false;}
inline bool operator >= (const StreamPointer& l, const StreamPointer& r) noexcept {return false;}

inline std::ostream& operator << (std::ostream& os, const StreamPointer& sp) noexcept { os << sp.byte << "." << sp.bit; return os; }
inline std::istream& operator >> (std::istream& is, StreamPointer& sp) noexcept { is >> sp.byte >> sp.bit; return is; }

#if 0
inline bool operator ++
inline bool operator +=
inline bool operator +
inline bool operator --
inline bool operator -=
inline bool operator -


#endif

#endif /* __LUT_LIBRARY_DEFLATE_COMMON_ALGO_UTILS__ */
