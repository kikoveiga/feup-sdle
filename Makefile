# Diretórios
SRC_DIR = src
BIN_DIR = bin
COMMON_DIR = $(SRC_DIR)/common
INCLUDE_DIR = include

CXX = g++
CXXFLAGS = -I$(INCLUDE_DIR) -std=c++17 -Wall -Wextra
LDFLAGS = -lzmq  

COMMON_SRC = $(COMMON_DIR)/ShoppingItem.cpp $(COMMON_DIR)/ShoppingList.cpp

CLIENT = $(BIN_DIR)/client
SERVER = $(BIN_DIR)/server

all: $(CLIENT) $(SERVER)

$(CLIENT): $(SRC_DIR)/client/client.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(SERVER): $(SRC_DIR)/server/server.cpp $(COMMON_SRC)
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
