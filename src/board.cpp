#include "board.hpp"
#include "piece.hpp"
#include "dimension.hpp"

#include <cstdint>
#include <iostream>
#include <set>
#include <map>
#include <utility>

Board::Board() {}

Board::Board(int rows, int cols, int block_size)
{
    this->rows = rows;
    this->cols = cols;
    this->block_size = block_size;
}

std::vector<sf::RectangleShape> Board::getBlocks()
{
    std::vector<sf::RectangleShape> rectangles;
    for (auto const &row : this->blocks)
    {
        for (auto const &block : row.second)
        {
            rectangles.push_back(block.second);
        }
    }
    return rectangles;
}

void Board::insertPiece(Piece *piece)
{
    // If the piece in a valid position, insert it into the board
    // blocks: maps y-value to the currently occupied x-values
    for (auto coord : piece->getCoords())
    {
        // Insert into block coordinates
        sf::RectangleShape block(sf::Vector2f(this->block_size, this->block_size));
        block.setFillColor(piece->getColor());
        block.setPosition(coord.x * this->block_size, coord.y * this->block_size);
        if (blocks.contains(coord.y) == false)
        {
            blocks[coord.y] = std::vector<std::pair<int, sf::RectangleShape>>{std::make_pair(coord.x, block)};
        }
        else
        {
            blocks[coord.y].push_back(std::make_pair(coord.x, block));
        }
    }
}

bool Board::validPiece(Piece *piece)
{
    // Checks if the pieces position is valid.
    for (auto coord : piece->getCoords())
    {
        // First: Check if any of the block's coordinates overextend the board
        if (coord.x < 0 || coord.x >= this->cols || coord.y < 0 || coord.y >= this->rows)
        {
            return false;
        }
        // Second: Check if any of the block's coordinates overlap with an already
        // inserted block.
        if (blocks.contains(coord.y))
        {
            for (const auto &pair : blocks[coord.y])
            {
                if (pair.first == coord.x)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

int calculateScore(int consecutive_cleared, int level)
{
    switch (consecutive_cleared)
    {
    case 0:
        return 0;
    case 1:
        return 40 * level;
    case 2:
        return 100 * level;
    case 3:
        return 300 * level;
    case 4:
        return 1200 * level;
    }
}

void Board::removeCompleted()
{
    std::map<int, std::vector<std::pair<int, sf::RectangleShape>>> new_coords;

    int increment_y = 0;
    int consecutiveCleared = 0;

    for (auto it = this->blocks.rbegin(); it != this->blocks.rend(); it++)
    {
        auto xs = it->second;
        auto y = it->first;
        if (xs.size() == this->cols)
        { // Row completely filled out
            // Increment all the new_coords y-indexes
            increment_y++;
            this->rowsCleared++;
            consecutiveCleared++;
        }
        else
        {
            // Add the row into new_coords
            new_coords[y + increment_y] = xs;
            this->score += calculateScore(consecutiveCleared, this->level);
            consecutiveCleared = 0;
        }
    }

    // Update the block positions based on their y values
    for (auto &row : new_coords) {
        for (auto &block : row.second) {
            block.second.setPosition(sf::Vector2f(block.second.getPosition().x, row.first * this->block_size));
        }
    }

    this->blocks = new_coords;
    this->score += calculateScore(consecutiveCleared, this->level);

    if (this->rowsCleared >= this->level * 5)
    {
        this->rowsCleared = 0;
        this->level++;
    }
}

int Board::getScore()
{
    return this->score;
}

int Board::getLevel()
{
    return this->level;
}

int Board::getRowsCleared()
{
    return this->rowsCleared;
}

std::vector<Coord> Board::getCoords() {
    std::vector<Coord> coords;
    for (auto row : this->blocks) {
        for (auto col : row.second) {
            coords.push_back(Coord{row.first, col.first});
        }
    }
    return coords;
}

Dimension Board::getDims() {
    return Dimension{this->rows, this->cols};
}