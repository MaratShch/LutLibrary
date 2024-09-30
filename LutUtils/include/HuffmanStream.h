#ifndef __LUT_LIBRARY_HUFFMAN_ENCODE_STREAM__
#define __LUT_LIBRARY_HUFFMAN_ENCODE_STREAM__

#include <type_traits>
#include <cstdint>


class CHuffmanStream
{

public:

 CHuffmanStream (void)
 {
  pStream = nullptr;
  streamSize = streamOffset = 0ull;
 }
 CHuffmanStream(const void* ptr, const size_t& bits_size)
 {
   pStream = static_cast<uint8_t*>(ptr);
   streamSize = bits_size;
   streamOffset = 0ull;
 }
 
 CHuffmanStream (void* ptr, const uint32_t& bytes, const uint32_t& bits)
 {
   pStream = static_cast<uint8_t*>(ptr);
   streamSize = static_cast<size_t>(bytes * 8u) + bits;
   streamOffset = 0ull;
 }

 bool isValid (void) const noexcept
 {
   return (nullptr != pStream && 0ull != streamSize);
 }
 
 const uint8_t readBit(void)
 {
	 const uint32_t byte = static_cast<uint32_t>(streamOffset >> 3);
	 const uint32_t bit  = static_cast<uint32_t>(streamOffset % 8ull);
	 streamOffset++;
	 return static_cast<uint8_t>(0x01u) & (pStream[byte] >> bit);
 }

// const uint32_t readBits (uint32_t count);

// const uint32_t readHuffmanBits (uint32_t count);

 const uint64_t getStreamOffset() const noexcept {return streamOffset;}

 void setStreamOffset(const uint64_t& newOffset)
 {
   if (newOffset <= streamSize) 
      streamOffset = newOffset; 
 }

 void resetStreamOffset (void) noexcept { setStreamOffset(0ull); }

 bool eohs(void) const noexcept { return streamOffset < streamSize; } /* End of Huffman Stream */

 uint64_t tillEnd (void) const noexcept {return streamSize - streamOffset;}


private:

 uint8_t* pStream;
 uint64_t streamSize;   /* size of Huffman Stream in bits */
 uint64_t streamOffset; /* bit offset of Huffman Stream */

};
	
#endif /* __LUT_LIBRARY_HUFFMAN_ENCODE_STREAM__ */ 
