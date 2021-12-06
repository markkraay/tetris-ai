#pragma once

#include <vector>
#include <cstdlib>
#include "piece_types.hpp"
#include "coord.hpp"
#include "SFML/Graphics.hpp"

class Board;

/**
 * @brief Basic Tetrimino piece. It's shape and color are dependent
 * upon its piece type.
 */
class Piece
{
private:
    PieceTypes::Type type;

    uint_fast16_t transformation_index = 0;
    std::vector<std::vector<Coord>> transformations;

    std::vector<Coord> coords;

    std::vector<sf::RectangleShape> blocks;
    sf::Color color;
    int block_size;
    bool moved = false;

public:
    Piece(PieceTypes::Type t, int size);
    Piece(Piece &p);
    Piece();

    std::vector<Coord> getCoords();
    sf::Color getColor();
    PieceTypes::Type getType();
    std::vector<sf::RectangleShape> getBlocks();
    int getBlockSize();
    bool hasBeenMoved();

    bool moveDown(Board &board);
    void moveLeft(Board &board);
    void moveRight(Board &board);
    void rotate(Board &board);
};
