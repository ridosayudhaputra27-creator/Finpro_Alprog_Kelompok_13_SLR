#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "LinkedList.h"
#include "Algorithms.h"
#include "../include/json.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

class Leaderboard {
private:
    LinkedList list;
    std::string saveFile = "data.json";

    // ambil semua player ke array sementara
    RegisteredPlayer** toArray(int& size) const {
        size = list.size();
        RegisteredPlayer** arr = new RegisteredPlayer*[size];
        for (int i = 0; i < size; i++) arr[i] = list.getAt(i);
        return arr;
    }

public:
    void addPlayer(RegisteredPlayer* p) {
        list.append(p);
    }

    // Sort dan perbarui rank â€” O(n log n)
    void sortAndRank() {
        int size;
        RegisteredPlayer** arr = toArray(size);

        SortAlgo::mergeSort(arr, 0, size - 1);

        for (int i = 0; i < size; i++) {
            arr[i]->setRank(i + 1);
        }

        list.rebuildFrom(arr, size);
        delete[] arr;
    }

    // cari berdasarkan skor setelahsort
    RegisteredPlayer* findByScore(int score) {
        int size;
        RegisteredPlayer** arr = toArray(size);
        int idx = SearchAlgo::binarySearchByScore(arr, size, score);
        RegisteredPlayer* result = (idx != -1) ? arr[idx] : nullptr;
        delete[] arr;
        return result;
    }

    // cari berdasarkan username 
    RegisteredPlayer* findByUsername(const std::string& name) {
        int size;
        RegisteredPlayer** arr = toArray(size);
        int idx = SearchAlgo::linearSearchByName(arr, size, name);
        RegisteredPlayer* result = (idx != -1) ? arr[idx] : nullptr;
        delete[] arr;
        return result;
    }

    // cetak semua pemain ke stdout
    void display() const {
        std::cout << "\n===== LEADERBOARD =====\n";
        Node* curr = list.getHead();
        while (curr) {
            curr->data->displayInfo();
            curr = curr->next;
        }
        std::cout << "=======================\n";
    }

    // serialize seluruh leaderboard ke JSON string
    std::string toJSON() const {
        std::ostringstream oss;
        oss << "{\"leaderboard\":[";
        Node* curr = list.getHead();
        bool first = true;
        while (curr) {
            if (!first) oss << ",";
            oss << curr->data->toJSON();
            first = false;
            curr = curr->next;
        }
        oss << "]}";
        return oss.str();
    }

   //simpan semua player ke data.json
   
    void saveToFile() const {
        json j;
        j["leaderboard"] = json::array();

        Node* curr = list.getHead();
        while (curr) {
            RegisteredPlayer* p = curr->data;
            j["leaderboard"].push_back({
                {"username",    p->getUsername()},
                {"score",       p->getScore()},
                {"email",       p->getEmail()},
                {"gamesPlayed", p->getGamesPlayed()}
            });
            curr = curr->next;
        }

        std::ofstream file(saveFile);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            std::cout << "[SERVER] Data disimpan ke " << saveFile << "\n";
        } else {
            std::cerr << "[SERVER] Gagal menyimpan ke " << saveFile << "\n";
        }
    }

// baca data json saat pertama kali running file

    void loadFromFile() {
        std::ifstream file(saveFile);
        if (!file.is_open()) {
            std::cout << "[SERVER] Tidak ada " << saveFile << ", mulai dari kosong.\n";
            return;
        }

        try {
            json j;
            file >> j;
            file.close();

            for (auto& item : j["leaderboard"]) {
                std::string uname = item["username"];
                int score         = item["score"];
                std::string email = item["email"];
                int games         = item["gamesPlayed"];
                addPlayer(new RegisteredPlayer(uname, score, email, games));
            }

            sortAndRank();
            std::cout << "[SERVER] Data dimuat dari " << saveFile
                      << " (" << size() << " player)\n";
        } catch (...) {
            std::cerr << "[SERVER] Gagal membaca " << saveFile << "\n";
        }
    }

    // parse JSON sederhana dan tambah player
    bool addFromJSON(const std::string& jsonStr) {
        auto extract = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\":";
            size_t pos = jsonStr.find(search);
            if (pos == std::string::npos) return "";
            pos += search.size();
            if (jsonStr[pos] == '"') {
                pos++;
                size_t end = jsonStr.find('"', pos);
                return jsonStr.substr(pos, end - pos);
            } else {
                size_t end = jsonStr.find_first_of(",}", pos);
                return jsonStr.substr(pos, end - pos);
            }
        };

        std::string uname    = extract("username");
        std::string scoreStr = extract("score");
        std::string email    = extract("email");
        std::string gamesStr = extract("gamesPlayed");

        if (uname.empty() || scoreStr.empty()) return false;

        int score = std::stoi(scoreStr);
        int games = gamesStr.empty() ? 0 : std::stoi(gamesStr);
        if (email.empty()) email = uname + "@game.com";

        addPlayer(new RegisteredPlayer(uname, score, email, games));
        return true;
    }

    int size() const { return list.size(); }
};

#endif
