#pragma once

#include <cstdlib>

/**
 * @brief Typing for different pieces.
 */
namespace PieceTypes
{
	enum Type
	{
		I,
		J,
		L,
		O,
		S,
		T,
		Z
	};

	inline Type random()
	{
		switch (rand() % 7)
		{
		case 0:
			return PieceTypes::Type::I;
		case 1:
			return PieceTypes::Type::J;
		case 2:
			return PieceTypes::Type::L;
		case 3:
			return PieceTypes::Type::O;
		case 4:
			return PieceTypes::Type::S;
		case 5:
			return PieceTypes::Type::T;
		case 6:
			return PieceTypes::Type::Z;
		}
	}
};
