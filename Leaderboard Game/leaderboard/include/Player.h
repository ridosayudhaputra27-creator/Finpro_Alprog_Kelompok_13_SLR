#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>

// ============================================================
// ABSTRAKSI: Abstract class dengan pure virtual method
// ============================================================
class Player {
protected:
    std::string username;  // ENKAPSULASI: protected member
    int score;
    int rank;

public:
    Player(const std::string& uname, int sc)
        : username(uname), score(sc), rank(0) {}

    virtual ~Player() {}

    // Pure virtual — wajib diimplementasikan subclass
    virtual std::string getRole() const = 0;
    virtual void displayInfo() const = 0;
    virtual std::string toJSON() const = 0;

    // ENKAPSULASI: getter & setter
    std::string getUsername() const { return username; }
    int getScore() const { return score; }
    int getRank() const { return rank; }

    void setScore(int sc) { score = sc; }
    void setRank(int r) { rank = r; }

    // Operator overload untuk perbandingan skor
    bool operator>(const Player& other) const {
        return score > other.score;
    }
};

#endif
