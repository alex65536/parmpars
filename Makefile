all: build

build:
	g++ test.cpp -o test --std=c++11 -O2
	g++ test_macros.cpp -o test_macros --std=c++11 -O2

.PHONY: all build
