#include "chess_engine/chess.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

namespace {

constexpr const char* kVersion = "2.1.0";

struct LaunchOptions {
    GameConfig config;
    unsigned int seed = static_cast<unsigned int>(time(nullptr));
    bool show_profile = true;
};

bool ParseBoolean(const std::string& value, bool& result) {
    const std::string lowered = ToLowerString(value);
    if (lowered == "1" || lowered == "true" || lowered == "yes") {
        result = true;
        return true;
    }
    if (lowered == "0" || lowered == "false" || lowered == "no") {
        result = false;
        return true;
    }
    return false;
}

bool ParseDifficulty(const std::string& value, unsigned short& result) {
    try {
        const int parsed = std::stoi(value);
        if (parsed < 1 || parsed > 6) {
            return false;
        }
        result = static_cast<unsigned short>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool ParseSeed(const std::string& value, unsigned int& seed) {
    try {
        const unsigned long parsed = std::stoul(value);
        if (parsed > 4294967295UL) {
            return false;
        }
        seed = static_cast<unsigned int>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool ParseStyle(const std::string& value, BotStyle& style) {
    const std::string lowered = ToLowerString(value);
    if (lowered == "balanced") {
        style = BALANCED;
        return true;
    }
    if (lowered == "aggressive") {
        style = AGGRESSIVE;
        return true;
    }
    if (lowered == "positional") {
        style = POSITIONAL;
        return true;
    }
    return false;
}

std::string StyleToString(const BotStyle style) {
    switch (style) {
        case AGGRESSIVE:
            return "aggressive";
        case POSITIONAL:
            return "positional";
        case BALANCED:
        default:
            return "balanced";
    }
}

std::string ModeToString(const GameConfig& config) {
    if (!config.against_bot) {
        return "pvp";
    }
    return config.two_bots ? "bvb" : "pvb";
}

bool ApplyPreset(const std::string& preset_name, LaunchOptions& options) {
    const std::string preset = ToLowerString(preset_name);

    if (preset == "classic") {
        options.config = GameConfig{};
        return true;
    }

    if (preset == "creator") {
        options.config.against_bot = true;
        options.config.two_bots = false;
        options.config.bot_is_white = false;
        options.config.white_player_name = "You";
        options.config.black_player_name = "CreatorBot";
        options.config.white_bot_difficulty = 2;
        options.config.black_bot_difficulty = 3;
        options.config.white_bot_style = POSITIONAL;
        options.config.black_bot_style = AGGRESSIVE;
        options.config.white_bot_random = false;
        options.config.black_bot_random = false;
        return true;
    }

    if (preset == "duel") {
        options.config.against_bot = true;
        options.config.two_bots = true;
        options.config.white_player_name = "Atlas";
        options.config.black_player_name = "Nyx";
        options.config.white_bot_difficulty = 4;
        options.config.black_bot_difficulty = 4;
        options.config.white_bot_style = POSITIONAL;
        options.config.black_bot_style = AGGRESSIVE;
        options.config.white_bot_random = false;
        options.config.black_bot_random = false;
        return true;
    }

    if (preset == "chaos") {
        options.config.against_bot = true;
        options.config.two_bots = true;
        options.config.white_player_name = "ChaosWhite";
        options.config.black_player_name = "ChaosBlack";
        options.config.white_bot_difficulty = 1;
        options.config.black_bot_difficulty = 1;
        options.config.white_bot_style = AGGRESSIVE;
        options.config.black_bot_style = AGGRESSIVE;
        options.config.white_bot_random = true;
        options.config.black_bot_random = true;
        return true;
    }

    return false;
}

void PrintUsage(const char* app_name) {
    std::cout << "TerminalChess-AI\n\n"
              << "Usage: " << app_name << " [options]\n\n"
              << "Options:\n"
              << "  --mode <pvp|pvb|bvb>         Match mode (player-vs-player, player-vs-bot, bot-vs-bot)\n"
              << "  --preset <classic|creator|duel|chaos>\n"
              << "                               One-command profile to make the game feel distinct\n"
              << "  --bot-color <white|black>    Bot side in --mode pvb (default: black)\n"
              << "  --white-name <name>          White player name\n"
              << "  --black-name <name>          Black player name\n"
              << "  --white-difficulty <1-6>     White bot search depth\n"
              << "  --black-difficulty <1-6>     Black bot search depth\n"
              << "  --white-style <name>         White bot style: balanced|aggressive|positional\n"
              << "  --black-style <name>         Black bot style: balanced|aggressive|positional\n"
              << "  --white-random <true|false>  Force white side random move selection\n"
              << "  --black-random <true|false>  Force black side random move selection\n"
              << "  --seed <number>              Deterministic random seed for reproducible games\n"
              << "  --no-profile                 Hide startup profile banner\n"
              << "  --version                    Print TerminalChess-AI version\n"
              << "  --help                       Show this help\n";
}

void PrintBanner() {
    std::cout << "\n"
              << "========================================\n"
              << "         TerminalChess-AI v2            \n"
              << "========================================\n";
}

void PrintProfile(const LaunchOptions& options) {
    const auto& cfg = options.config;
    std::cout << "Profile\n";
    std::cout << "  mode: " << ModeToString(cfg) << "\n";
    std::cout << "  seed: " << options.seed << "\n";

    if (!cfg.against_bot) {
        std::cout << "  white: " << cfg.white_player_name << " (human)\n";
        std::cout << "  black: " << cfg.black_player_name << " (human)\n";
    } else if (cfg.two_bots) {
        std::cout << "  white: " << cfg.white_player_name
                  << " [d=" << cfg.white_bot_difficulty
                  << ", style=" << StyleToString(cfg.white_bot_style)
                  << ", random=" << (cfg.white_bot_random ? "true" : "false") << "]\n";
        std::cout << "  black: " << cfg.black_player_name
                  << " [d=" << cfg.black_bot_difficulty
                  << ", style=" << StyleToString(cfg.black_bot_style)
                  << ", random=" << (cfg.black_bot_random ? "true" : "false") << "]\n";
    } else {
        if (cfg.bot_is_white) {
            std::cout << "  white: " << cfg.white_player_name
                      << " [bot, d=" << cfg.white_bot_difficulty
                      << ", style=" << StyleToString(cfg.white_bot_style)
                      << ", random=" << (cfg.white_bot_random ? "true" : "false") << "]\n";
            std::cout << "  black: " << cfg.black_player_name << " (human)\n";
        } else {
            std::cout << "  white: " << cfg.white_player_name << " (human)\n";
            std::cout << "  black: " << cfg.black_player_name
                      << " [bot, d=" << cfg.black_bot_difficulty
                      << ", style=" << StyleToString(cfg.black_bot_style)
                      << ", random=" << (cfg.black_bot_random ? "true" : "false") << "]\n";
        }
    }

    std::cout << "========================================\n\n";
}

bool ParseArgs(int argc, char* argv[], LaunchOptions& options) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--version") {
            std::cout << "TerminalChess-AI " << kVersion << "\n";
            std::exit(0);
        }

        if (arg == "--help") {
            PrintUsage(argv[0]);
            std::exit(0);
        }

        if (arg == "--no-profile") {
            options.show_profile = false;
            continue;
        }

        if (i + 1 >= argc) {
            std::cerr << "Missing value for argument: " << arg << "\n";
            return false;
        }

        const std::string value = argv[++i];
        if (arg == "--mode") {
            const std::string mode = ToLowerString(value);
            if (mode == "pvp") {
                options.config.against_bot = false;
                options.config.two_bots = false;
            } else if (mode == "pvb") {
                options.config.against_bot = true;
                options.config.two_bots = false;
            } else if (mode == "bvb") {
                options.config.against_bot = true;
                options.config.two_bots = true;
            } else {
                std::cerr << "Invalid mode: " << value << "\n";
                return false;
            }
        } else if (arg == "--preset") {
            if (!ApplyPreset(value, options)) {
                std::cerr << "Invalid preset: " << value << "\n";
                return false;
            }
        } else if (arg == "--bot-color") {
            const std::string bot_color = ToLowerString(value);
            if (bot_color == "white") {
                options.config.bot_is_white = true;
            } else if (bot_color == "black") {
                options.config.bot_is_white = false;
            } else {
                std::cerr << "Invalid bot color: " << value << "\n";
                return false;
            }
        } else if (arg == "--white-name") {
            options.config.white_player_name = value;
        } else if (arg == "--black-name") {
            options.config.black_player_name = value;
        } else if (arg == "--white-difficulty") {
            if (!ParseDifficulty(value, options.config.white_bot_difficulty)) {
                std::cerr << "Invalid white difficulty: " << value << "\n";
                return false;
            }
        } else if (arg == "--black-difficulty") {
            if (!ParseDifficulty(value, options.config.black_bot_difficulty)) {
                std::cerr << "Invalid black difficulty: " << value << "\n";
                return false;
            }
        } else if (arg == "--white-style") {
            if (!ParseStyle(value, options.config.white_bot_style)) {
                std::cerr << "Invalid white style: " << value << "\n";
                return false;
            }
        } else if (arg == "--black-style") {
            if (!ParseStyle(value, options.config.black_bot_style)) {
                std::cerr << "Invalid black style: " << value << "\n";
                return false;
            }
        } else if (arg == "--white-random") {
            if (!ParseBoolean(value, options.config.white_bot_random)) {
                std::cerr << "Invalid value for --white-random: " << value << "\n";
                return false;
            }
        } else if (arg == "--black-random") {
            if (!ParseBoolean(value, options.config.black_bot_random)) {
                std::cerr << "Invalid value for --black-random: " << value << "\n";
                return false;
            }
        } else if (arg == "--seed") {
            if (!ParseSeed(value, options.seed)) {
                std::cerr << "Invalid seed: " << value << "\n";
                return false;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return false;
        }
    }

    if (!options.config.against_bot) {
        options.config.white_bot_random = false;
        options.config.black_bot_random = false;
    }

    return true;
}

void PrintHumanPrompt() {
    std::cout << std::endl << TO_RIGHT << "Enter move coordinates (x1,y1)->(x2,y2):";
    std::cout << std::endl << std::endl << TO_RIGHT << "All possible moves:" << std::endl;
}

}  // namespace

int main(int argc, char* argv[]) {
    LaunchOptions options;
    if (!ParseArgs(argc, argv, options)) {
        return 1;
    }

    srand(options.seed);

    MaximizeConsoleWindow();
    if (options.show_profile) {
        PrintBanner();
        PrintProfile(options);
    }

    Chess chess(
        options.config.white_player_name,
        options.config.white_bot_difficulty,
        options.config.white_bot_style,
        options.config.white_bot_random,
        options.config.black_player_name,
        options.config.black_bot_difficulty,
        options.config.black_bot_style,
        options.config.black_bot_random
    );

    if (options.config.against_bot) {
        do {
            chess.PrintBoard();
            if (options.config.two_bots) {
                MoveCursorToXY(RIGHT, DOWN + 3 * BOARD_SIZE + 5);
                std::cout << CLEAR_LINE << std::endl << std::endl << CLEAR_LINE;
                while (chess.BotsTurn()) {
                }
            } else if (options.config.bot_is_white) {
                while (true) {
                    MoveCursorToXY(RIGHT, DOWN + 3 * BOARD_SIZE + 5);
                    std::cout << CLEAR_LINE << std::endl << std::endl << CLEAR_LINE;
                    if (!chess.BotsTurn()) {
                        break;
                    }
                    PrintHumanPrompt();
                    if (!chess.PlayersTurn()) {
                        break;
                    }
                }
            } else {
                while (true) {
                    if (!chess.PlayersTurn()) {
                        break;
                    }
                    MoveCursorToXY(RIGHT, DOWN + 3 * BOARD_SIZE + 5);
                    std::cout << CLEAR_LINE << std::endl << std::endl << CLEAR_LINE;
                    if (!chess.BotsTurn()) {
                        break;
                    }
                    PrintHumanPrompt();
                }
            }
        } while (chess.GameOver());
        return 0;
    }

    do {
        chess.PrintBoard();
        while (chess.PlayersTurn()) {
        }
    } while (chess.GameOver());

    return 0;
}
