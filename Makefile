.PHONY: all build install clean

all: build

build:
	@echo "Configuring and building project..."
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build

install: build
	@echo "Installing smart_terminal..."
	# Standard CMake install routine (requires sudo if installing to /usr/local)
	cmake --install build

clean:
	@echo "Cleaning build directory..."
	rm -rf build
