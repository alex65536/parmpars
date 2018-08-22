all: build

build:
	g++ test.cpp -o test --std=c++11 -O2

.PHONY: all build
