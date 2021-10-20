#pragma once

#include <cstdlib>

namespace ActionSpace {
	enum Action {
		Left, Right, Rotate, None
	};

	inline Action random() {
		switch(rand() % 4) {
			case 0:
				return Action::Left;
			case 1:
				return Action::Right;
			case 2:
				return Action::Rotate;
			case 3:
				return Action::None;
		}
	}
}