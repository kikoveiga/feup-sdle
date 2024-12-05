cmake_minimum_required(VERSION 3.30)

project(g14)

set(CMAKE_CXX_STANDARD 17)

# Add external libraries (zmqpp in your case)
add_subdirectory(external/zmqpp)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(external/zmqpp/src)

# Add common sources
set(COMMON_SOURCES
    src/common/PNCounter.cpp
    src/common/ShoppingItem.cpp
    src/common/ShoppingList.cpp
)

# Add executables
add_executable(client src/client/client.cpp ${COMMON_SOURCES})
add_executable(server src/server/server.cpp ${COMMON_SOURCES})
add_executable(main src/main.cpp ${COMMON_SOURCES})

# Link libraries
target_link_libraries(client PRIVATE zmqpp)
target_link_libraries(server PRIVATE zmqpp)
target_link_libraries(main PRIVATE zmqpp)
