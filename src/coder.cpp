#include "coder.hpp"

namespace decoder {

namespace to_json {
using json = nlohmann::json;

json parse_string(const std::string& encoded_value, std::size_t& index) {
    // Example: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(':', index);
    if (colon_index != std::string::npos) {
        std::string number_string = encoded_value.substr(index, colon_index-index);
        int64_t number = std::atoll(number_string.c_str());
        std::string str = encoded_value.substr(colon_index + 1, number);
        index = colon_index + 1 + number;
        return json(str);
    } else {
        throw std::runtime_error("Invalid encoded value: " + encoded_value);
    }
}

json parse_integer(const std::string& encoded_value, std::size_t& index) {
    index++;
    auto end = encoded_value.find('e', index);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid encoded value: " + encoded_value);
    }
    std::string number_string = encoded_value.substr(index, end - index);
    index = end + 1;
    return json(std::stoll(number_string));
}


json parse_list(const std::string& encoded_value, size_t& index) {
    json array = json::array();
    index++;
    while (encoded_value[index] != 'e') {
        array.push_back(decode_bencoded_value(encoded_value, index));
    }
    index++;
    return array;
}

json parse_dictionary(const std::string& encoded_value, std::size_t& index) {
    auto dict = json::object();
    index++;
    while (encoded_value[index] != 'e') {
        auto temp = decode_bencoded_value(encoded_value, index).dump();
        auto name = temp.substr(1, temp.size()-2);
        dict[name] = decode_bencoded_value(encoded_value, index);
    }
    index++;
    return dict;
}

json decode_bencoded_value(const std::string& encoded_value, std::size_t& index) {
    if (std::isdigit(encoded_value[index])) {
        return parse_string(encoded_value, index);
    } else if (encoded_value[index] == 'i') {
        return parse_integer(encoded_value, index);
    } else if (encoded_value[index] == 'l') {
        return parse_list(encoded_value, index);
    } else if (encoded_value[index] == 'd') {
        return parse_dictionary(encoded_value, index);
    } else {
        throw std::runtime_error("Unhandled encoded value: " + encoded_value);
    }
}

json decode_bencoded_value(const std::string& encoded_value) {
    size_t position = 0;
    return decode_bencoded_value(encoded_value, position);
}

} // to_json
namespace to_string {

BencodedValuePtr parse_string(const std::string& encoded_value, std::size_t& index) {
    // Пример: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(':', index);
    if (colon_index == std::string::npos) {
        throw std::runtime_error("Invalid bencoded string: " + encoded_value);
    }

    std::string length_str = encoded_value.substr(index, colon_index - index);
    size_t length = std::stoul(length_str);

    index = colon_index + 1; // Переходим к началу строки
    if (index + length > encoded_value.size()) {
        throw std::runtime_error("String length exceeds input size");
    }

    std::string result = encoded_value.substr(index, length);
    index += length; // Обновляем индекс
    return std::make_shared<BencodedValue>(result);
}

BencodedValuePtr parse_integer(const std::string& encoded_value, std::size_t& index) {
    if (encoded_value[index] != 'i') {
        throw std::runtime_error("Expected integer (i)");
    }
    index++; // Пропускаем 'i'
    size_t end = encoded_value.find('e', index);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid bencoded integer format");
    }

    std::string number_str = encoded_value.substr(index, end - index);
    int64_t number = std::stoll(number_str);

    index = end + 1; // Пропускаем 'e'
    return std::make_shared<BencodedValue>(number);
}

BencodedValuePtr parse_list(const std::string& encoded_value, std::size_t& index) {
    if (encoded_value[index] != 'l') {
        throw std::runtime_error("Expected list (l)");
    }
    index++; // Пропускаем 'l'

    std::vector<BencodedValuePtr> result;
    while (encoded_value[index] != 'e') {
        result.push_back(decode_bencoded_value(encoded_value, index));
    }

    index++; // Пропускаем 'e'
    return std::make_shared<BencodedValue>(result);
}

BencodedValuePtr parse_dictionary(const std::string& encoded_value, std::size_t& index) {
    if (encoded_value[index] != 'd') {
        throw std::runtime_error("Expected dictionary (d)");
    }
    index++; // Пропускаем 'd'

    std::unordered_map<std::string, BencodedValuePtr> result;
    while (encoded_value[index] != 'e') {
        std::string key = std::get<std::string>(parse_string(encoded_value, index)->value);
        BencodedValuePtr value = decode_bencoded_value(encoded_value, index);
        result[key] = value;
    }

    index++; // Пропускаем 'e'
    return std::make_shared<BencodedValue>(result);
}

BencodedValuePtr decode_bencoded_value(const std::string& encoded_value, std::size_t& index) {
    if (std::isdigit(encoded_value[index])) {
        return parse_string(encoded_value, index);
    } else if (encoded_value[index] == 'i') {
        return parse_integer(encoded_value, index);
    } else if (encoded_value[index] == 'l') {
        return parse_list(encoded_value, index);
    } else if (encoded_value[index] == 'd') {
        return parse_dictionary(encoded_value, index);
    } else {
        throw std::runtime_error("Unhandled bencoded value type: " + encoded_value.substr(index, 1));
    }
}

BencodedValuePtr decode_bencoded_value(const std::string& encoded_value) {
    size_t index = 0;
    return decode_bencoded_value(encoded_value, index);
}
} // to_string

}


namespace coder {
namespace to_string {
std::string encode(const std::string& val) {
    return std::to_string(val.length()) + ":" + val;
}
}
}
