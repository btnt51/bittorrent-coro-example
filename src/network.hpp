#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "torrent_file.hpp"
#include "utility.hpp"

namespace network {

class peer_connection {
public:
    peer_connection(boost::asio::any_io_executor&, boost::asio::ip::tcp::socket&, boost::asio::ip::tcp::endpoint&);

private:
    boost::asio::any_io_executor io_context;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::endpoint endpoint_;
    std::string peer_id;
};

class torrent_session {
public:
    torrent_session(boost::asio::io_service& io_service, const torrent_file& file,
        std::optional<boost::asio::ip::tcp::endpoint> = {});

    boost::asio::awaitable<boost::asio::ip::tcp::endpoint> resolve_ip_by_host();

    boost::asio::awaitable<void> run();

    ~torrent_session();
private:
    boost::asio::awaitable<bool> add_peers();

    boost::asio::awaitable<std::vector<boost::asio::ip::tcp::endpoint>> get_peers();

    void prepare_http_request(boost::asio::streambuf&);

    void generate_peer_id();

    boost::asio::awaitable<boost::asio::ip::tcp::socket> connect_to_tracker();

    boost::asio::awaitable<void> send_http_request(boost::asio::ip::tcp::socket& socket);

    boost::asio::awaitable<std::string> read_http_response(boost::asio::ip::tcp::socket& socket);

    std::vector<boost::asio::ip::tcp::endpoint> parse_peers_from_body(const std::string& body);


    boost::asio::io_context& io_;
    boost::asio::any_io_executor executor_;
    std::string peer_id_;
    std::optional<boost::asio::ip::tcp::endpoint> announce_host_;
    torrent_file file_;
    std::vector<peer_connection> peers_;
};

class torrent_manager {
public:
    torrent_manager(boost::asio::io_service &io_service);

    boost::asio::awaitable<void> add_torrent(const torrent_file& file);

    ~torrent_manager();
private:
    boost::asio::io_service &io_service_;
    std::vector<std::shared_ptr<torrent_session>> sessions_;

    std::unordered_map<std::string, boost::asio::ip::basic_endpoint<boost::asio::ip::tcp>,
                            utility::TransparentHash, utility::TransparentEqual> dns_cache_;
};

} // network

#endif //NETWORK_HPP
