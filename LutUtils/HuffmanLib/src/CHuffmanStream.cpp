#include "CHuffmanStream.h"
#include "CHuffmanBlock.h"


using namespace HuffmanUtils;

CHuffmanStream::CHuffmanStream (const uint8_t* pData, size_t inDataSize, CStreamPointer sp)
{
    if (nullptr != pData && 0ll < inDataSize)
        m_StreamData.assign(pData, pData + inDataSize);

    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
}


CHuffmanStream::CHuffmanStream (const std::vector<uint8_t>&& pData, CStreamPointer sp) : m_StreamData(std::move(pData))
{
    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
};
            
CHuffmanStream::CHuffmanStream (const std::vector<uint8_t>& pData, CStreamPointer sp) : m_StreamData(pData)
{
    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
}

template <size_t N>
CHuffmanStream::CHuffmanStream(const std::array<uint8_t, N>& pData, CStreamPointer sp) : m_StreamData(pData.begin(), pData.end())
{
    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
}


CHuffmanStream::~CHuffmanStream (void)
{
    m_StreamData.clear();
    return;
}


OutStreamT Encode (void)
{
   OutStreamT encodedData{};
   return encodedData; // not implemented yet
}


OutStreamT CHuffmanStream::Decode (void)
{
    OutStreamT decodedData{};
    bool finalBlock = false;
    bool isErr = false;
 
    do {
        m_blockCnt++;
        CHuffmanBlock hBlock (m_StreamData, m_Sp);
        OutStreamT dataChunk = hBlock.Decode();
   
        m_Sp = hBlock.GetStreamPointer();
        finalBlock = hBlock.isFinal();

        if (0 != dataChunk.size())
        {
            // resize target vector and copy decoded data chunk into targe vector
            decodedData.resize(decodedData.size() + dataChunk.size());
            std::copy(dataChunk.begin(), dataChunk.end(), decodedData.end() - dataChunk.size());
        }
        else
        {
            std::cout << "Decoded data size equal to zero" << std::endl;
            isErr = true;
        }

    } while (false == finalBlock && isErr == false);


    return decodedData;
}

