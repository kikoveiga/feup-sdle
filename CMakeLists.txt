cmake_minimum_required(VERSION 3.30)

project(g14)

set(CMAKE_CXX_STANDARD 14)

# Add external libraries (zmqpp in your case)
add_subdirectory(external/zmqpp)

# Include directories
include_directories(include)
include_directories(external/zmqpp/src)

# Add executables
add_executable(client src/client/client.cpp)
add_executable(server src/server/server.cpp)

# Link libraries
target_link_libraries(client PRIVATE zmqpp)
target_link_libraries(server PRIVATE zmqpp)

