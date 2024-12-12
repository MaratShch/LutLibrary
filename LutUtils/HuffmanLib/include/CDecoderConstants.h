#ifndef __C_HUFFMAN_BLOCK_DECODER_CONSTANTS__
#define __C_HUFFMAN_BLOCK_DECODER_CONSTANTS__


#include <array>
#include <tuple>
#include <type_traits>


namespace HuffmanUtils
{
    constexpr uint32_t EndOfBlock = 256u;

    constexpr uint32_t min_HLIT = 257u;
    constexpr uint32_t max_HLIT = 286u;
    constexpr uint32_t min_HDIST = 1u;
    constexpr uint32_t max_HDIST = 30u;
    constexpr uint32_t min_HCLEN = 4u;
    constexpr uint32_t max_HCLEN = 19u;

    constexpr uint32_t max_WindowSize = 32768u;

    // Alpahabet for build Code Lenghts For Code Lengths Tree
    constexpr std::array<uint32_t, max_HCLEN> cl4cl_dictionary_idx =
    {
        // https://datatracker.ietf.org/doc/html/rfc1951
        16u, 17u, 18u, 0u, 8u, 7u, 9u, 6u, 10u, 5u, 11u, 4u, 12u, 3u, 13u, 2u, 14u, 1u, 15u
    };

    using DistanceTable = std::tuple<int32_t /* code */, int32_t /* extra bits */, int32_t /* base length */>;

    constexpr std::array<DistanceTable, 29> cLengthCodes = 
    {{    // rfc1951
	  { 257, 0, 3  },
	  { 258, 0, 4  },
	  { 259, 0, 5  },
	  { 260, 0, 6  },
	  { 261, 0, 7  },
	  { 262, 0, 8  },
	  { 263, 0, 9  },
	  { 264, 0, 10 },
	  { 265, 1, 11 },
	  { 266, 1, 13 },
	  { 267, 1, 15 },
	  { 268, 1, 17 },
	  { 269, 2, 19 },
	  { 270, 2, 23 },
	  { 271, 2, 27 },
	  { 272, 3, 31 },
	  { 273, 3, 35 },
	  { 274, 3, 43 },
	  { 275, 3, 51 },
	  { 276, 3, 59 },
	  { 277, 4, 67 },
	  { 278, 4, 83 },
	  { 279, 4, 99 },
	  { 280, 4, 115},
	  { 281, 5, 131},
	  { 282, 5, 163},
	  { 283, 5, 195},
	  { 284, 5, 227},
	  { 285, 0, 258}   
    }};
    constexpr int32_t cLengthGetCode    (const int32_t& idx) {return std::get<0>(cLengthCodes[idx]);}
    constexpr int32_t cLengthGetExtra   (const int32_t& idx) {return std::get<1>(cLengthCodes[idx]);}
    constexpr int32_t cLengthGetBaseLen (const int32_t& idx) {return std::get<2>(cLengthCodes[idx]);}

    constexpr size_t  cLengthCodesSize = cLengthCodes.size();
    constexpr int32_t cLengthCodesMin  = cLengthGetCode(0);
    constexpr int32_t cLengthCodesMax  = cLengthGetCode(28);


    constexpr std::array<DistanceTable, 30> cDistanceCodes = 
    {{    // rfc1951
	  { 0,  0,  1    },
	  { 1,  0,  2    },
	  { 2,  0,  3    },
	  { 3,  0,  4    },
	  { 4,  1,  5    },
	  { 5,  1,  7    },
	  { 6,  2,  9    },
	  { 7,  2,  13   },
	  { 8,  3,  17   },
	  { 9,  3,  25   },
	  { 10, 4,  33   },
	  { 11, 4,  49   },
	  { 12, 5,  65   },
	  { 13, 5,  97   },
	  { 14, 6,  129  },
	  { 15, 6,  193  },
	  { 16, 7,  257  },
	  { 17, 7,  385  },
	  { 18, 8,  513  },
	  { 19, 8,  769  },
	  { 20, 9,  1025 },
	  { 21, 9,  1537 },
	  { 22, 10, 2049 },
	  { 23, 10, 3073 },
	  { 24, 11, 4097 },
	  { 25, 11, 6145 },
	  { 26, 12, 8193 },
	  { 27, 12, 12289},
	  { 28, 13, 16385},
	  { 29, 13, 24577}     
    }};

    constexpr int32_t cDistanceGetCode    (const int32_t& idx) {return std::get<0>(cDistanceCodes[idx]);}
    constexpr int32_t cDistanceGetExtra   (const int32_t& idx) {return std::get<1>(cDistanceCodes[idx]);}
    constexpr int32_t cDistanceGetBaseLen (const int32_t& idx) {return std::get<2>(cDistanceCodes[idx]);}

    constexpr size_t  cDistanceCodesSize = cDistanceCodes.size();
    constexpr int32_t cDistanceCodesMin  = cDistanceGetCode(0);

    
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
