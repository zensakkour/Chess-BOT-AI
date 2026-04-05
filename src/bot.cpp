#include "chess_engine/chess.hpp"

Bot::Bot(const std::string& name, const unsigned short& difficulty, BotStyle style) noexcept
    : Player(name), difficulty(difficulty), style(style) {
}

unsigned short Bot::GetDifficulty() const noexcept {
    return difficulty;
}

BotStyle Bot::GetStyle() const noexcept {
    return style;
}

std::string Bot::GetIdealMove(Chess& c) noexcept {
    return root.AlphaBetaRoot(c, difficulty, style);
}

std::string Bot::GetIdealMove(Chess& c, unsigned short difficulty_override, BotStyle style_override) noexcept {
    return root.AlphaBetaRoot(c, difficulty_override, style_override);
}

bool Bot::operator==(const Bot& b) const noexcept {
    return name == b.name;
}
