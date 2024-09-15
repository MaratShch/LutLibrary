#ifndef __LUT_LIBRARY_HUFFMAN_ENCODE_STREAM__
#define __LUT_LIBRARY_HUFFMAN_ENCODE_STREAM__

#include <type_traits>

class CHuffmanStream
{
public:

 CHuffmanStream(){;}
 CHuffmanStream(void* ptr) {;}
 
 void* operator new(size_t s) {return nullptr;|
 void  operator delete(void* p) {;}
  
private:

};
	
#endif /* __LUT_LIBRARY_HUFFMAN_ENCODE_STREAM__ */ 
