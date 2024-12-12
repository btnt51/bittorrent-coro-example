#include "torrent_file.hpp"

#include "sha_1.h"


Info::Info(const nlohmann::json& j) : length(j["length"]), name(j["name"]),
                                      pieces(j["pieces"]), piece_length(j["piece length"]) {}

std::string Info::get_info_hash() const {return crypto::sha_1(to_bencoded_string());}

std::string Info::get_pieces_hash() const {
    std::string res;
    for (auto i = 0; i < pieces.size(); i +=20) {
        res.append(crypto::utility::to_hex_string(reinterpret_cast<uint8_t*>(pieces.substr(i, 20).data()), 20)+"\n");
    }
    return res;
}

std::string Info::to_bencoded_string() const {
    return "d" + coder::to_string::encode_pair("length", length) +
        coder::to_string::encode_pair("name", name) +
        coder::to_string::encode_pair("piece length", piece_length) +
        coder::to_string::encode_pair("pieces", pieces)  + "e";
}

torrent_file::torrent_file(const nlohmann::json& json) : announce(json["announce"]), info(json["info"]) {}


const Info& torrent_file::get_info() const {
    return info;
}

Info& torrent_file::get_info() {
    return info;
}

std::size_t torrent_file::get_info_length() const {
    return info.length;
}

std::string_view torrent_file::get_announce() const {
    return announce;
}

