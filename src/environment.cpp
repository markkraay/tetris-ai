#include <tuple>
#include "environment.hpp"
#include "board.hpp"
#include "piece.hpp"
#include "piece_types.hpp"
#include "action_space.hpp"
#include "dimension.hpp"

typedef std::vector<std::vector<int>> Img;

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

void Environment::reset() {
    this->board = Board(this->row, this->col, this->block_size);
    this->piece = Piece(PieceTypes::random(), this->block_size);
    score.setString("Score: " + std::to_string(board.getScore()));
    level.setString("Level: " + std::to_string(board.getLevel()));
    rows_removed.setString("Rows Cleared: " + std::to_string(board.getRowsCleared()));
    this->active = true;
}

std::vector<std::vector<int>> Environment::getObservationSpace()
{
    auto dims = this->board.getDims();
    std::vector<std::vector<int>> binary_board(dims.row, std::vector(dims.col, 0));
    for (const auto &coord : this->board.getCoords()) {
        binary_board[coord.x][coord.y] = 1;
    }
    return binary_board;
}

std::vector<std::tuple<Img, std::vector<ActionSpace::Action>>> Environment::getPieceConfigurations() 
{
    std::vector<std::tuple<Img, std::vector<ActionSpace::Action>>> configurations;
    auto dims = this->board.getDims();
    Piece original_piece = this->piece;

    // Find the width of the current piece for the number of right movements
    int min, max;
    for (const Coord &c : original_piece.getCoords()) {
        if (c.x < min) min = c.x;
        if (c.x > max) max = c.x;
    }

    // Have to test each rotation and each row
    for (int i = 0; i < 4; i++) { // Assuming 4 rotations for each block (not the case for the square block)
        this->piece.rotate(this->board);
        Piece rotation_piece = this->piece;
        for (int j = min; j < max; j++) {
            rotation_piece.moveRight(this->board);
            Piece drop_piece = rotation_piece;
            int moves_down = 0;
            while (drop_piece.moveDown(this->board)) {
                moves_down++;
            }
            // Add the board configuration to the total configurations
            std::vector<std::vector<int>> img(dims.row, std::vector(dims.col, 0));
            for (const auto &coord : this->board.getCoords()) {
                img[coord.x][coord.y] = 1;
            }
            for (const auto &coord : drop_piece.getCoords()) {
                img[coord.x][coord.y] = 1;
            }
            std::vector<ActionSpace::Action> actions(i, ActionSpace::Action::Rotate);
            for (int a = 0; a < j; a++) actions.push_back(ActionSpace::Action::Right);
            for (int a = 0; a < moves_down; a++) actions.push_back(ActionSpace::Action::None);
            configurations.push_back(std::tuple<Img, std::vector<ActionSpace::Action>>{img, actions});
        }
    }
    this->piece = original_piece;
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
