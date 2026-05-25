#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "LinkedList.h"
#include "Algorithms.h"
#include <iostream>
#include <sstream>

// ============================================================
// Leaderboard: mengelola daftar pemain, sorting, searching
// ============================================================
class Leaderboard {
private:
    LinkedList list;

    // Helper: ambil semua player ke array sementara
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

    // Sort dan perbarui rank — O(n log n)
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

    // Cari berdasarkan skor — O(log n) setelah sort
    RegisteredPlayer* findByScore(int score) {
        int size;
        RegisteredPlayer** arr = toArray(size);
        int idx = SearchAlgo::binarySearchByScore(arr, size, score);
        RegisteredPlayer* result = (idx != -1) ? arr[idx] : nullptr;
        delete[] arr;
        return result;
    }

    // Cari berdasarkan username — O(n)
    RegisteredPlayer* findByUsername(const std::string& name) {
        int size;
        RegisteredPlayer** arr = toArray(size);
        int idx = SearchAlgo::linearSearchByName(arr, size, name);
        RegisteredPlayer* result = (idx != -1) ? arr[idx] : nullptr;
        delete[] arr;
        return result;
    }

    // Cetak semua pemain ke stdout
    void display() const {
        std::cout << "\n===== LEADERBOARD =====\n";
        Node* curr = list.getHead();
        while (curr) {
            curr->data->displayInfo(); // POLIMORFISME: virtual dispatch
            curr = curr->next;
        }
        std::cout << "=======================\n";
    }

    // Serialize seluruh leaderboard ke JSON string
    std::string toJSON() const {
        std::ostringstream oss;
        oss << "{\"leaderboard\":[";
        Node* curr = list.getHead();
        bool first = true;
        while (curr) {
            if (!first) oss << ",";
            oss << curr->data->toJSON(); // POLIMORFISME: virtual toJSON()
            first = false;
            curr = curr->next;
        }
        oss << "]}";
        return oss.str();
    }

    // Parse JSON sederhana dan tambah player
    // Format: {"username":"x","score":100,"email":"x@x.com","gamesPlayed":5}
    bool addFromJSON(const std::string& json) {
        auto extract = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\":";
            size_t pos = json.find(search);
            if (pos == std::string::npos) return "";
            pos += search.size();
            if (json[pos] == '"') {
                pos++;
                size_t end = json.find('"', pos);
                return json.substr(pos, end - pos);
            } else {
                size_t end = json.find_first_of(",}", pos);
                return json.substr(pos, end - pos);
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
