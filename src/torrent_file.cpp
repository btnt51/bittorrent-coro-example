#include "torrent_file.hpp"

#include <fstream>

#include "sha_1.h"


Info::Info(const nlohmann::json& j) : length(j["length"]), name(j["name"]),
                                      pieces(j["pieces"]), piece_length(j["piece length"]) {}

std::string Info::get_info_hash_hex() const {return crypto::sha_1_string(to_bencoded_string());}

std::string Info::get_info_hash_bin() const {return crypto::sha_1_binary_string(to_bencoded_string());}

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

torrent_file::torrent_file(const nlohmann::json& json) {
    initialize_from_json(json);
}

torrent_file::torrent_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    initialize_from_json(decoder::to_json::decode_bencoded_value(fileContent));
}

void torrent_file::initialize_from_json(const nlohmann::json& json) {
    announce = json.value("announce", "");
    info = {json["info"]};
}

const Info& torrent_file::get_info() const {
    return info;
}

Info& torrent_file::get_info() {
    return info;
}

std::size_t torrent_file::get_file_length() const {
    return info.length;
}

std::string_view torrent_file::get_announce() const {
    return announce;
}

