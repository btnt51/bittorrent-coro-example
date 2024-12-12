
#ifndef DECODER_H
#define DECODER_H

#include "lib/nlohmann/json.hpp"
namespace decoder {
namespace to_json {
nlohmann::json parse_string(const std::string& encoded_value, std::size_t& index);
nlohmann::json parse_integer(const std::string& encoded_value, std::size_t& index);
nlohmann::json parse_list(const std::string& encoded_value, size_t& index);
nlohmann::json parse_dictionary(const std::string& encoded_value, std::size_t& index);
nlohmann::json decode_bencoded_value(const std::string& encoded_value, std::size_t& index);
nlohmann::json decode_bencoded_value(const std::string& encoded_value);
}
namespace to_std {
std::string parse_string(const std::string& encoded_value, std::size_t& index);
long long parse_integer(const std::string& encoded_value, std::size_t& index);
std::vector<std::any> parse_list(const std::string& encoded_value, size_t& index);
std::unordered_map<std::string, std::any> parse_dictionary(const std::string& encoded_value, std::size_t& index);
std::any decode_bencoded_value(const std::string& encoded_value, std::size_t& index);
std::any decode_bencoded_value(const std::string& encoded_value);
}
}

namespace coder {
namespace to_string {
template<typename T>
concept Integral = std::integral<T>;

template<Integral T>
std::string encode(T val) {
    return "i" + std::to_string(val) + "e";
}

std::string encode(const std::string &val);

template<typename T>
std::string encode_pair(const std::string &key, T value) {
    return encode(key) + encode(value);
}
}
}

#endif //DECODER_H
