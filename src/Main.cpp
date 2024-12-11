#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"

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

json decode_bencoded_value(const std::string& encoded_value, std::size_t& index);

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

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }
        // You can use print statements as follows for debugging, they'll be visible when running tests.
        std::cerr << "Logs from your program will appear here!" << std::endl;

        // Uncomment this block to pass the first stage
        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded_value(encoded_value);
        std::cout << decoded_value.dump()  << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
