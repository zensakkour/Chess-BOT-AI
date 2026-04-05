# Architecture Overview

## Core Modules
- `src/main.cpp`: CLI entrypoint, preset system, startup profile banner, and game orchestration.
- `src/chess.cpp`: game state, move generation, move validation, board evaluation, and terminal UI.
- `src/path_node.cpp`: minimax search tree and alpha-beta pruning implementation.
- `src/bot.cpp`: bot wrapper over the search root.
- `src/player.cpp`: player identity, score tracking, and castling rights.
- `include/chess_engine/chess.hpp`: shared model definitions and engine interfaces.

## Engine Rules Implemented
- Legal move generation for all pieces.
- Castling.
- En passant.
- Pawn promotion.
- Fifty-move rule.
- Threefold repetition.
- Check and checkmate detection.

## Evaluation and Search
- Material values per piece.
- Piece-square tables for positional scoring.
- Alpha-beta pruning with configurable search depth.
- Optional random-move mode for either side.
- Style-aware root scoring to differentiate bot personalities:
  - `balanced`
  - `aggressive`
  - `positional`

## Runtime Profiles
- `classic`: neutral defaults.
- `creator`: human-vs-bot profile with aggressive black-side AI.
- `duel`: higher-depth bot-vs-bot benchmark profile.
- `chaos`: random-heavy bot-vs-bot profile for non-deterministic runs.

## Platform Behavior
- Uses Windows console APIs on Windows.
- Falls back to ANSI escape sequences and termios key input on Unix-like terminals.
