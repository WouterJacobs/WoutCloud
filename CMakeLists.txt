cmake_minimum_required(VERSION 3.26)
project(WoutCloud C)

# Set C standard version
set(CMAKE_C_STANDARD 11)

# Source files for both server and client
set(SERVER_SOURCES src/main.c)
set(CLIENT_SOURCES src/Client.c)  # Assuming Client.c is in the src directory

# Add server executable
add_executable(WoutCloud ${SERVER_SOURCES})

# Target linkage for server (assuming Winsock)
target_link_libraries(WoutCloud ws2_32)

# Add client executable
add_executable(Client ${CLIENT_SOURCES})

# Target linkage for client (assuming Winsock)
target_link_libraries(Client Ws2_32 Mswsock AdvApi32)
