#include "CFakedBlockDecoder.h"
#include "CDecoderConstants.h"
#include "CHuffmanIo.h"

using namespace HuffmanUtils;

CFakedBlockDecoder::~CFakedBlockDecoder (void)
{
    // nothing TODO
    return;
}


bool CFakedBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    std::cout << "Faked decoder (RAW data) started. SP = " << sp << std::endl;

    return true;
}

