.PHONY: run build clean

SRC = src/main.cc src/socket.cc
OBJ = build/main.o build/socket.o
TARGET = build/main

run: $(TARGET)
	@./$(TARGET)

$(TARGET): $(OBJ)
	@g++ $(OBJ) -o $(TARGET)

build/main.o: src/main.cc
	@mkdir -p build
	@g++ -c $< -o $@ -I src/

build/socket.o: src/socket.cc
	@mkdir -p build
	@g++ -c $< -o $@ -I src/

clean:
	@rm -rf build/
