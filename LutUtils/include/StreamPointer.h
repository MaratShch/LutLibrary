#ifndef __LUT_LIBRARY_UTILS_HUFFMAN_STREAM_POINTER_CLASS__
#define __LUT_LIBRARY_UTILS_HUFFMAN_STREAM_POINTER_CLASS__

#include <cstdint>
#include <iostream>


inline constexpr uint64_t make_stream_pointer_offset (const uint32_t& byte, const uint32_t& bit) noexcept
{
     return ((static_cast<uint64_t>(byte) << 32) | static_cast<uint64_t>(bit)); 
}

inline constexpr uint64_t make_absolute_bits_offset (const uint32_t& byte, const uint32_t& bit) noexcept
{
     return ((static_cast<uint64_t>(byte) << 3) + static_cast<uint64_t>(bit)); 
}


class CStreamPointer
{
/* public members and methods */
public:
      /* default constructor */
      constexpr	CStreamPointer (void) noexcept : bit {0u}, byte {0u} {};
      /* class constructors with parameters */	
      constexpr	CStreamPointer (const uint64_t& _offset) noexcept : bit { static_cast<uint32_t>(_offset & 0x07u) }, byte{ static_cast<uint32_t>((_offset >> 32) & 0xFFFFFFFFu) } {};
      constexpr	CStreamPointer (const uint32_t& _byte, const uint32_t& _bit) noexcept : byte {_byte}, bit {_bit}  {};
 
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

     void forward (const uint64_t add_offset){;}
     void forward (const uint32_t byte_offset, uint32_t bit_offset) {;}
	
	  /* Prefix increment operator */
	  CStreamPointer operator ++ () noexcept
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
	  CStreamPointer operator ++ (int) noexcept
	  {
		  CStreamPointer tmp(byte, bit);
		  ++bit;
		  if (bit >= 0x8u)
		  {
			  bit = 0x0u;
			  ++byte;
		  }
		  return tmp;
	  }

	  /* Prefix decrement operator */
	  CStreamPointer operator -- () noexcept
	  {
		  if (bit > 0x0u)
		  {
			  --bit;
		  }
		  else
		  {
			if (0u != byte) {bit = 0x7u; --byte;}
		  }
		  return *this;
	  }

	  /* Postfix decrement operator */
	  CStreamPointer operator -- (int) noexcept
	  {
		  CStreamPointer tmp(byte, bit);
		  if (bit > 0x0u)
		  {
			  --bit;
		  }
		  else
		  {
			if (0u != byte) {bit = 0x7u; --byte;}
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

	  inline CStreamPointer& operator += (const uint64_t& other) noexcept
	  {
		  auto const bits_sum  = bit  + static_cast<uint32_t>(other & 0x07u);
		  auto const bytes_sum = byte + static_cast<uint32_t>(other >> 32);
		  byte = (bytes_sum + bits_sum / 8);
		  bit  = bits_sum & 0x07u;
		  return *this;
	  }

	  inline CStreamPointer& operator += (CStreamPointer other) noexcept
	  {
		  auto const bits_sum  = bit  + other.bit;
		  auto const bytes_sum = byte + other.byte;
		  byte = (bytes_sum + bits_sum / 8);
		  bit  = bits_sum & 0x07u;
		  return *this;
	  }

	  inline CStreamPointer& operator -= (const uint64_t& other) noexcept
	  {
                  const int64_t currn_bits_offset = _absolute_bits_offset();
                  const int64_t other_byte_offset = _absolute_bits_offset(other);
		  const int64_t tmp_sub = currn_bits_offset - other_byte_offset;
		  const int64_t sub = (tmp_sub < 0ll ? 0ll : tmp_sub);                  
                  bit  = static_cast<uint32_t>(sub & 0x07);
		  byte = static_cast<uint32_t>((sub >> 3) & 0xFFFFFFFF);
		  return *this;
	  }

	  inline CStreamPointer& operator -= (CStreamPointer other) noexcept
	  {
		  return ((*this) -= other.get());
	  }

          uint64_t absolute_bits_offset (void) const noexcept {return _absolute_bits_offset();}	 

/* private members and methods */
private:
	uint32_t bit;	/* offset in bits in current byte [valid value - 0...7]   */	 
	uint32_t byte;	/* offset in bytes from start of stream [zero enumerated] */	

	constexpr int64_t _absolute_bits_offset (void)                                      const noexcept {return (static_cast<int64_t>(this->byte) << 3) + static_cast<int64_t>(this->bit);}
	constexpr int64_t _absolute_bits_offset (const uint32_t& byte, const uint32_t& bit) const noexcept {return (static_cast<int64_t>(byte) << 3) + static_cast<int64_t>(bit);}
        constexpr int64_t _absolute_bits_offset (const uint64_t& offset)                    const noexcept {return ((offset >> 29) | (offset & 0x07u));}

/* friends */
	friend std::ostream& operator << (std::ostream& os, const CStreamPointer& sp)  noexcept { os << sp.byte << "." << sp.bit; return os; }
	friend inline std::istream& operator >> (std::istream& is, CStreamPointer& sp) noexcept { is >> sp.byte; is >> sp.bit; return is; }

	friend inline CStreamPointer operator + (const CStreamPointer& sp1, const CStreamPointer& sp2) noexcept
	{
		auto const bit_sum = sp1.bit + sp2.bit;
		auto const byte_sum= sp1.byte+ sp2.byte + bit_sum / 8;
		return CStreamPointer{byte_sum, bit_sum & 0x07u};
	}

	friend inline CStreamPointer operator + (const CStreamPointer& sp1, const uint64_t& sp2) noexcept
	{
		auto const bit_sum = sp1.bit + static_cast<uint32_t>(sp2 & 0x07u);
		auto const byte_sum= sp1.byte+ static_cast<uint32_t>(sp2 >> 32) + bit_sum / 8;
		return CStreamPointer{byte_sum, bit_sum & 0x07u};
	}

	friend inline CStreamPointer operator + (const uint64_t& sp1, const CStreamPointer& sp2) noexcept
	{
		auto const bit_sum = sp2.bit + static_cast<uint32_t>(sp1 & 0x07u);
		auto const byte_sum= sp2.byte+ static_cast<uint32_t>(sp1 >> 32) + bit_sum / 8;
		return CStreamPointer{byte_sum, bit_sum & 0x07u};
	}

	friend inline CStreamPointer operator - (const CStreamPointer& sp1, const CStreamPointer& sp2) noexcept
	{
                const int64_t sp1_bits_abs_offset = sp1.absolute_bits_offset();
                const int64_t sp2_bits_abs_offset = sp2.absolute_bits_offset();
		const int64_t tmp_sub = sp1_bits_abs_offset - sp2_bits_abs_offset;
                const uint64_t newVal = static_cast<const uint64_t>(tmp_sub < 0ll ? 0ll : tmp_sub);
		return CStreamPointer{static_cast<uint32_t>((newVal >> 3) & 0xFFFFFFFFu), static_cast<uint32_t>(newVal & 0x07u)};
	}

	friend inline CStreamPointer operator - (const CStreamPointer& sp1, const uint64_t& sp2) noexcept
	{
                const int64_t sp1_bits_abs_offset = sp1.absolute_bits_offset();
 		const int64_t tmp_sub = sp1_bits_abs_offset - static_cast<int64_t>(sp2);
                const uint64_t newVal = static_cast<const uint64_t>(tmp_sub < 0ll ? 0ll : tmp_sub);
		return CStreamPointer{static_cast<uint32_t>((newVal >> 3) & 0xFFFFFFFFu), static_cast<uint32_t>(newVal & 0x07u)};
	}

	friend inline CStreamPointer operator - (const uint64_t& sp1, const CStreamPointer& sp2) noexcept
	{
		const int64_t tmp_sub = sp1 - sp2.absolute_bits_offset();
                const uint64_t newVal = static_cast<const uint64_t>(tmp_sub < 0ll ? 0ll : tmp_sub);
		return CStreamPointer{static_cast<uint32_t>((newVal >> 3) & 0xFFFFFFFFu), static_cast<uint32_t>(newVal & 0x07u)};
	}


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


#endif /* __LUT_LIBRARY_UTILS_HUFFMAN_STREAM_POINTER_CLASS__ */
