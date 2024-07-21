#ifndef __LUT_LIBRARY_DEFLATE_ENCODER_ALGO_UTILS__
#define __LUT_LIBRARY_DEFLATE_ENCODER_ALGO_UTILS__

#include "deflate_common.h"


class CDeflateBasicEncoder
{
public:
	CDeflateBasicEncoder() { ; }
	virtual ~CDeflateBasicEncoder() { ; }
	
	std::vector<uint8_t> Encode (const std::vector<uint8_t>& rawDat);

private:
};


#endif /* __LUT_LIBRARY_DEFLATE_ENCODER_ALGO_UTILS__ */
