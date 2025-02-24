#ifndef __LUT_LIBRARY_LUT_HALD__
#define __LUT_LIBRARY_LUT_HALD__

#include "lutElement.h"
#include "lutErrors.h"
#include "endian_utils.h"
#include "crc_utils.h"
#include "string_view.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <memory>
#include "CHuffmanStream.h"
#include "CReversibleFilter.h"

//#define _DEBUG_SAVE_IDAT

#ifdef _DEBUG_SAVE_IDAT
 #include <iomanip>
#endif

namespace PNG
{
	constexpr uint8_t DEFLATE   = static_cast<uint8_t>(0u);
	constexpr uint8_t COLOR_RGB = static_cast<uint8_t>(2u);

	constexpr std::uint32_t Chunk (const char& a, const char& b, const char& c, const char& d) noexcept
    {
		return ((static_cast<uint32_t>(a) << 24) +
                (static_cast<uint32_t>(b) << 16) +
                (static_cast<uint32_t>(c) << 8 ) +
                (static_cast<uint32_t>(d)));
	}
}


template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr> 
class CHaldLut
{
public:
	LutElement::lutFileName const getLutFileName (void) {return m_lutName;}
	LutErrorCode::LutState getLastError(void) { return m_error; }
	LutElement::lutSize const getLutSize (void) { return m_lutSize; }
	LutElement::lutSize const getLutComponentSize (const LutElement::LutComponent component) {(void)component; return getLutSize();}

	void set_property_3D (void) noexcept { m_3d_lut = true; }
	void set_property_1D (void) noexcept { m_3d_lut = false; }
	bool is_3D_property  (void) const noexcept { return m_3d_lut; }

	LutErrorCode::LutState LoadFile (std::ifstream& lutFile)
	{
		/* clear file stream status */
		lutFile.clear();
		/* cleanup internal objects before parsing */
		_cleanup();

		lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);

		/* read and validate PNG signature in Hald LUT file */
		bool isPng = verifyPngFileSignature(readPngSignature(lutFile));
		if (true == isPng)
		{
            uint32_t sections_IDAT = 0u;
			bool continueRead = true;
			do
			{
                // if we have number of IDAT sections - let's rename it to IDAT0, IDAT1, etc... 
				std::unordered_map<std::string, std::vector<uint8_t>> chunkMap = std::move(readPngChunk(lutFile, sections_IDAT));
				mHaldChunkOrig.insert(chunkMap.begin(), chunkMap.end());

                // add logic for handle multiple IDAT sections in one PNG
                const std::string idatName = (0u == sections_IDAT ? "IDAT" : "IDAT" + std::to_string(sections_IDAT));
                auto idat = mHaldChunkOrig.find(idatName);
                auto it = chunkMap.find({ idatName });
                if (it != chunkMap.end())
                    sections_IDAT++;
 
				auto it1 = chunkMap.find({"IEND"});
				auto it2 = chunkMap.find({"NONE"});
				if (it1 != chunkMap.end() || it2 != chunkMap.end())
					continueRead = false;
			} while (true == continueRead);

            m_IdatNumber = sections_IDAT;

            bool idatValid = (m_IdatNumber > 0u ? true : false);
            if (m_IdatNumber > 1u) // we need merge all IDATx sections with IDAT before start decoding
                idatValid = merge_IDAT_Sections();

            if (true == idatValid)
            {
                if (true == parseIHDR(mHaldChunkOrig[{"IHDR"}]))
                {
                    if (true == decodeIDAT(mHaldChunkOrig[{"IDAT"}]))
                        m_error = LutErrorCode::LutState::OK;
                }
            }
		}
		else
			m_error = LutErrorCode::LutState::ReadError;
		return m_error;
	}
	
	LutErrorCode::LutState LoadFile (const string_view& lutFileName)
	{
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		if (!lutFileName.empty() && lutFileName != m_lutName)
		{
			std::ifstream haldLut { lutFileName, std::ios::binary };
			if (!haldLut.good())
				return LutErrorCode::LutState::FileNotOpened;
			
			m_lutName = lutFileName;
			err = LoadFile (haldLut);
			haldLut.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		return err;
	}

	LutErrorCode::LutState LoadFile (const char* lutFileName)
	{
		return (nullptr != lutFileName && '\0' != lutFileName[0]) ? LoadFile (string_view{ lutFileName }) : LutErrorCode::LutState::GenericError;
	}


	LutErrorCode::LutState LoadFile (const std::string& lutFileName)
	{ 
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		if (!lutFileName.empty() && lutFileName != m_lutName)
		{
			std::ifstream haldLut{ lutFileName, std::ios::binary };
			if (!haldLut.good())
				return LutErrorCode::LutState::FileNotOpened;
			
			m_lutName = lutFileName;
			err = LoadFile (haldLut);
			haldLut.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		return err;
	}

	
protected:
private:
	LutElement::lutTable3D<T>  m_lutBody3D;
	LutElement::lutTable1D<T>  m_lutBody1D;
 	LutElement::lutFileName    m_lutName;
	LutElement::lutSize        m_lutSize;
	LutErrorCode::LutState     m_error = LutErrorCode::LutState::NotInitialized;
	
	/* Original PNG file chunks. Let's save this chunks for keep possibility restore original PNG file */
	std::unordered_map<std::string, std::vector<uint8_t>> mHaldChunkOrig{};

    uint32_t m_sizeX;
    uint32_t m_sizeY;
	uint32_t m_bitDepth;
    uint32_t m_Channels;
	uint32_t m_CompressionMethod;
    uint32_t m_IdatNumber;
	bool m_3d_lut = true;

	void _cleanup (void)
	{
		m_lutBody3D.clear();
		m_lutBody1D.clear();
		m_lutSize = 0u;
		m_error = LutErrorCode::LutState::NotInitialized;
        m_sizeX = m_sizeY = 0u;
        m_Channels = 0u;
		m_bitDepth = 0u;
        m_IdatNumber = 0u;
		m_3d_lut = true;
		return;
	}

	bool verifyPngFileSignature (const uint64_t& signature) noexcept {return (signature == 0x89504E470D0A1A0Au);}

	const uint64_t readPngSignature (std::ifstream& lutFile)
	{
		uint64_t signature = 0u, signature_le = 0u;
		lutFile.read (reinterpret_cast<char*>(&signature), sizeof(signature));
		if (true == lutFile.good())
			signature_le = endian_convert (signature);
		return signature_le;
	}

	std::string encodeChunkName (const uint32_t& chunkN)
	{
		switch (chunkN)
		{
			case PNG::Chunk('I','H','D','R'):
				return "IHDR";
			case PNG::Chunk('p','H','Y','s'):
				return "pHYs";
			case PNG::Chunk('i','C','C','P'):
				return "iCCP";
			case PNG::Chunk('g','A','M','A'):
				return "gAMA";
			case PNG::Chunk('I','D','A','T'):
				return "IDAT";
			case PNG::Chunk('P','L','T','E'):
				return "PLTE";
			case PNG::Chunk('t','E','X','t'):
				return "tEXt";
			case PNG::Chunk('c','H','R','M'):
				return "cHRM";
			case PNG::Chunk('H','A','L','D'):
				return "HALD";
			case PNG::Chunk('I','E','N','D'):
				return "IEND";
			case PNG::Chunk('t','R','N','S'):
				return "tRNS";
			case PNG::Chunk('s','B','I','T'):
				return "sBIT";
			case PNG::Chunk('s','R','G','B'):
				return "sRGB";
			case PNG::Chunk('i','T','X','t'):
				return "iTXt";
			case PNG::Chunk('z','T','X','t'):
				return "zTXt";
			case PNG::Chunk('b','K','G','D'):
				return "bKGD";
			case PNG::Chunk('h','I','S','T'):
				return "hIST";
			case PNG::Chunk('s','P','L','T'):
				return "sPLT";
			case PNG::Chunk('t','I','M','E'):
				return "tIME";	
			default:
				return "NONE";
		}
		return "NONE"; /* just for avoid compilation warning */
	}

    bool merge_IDAT_Sections()
    {
        constexpr size_t sectionNameSize = sizeof(PNG::Chunk('I','D','A','T'));
        uint32_t idx = 1u;
        bool bContinue = true;

        while (bContinue)
        {
            const std::string numberedKey = "IDAT" + std::to_string(idx);
            const auto& it = mHaldChunkOrig.find(numberedKey);
            if (it != mHaldChunkOrig.end())
            {
                if (it->second.size() > sectionNameSize)
                {
                    mHaldChunkOrig["IDAT"].insert(mHaldChunkOrig["IDAT"].end(), it->second.begin() + sectionNameSize, it->second.end());
                }
                mHaldChunkOrig.erase(it);
                idx++;
            }
            else
                bContinue = false;
        }
        return true;
    }

	std::unordered_map<std::string, std::vector<uint8_t>> readPngChunk (std::ifstream& lutFile, const uint32_t& idat_enum = 0u)
	{
		int32_t chunkSize = -1;
		std::unordered_map<std::string, std::vector<uint8_t>> invalid_dict;
		invalid_dict["NONE"] = {};

		/* read chunk size and name */
		lutFile.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
		if (true == lutFile.good())
		{
			/* get chunk size in little endian byte order */
			const int32_t chunkSize_le = endian_convert (chunkSize);
			const int32_t rSize = sizeof(uint32_t) + chunkSize_le; /* chunk name + chunk size */

			std::vector<uint8_t> data (rSize);
			uint32_t crc32 = 0u;

			/* read chunk name and chunk data */
			lutFile.read(reinterpret_cast<char*>(&data[0]), data.size());
			/* read chunk CRC32 */
			lutFile.read(reinterpret_cast<char*>(&crc32), sizeof(crc32));
			if (true == lutFile.good())
			{
				/* convert readed CRC32 value from BE to LE */
				const uint32_t crc32_le = endian_convert(crc32);
				/* compute CRC32 value of current chunk */
				const uint32_t computed_crc32 = crc32_reflected(data);
				/* validate CRC32: compare betwen value readed from file and computed */
				if (computed_crc32 == crc32_le)
				{
					/* If the CRC32 is valid, create and return a map to the caller with keys and data, 
					   where the chunk name is used as the string value for the key */
					std::string chunkName = encodeChunkName(endian_convert(*reinterpret_cast<uint32_t*>(&data[0])));
                    if ("IDAT" == chunkName && 0u != idat_enum)
                        chunkName += std::to_string(idat_enum);

					std::unordered_map<std::string, std::vector<uint8_t>> dict;
					dict[chunkName] = std::move(data);
					return dict;
				}
			} /* if (true == lutFile.good()) */

		} /* if (true == lutFile.good()) */
		
		/* return invalid dictionary as map (with key value "NONE") if something going bad */
		return invalid_dict;
	}

#ifdef _DEBUG_SAVE_IDAT
	void idat_save_dbg (const std::vector<uint8_t>& data_vector)
	{
		const std::string fullFilePath = m_lutName + ".txt";
		std::ofstream outFile (fullFilePath, std::ios::out | std::ios::trunc);
		if (outFile.good())
		{
			const size_t size = data_vector.size();
			outFile << "/* THIS IS IDAT SECTION SAVED FOR DEBUG PURPOSES FROM HALD LUT FILE: " << m_lutName << " */" << std::endl;
			outFile << "/* DATA SIZE (without IDAT section signature) = " << (size - 4) << " BYTES   */" << std::endl;
			for (size_t i = 4; i < size; i++) /* exclude IDAT pattern itself from record */
			{
				const int32_t val = static_cast<const int32_t>(data_vector.at(i));
				outFile << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << val << (i != size-1 ? ", " : " ");
				if (0u != i && 0u == (i % 32))
					outFile << std::endl;
			}
			outFile.close();
		} /* if (outFile.good()) */
		return;
	}
#endif

	bool decodeIDAT (const std::vector<uint8_t>& ihdrData)
	{
		bool bRet = false;
		/* size of IDAT section in bytes, include section signature */
		const size_t idatSize = ihdrData.size();
		if (0u != idatSize)
		{
#ifdef _DEBUG_SAVE_IDAT
			this->idat_save_dbg(ihdrData);
#endif /* #ifdef _DEBUG_SAVE_IDAT */

            HuffmanUtils::CStreamPointer sp(HuffmanUtils::byte2sp(4u)); // forward stream pointer on 4 bytes for avoid IDAT header name
            HuffmanUtils::CHuffmanStream deflateStream (std::move(ihdrData), sp);
            std::vector<uint8_t> decodedData = deflateStream.Decode();

            const bool integrityStatus = deflateStream.StreamIntegrityStatus();
            if (true == integrityStatus)
            {
                // remove/cleanup all PNG chunks for decrease memory usage
                mHaldChunkOrig.clear();
                //

                size_t decodedDataSize = 0ull;
                const size_t expectedDataSize = m_lutSize * m_lutSize * m_lutSize * 3ull;
                const int32_t bpp = static_cast<int32_t>(m_bitDepth * m_Channels); // bits per pixel

                // apply reverse filter for Huffman decoded data
                std::vector<uint8_t>  vecRGB8{};
                std::vector<uint16_t> vecRGB16{};

                if (8u == m_bitDepth)
                {
                    vecRGB8 = HuffmanUtils::filter_data_reconstruct8 (decodedData, m_sizeX, m_sizeY, bpp, m_Channels);
                    decodedDataSize = vecRGB8.size();
                }
                else
                {
                    vecRGB16 = HuffmanUtils::filter_data_reconstruct (decodedData, m_sizeX, m_sizeY, bpp, m_Channels);
                    decodedDataSize = vecRGB16.size();
                }

                if (decodedDataSize >= expectedDataSize)
                {
                    // resize LUT buffer
                    m_lutBody3D = std::move(LutElement::lutTable3D<T>(m_lutSize, LutElement::lutTable2D<T>(m_lutSize, LutElement::lutTable1D<T>(m_lutSize, LutElement::lutTableRaw<T>(3)))));

                    uint32_t b, g, r, dec = 0u;
                    // fill LUT data
                    if (8u == m_bitDepth)
                    { // copy from vector with 8 bits values
                        for (b = 0u; b < m_lutSize; b++)
                            for (g = 0u; g < m_lutSize; g++)
                                for (r = 0u; r < m_lutSize; r++)
                                {
                                    m_lutBody3D[r][g][b] = {
                                        static_cast<float>(vecRGB8[dec + 0u]),
                                        static_cast<float>(vecRGB8[dec + 1u]),
                                        static_cast<float>(vecRGB8[dec + 2u]) };
                                    dec += 3u;  
                                }
                    }
                    else
                    { // copy from vector with 16 bits values
                        for (b = 0u; b < m_lutSize; b++)
                            for (g = 0u; g < m_lutSize; g++)
                                for (r = 0u; r < m_lutSize; r++)
                                {
                                    m_lutBody3D[r][g][b] = {
                                        static_cast<float>(vecRGB16[dec + 0u]),
                                        static_cast<float>(vecRGB16[dec + 1u]),
                                        static_cast<float>(vecRGB16[dec + 2u]) };
                                    dec += 3u;  
                                }
                    }

                    bRet = true;

                } // if (decodedDataSize >= expectedDataSize)
                else
                    bRet = false;
            } // if (true == integrityStatus)
		} // if (0u != idatSize)

#ifndef _DEBUG
        // here we may remove all PNG sections from object for reduce memory consumption
        mHaldChunkOrig.clear();
#endif
		return bRet;
	}

	bool parseIHDR (const std::vector<uint8_t>& ihdrData)
	{
		uint32_t width = 0u, height = 0u; 
		uint8_t  bitDepth = 0u, colorType = 0u, compressionMethod = 0u, filterMethod = 0u, interlaceMethod = 0u;

		bool bRet = false;

		if (17u == ihdrData.size()) /* let's check that vector contains IHDR data too, and not only section name */
		{
			width  = endian_convert(*reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(&ihdrData[4])));
			height = endian_convert(*reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(&ihdrData[8])));
			bitDepth = ihdrData[12], colorType = ihdrData[13], compressionMethod = ihdrData[14], filterMethod = ihdrData[15], interlaceMethod = ihdrData[16];

			auto const isPowerOf2 = [&](auto const x) noexcept -> bool {return ((x != 0) && !(x & (x - 1))); };

			if (0u != width && width == height && true == isPowerOf2(bitDepth) && bitDepth <= static_cast<uint8_t>(32u) &&
				PNG::COLOR_RGB == colorType && PNG::DEFLATE == compressionMethod)
			{
                // monochrome LUT not supported yet!!!
			    m_CompressionMethod = compressionMethod;
			    m_bitDepth = static_cast<uint32_t>(bitDepth);
			    m_lutSize = static_cast<LutElement::lutSize>(std::cbrt(static_cast<float>(width * height)));
                m_sizeX = width;
                m_sizeY = height;
                m_Channels = 3u;
			    bRet = true;
			} /* if (0u != width && width == height && true == isPowerOf2(bitDepth) && bitDepth .... */
		}
		return bRet;
	}

};


#endif /* __LUT_LIBRARY_LUT_HALD__ */ 
