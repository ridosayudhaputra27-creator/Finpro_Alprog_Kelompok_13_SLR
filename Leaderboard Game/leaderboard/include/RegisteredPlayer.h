#ifndef REGISTERED_PLAYER_H
#define REGISTERED_PLAYER_H

#include "Player.h"
#include <sstream>

// ============================================================
// PEWARISAN: RegisteredPlayer mewarisi Player
// ============================================================
class RegisteredPlayer : public Player {
private:
    std::string email;      // ENKAPSULASI: private member
    int gamesPlayed;
    std::string tier;       // "Bronze", "Silver", "Gold", "Platinum"

    std::string calcTier() const {
        if (score >= 10000) return "Platinum";
        if (score >= 5000)  return "Gold";
        if (score >= 1000)  return "Silver";
        return "Bronze";
    }

public:
    RegisteredPlayer(const std::string& uname, int sc, const std::string& em, int games)
        : Player(uname, sc), email(em), gamesPlayed(games) {
        tier = calcTier();
    }

    // ENKAPSULASI: getter
    std::string getEmail() const { return email; }
    int getGamesPlayed() const { return gamesPlayed; }
    std::string getTier() const { return tier; }

    void updateTier() { tier = calcTier(); }

    // ============================================================
    // POLIMORFISME: override pure virtual methods
    // ============================================================
    std::string getRole() const override {
        return "RegisteredPlayer";
    }

    void displayInfo() const override {
        std::cout << "[#" << rank << "] "
                  << username << " | Score: " << score
                  << " | Tier: " << tier
                  << " | Games: " << gamesPlayed << "\n";
    }

    std::string toJSON() const override {
        std::ostringstream oss;
        oss << "{"
            << "\"rank\":" << rank << ","
            << "\"username\":\"" << username << "\","
            << "\"score\":" << score << ","
            << "\"tier\":\"" << tier << "\","
            << "\"email\":\"" << email << "\","
            << "\"gamesPlayed\":" << gamesPlayed << ","
            << "\"role\":\"" << getRole() << "\""
            << "}";
        return oss.str();
    }
};

#endif
