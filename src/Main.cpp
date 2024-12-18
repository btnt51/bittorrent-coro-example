#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <boost/asio.hpp>

#include "coder.hpp"
#include "network.hpp"
#include "sha_1.h"
#include "torrent_file.hpp"
#include "lib/nlohmann/json.hpp"

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
        nlohmann::json decoded_value = decoder::to_json::decode_bencoded_value(encoded_value);
        std::cout << decoded_value.dump()  << std::endl;
    } else if (command == "info") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " info <encoded_value>" << std::endl;
            return 1;
        }
        std::filesystem::path filePath = argv[2];
        torrent_file tor_file(filePath);
        std::cout << "Tracker URL: " << tor_file.get_announce() << std::endl;
        std::cout << "Length: " << tor_file.get_file_length() << std::endl;
        std::cout << "Info Hash: " << tor_file.get_info().get_info_hash_hex() << std::endl;
        std::cout << "Piece Length: " << tor_file.get_info().piece_length << std::endl;
        std::cout << "Piece Hashes:\n" << tor_file.get_info().get_pieces_hash();
    } else if (command == "peers") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " info <encoded_value>" << std::endl;
            return 1;
        }
        std::filesystem::path filePath = argv[2];
        torrent_file tor_file(filePath);
        boost::asio::io_context io_context;
        network::torrent_manager torrent_manager(io_context);
        boost::asio::co_spawn(
            io_context,
            [&torrent_manager, &tor_file]() -> boost::asio::awaitable<void> {
                co_await torrent_manager.add_torrent(tor_file);
                co_return;
            },
            boost::asio::detached
        );

        // Запускаем io_context
        io_context.run();

    }else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
