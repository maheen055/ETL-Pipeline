# Makefile to compile the program into a.out

all: main.cpp CountryData.cpp
	g++ -g -std=c++17 main.cpp CountryData.cpp