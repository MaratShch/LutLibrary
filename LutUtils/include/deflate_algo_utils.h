#ifndef __LUT_LIBRARY_DEFLATE_ENCODER_ALGO_UTILS__
#define __LUT_LIBRARY_DEFLATE_ENCODER_ALGO_UTILS__

#include <vector>

class CDeflateBasicDecoder
{
public:
	CDeflateBasicDecoder() { ; }
	virtual ~CDeflateBasicDecoder() { ; }

	std::vector<uint8_t> Decode (const std::vector<uint8_t>& iDat) {
		std::vector<uint8_t> dummy{};
		return dummy;
	}

private:
};


class CDeflateBasicEncoder
{
public:
private:
};


#endif /* __LUT_LIBRARY_DEFLATE_ENCODER_ALGO_UTILS__ */