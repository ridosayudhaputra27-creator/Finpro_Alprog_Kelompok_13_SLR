// ============================================================
// CLIENT — Leaderboard Game (POSIX / Linux / WSL)
// Kompilasi: g++ client.cpp -o client
// ============================================================

#include <iostream>
#include <string>
#include <cstring>

// POSIX socket headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT      8080
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE  8192

// Kirim request ke server, kembalikan respons
std::string sendRequest(int sock, const std::string& msg) {
    std::string toSend = msg + "\n";
    send(sock, toSend.c_str(), (int)toSend.size(), 0);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    recv(sock, buf, BUF_SIZE - 1, 0);
    return std::string(buf);
}

// Cetak leaderboard dari JSON string
void printLeaderboard(const std::string& json) {
    std::cout << "\n+==================================================+\n";
    std::cout << "|          ** GAME LEADERBOARD **                  |\n";
    std::cout << "+==================================================+\n";

    size_t pos = 0;
    int playerCount = 0;

    while ((pos = json.find("\"rank\":", pos)) != std::string::npos) {
        playerCount++;
        size_t rankStart = pos + 7;
        size_t rankEnd   = json.find(',', rankStart);
        int rank = std::stoi(json.substr(rankStart, rankEnd - rankStart));

        auto extractField = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\":";
            size_t p = json.find(search, pos);
            if (p == std::string::npos) return "-";
            p += search.size();
            if (json[p] == '"') {
                p++;
                size_t e = json.find('"', p);
                return json.substr(p, e - p);
            }
            size_t e = json.find_first_of(",}", p);
            return json.substr(p, e - p);
        };

        std::string uname = extractField("username");
        std::string score = extractField("score");
        std::string tier  = extractField("tier");
        std::string games = extractField("gamesPlayed");

        std::cout << "| #" << rank << " " << uname;
        int pad = 20 - (int)uname.size() - (rank >= 10 ? 2 : 1);
        for (int i = 0; i < pad; i++) std::cout << ' ';
        std::cout << "Score: " << score;
        pad = 8 - (int)score.size();
        for (int i = 0; i < pad; i++) std::cout << ' ';
        std::cout << tier << " | " << games << " games  |\n";

        pos = rankEnd;
    }

    if (playerCount == 0)
        std::cout << "|  (Leaderboard kosong)                            |\n";
    std::cout << "+==================================================+\n";
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[ERROR] Gagal membuat socket\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[ERROR] Tidak bisa konek ke server.\n";
        std::cerr << "        Pastikan server sudah berjalan terlebih dahulu!\n";
        close(sock);
        return 1;
    }

    std::cout << "[OK] Terhubung ke Leaderboard Server ("
              << SERVER_IP << ":" << PORT << ")\n";

    int choice;
    while (true) {
        std::cout << "\n+-----------------------------+\n";
        std::cout << "|   MENU LEADERBOARD CLIENT   |\n";
        std::cout << "+-----------------------------+\n";
        std::cout << "| 1. Lihat Leaderboard        |\n";
        std::cout << "| 2. Tambah Player            |\n";
        std::cout << "| 3. Cari Player by Score     |\n";
        std::cout << "| 4. Cari Player by Username  |\n";
        std::cout << "| 5. Keluar                   |\n";
        std::cout << "+-----------------------------+\n";
        std::cout << "Pilih: ";
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {
            std::string resp = sendRequest(sock, "GET");
            printLeaderboard(resp);

        } else if (choice == 2) {
            std::string uname, email;
            int score, games;
            std::cout << "Username : "; std::getline(std::cin, uname);
            std::cout << "Score    : "; std::cin >> score;
            std::cout << "Games    : "; std::cin >> games;
            std::cin.ignore();
            std::cout << "Email    : "; std::getline(std::cin, email);

            std::string json = "{\"username\":\"" + uname + "\","
                             + "\"score\":"       + std::to_string(score) + ","
                             + "\"email\":\""     + email + "\","
                             + "\"gamesPlayed\":" + std::to_string(games) + "}";

            std::string resp = sendRequest(sock, "ADD " + json);
            std::cout << "Server: " << resp;

        } else if (choice == 3) {
            int score;
            std::cout << "Cari skor: "; std::cin >> score;
            std::cin.ignore();
            std::string resp = sendRequest(sock, "FIND_SCORE " + std::to_string(score));
            std::cout << "Hasil: " << resp;

        } else if (choice == 4) {
            std::string name;
            std::cout << "Cari username: "; std::getline(std::cin, name);
            std::string resp = sendRequest(sock, "FIND_NAME " + name);
            std::cout << "Hasil: " << resp;

        } else if (choice == 5) {
            sendRequest(sock, "QUIT");
            std::cout << "Koneksi ditutup. Sampai jumpa!\n";
            break;

        } else {
            std::cout << "Pilihan tidak valid.\n";
        }
    }

    close(sock);   // POSIX: close(), bukan closesocket()
    return 0;
}
