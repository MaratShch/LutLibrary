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

}

#endif // __C_HUFFMAN_BLOCK_DECODER_CONSTANTS__
