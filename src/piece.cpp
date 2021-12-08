#include "piece.hpp"
#include "coord.hpp"
#include "board.hpp"
#include "piece_types.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

Piece::Piece() {}

Piece::Piece(const Piece &p) {
    this->block_size = p.block_size;
    this->blocks = p.blocks;
    this->color = p.color;
    this->coords = p.coords;
    this->moved = p.moved;
    this->transformation_index = p.transformation_index;
    this->transformations = p.transformations;
    this->type = p.type;
}

Piece::Piece(PieceTypes::Type t, int size)
{
    this->type = t;
    this->block_size = size;

    switch (t)
    {
    case PieceTypes::Type::I:
        this->coords = std::vector<Coord>{Coord{0, 0}, Coord{1, 0}, Coord{2, 0}, Coord{3, 0}};
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{2, -1}, Coord{1, 0}, Coord{0, 1}, Coord{-1, 2}},
            {Coord{1, 2}, Coord{0, 1}, Coord{-1, 0}, Coord{-2, -1}},
            {Coord{-2, 1}, Coord{-1, 0}, Coord{0, -1}, Coord{1, -2}},
            {Coord{-1, -2}, Coord{0, -1}, Coord{1, 0}, Coord{2, 1}},
        };
        this->color = sf::Color(95, 237, 230);
        break;
    case PieceTypes::Type::J:
        this->coords = {Coord{0, 0}, Coord{0, 1}, Coord{1, 1}, Coord{2, 1}},
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{2, 0}, Coord{1, -1}, Coord{0, 0}, Coord{-1, 1}},
            {Coord{0, 2}, Coord{1, 1}, Coord{0, 0}, Coord{-1, -1}},
            {Coord{-2, 0}, Coord{-1, 1}, Coord{0, 0}, Coord{1, -1}},
            {Coord{0, -2}, Coord{-1, -1}, Coord{0, 0}, Coord{1, 1}},
        },
        this->color = sf::Color::Blue;
        break;
    case PieceTypes::Type::L:
        this->coords = std::vector<Coord>{Coord{0, 1}, Coord{1, 1}, Coord{2, 1}, Coord{2, 0}};
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{1, -1}, Coord{0, 0}, Coord{-1, 1}, Coord{0, 2}},
            {Coord{1, 1}, Coord{0, 0}, Coord{-1, -1}, Coord{-2, 0}},
            {Coord{-1, 1}, Coord{0, 0}, Coord{1, -1}, Coord{0, -2}},
            {Coord{-1, -1}, Coord{0, 0}, Coord{1, 1}, Coord{2, 0}},
        };
        this->color = sf::Color(255, 119, 0);
        break;
    case PieceTypes::Type::O:
        this->coords = std::vector<Coord>{Coord{0, 0}, Coord{1, 0}, Coord{0, 1}, Coord{1, 1}};
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{0, 0}, Coord{0, 0}, Coord{0, 0}, Coord{0, 0}}};
        this->color = sf::Color::Yellow;
        break;
    case PieceTypes::Type::S:
        this->coords = std::vector<Coord>{Coord{0, 1}, Coord{1, 1}, Coord{1, 0}, Coord{2, 0}};
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{2, 1}, Coord{1, 0}, Coord{0, 1}, Coord{-1, 0}},
            {Coord{-2, 0}, Coord{-1, 1}, Coord{0, 0}, Coord{1, 1}},
            {Coord{1, 0}, Coord{0, -1}, Coord{-1, 0}, Coord{-2, -1}},
            {Coord{-1, -1}, Coord{0, 0}, Coord{1, -1}, Coord{2, 0}},
        };
        this->color = sf::Color::Green;
        break;
    case PieceTypes::Type::T:
        this->coords = std::vector<Coord>{Coord{0, 1}, Coord{1, 1}, Coord{2, 1}, Coord{1, 0}};
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{1, -1}, Coord{0, 0}, Coord{-1, 1}, Coord{1, 1}},
            {Coord{1, 1}, Coord{0, 0}, Coord{-1, -1}, Coord{-1, 1}},
            {Coord{-1, 1}, Coord{0, 0}, Coord{1, -1}, Coord{-1, -1}},
            {Coord{-1, -1}, Coord{0, 0}, Coord{1, 1}, Coord{1, -1}},
        };
        this->color = sf::Color(133, 3, 130);
        break;
    case PieceTypes::Type::Z:
        this->coords = std::vector<Coord>{Coord{0, 0}, Coord{1, 0}, Coord{1, 1}, Coord{2, 1}};
        this->transformations = std::vector<std::vector<Coord>>{
            {Coord{2, 0}, Coord{1, 1}, Coord{0, 0}, Coord{-1, 1}},
            {Coord{0, 2}, Coord{-1, 1}, Coord{0, 0}, Coord{-1, -1}},
            {Coord{-2, 0}, Coord{-1, -1}, Coord{0, 0}, Coord{1, -1}},
            {Coord{0, -2}, Coord{1, -1}, Coord{0, 0}, Coord{1, 1}}},
        this->color = sf::Color::Red;
        break;
    default:
        break;
    }

    for (auto coord : this->coords)
    {
        sf::RectangleShape block(sf::Vector2f(size, size));
        block.setPosition(sf::Vector2f(coord.x * size, coord.y * size));
        block.setFillColor(this->color);
        this->blocks.push_back(block);
    }
}

std::vector<Coord> Piece::getCoords()
{
    return this->coords;
}

sf::Color Piece::getColor()
{
    return this->color;
}

std::vector<sf::RectangleShape> Piece::getBlocks() 
{
    return this->blocks;
}

bool Piece::moveDown(Board &board)
{
    // First substract the coordinates
    std::for_each(coords.begin(), coords.end(), [](Coord &c)
                  { c.y++; });

    if (board.validPiece(this))
    {
        // Adjust the block positions
        for (auto &block : this->blocks)
        {
            block.move(0, this->block_size);
        }
        this->moved = true;
        return true;
    }
    // Not a valid piece, so return the y-coordinates
    std::for_each(coords.begin(), coords.end(), [](Coord &c)
                  { c.y--; });
    return false;
}

bool Piece::moveLeft(Board &board)
{
    // First substract the coordinates
    std::for_each(coords.begin(), coords.end(), [](Coord &c)
                  { c.x--; });

    if (board.validPiece(this))
    {
        // Adjust the block positions
        for (auto &block : this->blocks)
        {
            block.move(-this->block_size, 0);
        }
        return true;
    }
    // Not a valid piece, so return the y-coordinates
    std::for_each(coords.begin(), coords.end(), [](Coord &c)
                  { c.x++; });
    return false;
}

bool Piece::moveRight(Board &board)
{
    // First substract the coordinates
    std::for_each(coords.begin(), coords.end(), [](Coord &c)
                  { c.x++; });

    if (board.validPiece(this))
    {
        // Adjust the block positions
        for (auto &block : this->blocks)
        {
            block.move(this->block_size, 0);
        }
        return true;
    }
    // Not a valid piece, so return the y-coordinates
    std::for_each(coords.begin(), coords.end(), [](Coord &c)
                  { c.x--; });
    return false;
}

bool Piece::rotate(Board &board)
{
    // First transform the positions
    auto trans = this->transformations[this->transformation_index];

    // Adjust the positions
    for (int i = 0; i < this->coords.size(); i++)
    {
        this->coords[i].x += trans[i].x;
        this->coords[i].y += trans[i].y;
    }

    if (board.validPiece(this))
    {
        // If the rotation is valid, move the block graphics
        for (int i = 0; i < this->blocks.size(); i++)
        {
            blocks[i].move(trans[i].x * this->block_size, trans[i].y * this->block_size);
        }
        this->transformation_index = (this->transformation_index + 1) % this->transformations.size();
        return true;
    }

    // Return the positions to normal
    for (int i = 0; i < this->coords.size(); i++)
    {
        this->coords[i].x -= trans[i].x;
        this->coords[i].y -= trans[i].y;
    }
    return false;
}

bool Piece::hasBeenMoved()
{
    return this->moved;
}

int Piece::getBlockSize() {
    return this->block_size;
}