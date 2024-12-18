#ifndef TORRENT_FILE_H
#define TORRENT_FILE_H
#include <cstdint>
#include <string>

#include "coder.hpp"

struct Info {
    Info() = default;
    std::size_t length;
    std::string name;
    size_t piece_length;
    std::string pieces;

    Info(const nlohmann::json& j);

    std::string get_info_hash_hex() const;

    std::string get_info_hash_bin() const;


    std::string get_pieces_hash() const;
private:
    std::string to_bencoded_string() const;
};

class torrent_file {
public:
    torrent_file(const nlohmann::json& json);

    torrent_file(const std::filesystem::path& path);

    const Info& get_info() const;
    Info& get_info();
    std::size_t get_file_length() const;
    std::string_view get_announce() const;

private:
    void initialize_from_json(const nlohmann::json& json);
    std::string announce;
    Info info;
};




#endif //TORRENT_FILE_H
