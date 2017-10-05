#include <iostream>
#include <algorithm>

#include "Game.h"


Game::Game()
{
    for (int i = 0; i < BOARD_SIZE; ++i) {
        m_board[i].fill(CellState::Empty);
    }
    std::fill(m_votes_for_move.begin(), m_votes_for_move.end(), 0);
}

int Game::submit_move(int move)
{
    if (!is_legal(move)) {
        return -1;
    }

    m_votes_for_move[move]++;

    return move;
}

int Game::choose_move() const
{
    int res = std::distance(m_votes_for_move.begin(), std::max_element(m_votes_for_move.begin(), m_votes_for_move.end()));
    std::cout << "choose_move res = " << res << std::endl;
    return res;
}

bool Game::is_legal(int move) const
{
    if (move < 0 or move >= BOARD_SIZE_SQUARED)
        return false;

    int column;
    int row;
    std::tie(row, column) = move_to_pair(move);

    return m_board[row][column] == CellState::Empty;
}

std::pair<int, int> Game::move_to_pair(const int move)
{
    int column = move % BOARD_SIZE;
    int row = move / BOARD_SIZE;
    return std::make_pair(row, column);
}

std::pair<int, int> Game::end_turn()
{
    int move = choose_move();
    int column;
    int row;
    std::tie(row, column) = move_to_pair(move);

    if (m_currentplayer == Player::White)
    {
        m_board[row][column] = CellState::White;
        m_currentplayer = Player::Black;
    } else
    {
        m_board[row][column] = CellState::Black;
        m_currentplayer = Player::White;
    }

    std::fill(m_votes_for_move.begin(), m_votes_for_move.end(), 0);

    return std::make_pair(move, (m_currentplayer == Player::White) ? WHITE : BLACK);
}

// Implement game state management
// TBD...    
