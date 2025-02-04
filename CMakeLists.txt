cmake_minimum_required(VERSION 3.30)

project(g14)

set(CMAKE_CXX_STANDARD 17)

# Add external libraries (zmqpp in your case)
add_subdirectory(external/zmqpp)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(external/zmqpp/src)
include_directories(/usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi)

# Set MongoDB driver paths
set(CMAKE_PREFIX_PATH "/usr/local/lib/cmake/mongocxx-3.10.1" "/usr/local/lib/cmake/bsoncxx-3.10.1")
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

# Add common sources
set(COMMON_SOURCES
        src/common/CCounter.cpp
    src/common/ShoppingItem.cpp
    src/common/ShoppingList.cpp
    src/common/Message.cpp
)

set(SERVER_SOURCES
    src/server/Broker.cpp
    src/server/Worker.cpp
    src/server/Database.cpp
)

# Add executables
add_executable(client src/client/Client.cpp ${COMMON_SOURCES})
add_executable(main_broker src/server/main_broker.cpp ${COMMON_SOURCES} ${SERVER_SOURCES})
add_executable(main_worker src/server/main_worker.cpp ${COMMON_SOURCES} ${SERVER_SOURCES})
add_executable(main src/main.cpp ${COMMON_SOURCES})

# Link libraries
target_link_libraries(client PRIVATE zmqpp)
target_link_libraries(main_broker PRIVATE zmqpp mongocxx bsoncxx)
target_link_libraries(main_worker PRIVATE zmqpp mongocxx bsoncxx)
target_link_libraries(main PRIVATE zmqpp mongocxx bsoncxx)
