#pragma once

#include <chat.h>
#include <session.h>
#include <vector>
#include <thread>

class Server
{
public:
    Server(io::io_context& io_context);
    ~Server();
    void start(uint16_t port, const std::string &roomNames, int amountThreads = std::thread::hardware_concurrency());
    void stop();    
private:
    void do_accept();

    io::io_context &io_context;
    tcp::socket socket;
    std::unique_ptr<tcp::acceptor> acceptor_ptr;    
    std::unique_ptr<Chat> chat_ptr;
    std::vector<std::thread> threads;
};