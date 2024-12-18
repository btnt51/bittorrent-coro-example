#ifndef SHA_1_H
#define SHA_1_H
#include <vector>
#include <string>
#include <cstdint>
namespace crypto {
namespace utility {
inline uint32_t rotate_left(uint32_t value, unsigned int bits);

inline uint32_t to_uint32_be(const uint8_t* bytes);

std::string to_hex_string(const uint8_t* digest, size_t length);
}
std::vector<std::uint8_t> sha_1_binary(const std::vector<uint8_t>& data);
std::vector<std::uint8_t> sha_1_binary(const std::string& data);
std::string sha_1_binary_string(const std::string& data);
std::string sha_1_string(const std::vector<uint8_t>& data);
std::string sha_1_string(const std::string& data);
}
#endif //SHA_1_H
