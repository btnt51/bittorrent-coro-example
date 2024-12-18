#include "network.hpp"

#include <iostream>
#include <random>
#include <boost/asio.hpp>
namespace network {
torrent_session::torrent_session(boost::asio::io_context& io_context, const torrent_file& file,
    std::optional<boost::asio::ip::tcp::endpoint> endpoint) : io_(io_context), file_(file), announce_host_(endpoint) {
    generate_peer_id();
}

boost::asio::awaitable<boost::asio::ip::tcp::endpoint> torrent_session::resolve_ip_by_host() {
    boost::asio::ip::tcp::resolver resolver(io_);
    try {
        auto results = co_await resolver.async_resolve(utility::http::extract_host_from_announce(std::string{file_.get_announce()}), "", boost::asio::use_awaitable);
        if (results.empty()) {
            throw std::runtime_error("No endpoints resolved for host");
        }
        announce_host_ = *results.begin();
        co_return *announce_host_;
    } catch (const std::exception& ex) {
        throw std::runtime_error("Failed to resolve host: " + std::string(ex.what()));
    }
}

boost::asio::awaitable<void> torrent_session::run() {
    co_await add_peers();
}

torrent_session::~torrent_session() {
}

boost::asio::awaitable<bool> torrent_session::add_peers() {
    auto peers = co_await get_peers();
    std::cout << "peers size: " << peers.size() << std::endl;
    for (const auto& peer : peers) {
            std::cout << peer << "\n";
    }
}

boost::asio::awaitable<boost::asio::ip::tcp::socket> torrent_session::connect_to_tracker() {
    boost::asio::ip::tcp::socket socket(io_);
    std::cout << "xyi1 announce_host: " << *announce_host_ << std::endl;
    co_await socket.async_connect(*announce_host_, boost::asio::use_awaitable);

    if (!socket.is_open()) {
        throw std::runtime_error("Socket is not open. Failed to connect to tracker.");
    }
    co_return std::move(socket);
}

boost::asio::awaitable<void> torrent_session::send_http_request(boost::asio::ip::tcp::socket& socket) {
    boost::asio::streambuf request;
    prepare_http_request(request);

    co_await boost::asio::async_write(socket, request, boost::asio::use_awaitable);
}

boost::asio::awaitable<std::string> torrent_session::read_http_response(boost::asio::ip::tcp::socket& socket) {
    boost::asio::streambuf response;
    co_await boost::asio::async_read_until(socket, response, "\r\n\r\n", boost::asio::use_awaitable);
    std::istream response_stream(&response);
    std::string line;

    // Skip headers
    while (std::getline(response_stream, line) && line != "\r");

    // Read body
    std::string body((std::istreambuf_iterator<char>(response_stream)), std::istreambuf_iterator<char>());
    co_return body;
}

std::vector<boost::asio::ip::tcp::endpoint> torrent_session::parse_peers_from_body(const std::string& body) {
    if (body.empty()) {
        return {};
    }
    return utility::http::parse_compact_peers(body);
}

boost::asio::awaitable<std::vector<boost::asio::ip::tcp::endpoint>> torrent_session::get_peers() {
    if (not announce_host_) {
        co_await resolve_ip_by_host();
    }

    // Connect to tracker
    boost::asio::ip::tcp::socket socket = co_await connect_to_tracker();

    // Send HTTP request
    co_await send_http_request(socket);

    // Read response
    std::string body = co_await read_http_response(socket);
    std::cout << body << std::endl;

    co_return parse_peers_from_body(body);;
}

void torrent_session::prepare_http_request(boost::asio::streambuf& request) {
    std::ostream request_stream(&request);
    std::string query_str = "/announce?info_hash=" + file_.get_info().get_info_hash_bin() +
                            "&peer_id=" + peer_id_ +
                            "&port=6881" +
                            "&uploaded=0" +
                            "&downloaded=0" +
                            "&left=" + std::to_string(file_.get_file_length()) +
                            "&compact=1";
    request_stream << "GET " << query_str << " HTTP/1.1\r\n";  // note that you can change it if you wish to HTTP/1.0
    request_stream << "Host: " << utility::http::extract_host_from_announce(std::string{file_.get_announce()}) << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
}

void torrent_session::generate_peer_id() {
    static const char charset[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

    peer_id_ = "-PC0001-";
    for (int i = 0; i < 12; ++i) {
        peer_id_ += charset[dist(rng)];
    }
}

torrent_manager::torrent_manager(boost::asio::io_service& io_service) : io_service_(io_service) {
}

boost::asio::awaitable<void> torrent_manager::add_torrent(const torrent_file& file) {
    std::optional<boost::asio::ip::tcp::endpoint> endpoint;
    if (auto it = dns_cache_.find(file.get_announce()); it != dns_cache_.end()) {
        endpoint = it->second;
    }
    auto tor_session = std::make_shared<torrent_session>(io_service_, file, endpoint);
    if (endpoint == std::nullopt) {
        dns_cache_[std::string(file.get_announce())] = co_await tor_session->resolve_ip_by_host();
    }

    sessions_.push_back(tor_session);
    boost::asio::co_spawn(io_service_.get_executor(), tor_session->run(), boost::asio::detached);
}

torrent_manager::~torrent_manager() {
}
} // network