#include <chat.h>
#include <iostream>
#include <sstream>

//TODO: check mutexes on correct

Chat::Chat(const std::string &roomNames)
{
    std::stringstream ss(roomNames);

    std::string roomName;
    while (ss)
    {
        ss >> roomName;
        rooms[roomName];
    }

}

bool Chat::addUser(chatParticipant_ptr participant_ptr, const std::string &username)
{
    std::unique_lock<std::shared_mutex> uniqueLock(usernamesMutex);
    auto [it, success] = usernames.emplace(std::make_pair(username, participant_ptr));

    return success;
}

void Chat::removeUser(chatParticipant_ptr participant_ptr)
{
    std::unique_lock<std::shared_mutex> uniqueLock(usernamesMutex);
    usernames.erase(participant_ptr->username());
}

bool Chat::joinToRoom(chatParticipant_ptr participant_ptr, const std::string &roomName)
{
    std::unique_lock<std::shared_mutex> uniqueLock(roomsMutex);
    auto it = rooms.find(roomName);

    if (it != rooms.end())
    {
        it->second.emplace(participant_ptr); 

        return true;
    }

    return false;
}

void Chat::leaveRoom(chatParticipant_ptr participant_ptr)
{
    std::unique_lock<std::shared_mutex> uniqueLock(roomsMutex);
    rooms.at(participant_ptr->roomname()).erase(participant_ptr);
}

void clearChat()
{
    
}

void Chat::deliverMessage(chatParticipant_ptr sender_ptr, std::string message)
{
    std::shared_lock<std::shared_mutex> sharedLock(roomsMutex);
    for (auto participant_ptr : rooms.at(sender_ptr->roomname()))
    {
        if (participant_ptr != sender_ptr){
            participant_ptr->deliverMessage(message);
        }
    }
}

std::string Chat::getRooms() const
{
    std::string roomNames;
    std::unique_lock<std::shared_mutex> sharedLock(roomsMutex);
    for (auto [roomName, participants] : rooms)
    {
            roomNames += roomName + "(" + std::to_string(participants.size()) + ")  ";
    }

    return roomNames;
}

std::string Chat::roomParticipants(const std::string &roomName) const
{
    std::string res;
    std::unique_lock<std::shared_mutex> sharedLock(roomsMutex);
    for (auto participant : rooms.at(roomName))
        res += participant->username() + " ";

    return res;
}