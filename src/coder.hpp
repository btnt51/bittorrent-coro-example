
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
namespace to_string {
struct BencodedValue;

using BencodedValuePtr = std::shared_ptr<BencodedValue>;

struct BencodedValue {
    using ValueType = std::variant<
        std::string,
        int64_t,
        std::vector<BencodedValuePtr>,
        std::unordered_map<std::string, BencodedValuePtr>>;

    ValueType value;

    // Конструкторы для удобного создания значений
    BencodedValue(std::string val) : value(std::move(val)) {}
    BencodedValue(int64_t val) : value(val) {}
    BencodedValue(std::vector<BencodedValuePtr> val) : value(std::move(val)) {}
    BencodedValue(std::unordered_map<std::string, BencodedValuePtr> val) : value(std::move(val)) {}
};

BencodedValuePtr parse_string(const std::string& encoded_value, std::size_t& index);
BencodedValuePtr parse_integer(const std::string& encoded_value, std::size_t& index);
BencodedValuePtr parse_list(const std::string& encoded_value, size_t& index);
BencodedValuePtr parse_dictionary(const std::string& encoded_value, std::size_t& index);
BencodedValuePtr decode_bencoded_value(const std::string& encoded_value, std::size_t& index);
BencodedValuePtr decode_bencoded_value(const std::string& encoded_value);
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
