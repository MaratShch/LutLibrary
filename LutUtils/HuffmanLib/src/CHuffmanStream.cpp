#include "CHuffmanStream.h"

using namespace HuffmanUtils;

std::vector<std::pair<uint32_t, uint32_t>> HuffmanUtils::generate_huffman_codes (const std::vector<uint32_t>& code_lengths, int32_t num_symbols)
{
    constexpr uint32_t MAX_CODES = 19u;

    int32_t code_length_counts[MAX_CODES] = { 0 };
    uint32_t next_code[MAX_CODES] = { 0u };

    // Step 1: Count how many symbols have each code length
    for (int32_t i = 0; i < num_symbols; i++)
    {
        if (code_lengths[i] != 0)
        {
            code_length_counts[code_lengths[i]]++;
        }
    }

    // Step 2: Determine the first code for each length
    uint32_t code = 0;
    for (int32_t length = 1; length < num_symbols; length++) {
        code = (code + code_length_counts[length - 1]) << 1;
        next_code[length] = code;
    }

    // Step 3: Assign codes to symbols
    std::vector<std::pair<uint32_t, uint32_t>> outCodes{};

    for (int32_t i = 0; i < num_symbols; i++)
    {
        std::pair<uint32_t, uint32_t> hCode{};

        uint32_t length = code_lengths[i];
        if (length != 0)
        {
            hCode.first  = next_code[length];
            hCode.second = length;
            next_code[length]++;
            outCodes.push_back(hCode);
        }
        else
            outCodes.push_back(hCode);
    }

    return outCodes;
}
