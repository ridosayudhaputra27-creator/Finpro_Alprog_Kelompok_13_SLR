CXX     = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I./include

all: server client

server: src/server.cpp include/Leaderboard.h include/LinkedList.h include/Algorithms.h include/RegisteredPlayer.h include/Player.h
	$(CXX) $(CXXFLAGS) src/server.cpp -o server

client: src/client.cpp
	$(CXX) $(CXXFLAGS) src/client.cpp -o client

clean:
	rm -f server client

.PHONY: all clean
