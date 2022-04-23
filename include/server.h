#pragma once

#include <chat.h>
#include <session.h>

class Server
{
public:
    Server(io::io_context &io_context, const tcp::endpoint &endpoint,const std::string &rooms);

private:
    void do_accept();

    tcp::acceptor acceptor;
    tcp::socket socket;
    Chat chat;
};