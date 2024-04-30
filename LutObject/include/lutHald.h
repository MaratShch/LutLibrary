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

namespace PNG
{
	constexpr uint8_t DEFLATE = static_cast<uint8_t>(0u);
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
			bool continueRead = true;
			do
			{
				std::unordered_map<std::string, std::vector<uint8_t>> chunkMap = std::move(readPngChunk(lutFile));
				mHaldChunkOrig.insert(chunkMap.begin(), chunkMap.end());
				auto it1 = chunkMap.find({"IEND"});
				auto it2 = chunkMap.find({"NONE"});
				if (it1 != chunkMap.end() || it2 != chunkMap.end())
					continueRead = false;
			} while (true == continueRead);

			if (true == parseIHDR (mHaldChunkOrig[{"IHDR"}]))
			{
				if (true == encodeIDAT (mHaldChunkOrig[{"IDAT"}]))
					m_error = LutErrorCode::LutState::OK;
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

	uint32_t m_bitDepth;
	uint32_t m_CompressionMethod;
	bool m_3d_lut = true;

	void _cleanup (void)
	{
		m_lutBody3D.clear();
		m_lutBody1D.clear();
		m_lutName.clear();
		m_lutSize = 0u;
		m_error = LutErrorCode::LutState::NotInitialized;
		m_bitDepth = 0u;
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

	std::unordered_map<std::string, std::vector<uint8_t>> readPngChunk (std::ifstream& lutFile)
	{
		using byte = uint8_t;
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
			uint32_t crc32 = 0u, crc32_le = 0u;

			/* read chunk name and chunk data */
			lutFile.read(reinterpret_cast<char*>(&data[0]), data.size());
			/* read chunk CRC32 */
			lutFile.read(reinterpret_cast<char*>(&crc32), sizeof(crc32));
			if (true == lutFile.good())
			{
				/* convert readed CRC32 value from BE to LE */
				crc32_le = endian_convert(crc32);
				/* compute CRC32 value of current chunk */
				const uint32_t computed_crc32 = crc32_reflected(data);
				/* validate CRC32: compare betwen value readed from file and computed */
				if (computed_crc32 == crc32_le)
				{
					/* if CRC32 is valid - create and retirn to caller map with key and data, 
					   where as a key used chunk name as string value */
					std::string chunkName = encodeChunkName(endian_convert(*reinterpret_cast<uint32_t*>(&data[0])));
					std::unordered_map<std::string, std::vector<uint8_t>> dict;
					dict[chunkName] = std::move(data);
					return dict;
				}
			} /* if (true == lutFile.good()) */

		} /* if (true == lutFile.good()) */
		
		/* return invalid dictionary as map (with key value "NONE") if something going bad */
		return invalid_dict;
	}

	bool encodeIDAT (const std::vector<uint8_t>& ihdrData)
	{
		/* TODO ... */
		return true;
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
				PNG::COLOR_RGB == colorType&& PNG::DEFLATE == compressionMethod)
			{
				m_CompressionMethod = compressionMethod;
				m_bitDepth = static_cast<uint32_t>(bitDepth);
				m_lutSize = static_cast<LutElement::lutSize>(std::cbrt(static_cast<float>(width * height)));
				bRet = true;
			} /* if (0u != width && width == height && true == isPowerOf2(bitDepth) && bitDepth .... */
		}
		return bRet;
	}

};


#endif /* __LUT_LIBRARY_LUT_HALD__ */ 
