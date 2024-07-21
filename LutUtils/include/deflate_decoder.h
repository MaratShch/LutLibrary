#ifndef __LUT_LIBRARY_DEFLATE_DECODER_ALGO_UTILS__
#define __LUT_LIBRARY_DEFLATE_DECODER_ALGO_UTILS__

#include "deflate_common.h"

class CDeflateBasicDecoder
{
public:
	CDeflateBasicDecoder() { ; }
	virtual ~CDeflateBasicDecoder() { ; }

	std::vector<uint8_t> Decode (const std::vector<uint8_t>& iDat);

private:
};


#endif /* __LUT_LIBRARY_DEFLATE_DECODER_ALGO_UTILS__ */
