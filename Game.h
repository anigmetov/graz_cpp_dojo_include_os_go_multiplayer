#pragma once

#include <array>

#include "GameSettings.h"

class Game {

public:
    enum class CellState {
        Empty, Black, White
    };
    enum class Player {
        Black, White
    };
    Game();
    int submit_move(int move);
    std::pair<int, int> end_turn();
private:
    bool is_legal(int move) const;
    int choose_move() const;
    static std::pair<int, int> move_to_pair(const int move);
    std::array<std::array<CellState, BOARD_SIZE>, BOARD_SIZE> m_board;
    std::array<int, BOARD_SIZE_SQUARED> m_votes_for_move;
    Player m_currentplayer {Player::Black};
};
