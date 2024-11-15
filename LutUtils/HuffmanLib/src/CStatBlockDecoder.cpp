#include "CStatBlockDecoder.h"

using namespace HuffmanUtils;


CStreamPointer CStatBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp)
{
  CStreamPointer sp;
  return sp;
}

CStreamPointer CStatBlockDecoder::decode (const uint8_t* in, uint8_t* out, size_t outSstorageSize, CStreamPointer& inSp)
{
    return{ 0 };
}
