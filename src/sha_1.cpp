#include "sha_1.h"

#include <iomanip>
#include <sstream>

namespace crypto {

namespace utility {
inline uint32_t rotate_left(uint32_t value, unsigned int bits) {
    return (value << bits) | (value >> (32 - bits));
}

inline uint32_t to_uint32_be(const uint8_t* bytes) {
    return (static_cast<uint32_t>(bytes[0]) << 24) |
           (static_cast<uint32_t>(bytes[1]) << 16) |
           (static_cast<uint32_t>(bytes[2]) <<  8) |
           (static_cast<uint32_t>(bytes[3]));
}

std::string to_hex_string(const uint8_t* digest, size_t length) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(digest[i]);
    }
    return oss.str();
}

std::string to_binary_string(const uint8_t* data, size_t length) {
    std::ostringstream oss;
    oss.fill('0');
    oss << std::hex;

    for (size_t i = 0; i < length; ++i) {
        unsigned char c = data[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            oss << c; // Оставляем символы без изменений
        } else {
            oss << '%' << std::setw(2) << static_cast<int>(c); // URL-кодирование
        }
    }
    return oss.str();
}

}

using namespace crypto::utility;
std::vector<uint8_t> sha_1_binary(const std::vector<uint8_t>& data) {
    std::uint32_t h0 = 0x67452301;
    std::uint32_t h1 = 0xEFCDAB89;
    std::uint32_t h2 = 0x98BADCFE;
    std::uint32_t h3 = 0x10325476;
    std::uint32_t h4 = 0xC3D2E1F0;

    std::vector<uint8_t> paddedData = data;


    paddedData.push_back(0x80);

    while ((paddedData.size() % 64) != 56) {
        paddedData.push_back(0x00);
    }

    uint64_t bit_len = static_cast<uint64_t>(data.size()) * 8;
    for (int i = 7; i >= 0; i--) {
        paddedData.push_back(static_cast<uint8_t>((bit_len >> (i * 8)) & 0xFF));
    }

    for (std::size_t  chunkOffset =0; chunkOffset < paddedData.size(); chunkOffset += 64) {
         uint8_t* chunk = &paddedData[chunkOffset];

        // Расширяем 16 слов до 80
        uint32_t w[80];
        for (int i = 0; i < 16; i++) {
            w[i] = to_uint32_be(chunk + i*4);
        }
        for (int i = 16; i < 80; i++) {
            w[i] = rotate_left(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
        }

        std::uint32_t a = h0;
        std::uint32_t b = h1;
        std::uint32_t c = h2;
        std::uint32_t d = h3;
        std::uint32_t e = h4;

        for (int i = 0; i < 80; ++i) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = rotate_left(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotate_left(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    std::vector<uint8_t> digest(20);
    digest[0]  = (h0 >> 24) & 0xff;
    digest[1]  = (h0 >> 16) & 0xff;
    digest[2]  = (h0 >> 8) & 0xff;
    digest[3]  = (h0) & 0xff;
    digest[4]  = (h1 >> 24) & 0xff;
    digest[5]  = (h1 >> 16) & 0xff;
    digest[6]  = (h1 >> 8) & 0xff;
    digest[7]  = (h1) & 0xff;
    digest[8]  = (h2 >> 24) & 0xff;
    digest[9]  = (h2 >> 16) & 0xff;
    digest[10] = (h2 >> 8) & 0xff;
    digest[11] = (h2) & 0xff;
    digest[12] = (h3 >> 24) & 0xff;
    digest[13] = (h3 >> 16) & 0xff;
    digest[14] = (h3 >> 8) & 0xff;
    digest[15] = (h3) & 0xff;
    digest[16] = (h4 >> 24) & 0xff;
    digest[17] = (h4 >> 16) & 0xff;
    digest[18] = (h4 >> 8) & 0xff;
    digest[19] = (h4) & 0xff;

    return digest;
}

std::vector<std::uint8_t> sha_1_binary(const std::string& str) {
    std::vector<uint8_t> data(str.begin(), str.end());
    return sha_1_binary(data);
}

std::string sha_1_binary_string(const std::string& data) {
    std::vector<uint8_t> digest = sha_1_binary(data);
    return to_binary_string(digest.data(), digest.size());
}


std::string sha_1_string(const std::vector<uint8_t>& data) {
    auto digest = sha_1_binary(data);
    return to_hex_string(digest.data(), digest.size());
}

std::string sha_1_string(const std::string& str) {
    std::vector<uint8_t> data(str.begin(), str.end());
    return sha_1_string(data);
}
}
