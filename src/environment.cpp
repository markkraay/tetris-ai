#include <iostream>
#include <tuple>
#include "environment.hpp"
#include "board.hpp"
#include "piece.hpp"
#include "piece_types.hpp"
#include "action_space.hpp"
#include "dimension.hpp"

Environment::Environment(int r, int c, int bs)
{
    this->row = r;
    this->col = c;
    this->block_size = bs;

    this->board = Board(r, c, bs);
    this->piece = Piece(PieceTypes::random(), bs);

    int side_panel_width = c * bs;
    this->window = new sf::RenderWindow(sf::VideoMode(c * bs + side_panel_width, r * bs), "Tetris");
    this->window->setVerticalSyncEnabled(true);

    // Tetris Logo
    this->tetris_logo_texture.loadFromFile(RESOURCE_PATH "tetris-logo.png");
    this->tetris_logo.setTexture(this->tetris_logo_texture);

    auto size = this->tetris_logo.getGlobalBounds();
    this->tetris_logo.scale(200 / size.width, 150 / size.height);
    this->tetris_logo.setPosition(sf::Vector2f(
        c * bs + (side_panel_width - this->tetris_logo.getGlobalBounds().width) / 2,
        (r * bs) / 5));

    // Dividing Line
    this->dividing_line = sf::RectangleShape(sf::Vector2f(1, r * bs));
    this->dividing_line.setFillColor(sf::Color::White);
    this->dividing_line.setPosition(c * bs, 0);

    // Score Text, Level Info
    this->font.loadFromFile(RESOURCE_PATH "IBMPlexSans-SemiBold.ttf");
    this->score = sf::Text("Score: " + std::to_string(this->board.getScore()), this->font, 20);
    this->score.setPosition(
        c * bs + (side_panel_width - score.getLocalBounds().width) / 2,
        r * bs / 2);
    this->level = sf::Text("Level: " + std::to_string(this->board.getLevel()), this->font, 20);
    this->level.setPosition(
        c * bs + (side_panel_width - level.getLocalBounds().width) / 2,
        score.getPosition().y + score.getLocalBounds().height + 20);
    this->rows_removed = sf::Text("Rows Cleared: " + std::to_string(this->board.getRowsCleared()), this->font, 20);
    this->rows_removed.setPosition(
        c * bs + (side_panel_width - rows_removed.getLocalBounds().width) / 2,
        level.getPosition().y + level.getLocalBounds().height + 20);
}

void Environment::render()
{
    this->window->clear();
    for (const auto b : piece.getBlocks())
    {
        this->window->draw(b);
    }
    for (const auto b : board.getBlocks())
    {
        this->window->draw(b);
    }
    this->window->draw(tetris_logo);
    this->window->draw(dividing_line);
    this->window->draw(score);
    this->window->draw(level);
    this->window->draw(rows_removed);
    this->window->display();
}

bool Environment::moveDown()
{
    if (piece.moveDown(board) == false)
    {
        if (piece.hasBeenMoved() == false)
        {
            // Piece was not able to be moved when it spawned in, therefore the game is over
            active = false;
            return false;
        }
        else
        {
            // Insert the old piece into the board
            board.insertPiece(&piece);
            board.removeCompleted();
            // And Update the display
            score.setString("Score: " + std::to_string(board.getScore()));
            level.setString("Level: " + std::to_string(board.getLevel()));
            rows_removed.setString("Rows Cleared: " + std::to_string(board.getRowsCleared()));

            piece = Piece(PieceTypes::random(), piece.getBlockSize());
            this->getPieceConfigurations();
        }
    }
    // The game can continue
    return true;
}

void Environment::run()
{
    while (this->window->isOpen())
    {
        this->window->setFramerateLimit(2);
        if (moveDown() == false)
        {
            // The game is over.
            this->window->close();
        }
        sf::Event event;
        while (this->window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                this->window->close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                {
                    piece.moveLeft(board);
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    piece.moveRight(board);
                }
                else if (event.key.code == sf::Keyboard::Up)
                {
                    piece.rotate(board);
                }
                else if (event.key.code == sf::Keyboard::Down)
                {
                    this->window->setFramerateLimit(20);
                }
            }
        }
        render();
    }
}

void Environment::reset()
{
    this->board = Board(this->row, this->col, this->block_size);
    this->piece = Piece(PieceTypes::random(), this->block_size);
    score.setString("Score: " + std::to_string(board.getScore()));
    level.setString("Level: " + std::to_string(board.getLevel()));
    rows_removed.setString("Rows Cleared: " + std::to_string(board.getRowsCleared()));
    this->active = true;
}

Img Environment::getObservationSpace()
{
    auto dims = this->board.getDims();
    Img binary_board(dims.row, std::vector<int>(dims.col, 0));
    for (const auto &coord : this->board.getCoords())
    {
        binary_board[coord.y][coord.x] = 1;
    }
    return binary_board;
}

void Environment::move_down_helper(std::vector<std::tuple<Img, std::vector<ActionSpace::Action>>> &configs, Piece piece, int moves, ActionSpace::Action action, int rotations) 
{
    int m_down = 0;
    Piece down_piece = piece;
    while(down_piece.moveDown(this->board)) {
        m_down++;
    }
    Img img = this->getObservationSpace();
    for (const Coord &c : down_piece.getCoords()) {
        img[c.y][c.x] = 1;
    }
    std::vector<ActionSpace::Action> actions(rotations, ActionSpace::Action::Rotate);
    for (int i = 0; i < moves; i++) actions.push_back(action);
    for (int i = 0; i <= m_down; i++) actions.push_back(ActionSpace::Action::None);
    configs.push_back(std::make_tuple(img, actions));
}

std::vector<std::tuple<Img, std::vector<ActionSpace::Action>>> Environment::getPieceConfigurations()
{
    std::vector<std::tuple<Img, std::vector<ActionSpace::Action>>> configurations;
    auto dims = this->board.getDims();
    Piece original_piece = this->piece;

    // Find the width of the current piece for the number of right movements
    int rotations = 0;
    while(original_piece.rotate(this->board) && rotations < 4) {
        rotations++;
        // Straight down
        move_down_helper(configurations, original_piece, 0, ActionSpace::Action::None, rotations);
        // Left
        int m_left = 0;
        Piece left_piece = original_piece;
        while(left_piece.moveLeft(this->board)) {
            m_left++;
            move_down_helper(configurations, left_piece, m_left, ActionSpace::Action::Left, rotations);
        }
        // Right
        int m_right = 0;
        Piece right_piece = original_piece;
        while(right_piece.moveRight(this->board)) {
            m_right++;
            move_down_helper(configurations, right_piece, m_right, ActionSpace::Action::Right, rotations);
        }
    }
    return configurations;
}

void Environment::executeAction(ActionSpace::Action action)
{
    if (active == false)
        return;

    switch (action)
    {
    case ActionSpace::Action::Left:
        piece.moveLeft(board);
        return;
    case ActionSpace::Action::Right:
        piece.moveRight(board);
        return;
    case ActionSpace::Action::Rotate:
        piece.rotate(board);
        return;
    case ActionSpace::Action::None:
        moveDown();
        return;
    }
}

bool Environment::isActive()
{
    return this->active;
}
