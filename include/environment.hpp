#pragma once

#include "coord.hpp"
#include "board.hpp"
#include "piece.hpp"
#include "action_space.hpp"

/**
 * @brief Provides wrapper functionality for the Board class, implementing
 * game graphics and providing mechanisms for controlling the Board.
 */
class Environment {
	private:
		bool active;
		int row;
		int col;
		int block_size;

		sf::RenderWindow *window;

		sf::Font font;
		sf::Text score;
		sf::Text level;
		sf::Text rows_removed;

		sf::RectangleShape dividing_line;

		sf::Texture tetris_logo_texture;
		sf::Sprite tetris_logo;

		Board board;
		Piece piece; 
	public:
		Environment(int r=20, int c=10, int bs=30); // Rows, columns, block size

		void render();
		void reset();
		void run();
		bool moveDown();

		std::vector<std::vector<int>> getObservationSpace();
		std::vector<std::vector<std::vector<int>>> getPieceConfigurations();
		void executeAction(ActionSpace::Action action);
		bool isActive();
};
