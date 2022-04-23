#include <server.h>
#include <iostream>

Server::Server(io::io_context &io_context, const tcp::endpoint &endpoint, const std::string &roomNames) : 
    acceptor(io_context, endpoint),
    socket(io_context),
    chat(roomNames)
{
    std::cout << "Server is running...\n";
    do_accept();
}

void Server::do_accept()
{
    acceptor.async_accept(socket, [this](const boost::system::error_code &ec)
    {
        if(!ec)
        {
            std::make_shared<Session>(std::move(socket), chat)->start();
            
        }
        else
        {
            std::cout << ec.what() << std::endl;
        } 
        
        do_accept(); 
    });
}
