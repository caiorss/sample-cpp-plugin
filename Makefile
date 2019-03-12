
CC := clang++

all: PluginA.so

PluginA.so: PluginA.cpp interfaces.hpp
	$(CC) PluginA.cpp -o PluginA.so -shared -fPIC -Wall -std=c++1z

