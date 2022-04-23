# Chat server project
This repo is one thread implementation chat server on Boost::Asio. For process message from participants is used pattern State machine.

For chat starting need choose username that is not used. Server have rooms and chat participants are free to move around the rooms.

## Commands

* command start with charachter /
* in case wrong command, client recieve message about worng command

list of commands:

  1. /help - list of available commands
   
  2. /login &lt;nickname&gt; - sing in to the chat
   
  3. /logout - exit in order start with the new username
   
  4. /exit - exit from the chat
   
  5. /goto &lt;roomname&gt; - go to the selected room
   
  6. /leave - leave from the current room
   
## Session states
* login state - available commands: help, login, exit
  
* select chat - available commands: help, goto, exit
  
* in chat - available commands: help, leave, exit and any message that doesn't match with command will be like a simple message

## Message structure

* header (8 byte) - string representation of data size in hex
* data - string data
  
## Config file

The configuration file specifies the port (default 9999) and the list of rooms (default "general").

In case config file name differs from default name (config.txt), use option --config &lt;filename&gt;

```
$ ./chat_client --config myconfig.txt
```
