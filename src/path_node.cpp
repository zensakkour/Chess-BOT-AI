#include "chess_engine/chess.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>

namespace {

float PieceValue(const char piece) {
    switch (piece) {
        case W_PAWN:
        case B_PAWN:
            return 1.0f;
        case W_KNIGHT:
        case B_KNIGHT:
        case W_BISHOP:
        case B_BISHOP:
            return 3.0f;
        case W_ROOK:
        case B_ROOK:
            return 5.0f;
        case W_QUEEN:
        case B_QUEEN:
            return 9.0f;
        case W_KING:
        case B_KING:
            return 20.0f;
        default:
            return 0.0f;
    }
}

float CenterPreference(const short x, const short y) {
    const float dx = std::fabs(3.5f - static_cast<float>(x));
    const float dy = std::fabs(3.5f - static_cast<float>(y));
    return 4.0f - (dx + dy);  // max when close to center squares.
}

float StyleBonus(const std::string& move, const char captured_piece, const BotStyle style) {
    const short to_x = move[2];
    const short to_y = move[3];
    const float capture_score = PieceValue(captured_piece);
    const float center_score = CenterPreference(to_x, to_y);

    switch (style) {
        case AGGRESSIVE:
            return (0.30f * capture_score) + (0.03f * center_score);
        case POSITIONAL:
            return (0.18f * center_score) + (0.06f * capture_score);
        case BALANCED:
        default:
            return (0.12f * capture_score) + (0.08f * center_score);
    }
}

}  // namespace

void PathNode::CreateSubtree(Chess& c) noexcept {
    const auto all_moves = c.AllMoves();
    for (auto move : all_moves) {
        Chess::ChangeToRealCoordinates(move[0], move[1], move[2], move[3]);
        child_node_list.emplace(move, PathNode());
    }
}

float PathNode::AlphaBeta(Chess& c, unsigned short depth, float alpha, float beta, const bool& maximizing_player,
                          const bool& initial_turn) noexcept {
    if (depth == 0) {
        return c.EvaluateBoard(initial_turn);
    }

    CreateSubtree(c);
    if (child_node_list.empty()) {
        return c.EvaluateBoard(initial_turn);
    }

    float points = maximizing_player ? std::numeric_limits<float>::lowest() : std::numeric_limits<float>::max();
    for (auto& node : child_node_list) {
        if (c.GetPiece(node.first[2], node.first[3]) == W_KING - 7 * c.GetTurn()) {
            child_node_list.clear();
            return maximizing_player ? 9999.0f : -9999.0f;
        }

        c.MovePiece(node.first[0], node.first[1], node.first[2], node.first[3], false, false);
        const float score = node.second.AlphaBeta(c, static_cast<unsigned short>(depth - 1), alpha, beta,
                                                  !maximizing_player, initial_turn);
        c.MovePieceBack(node.first[0], node.first[1], node.first[2], node.first[3]);

        if (maximizing_player) {
            points = std::max(points, score);
            alpha = std::max(alpha, points);
        } else {
            points = std::min(points, score);
            beta = std::min(beta, points);
        }

        if (alpha >= beta) {
            break;
        }
    }

    child_node_list.clear();
    return points;
}

std::string PathNode::AlphaBetaRoot(Chess& c, unsigned short difficulty, BotStyle style) noexcept {
    CreateSubtree(c);
    if (child_node_list.empty()) {
        return "";
    }

    std::vector<std::string> ideal_moves;
    float max_move_score = std::numeric_limits<float>::lowest();
    for (auto& node : child_node_list) {
        const char captured_piece = c.GetPiece(node.first[2], node.first[3]);
        if (captured_piece == W_KING - 7 * c.GetTurn()) {
            child_node_list.clear();
            return node.first;
        }

        c.MovePiece(node.first[0], node.first[1], node.first[2], node.first[3], false, false);
        const float move_score = node.second.AlphaBeta(c, difficulty, -10000.0f, 10000.0f, false, !c.GetTurn());
        c.MovePieceBack(node.first[0], node.first[1], node.first[2], node.first[3]);
        const float adjusted_score = move_score + StyleBonus(node.first, captured_piece, style);

        if (adjusted_score > max_move_score) {
            max_move_score = adjusted_score;
            ideal_moves.clear();
            ideal_moves.emplace_back(node.first);
        } else if (adjusted_score == max_move_score) {
            ideal_moves.emplace_back(node.first);
        }
    }

    child_node_list.clear();
    if (ideal_moves.empty()) {
        return "";
    }

    auto move = ideal_moves.cbegin();
    std::advance(move, GetRandomNumber<unsigned short>(0, static_cast<unsigned short>(ideal_moves.size() - 1)));
    return *move;
}
