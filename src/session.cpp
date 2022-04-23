#include <session.h>
#include <iostream>
#include <iomanip>

Session::Session(tcp::socket &&socket, Chat &chat) : socket(std::move(socket)), chat(chat)
{

    commandDescriptions = std::unordered_map<std::string, std::string>{
        {"/help", " - list of available commands"},
        {"/login", " <nickname>  - sing in to the chat"},
        {"/logout", " - exit in order start with the new username"},
        {"/exit", " - exit from the chat"},
        {"/goto", " <roomname> - go to the selected room"},
        {"/leave", " - leave from the current room"}};
}

void Session::start()
{
    std::cout << "connection extablished with: " << socket.remote_endpoint() << std::endl;
    commandHandler = &Session::login;
    deliverMessage("Welcome to Chat!\n\n" +
                   createCommandDescriptions("/login"));
    do_read_header();
}

void Session::deliverMessage(const std::string &message)
{
    std::stringstream ss;
    ss << std::setw(header_length) << std::hex << message.size() << message;
    bool write_in_progress = !writeMessages.empty();
    writeMessages.push(ss.str());
    if (!write_in_progress)
    {
        do_write();
    }
}

std::string Session::roomname() const
{
    return roomName;
}

std::string Session::username() const
{
    return userName;
}

std::vector<std::string> Session::split(const std::string &s, int picies)
{
    std::vector<std::string> res;
    size_t i = 0, j = s.find_first_of(' '), p = 1;
    while (j != std::string::npos && p < picies)
    {
        res.emplace_back(s.substr(i, j - i));
        i = j + 1;
        j = s.find_first_of(' ', i);
        ++p;
    }
    res.emplace_back(s.substr(i));
    return res;
}

void Session::do_read_header()
{
    auto self = shared_from_this();

    io::async_read(socket, io::buffer(header),
    [this, self](const boost::system::error_code &ec, std::size_t)
    {
        if (!ec)
        {
            do_read_body();
            
        }
        else
        {
            endSession();
        }
    });
}

void Session::do_read_body()
{
    auto self = shared_from_this();
    size_t incomingDataSize;
    std::stringstream ss(std::string(header, header_length));
    ss >> std::hex >> incomingDataSize;
    incomingData.resize(incomingDataSize);
    io::async_read(socket, io::buffer(incomingData),
    [this, self](const boost::system::error_code &ec, std::size_t)
    {
        if (!ec)
        {
            std::string messageRetrieved(incomingData.begin(), incomingData.end());
            auto parsedData = split(messageRetrieved);
            if (!parsedData.empty())
                (this->*commandHandler)(parsedData);
            do_read_header();
        }
        else
        {
            endSession();
        }
    });
}

void Session::do_write()
{
    auto self = shared_from_this();
    io::async_write(socket, io::buffer(writeMessages.front()),
    [this, self](const boost::system::error_code &ec, size_t)
    {
        if (!ec)
        {
            writeMessages.pop();
            if (!writeMessages.empty())
            {
                do_write();
            }
        }
        else
        {
            endSession();
        }
    });
}

void Session::endSession()
{
    if (!roomName.empty()){
        chat.deliverMessage(shared_from_this(), "<< " + userName + " leave the chat >>");
        chat.leaveRoom(shared_from_this());
    }
    if (!userName.empty())
        chat.removeUser(shared_from_this());

    std::cout << "connection lost with: " << socket.remote_endpoint() << std::endl;
}
 
template <typename... Args>
std::string Session::createCommandDescriptions(Args... args)
{
    return ("/help" + commandDescriptions["/help"] + "\n") + 
           ((args + commandDescriptions[args] + "\n") + ...) +
           ("/exit" + commandDescriptions["/exit"] + "\n");
}

void Session::login(const std::vector<std::string> &parsedData)
{
    if (parsedData[0] == "/login" && parsedData.size() == 2)
    {
        if (chat.addUser(shared_from_this(), parsedData[1]))
        {
            userName = parsedData[1];
            deliverMessage("Select room:\n" + chat.getRooms() + "\n\n" +
                           createCommandDescriptions("/goto"));
            commandHandler = &Session::selectChat;
        }
        else
        {
            deliverMessage("This nickname is already used, choose another");
        }
    }
    else if (parsedData[0] == "/help")
    {
        deliverMessage(createCommandDescriptions("/login"));
    }
    else
    {
        deliverMessage("Wrong command, try command /help");
    }
}

void Session::selectChat(const std::vector<std::string> &parsedData)
{
    if (parsedData[0] == "/goto" && parsedData.size() == 2)
    {
        if (chat.joinToRoom(shared_from_this(), parsedData[1]))
        {
            roomName = parsedData[1];
            deliverMessage("Welcome to room " + parsedData[1] +
                           "\nparticipants: " + chat.roomParticipants(parsedData[1]) + "\n\n" +
                           createCommandDescriptions("/leave"));
            chat.deliverMessage(shared_from_this(), "<< " + userName + " join to the chat >>");
            commandHandler = &Session::inChat;
        }
        else
        {
            deliverMessage("Such room doesn't exist");
        }
    }
    else if (parsedData[0] == "/logout")
    {
        chat.removeUser(shared_from_this());
        deliverMessage("Welcome to Chat!\n\n" +
                       createCommandDescriptions("/login"));
        userName = "";
        commandHandler = &Session::login;
    }
    else if (parsedData[0] == "/help")
    {
        deliverMessage(createCommandDescriptions("/goto", "/logout"));
    }
    else
    {
        deliverMessage("Wrong command, try command: /help");
    }
}

void Session::inChat(const std::vector<std::string> &parsedData)
{
    if (parsedData[0] == "/leave")
    {
        chat.deliverMessage(shared_from_this(), "<< " + userName + " leave the chat >>");
        chat.leaveRoom(shared_from_this());
        roomName = "";
        deliverMessage("Select room:\n" + chat.getRooms() + "\n\n" +
                       createCommandDescriptions("/goto", "/logout"));
        commandHandler = &Session::selectChat;
    }
    else if (parsedData[0] == "/help")
    {
        deliverMessage(createCommandDescriptions("/leave"));
    }
    else
    {
        chat.deliverMessage(shared_from_this(), "<" + userName + "> " +
                                                    (parsedData.size() > 1 ? parsedData[0] + ' ' + parsedData[1] : parsedData[0]));
    }
}
