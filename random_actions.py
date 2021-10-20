# Random choice algorithm

from ..build.tetris_environment import Environment, ActionSpace
import random

if __name__ == "__main__":
	environment = Environment()

	while environment.isActive():
		choice = random.randint(0, 3)
		if choice == 1:
			environment.executeAction(ActionSpace.left)
		elif choice == 2:
			environment.executeAction(ActionSpace.right)
		elif choice == 3:
			environment.executeAction(ActionSpace.rotate)
		else:
			environment.executeAction(ActionSpace.none)

		environment.render()