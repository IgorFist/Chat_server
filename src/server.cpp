#include <server.h>
#include <iostream>

Server::Server(io::io_context &io_context):
    io_context(io_context),
    socket(io_context)
{



}

Server::~Server()
{

}

void Server::start(uint16_t port,  const std::string &roomNames, int amountThreads)
{
    acceptor_ptr.reset(new tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), port)));
    chat_ptr.reset(new Chat(roomNames));
    std::cout << "Server is running...\n";
    do_accept();

    for(int i = 0; i < amountThreads; ++i)
        threads.emplace_back([this]()
        {
            io_context.run();
        });
}

void Server::stop()
{
    io_context.stop();

    for(auto &t: threads)
        t.join();
    
    std::cout << "Server is stopped...\n";
}  

void Server::do_accept()
{
    acceptor_ptr->async_accept(socket, [this](const boost::system::error_code &ec)
    {
        if(!ec)
        {
            std::make_shared<Session>(io_context, std::move(socket), *chat_ptr)->start();
            
        }
        else
        {
            std::cout << ec.what() << std::endl;
        } 
        
        do_accept(); 
    });
}
