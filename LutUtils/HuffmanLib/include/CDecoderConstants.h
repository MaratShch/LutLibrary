#ifndef __C_HUFFMAN_BLOCK_DECODER_CONSTANTS__
#define __C_HUFFMAN_BLOCK_DECODER_CONSTANTS__


#include <array>
#include <tuple>
#include <type_traits>


namespace HuffmanUtils
{
    constexpr uint32_t EndOfBlock = 256u;

    // Alpahabet for build Code Lenghts For Code Lengths Tree
    constexpr std::array<uint32_t, 19> cl4cl_dictionary_idx =
    {
        // https://datatracker.ietf.org/doc/html/rfc1951
        16u, 17u, 18u, 0u, 8u, 7u, 9u, 6u, 10u, 5u, 11u, 4u, 12u, 3u, 13u, 2u, 14u, 1u, 15u
    };

    using DistanceTable = std::tuple<uint32_t /* code */, uint32_t /* extra bits */, uint32_t /* base length */>;

    constexpr std::array<DistanceTable, 29> cLengthCodes = 
    {{    // rfc1951
	  { 257u, 0u, 3u  },
	  { 258u, 0u, 4u  },
	  { 259u, 0u, 5u  },
	  { 260u, 0u, 6u  },
	  { 261u, 0u, 7u  },
	  { 262u, 0u, 8u  },
	  { 263u, 0u, 9u  },
	  { 264u, 0u, 10u },
	  { 265u, 1u, 11u },
	  { 266u, 1u, 13u },
	  { 267u, 1u, 15u },
	  { 268u, 1u, 17u },
	  { 269u, 2u, 19u },
	  { 270u, 2u, 23u },
	  { 271u, 2u, 27u },
	  { 272u, 3u, 31u },
	  { 273u, 3u, 35u },
	  { 274u, 3u, 43u },
	  { 275u, 3u, 51u },
	  { 276u, 3u, 59u },
	  { 277u, 4u, 67u },
	  { 278u, 4u, 83u },
	  { 279u, 4u, 99u },
	  { 280u, 4u, 115u},
	  { 281u, 5u, 131u},
	  { 282u, 5u, 163u},
	  { 283u, 5u, 195u},
	  { 284u, 5u, 227u},
	  { 285u, 0u, 258u}   
    }};
    constexpr uint32_t cLengthGetCode    (const uint32_t& idx) {return std::get<0>(cLengthCodes[idx]);}
    constexpr uint32_t cLengthGetExtra   (const uint32_t& idx) {return std::get<1>(cLengthCodes[idx]);}
    constexpr uint32_t cLengthGetBaseLen (const uint32_t& idx) {return std::get<2>(cLengthCodes[idx]);}

    constexpr size_t   cLengthCodesSize = cLengthCodes.size();
    constexpr uint32_t cLengthCodesMin  = cLengthGetCode(0u);
    constexpr uint32_t cLengthCodesMax  = cLengthGetCode(28u);


    constexpr std::array<DistanceTable, 30> cDistanceCodes = 
    {{    // rfc1951
	  { 0u,  0u,  1u    },
	  { 1u,  0u,  2u    },
	  { 2u,  0u,  3u    },
	  { 3u,  0u,  4u    },
	  { 4u,  1u,  5u    },
	  { 5u,  1u,  7u    },
	  { 6u,  2u,  9u    },
	  { 7u,  2u,  13u   },
	  { 8u,  3u,  17u   },
	  { 9u,  3u,  25u   },
	  { 10u, 4u,  33u   },
	  { 11u, 4u,  49u   },
	  { 12u, 5u,  65u   },
	  { 13u, 5u,  97u   },
	  { 14u, 6u,  129u  },
	  { 15u, 6u,  193u  },
	  { 16u, 7u,  257u  },
	  { 17u, 7u,  385u  },
	  { 18u, 8u,  513u  },
	  { 19u, 8u,  769u  },
	  { 20u, 9u,  1025u },
	  { 21u, 9u,  1537u },
	  { 22u, 10u, 2049u },
	  { 23u, 10u, 3073u },
	  { 24u, 11u, 4097u },
	  { 25u, 11u, 6145u },
	  { 26u, 12u, 8193u },
	  { 27u, 12u, 12289u},
	  { 28u, 13u, 16385u},
	  { 29u, 13u, 24577u}     
    }};

    constexpr uint32_t cDistanceGetCode    (const uint32_t& idx) {return std::get<0>(cDistanceCodes[idx]);}
    constexpr uint32_t cDistanceGetExtra   (const uint32_t& idx) {return std::get<1>(cDistanceCodes[idx]);}
    constexpr uint32_t cDistanceGetBaseLen (const uint32_t& idx) {return std::get<2>(cDistanceCodes[idx]);}

    constexpr size_t   cDistanceCodesSize = cDistanceCodes.size();
    constexpr uint32_t cDistanceCodesMin  = cDistanceGetCode(0);

    
    constexpr std::array<uint16_t, 144> HuffmanStaticCodes8
    {   // alphabet range: 0x00 - 0x8F
	    0b00110000, 0b00110001, 0b00110010, 0b00110011, 
        0b00110100, 0b00110101, 0b00110110, 0b00110111,
        0b00111000, 0b00111001, 0b00111010, 0b00111011,
	    0b00111100, 0b00111101, 0b00111110, 0b00111111,
	    0b01000000, 0b01000001, 0b01000010, 0b01000011,
	    0b01000100, 0b01000101, 0b01000110, 0b01000111,
	    0b01001000, 0b01001001, 0b01001010, 0b01001011,
	    0b01001100, 0b01001101, 0b01001110, 0b01001111,
	    0b01010000, 0b01010001, 0b01010010, 0b01010011,
	    0b01010100, 0b01010101, 0b01010110, 0b01010111,
	    0b01011000, 0b01011001, 0b01011010, 0b01011011,
	    0b01011100, 0b01011101, 0b01011110, 0b01011111,
	    0b01100000, 0b01100001, 0b01100010, 0b01100011,
	    0b01100100, 0b01100101, 0b01100110, 0b01100111,
	    0b01101000, 0b01101001, 0b01101010, 0b01101011,
	    0b01101100, 0b01101101, 0b01101110, 0b01101111,
	    0b01110000, 0b01110001, 0b01110010, 0b01110011,
	    0b01110100, 0b01110101, 0b01110110, 0b01110111,
	    0b01111000, 0b01111001, 0b01111010, 0b01111011,
	    0b01111100, 0b01111101, 0b01111110, 0b01111111,
	    0b10000000, 0b10000001, 0b10000010, 0b10000011,
	    0b10000100, 0b10000101, 0b10000110, 0b10000111,
	    0b10001000, 0b10001001, 0b10001010, 0b10001011,
	    0b10001100, 0b10001101, 0b10001110, 0b10001111,
	    0b10010000, 0b10010001, 0b10010010, 0b10010011,
	    0b10010100, 0b10010101, 0b10010110, 0b10010111,
	    0b10011000, 0b10011001, 0b10011010, 0b10011011,
	    0b10011100, 0b10011101, 0b10011110, 0b10011111,
	    0b10100000, 0b10100001, 0b10100010, 0b10100011,
	    0b10100100, 0b10100101, 0b10100110, 0b10100111,
	    0b10101000, 0b10101001, 0b10101010, 0b10101011,
	    0b10101100, 0b10101101, 0b10101110, 0b10101111,
	    0b10110000, 0b10110001, 0b10110010, 0b10110011,
	    0b10110100, 0b10110101, 0b10110110, 0b10110111,
	    0b10111000, 0b10111001, 0b10111010, 0b10111011,
	    0b10111100, 0b10111101, 0b10111110, 0b10111111
    }; // constexpr std::array<uint16_t, 144> HuffmanStaticCodes8

    constexpr std::array<uint16_t, 112> HuffmanStaticCodes9
    {   // alphabet range: 0x90 - 0xFF
	    0b110010000, 0b110010001, 0b110010010, 0b110010011,
	    0b110010100, 0b110010101, 0b110010110, 0b110010111,
	    0b110011000, 0b110011001, 0b110011010, 0b110011011,
	    0b110011100, 0b110011101, 0b110011110, 0b110011111,
	    0b110100000, 0b110100001, 0b110100010, 0b110100011,
	    0b110100100, 0b110100101, 0b110100110, 0b110100111,
	    0b110101000, 0b110101001, 0b110101010, 0b110101011,
	    0b110101100, 0b110101101, 0b110101110, 0b110101111,
	    0b110110000, 0b110110001, 0b110110010, 0b110110011,
	    0b110110100, 0b110110101, 0b110110110, 0b110110111,
	    0b110111000, 0b110111001, 0b110111010, 0b110111011,
	    0b110111100, 0b110111101, 0b110111110, 0b110111111,
	    0b111000000, 0b111000001, 0b111000010, 0b111000011,
	    0b111000100, 0b111000101, 0b111000110, 0b111000111,
	    0b111001000, 0b111001001, 0b111001010, 0b111001011,
	    0b111001100, 0b111001101, 0b111001110, 0b111001111,
	    0b111010000, 0b111010001, 0b111010010, 0b111010011,
	    0b111010100, 0b111010101, 0b111010110, 0b111010111,
	    0b111011000, 0b111011001, 0b111011010, 0b111011011,
	    0b111011100, 0b111011101, 0b111011110, 0b111011111,
	    0b111100000, 0b111100001, 0b111100010, 0b111100011,
	    0b111100100, 0b111100101, 0b111100110, 0b111100111,
	    0b111101000, 0b111101001, 0b111101010, 0b111101011,
	    0b111101100, 0b111101101, 0b111101110, 0b111101111,
	    0b111110000, 0b111110001, 0b111110010, 0b111110011,
	    0b111110100, 0b111110101, 0b111110110, 0b111110111,
	    0b111111000, 0b111111001, 0b111111010, 0b111111011,
	    0b111111100, 0b111111101, 0b111111110, 0b111111111
    }; // constexpr std::array<uint16_t, 112> HuffmanStaticCodes9`

    constexpr std::array<uint16_t, 32> HuffmanStaticLengths
    {   // Static Huffman Codes for Range 256–287
        0b0000000, // end of block
        // 7 bits length [257 - 279 range]
        0b0000001,  0b0000010,  0b0000011,  0b0000100,
        0b0000101,  0b0000110,  0b0000111,  0b0001000,
        0b0001001,  0b0001010,  0b0001011,  0b0001100,
        0b0001101,  0b0001110,  0b0001111,  0b0010000,
        0b0010001,  0b0010010,  0b0010011,  0b0010100,
        0b0010101,  0b0010110,  0b0010111,
        // 8 bits length [280 - 287 range]
        0b11000000, 0b11000001, 0b11000010, 0b11000011,
        0b11000100, 0b11000101, 0b11000110, 0b11000111      
    }; // constexpr std::array<uint16_t, 32> HuffmanStaticLengths
 
    constexpr uint16_t StaticEndOfBlock = HuffmanStaticLengths[0];



}

#endif // __C_HUFFMAN_BLOCK_DECODER_CONSTANTS__
