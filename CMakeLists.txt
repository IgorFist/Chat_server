cmake_minimum_required( VERSION 3.0 )

project(chat_server)

find_package(Boost COMPONENTS system thread program_options REQUIRED)

include_directories( ./include ./src)


add_executable(chat_server ./src/main.cpp ./src/server.cpp ./src/session.cpp ./src/chat.cpp)

set_target_properties(chat_server PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(chat_server 
    ${Boost_LIBRARIES}
)

