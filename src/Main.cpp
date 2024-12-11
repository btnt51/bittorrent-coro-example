#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

json parse_string(std::string& encoded_value) {
    // Example: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(':');
    if (colon_index != std::string::npos) {
        std::string number_string = encoded_value.substr(0, colon_index);
        int64_t number = std::atoll(number_string.c_str());
        std::string str = encoded_value.substr(colon_index + 1, number);
        encoded_value.erase(0, colon_index + 1 + number);
        return json(str);
    } else {
        throw std::runtime_error("Invalid encoded value: " + encoded_value);
    }
}

json parse_integer(std::string& encoded_value) {
    auto it = encoded_value.find('e');
    if (it == std::string::npos || encoded_value[it] != 'e') {
        throw std::runtime_error("Invalid encoded value: " + encoded_value);
    }
    std::string number_string = encoded_value.substr(1, it);
    int64_t number = std::atoll(number_string.c_str());
    encoded_value.erase(0, it+1);
    return json(number);
}

json parse_list(const std::string& encoded_value) {
    auto temp = encoded_value.substr(0, encoded_value.size()-1);
    json result;
    while (not temp.empty()) {
        if (std::isdigit(temp[0])) {
            result.push_back(parse_string(temp));
            continue;
        }
        if (temp[0] == 'i') {
            result.push_back(parse_integer(temp));
            temp.erase(0,temp.find('e'));
            continue;
        }
    }
    return result;
}

json decode_bencoded_value(const std::string& encoded_value) {
    switch (encoded_value[0]) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
        auto temp = encoded_value.substr(1);
        return parse_string(temp);
        break;
    }
    case 'i': {
        auto temp = encoded_value.substr(1);
        return parse_integer(temp);
        break;
    }
    case 'l': {
        auto temp = encoded_value.substr(1, encoded_value.size()-1);
        return parse_list(temp);
        break;
    }
    default:
        throw std::runtime_error("Unhandled encoded value: " + encoded_value);
    }
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
        std::cout << (decoded_value.dump() == "null" ? "[]" : decoded_value.dump())  << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
