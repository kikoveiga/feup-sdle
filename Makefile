# Directories
SRC_DIR = src
BIN_DIR = bin
COMMON_DIR = $(SRC_DIR)/common
INCLUDE_DIR = include
MONGO_INCLUDE_DIR = /usr/local/include
MONGO_LIB_DIR = /usr/local/lib

CXX = g++
CXXFLAGS = -I$(INCLUDE_DIR) -I$(MONGO_INCLUDE_DIR)/mongocxx/v_noabi -I$(MONGO_INCLUDE_DIR)/bsoncxx/v_noabi -std=c++17 -Wall -Wextra
LDFLAGS = -L$(MONGO_LIB_DIR) -lmongocxx -lbsoncxx -lzmq

COMMON_SRC = $(COMMON_DIR)/ShoppingItem.cpp $(COMMON_DIR)/ShoppingList.cpp $(COMMON_DIR)/PNCounter.cpp $(COMMON_DIR)/Message.cpp

CLIENT = $(BIN_DIR)/client
SERVER = $(BIN_DIR)/server
MAIN = $(BIN_DIR)/main

all: $(CLIENT) $(SERVER) $(MAIN)

$(CLIENT): $(SRC_DIR)/client/Client.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(SERVER): $(SRC_DIR)/server/Server.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(MAIN): $(SRC_DIR)/main.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -f $(BIN_DIR)/*

.PHONY: run-client
run-client: $(CLIENT)
	$(CLIENT)

.PHONY: run-server
run-server: $(SERVER)
	$(SERVER)
