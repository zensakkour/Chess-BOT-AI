#ifndef CHESS_ENGINE_CHESS_HPP
#define CHESS_ENGINE_CHESS_HPP

#include <forward_list>
#include <map>
#include <string>
#include <vector>

constexpr short BOARD_SIZE = 8;
constexpr short BOX_WIDTH = 10;
constexpr short DOWN = 3;
constexpr short RIGHT = 10;
constexpr unsigned short MOVES_PER_LINE = 5;

#define TO_DOWN std::string(DOWN, '\n')
#define TO_RIGHT std::string(RIGHT, ' ')
#define CLEAR_LINE std::string(100, ' ')

// Types of chess pieces.
enum ChessPieces : char {
    B_KING = -6,
    B_QUEEN,
    B_BISHOP,
    B_KNIGHT,
    B_ROOK,
    B_PAWN,
    EMPTY,
    W_KING,
    W_QUEEN,
    W_BISHOP,
    W_KNIGHT,
    W_ROOK,
    W_PAWN
};

// Types of player moves.
enum Moves {
    NORMAL,
    CASTLING,
    PROMOTION,
    EN_PASSANT
};

// Possible options on how the game can end.
enum Endgame {
    CHECKMATE,
    FIFTY_MOVES,
    THREEFOLD_REP,
    QUIT
};

enum BotStyle {
    BALANCED,
    AGGRESSIVE,
    POSITIONAL
};

struct GameConfig {
    bool against_bot = true;
    bool two_bots = true;
    bool bot_is_white = false;
    bool white_bot_random = false;
    bool black_bot_random = false;
    unsigned short white_bot_difficulty = 2;
    unsigned short black_bot_difficulty = 2;
    BotStyle white_bot_style = BALANCED;
    BotStyle black_bot_style = BALANCED;
    std::string white_player_name = "White";
    std::string black_player_name = "Black";
};

inline constexpr char STARTING_BOARD[BOARD_SIZE][BOARD_SIZE] = {
    {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK},
    {B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN},
    {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK}
};

class Player;
class PathNode;
class Bot;
class Chess;

class Player {
protected:
    std::string name;
    unsigned short score = 0;
    bool castling = true;

public:
    explicit Player(const std::string& name) noexcept;
    std::string GetName() const noexcept;
    unsigned short GetScore() const noexcept;
    bool GetCastling() const noexcept;
    void SetCastling(const bool& castling) noexcept;
    void IncreaseScore(const unsigned short& inc) noexcept;
    void Reset() noexcept;
    bool operator==(const Player& p) const noexcept;
};

class PathNode {
private:
    std::map<std::string, PathNode> child_node_list;
    void CreateSubtree(Chess& c) noexcept;
    float AlphaBeta(Chess& c, unsigned short depth, float alpha, float beta, const bool& maximizing_player,
                    const bool& initial_turn) noexcept;

public:
    std::string AlphaBetaRoot(Chess& c, unsigned short difficulty, BotStyle style) noexcept;
};

class Bot : public Player {
private:
    PathNode root;
    unsigned short difficulty;
    BotStyle style;

public:
    Bot(const std::string& name, const unsigned short& difficulty, BotStyle style) noexcept;
    unsigned short GetDifficulty() const noexcept;
    BotStyle GetStyle() const noexcept;
    std::string GetIdealMove(Chess& c) noexcept;
    std::string GetIdealMove(Chess& c, unsigned short difficulty, BotStyle style) noexcept;
    bool operator==(const Bot& b) const noexcept;
};

class Chess {
private:
    char board[BOARD_SIZE][BOARD_SIZE];
    Bot white;
    Bot black;
    std::vector<std::pair<Moves, std::string>> all_game_moves;
    bool whites_turn = true;
    unsigned short moves_after_last_pawn_move_or_capture = 0;
    bool white_bot_random;
    bool black_bot_random;

    static bool WithinBounds(const short& coord) noexcept;
    static bool WithinBounds(const short& x, const short& y) noexcept;
    static void ChangeToString(char& x1, char& y1, char& x2, char& y2) noexcept;
    static std::string ToString(const short& x1, const short& y1, const short& x2, const short& y2) noexcept;
    static std::string PieceNameToString(const char& piece) noexcept;
    static float EvaluatePiece(const char& piece) noexcept;
    static void ClearAllMoves(const unsigned short& n) noexcept;
    static void PrintSeparator(const char& ch) noexcept;
    static void CopyBoard(const char from[BOARD_SIZE][BOARD_SIZE], char to[BOARD_SIZE][BOARD_SIZE]) noexcept;
    static bool AreBoardsEqual(const char board1[BOARD_SIZE][BOARD_SIZE],
                               const char board2[BOARD_SIZE][BOARD_SIZE]) noexcept;
    static bool CanMovePiece(const short& x1, const short& y1, const short& x2, const short& y2,
                             const std::forward_list<std::string>& all_moves) noexcept;

    Bot& GetCurrentPlayer() noexcept;
    Bot GetCurrentPlayerConst() const noexcept;
    Bot& GetOtherPlayer() noexcept;
    Bot GetOtherPlayerConst() const noexcept;
    void ChangeTurn() noexcept;
    void AppendToAllGameMoves(const short& x1, const short& y1, const short& x2, const short& y2) noexcept;
    void Reset() noexcept;
    void CheckCoordinates(const short& x, const short& y, const std::string& func_name) const noexcept(false);
    bool EndGameText(const unsigned short& n, const Endgame& end_game) const noexcept;
    short GetEnPassant(const short& x, const short& y) const noexcept;

    template <class Iterator>
    short GetEnPassant(const char board[BOARD_SIZE][BOARD_SIZE], const Iterator& it) const noexcept;

    bool ThreefoldRepetition() const noexcept;
    bool IsCheck(const bool& turn) const noexcept;
    bool IsCheck(std::string& move) noexcept;
    std::forward_list<std::string> PawnMoves(const short& x, const short& y) const noexcept;
    std::forward_list<std::string> RookMoves(const short& x, const short& y) const noexcept;
    std::forward_list<std::string> KnightMoves(const short& x, const short& y) const noexcept;
    std::forward_list<std::string> BishopMoves(const short& x, const short& y) const noexcept;
    std::forward_list<std::string> QueenMoves(const short& x, const short& y) const noexcept;
    std::forward_list<std::string> KingMoves(const short& x, const short& y) const noexcept;
    std::string GetRandomMove() noexcept;
    void ManuallyPromotePawn(const short& x, const short& y) noexcept;
    void UpdateBoard(const short& x, const short& y) const noexcept;
    void UpdateScore(const Bot& p) const noexcept;
    float EvaluatePosition(const short& x, const short& y) const noexcept;
    void PrintAllMovesMadeInOrder() const noexcept;
    bool CheckEndgame(const unsigned short& n = 0) noexcept;

public:
    Chess(const std::string& player1, const unsigned short& difficulty1, BotStyle style1, const bool& random1,
          const std::string& player2, const unsigned short& difficulty2, BotStyle style2, const bool& random2) noexcept;
    static void ChangeToRealCoordinates(char& x1, char& y1, char& x2, char& y2) noexcept;
    char GetPiece(const short& x, const short& y) const noexcept;
    bool GetTurn() const noexcept;
    std::forward_list<std::string> AllMoves() noexcept;
    void MovePiece(const short& x1, const short& y1, const short& x2, const short& y2, const bool& manual_promotion,
                   const bool& update_board) noexcept;
    void MovePieceBack(const short& x1, const short& y1, const short& x2, const short& y2) noexcept;
    float EvaluateBoard(const bool& turn) const noexcept;
    void PrintBoard() const noexcept;
    bool PlayersTurn() noexcept;
    bool BotsTurn() noexcept;
    bool GameOver() noexcept;
};

void MoveCursorToXY(const short& x, const short& y) noexcept;
void MaximizeConsoleWindow() noexcept;
void ClearScreen() noexcept;
char ReadSingleKey() noexcept;
std::string ToLowerString(std::string s) noexcept;

template <class T>
T GetRandomNumber(const T& min, const T& max) noexcept {
    if (max <= min) {
        return min;
    }
    return min + static_cast<T>(static_cast<double>(rand()) / static_cast<double>(RAND_MAX + 1.0) * (max - min + 1));
}

#endif
