# Diretórios
SRC_DIR = src
BIN_DIR = bin
COMMON_DIR = $(SRC_DIR)/common
INCLUDE_DIR = include

# Usando pkg-config para as flags de compilação e linkagem
CXXFLAGS = $(shell pkg-config --cflags libmongocxx) -I$(INCLUDE_DIR)
LDFLAGS = $(shell pkg-config --libs libmongocxx) -lzmq

# Compilador
CXX = g++

# Arquivos fonte
COMMON_SRC = $(COMMON_DIR)/ShoppingItem.cpp \
             $(COMMON_DIR)/ShoppingList.cpp \
             $(COMMON_DIR)/PNCounter.cpp \
             $(COMMON_DIR)/Message.cpp

# Alvos
CLIENT = $(BIN_DIR)/client
SERVER = $(BIN_DIR)/server
MAIN = $(BIN_DIR)/main

# Compilação de todos os alvos
all: $(CLIENT) $(SERVER) $(MAIN)

# Client
$(CLIENT): $(SRC_DIR)/client/Client.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Server
$(SERVER): $(SRC_DIR)/server/Server.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Main
$(MAIN): $(SRC_DIR)/main.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Limpeza dos binários
.PHONY: clean
clean:
	rm -f $(BIN_DIR)/*

# Executar o client
.PHONY: run-client
run-client: $(CLIENT)
	$(CLIENT)

# Executar o server
.PHONY: run-server
run-server: $(SERVER)
	$(SERVER)

# Garantir que as bibliotecas MongoDB e ZeroMQ sejam encontradas pelo linker em tempo de execução
.PHONY: set-library-path
set-library-path:
	export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
