#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <thread>
#include <mutex>

// POSIX socket headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "include/Leaderboard.h"

#define PORT        8080
#define BUFFER_SIZE 4096

//proteksi leaderboard pake multithreading

Leaderboard globalBoard;
std::mutex   boardMutex;

void handleClient(int clientSock, std::string clientIP) {
    std::cout << "[SERVER] Client terhubung: " << clientIP << "\n";
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesRead = recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesRead <= 0) break;

        std::string request(buffer);
        while (!request.empty() &&
               (request.back() == '\n' || request.back() == '\r'))
            request.pop_back();

        std::string response;

        if (request.size() >= 3 && request.substr(0, 3) == "ADD") {
            std::string json = request.substr(4);
            std::lock_guard<std::mutex> lock(boardMutex);
            bool ok = globalBoard.addFromJSON(json);
            globalBoard.sortAndRank();
            // SAVE otomatis setiap kali ada player baru
            if (ok) globalBoard.saveToFile();
            response = ok
                ? "{\"status\":\"OK\",\"message\":\"Player added\"}"
                : "{\"status\":\"ERROR\",\"message\":\"Invalid JSON\"}";

        } else if (request == "GET") {
            std::lock_guard<std::mutex> lock(boardMutex);
            response = globalBoard.toJSON();

        } else if (request.size() >= 10 && request.substr(0, 10) == "FIND_SCORE") {
            int score = std::stoi(request.substr(11));
            std::lock_guard<std::mutex> lock(boardMutex);
            RegisteredPlayer* p = globalBoard.findByScore(score);
            response = p ? p->toJSON() : "{\"status\":\"NOT_FOUND\"}";

        } else if (request.size() >= 9 && request.substr(0, 9) == "FIND_NAME") {
            std::string name = request.substr(10);
            std::lock_guard<std::mutex> lock(boardMutex);
            RegisteredPlayer* p = globalBoard.findByUsername(name);
            response = p ? p->toJSON() : "{\"status\":\"NOT_FOUND\"}";

        } else if (request == "QUIT") {
            response = "{\"status\":\"BYE\"}";
            send(clientSock, response.c_str(), (int)response.size(), 0);
            break;

        } else {
            response = "{\"status\":\"ERROR\",\"message\":\"Unknown command\"}";
        }

        response += "\n";
        send(clientSock, response.c_str(), (int)response.size(), 0);
        std::cout << "[SERVER] [" << clientIP << "] "
                  << request.substr(0, 20) << " -> "
                  << response.substr(0, 60) << "\n";
    }

    close(clientSock);
    std::cout << "[SERVER] Client putus: " << clientIP << "\n";
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        std::cerr << "[ERROR] Gagal membuat socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(PORT);

    if (bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[ERROR] Bind gagal\n";
        close(serverSock);
        return 1;
    }

    listen(serverSock, 10);
    std::cout << "[SERVER] Leaderboard Server berjalan di port " << PORT << "\n";

    // LOAD data dari file saat server nyala
    {
        std::lock_guard<std::mutex> lock(boardMutex);
        globalBoard.loadFromFile();

        // Seed data awal hanya jika file tidak ada / kosong
        if (globalBoard.size() == 0) {
            std::cout << "[SERVER] Menggunakan seed data awal.\n";
            globalBoard.addPlayer(new RegisteredPlayer("Alice",   9500,  "alice@game.com",   42));
            globalBoard.addPlayer(new RegisteredPlayer("Bob",     3200,  "bob@game.com",     18));
            globalBoard.addPlayer(new RegisteredPlayer("Charlie", 12000, "charlie@game.com", 75));
            globalBoard.addPlayer(new RegisteredPlayer("Diana",   7800,  "diana@game.com",   55));
            globalBoard.addPlayer(new RegisteredPlayer("Eve",     500,   "eve@game.com",      5));
            globalBoard.sortAndRank();
            globalBoard.saveToFile();
        }
    }

    std::cout << "[SERVER] Menunggu koneksi client...\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock < 0) continue;

        std::string clientIP(inet_ntoa(clientAddr.sin_addr));
        std::thread t(handleClient, clientSock, clientIP);
        t.detach();
    }

    close(serverSock);
    return 0;
}
