all: bin/client

bin/client: src/client/client.cpp src/common/ShoppingItem.cpp src/common/ShoppingList.cpp
		g++ -Iinclude src/client/client.cpp src/common/ShoppingItem.cpp src/common/ShoppingList.cpp -o bin/client

.PHONY: clean
clean: 
		rm -f bin/client

.PHONY: run
run:
		./bin/client
