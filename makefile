run: build
	@./build/main

build:
	@mkdir -p build/
	@g++ src/main.cc -o build/main

clean:
	@rm -rf build/
