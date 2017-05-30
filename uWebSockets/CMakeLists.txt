cmake_minimum_required (VERSION 2.8.1)
set(CMAKE_CXX_STANDARD 11)
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
cmake_policy(SET CMP0015 NEW) #for relative paths in link_directories

project (uWS)

include_directories(
src
)

add_library(uWS STATIC
src/Extensions.cpp
src/Group.cpp
src/Networking.cpp
src/Hub.cpp
src/Node.cpp
src/WebSocket.cpp
src/HTTPSocket.cpp
src/Socket.cpp
src/Epoll.cpp
)

# link this with your binary
#pthread
#ssl
#crypto
#z
#uv

install(TARGETS uWS
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
