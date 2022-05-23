#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <memory>

class ChatParticipant
{
public:
    virtual ~ChatParticipant() = default;
    virtual void deliverMessage(const std::string &message) = 0;
    virtual std::string roomname() const = 0;
    virtual std::string username() const = 0;
};

using chatParticipant_ptr = std::shared_ptr<ChatParticipant>;

class Chat
{
public:
    Chat(const std::string &roomNames);
    void clearChat();
    bool addUser(chatParticipant_ptr participant_ptr, const std::string &username);
    void removeUser(chatParticipant_ptr participant_ptr);
    bool joinToRoom(chatParticipant_ptr participant_ptr, const std::string &roomName);
    void leaveRoom(chatParticipant_ptr);
    void deliverMessage(chatParticipant_ptr sender_ptr, std::string message);    
    std::string getRooms() const;
    std::string roomParticipants(const std::string &roomName) const;
    
private:
    std::unordered_map<std::string, chatParticipant_ptr> usernames;
    std::unordered_map<std::string,std::unordered_set<chatParticipant_ptr>> rooms;
    mutable std::shared_mutex usernamesMutex;
    mutable std::shared_mutex roomsMutex;
};