cmake_minimum_required(VERSION 3.30)
project(g14)

# Set the C++ standard to C++17 (or higher if needed)
set(CMAKE_CXX_STANDARD 14)

# Find ZeroMQ package
find_package(ZeroMQ REQUIRED)

# Include directories for common headers
include_directories(include)
include_directories(common)

# Add the common source files that both client and server will use
add_library(common
        common/PNCounter.cpp
        common/ShoppingItem.cpp
        common/ShoppingList.cpp
        common/messages.hpp
)

# Create the client executable
add_executable(client
        src/client/client.cpp
)

# Link the client executable with ZeroMQ and the common library
target_link_libraries(client common ZeroMQ::ZeroMQ)

# Create the server executable
add_executable(server
        src/server/server.cpp
)

# Link the server executable with ZeroMQ and the common library
target_link_libraries(server common ZeroMQ::ZeroMQ)

# Optionally, add more configurations or tests if needed
