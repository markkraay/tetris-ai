#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <map>
#include <set>

class Piece;

/**
 * @brief Tetris game board
 * 
 */
class Board
{
private:
    int cols;
    int rows;
    int block_size;

    std::map<int, std::vector<std::pair<int, sf::RectangleShape>>> blocks;

    int score = 0;
    int level = 1;
    int rowsCleared = 0;

public:
    Board();
    Board(int rows, int cols, int block_size);
    void insertPiece(Piece *piece);
    bool validPiece(Piece *piece);

    std::vector<sf::RectangleShape> getBlocks();
    int getScore();
    int getLevel();
    int getRowsCleared();

    void removeCompleted();
};
