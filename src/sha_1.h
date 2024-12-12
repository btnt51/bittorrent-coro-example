#ifndef SHA_1_H
#define SHA_1_H
#include <vector>
#include <string>
#include <cstdint>
namespace crypto {
std::string sha_1(const std::vector<uint8_t>& data);
std::string sha_1(const std::string& data);
}
#endif //SHA_1_H
