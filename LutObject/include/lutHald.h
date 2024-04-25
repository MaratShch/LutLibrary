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

//enum class PngChunkTypes
//{
//	'IHDR',
//	'IDAT',
//      'PLTE',
//      'tEXt',
//      'IEND',
//      'pHYs',
//      'iCCP',
//      'gAMA',
//      'cHRM',
//      'HALD'
//};

constexpr int32_t i = 'IHDR';

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
			if (true == readIHDRChunk (lutFile))
			{
				/* read and encode LUT body using DEFLATE algorithm */

				m_error = LutErrorCode::LutState::OK;
			}
			else
				m_error = LutErrorCode::LutState::GenericError;
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
	
	/* Original PNG file chunks. Let's save this chunks for keep possibilty restore original PNG file 
	   and parse PNG/HALD LUT data from memory */
	std::unordered_map<std::string, std::vector<uint8_t>> mHaldChunkOrig{};

	uint32_t m_bitDepth;
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

	std::vector<uint8_t> readPngChunk (std::ifstream& lutFile, size_t& chunkSize, uint32_t& chunkType)
	{
		chunkSize = -1;
		uint32_t crc32  = 0u;
		/* read chunk */
		/* validate CRC */
		/* detect chunkSize and chunkType */
		return false;
	}

	bool readIHDRChunk (std::ifstream& lutFile)
	{
		int32_t chunkSize = -1;
		uint32_t crc32  = 0u;
		uint32_t width  = 0u, height = 0u;
		uint8_t  bitDepth = 0u, colorType = 0u, compressionMethod = 0u, filterMethod = 0u, interlaceMethod = 0u;
		bool bRet = false;

		constexpr int32_t nameIHDR = static_cast<const int32_t>('IHDR');
 		constexpr size_t  sizeIHDRData = sizeof(width) + sizeof(height) + sizeof(bitDepth) + sizeof(colorType) + 
                                                 sizeof(compressionMethod) + sizeof(filterMethod) + sizeof(interlaceMethod); 
		constexpr size_t  sizeIHDR = sizeof(nameIHDR) + sizeIHDRData;
		std::array<uint8_t, sizeIHDR> ihdr_data{};

		/* Read IHDR chunk size value   */	
		lutFile.read (reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
		/* Read IHDR chunk data content */
		lutFile.read (reinterpret_cast<char*>(&ihdr_data[0]), ihdr_data.size());
		/* Read IHDR chunk CRC32 value  */
		lutFile.read (reinterpret_cast<char*>(&crc32), sizeof(crc32));

		if (true == lutFile.good())
		{
			/* convert IHDR chunk size to Little Endian  */
			const int32_t  chunkSize_le = endian_convert (chunkSize);
			/* convert IHDR chunk name to Little Endian  */
			const int32_t  chunkName_le = endian_convert (*reinterpret_cast<int32_t* >(&ihdr_data[0]));
			/* convert IHDR chunk CRC32 to Little Endian */
			const uint32_t crc32_le     = endian_convert (crc32);

			/* compute CRC32 for IHDR chunk: a four-byte CRC calculated on the preceding bytes in the chunk, 
                           including the chunk type field and chunk data fields, but not including the length field. */	
			const uint32_t computed_crc32 = crc32_reflected (ihdr_data);
			
			if (computed_crc32 == crc32_le) /* validate CRC from IHDR chunk */
			{
				if (nameIHDR == chunkName_le && sizeIHDRData == chunkSize_le)
				{
					width  = endian_convert(*reinterpret_cast<uint32_t*>(&ihdr_data[4]));
					height = endian_convert(*reinterpret_cast<uint32_t*>(&ihdr_data[8]));
					bitDepth = ihdr_data[12], colorType = ihdr_data[13], compressionMethod = ihdr_data[14], filterMethod = ihdr_data[15], interlaceMethod = ihdr_data[16];
					auto const isPowerOf2 = [&](auto const x) noexcept -> bool {return ((x != 0) && !(x & (x - 1)));};

					if (0u != width && width == height && true == isPowerOf2(bitDepth) && bitDepth <= static_cast<uint8_t>(32u) &&
                                            static_cast<uint8_t>(2u) == colorType /* RGB*/)
					{
						m_bitDepth = static_cast<uint32_t>(bitDepth);
						m_lutSize  = static_cast<LutElement::lutSize>(std::cbrt(static_cast<float>(width * height)));
						bRet = true;
					} /* if (0u != width && width == height && true == isPowerOf2(bitDepth) && bitDepth .... */

				} /* if (nameIHDR == chunkName && sizeIHDR == chunkSize) */

			} /* if (computed_crc32 == crc32)  */

		} /* if (true == lutFile.good()) */

		return bRet;
	}

};


#endif /* __LUT_LIBRARY_LUT_HALD__ */ 
