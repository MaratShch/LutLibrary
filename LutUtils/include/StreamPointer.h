#ifndef __LUT_LIBRARY_UTILS_HUFFMAN_STREAM_POINTER_CLASS__
#define __LUT_LIBRARY_UTILS_HUFFMAN_STREAM_POINTER_CLASS__

#include <cstdint>
#include <iostream>

class CStreamPointer
{
public:
      /* default constructor */
      constexpr	CStreamPointer (void) noexcept : bit {0u}, byte {0u} {};
	  /* class constructors with parameters */	
      constexpr	CStreamPointer (const uint32_t& _bit,    const uint32_t& _byte) noexcept : bit {_bit}, byte {_byte} {};
	  constexpr	CStreamPointer (const uint64_t& _offset) noexcept : bit { static_cast<uint32_t>(_offset & 0x07u) }, byte{ static_cast<uint32_t>((_offset >> 32) & 0xFFFFFFFFu) } {};

      /* copy and move constructors */
      constexpr CStreamPointer (const CStreamPointer& other_sp) noexcept = default;
      constexpr CStreamPointer (CStreamPointer&& other_sp)      noexcept = default;

      /* class destructor */
      virtual ~CStreamPointer (void) {;}

      /* copy and move operators */
      CStreamPointer& operator= (const CStreamPointer& other) = default;
      CStreamPointer& operator= (CStreamPointer&& other) noexcept {swap(other); return *this;}

	  /* member access API's */	
	  uint32_t bits (void) const noexcept { return bit; }
	  uint32_t bytes(void) const noexcept { return byte; }
	  
	  const uint64_t get(void) const noexcept
	  {
		  return (static_cast<uint64_t>(byte) << 32 | static_cast<uint64_t>(bit));
	  }

	  void set (const uint32_t& byte_offset, const uint32_t& bits_offset) noexcept
	  { 
		  byte = byte_offset + (bits_offset / 8u);
		  bit  = bits_offset & 0x0000007u;  
	  }

	  void set (const uint64_t& new_offset) noexcept
	  { 
		  const uint32_t _byte_offset = static_cast<const uint32_t>((new_offset >> 32) & 0xFFFFFFFFu);
		  const uint32_t _bit_offset  = static_cast<const uint32_t>(new_offset & 0xFFFFFFFFu);
		  set (_byte_offset, _bit_offset);
	  }

	  void reset (void) noexcept { set(0u, 0u); }

      /* swap API */
      void swap (CStreamPointer& other) noexcept {std::swap(bit, other.bit), std::swap(byte, other.byte);}

      void forward (const uint64_t add_offset) {;}
      void forward (const uint32_t byte_offset, uint32_t bit_offset) {;}
	
	  /* Prefix increment operator */
	  CStreamPointer& operator ++ () noexcept
	  {
		  ++bit;
		  if (bit >= 0x8u)
		  {
			  bit = 0x0u;
			  ++byte;
		  }
		  return *this;
	  }

	  /* Postfix increment operator */
	  CStreamPointer& operator ++ (int) noexcept
	  {
		  CStreamPointer tmp = *this;
		  ++bit;
		  if (bit >= 0x8u)
		  {
			  bit = 0x0u;
			  ++byte;
		  }
		  return tmp;
	  }

	  /* Prefix decrement operator */
	  CStreamPointer& operator -- () noexcept
	  {
		  if (bit > 0x0u)
		  {
			  --bit;
		  }
		  else
		  {
			  bit = 0x7u;
			  --byte;
		  }
		  return *this;
	  }

	  /* Postfix decrement operator */
	  CStreamPointer& operator -- (int) noexcept
	  {
		  CStreamPointer tmp = *this;
		  if (bit > 0x0u)
		  {
			  --bit;
		  }
		  else
		  {
			  bit = 0x7u;
			  --byte;
		  }
		  return tmp;
	  }

	  int compare (const uint64_t& other_sp)  const noexcept
	  {
		  const uint64_t current = get();
		  if (current == other_sp) return 0;
		  else if (current < other_sp) return -1;
		  else return 1;
	  }

	  int compare (const CStreamPointer& other_sp) const noexcept { return compare (other_sp.get()); }


private:
	uint32_t bit;	/* offset in bits in current byte [valid value - 0...7]   */	 
	uint32_t byte;	/* offset in bytes from start of stream [zero enumerated] */	

friend inline std::ostream& operator << (std::ostream& os, const CStreamPointer& sp) noexcept {	os << sp.byte << "." << sp.bit; return os; }
friend inline std::istream& operator >> (std::istream& is, CStreamPointer& sp)       noexcept { is >> sp.byte; is >> sp.bit; return is; }

}; /* class CStreamPointer */


inline bool operator != (const CStreamPointer& l, const CStreamPointer& r) noexcept { return l.get() != r.get(); }
inline bool operator != (const CStreamPointer& l, const uint64_t& r)       noexcept { return l.get() != r;       }
inline bool operator != (const uint64_t& l, const CStreamPointer& r)       noexcept { return l       != r.get(); }

inline bool operator == (const CStreamPointer& l, const CStreamPointer& r) noexcept { return l.get() == r.get(); }
inline bool operator == (const CStreamPointer& l, const uint64_t& r)       noexcept { return l.get() == r;       }
inline bool operator == (const uint64_t& l, const CStreamPointer& r)       noexcept { return l       ==  r.get();}

inline bool operator <  (const CStreamPointer& l, const CStreamPointer& r) noexcept { return l.get() < r.get(); }
inline bool operator <  (const CStreamPointer& l, const uint64_t& r)       noexcept { return l.get() < r;       }
inline bool operator <  (const uint64_t& l, const CStreamPointer& r)       noexcept { return l       < r.get(); }

inline bool operator >  (const CStreamPointer& l, const CStreamPointer& r) noexcept { return l.get() > r.get(); }
inline bool operator >  (const CStreamPointer& l, const uint64_t& r)       noexcept { return l.get() > r;       }
inline bool operator >  (const uint64_t& l, const CStreamPointer& r)       noexcept { return l       > r.get(); }

inline bool operator <=  (const CStreamPointer& l, const CStreamPointer& r) noexcept { return l.get() <= r.get(); }
inline bool operator <=  (const CStreamPointer& l, const uint64_t& r)       noexcept { return l.get() <= r;       }
inline bool operator <=  (const uint64_t& l, const CStreamPointer& r)       noexcept { return l       <= r.get(); }

inline bool operator >=  (const CStreamPointer& l, const CStreamPointer& r) noexcept { return l.get() >= r.get(); }
inline bool operator >=  (const CStreamPointer& l, const uint64_t& r)       noexcept { return l.get() >= r;       }
inline bool operator >=  (const uint64_t& l, const CStreamPointer& r)       noexcept { return l       >= r.get(); }

#if 0
inline bool operator +=
inline bool operator +
inline bool operator -=
inline bool operator -


#endif

#endif /* __LUT_LIBRARY_UTILS_HUFFMAN_STREAM_POINTER_CLASS__ */
