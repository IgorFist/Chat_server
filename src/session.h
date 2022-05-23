#pragma once

#include <chat.h>
#include <memory>
#include <vector>
#include <queue>
#include <boost/asio.hpp>

namespace io = boost::asio;
using namespace boost::asio::ip;

class Session : public ChatParticipant, public std::enable_shared_from_this<Session>
{
public:
    Session(io::io_context& io_socket,tcp::socket &&socket, Chat &chat);
    void start();
    void deliverMessage(const std::string &message) override;
    std::string roomname() const override;
    std::string username() const override;

private:
    // split message into command and args
    static std::vector<std::string> split(const std::string &s, int picies = 2);

    //read/write network data
    void do_read_header();
    void do_read_body();
    void do_write();

    void endSession();

    template <typename... Args>
    std::string createCommandDescriptions(Args... args);

    // session state
    // pointer on member function
    void (Session::*commandHandler)(const std::vector<std::string> &parsedData);
    // states
    void login(const std::vector<std::string> &parsedData);
    void selectChat(const std::vector<std::string> &parsedData);
    void inChat(const std::vector<std::string> &parsedData);

    io::io_context::strand writeStrand;
    tcp::socket socket;
    Chat &chat;
    std::queue<std::string> writeMessages;
    enum{header_length = 8};
    char header[header_length];
    std::vector<char> incomingData;
    std::unordered_map<std::string, std::string> commandDescriptions;

    std::string roomName;
    std::string userName;
};