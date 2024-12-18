#ifndef UTILITY_HPP
#define UTILITY_HPP
#include <string>

#include "coder.hpp"

namespace utility {
struct TransparentHash {
    using is_transparent = void;

    std::size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }

    std::size_t operator()(const std::string &s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
};

struct TransparentEqual {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
        return lhs == rhs;
    }

    bool operator()(const std::string &lhs, const std::string &rhs) const noexcept {
        return lhs == rhs;
    }

    bool operator()(const std::string &lhs, std::string_view rhs) const noexcept {
        return lhs == rhs;
    }

    bool operator()(std::string_view lhs, const std::string &rhs) const noexcept {
        return lhs == rhs;
    }
};

namespace http {
inline std::string extract_host_from_announce(const std::string& announce_url) {
    // Пример: "http://tracker.example.com:6969/announce"
    // Шаг 1: найти `://`
    auto pos = announce_url.find("://");
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid announce URL: missing scheme");
    }

    // Начинаем после "://"
    auto start = pos + 3;
    // Шаг 2: найти следующий '/'
    auto slash_pos = announce_url.find('/', start);
    if (slash_pos == std::string::npos) {
        // Нет / - странный URL, но допустим весь остаток это хост
        slash_pos = announce_url.size();
    }

    // Хост:порт в announce_url[start : slash_pos]
    std::string host_port = announce_url.substr(start, slash_pos - start);

    // Найдём двоеточие, указывающее на порт
    auto colon_pos = host_port.find(':');
    if (colon_pos != std::string::npos) {
        // хост до двоеточия
        return host_port.substr(0, colon_pos);
    } else {
        // Порт не указан, значит весь host_port - это хост
        return host_port;
    }
}

inline std::vector<boost::asio::ip::tcp::endpoint> parse_compact_peers(const std::string& body) {
    std::vector<boost::asio::ip::tcp::endpoint> endpoints;

    auto res = decoder::to_string::decode_bencoded_value(body);
    if (auto dict = std::get_if<std::unordered_map<std::string, decoder::to_string::BencodedValuePtr>>(&res->value)) {
        if (dict->contains("peers")) {
            if (auto compact_peers = std::get_if<std::string>(&dict->at("peers")->value)) {
                if (compact_peers->size() % 6 != 0) {
                    throw std::runtime_error("Invalid compact peer list size");
                }
                for (size_t i = 0; i < compact_peers->size(); i += 6) {
                    unsigned char ip1 = (*compact_peers)[i];
                    unsigned char ip2 = (*compact_peers)[i + 1];
                    unsigned char ip3 = (*compact_peers)[i + 2];
                    unsigned char ip4 = (*compact_peers)[i + 3];
                    unsigned short port = (static_cast<unsigned char>((*compact_peers)[i + 4]) << 8) |
                                          static_cast<unsigned char>((*compact_peers)[i + 5]);

                    boost::asio::ip::address ip = boost::asio::ip::address_v4({ip1, ip2, ip3, ip4});
                    endpoints.emplace_back(ip, port);
                }
            }
        }
    }


    return endpoints;
}

inline std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (unsigned char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << int(c);
        }
    }

    return escaped.str();
}

}

}
#endif //UTILITY_HPP
