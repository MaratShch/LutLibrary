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
#include <array>
#include <cmath>

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
		constexpr size_t pngSignatureSize { 8 };
		uint64_t signature { 0u };
		char cBuffer[pngSignatureSize + 1]{};
		lutFile.read (cBuffer, pngSignatureSize);
		if (true == lutFile.good())
		{
			for (size_t i = 0u; i < pngSignatureSize; i++)
				signature |= (static_cast<uint64_t>(cBuffer[i]) << ((7 - i) * 8));
		}
		return signature;
	}

	bool readIHDRChunk(std::ifstream& lutFile)
	{
		/* read chunk length and chunk type */
		int32_t chunkSize = -1;
		int32_t chunkName = -1;
		uint32_t crc32 = 0xFFFFFFFFu;
		bool bRet = false;

		lutFile.read (reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
		lutFile.read (reinterpret_cast<char*>(&chunkName), sizeof(chunkName));
		if (true == lutFile.good())
		{
			uint32_t width = 0u, height = 0u;
			uint8_t  bitDepth, colorType, compressionMethod, filterMethod, interlaceMethod;
			constexpr size_t sizeIHDRData = sizeof(width) + sizeof(height) + sizeof(bitDepth) + sizeof(colorType) +
				sizeof(compressionMethod) + sizeof(filterMethod) + sizeof(interlaceMethod);
			constexpr int32_t nameIHDR = static_cast<const int32_t>('IHDR');

			const int32_t chunkSize_le = endian_convert(chunkSize);
			const int32_t chunkName_le = endian_convert(chunkName);

			if (nameIHDR == chunkName_le && sizeIHDRData >= static_cast<size_t>(chunkSize_le))
			{
				constexpr size_t sizeIHDRDataWithCRC = sizeIHDRData + sizeof(crc32);
				std::array<uint8_t, sizeIHDRDataWithCRC> ihdr_data{};
				lutFile.read (reinterpret_cast<char*>(&ihdr_data[0]), ihdr_data.size());
				if (true == lutFile.good())
				{
					width  = endian_convert(*reinterpret_cast<uint32_t*>(&ihdr_data[0]));
					height = endian_convert(*reinterpret_cast<uint32_t*>(&ihdr_data[4]));
					bitDepth = ihdr_data[8], colorType = ihdr_data[9], compressionMethod = ihdr_data[10], filterMethod = ihdr_data[11], interlaceMethod = ihdr_data[12];
					/* store Reflected CRC32 value of IHDR section in temporary variable */					
					crc32 = *(reinterpret_cast<int32_t*>(&ihdr_data[13]));
					ihdr_data[13] = ihdr_data[14] = ihdr_data[15] = ihdr_data[16] = 0x0u;
					const uint32_t computed_crc32 = crc32_reflected (ihdr_data);
					
					if (computed_crc32 == crc32) /* validate CRC from IHDR chunk */
					{
					auto const isPowerOf2 = [&](auto const x) noexcept -> bool {return ((x != 0) && !(x & (x - 1)));};

					if (0u != width && width == height && true == isPowerOf2(bitDepth) && bitDepth <= static_cast<uint8_t>(32u) && static_cast<uint8_t>(2u) == colorType /* RGB*/)
					{
						m_bitDepth = static_cast<uint32_t>(bitDepth);
						m_lutSize = static_cast<LutElement::lutSize>(std::cbrt(static_cast<float>(width * height)));
						bRet = true;
					}
					}
					else
						std::cout << "Expected CRC32: " << crc32 << "  Computed CRC32: " << computed_crc32 << std::endl;

				}
			}
		}
		return bRet;
	}

};


#endif /* __LUT_LIBRARY_LUT_HALD__ */ 
