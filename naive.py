from build.tetris_environment import Environment, ActionSpace
from time import sleep
from random import randint

def main():
	environment = Environment()
	environment.render()
	while environment.isActive():
		choice = randint(0, 4)
		if choice == 0:
			action = ActionSpace.none
		elif choice == 1:
			action = ActionSpace.left
		elif choice == 2:
			action = ActionSpace.right
		elif choice == 3:
			action = ActionSpace.rotate
		environment.executeAction(action)
		environment.render()
		sleep(0.5)

if __name__ == "__main__":
	main()
