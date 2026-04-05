#include "chess_engine/chess.hpp"

Player::Player(const std::string& name) noexcept : name(name) {
}

std::string Player::GetName() const noexcept {
    return name;
}

unsigned short Player::GetScore() const noexcept {
    return score;
}

bool Player::GetCastling() const noexcept {
    return castling;
}

void Player::SetCastling(const bool& castling_value) noexcept {
    castling = castling_value;
}

void Player::IncreaseScore(const unsigned short& inc) noexcept {
    score += inc;
}

void Player::Reset() noexcept {
    score = 0;
    castling = true;
}

bool Player::operator==(const Player& p) const noexcept {
    return name == p.name;
}
