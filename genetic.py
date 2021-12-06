from build.tetris_environment import Environment, ActionSpace
import random

class FitnessFunction():
	def __init__(self, a_h_w, c_l_w, h_w, b_w):
		r = lambda: random.random()
		self.fitness = 0
		self.aggregate_height_w = a_h_w or r()
		self.complete_lines_w = c_l_w or r()
		self.holes_w = h_w or r()
		self.bumpiness_w = b_w or r()

	def get_fitness(self, observation):
		# Generate a new fitness measurement
		aggregate_height = observation.tolist().count(1)
		complete_lines = (self.old_observation.tolist().count(1) - observation.tolist().count(1)) / old_observation.shape[0]
		bumpiness = 0
		for col in range(1, observation.shape[1]):
			c1 = observation[:, col-1].tolist().count(1)
			c2 = observation[:, col].tolist().count(1)
			bumpiness += abs(c1 - c2)

		# Find the number of holes
		holes = 0
		observation = 1 - x
		def fill_in(r, c):
			if r >= 0 and r < x.shape[0] and c >= 0 and c < x.shape[1] and x[r][c] == 1:
				x[r][x] = 0
				fill_in(r+1, c)
				fill_in(r-1, c)
				fill_in(r, c+1)
				fill_in(r, c-1)
			return
		for c in range(x.shape[1]):
			fill_in(0, c)
		for c in range(x.shape[1]):
			for r in range(x.shape[0]):
				if x[r][c] == 1:
					holes += 1

		return (self.aggregate_height_w * aggregate_height +
			self.complete_line_w * complete_lines +
			self.holes_w * holes +
			self.bumpiness_w * bumpiness)

	def find_best_actions(self, observations):
		# Using the weights, we have to test which piece configuration
		# results in the highest score.
		for observation in observations:
			pass


def main():
	environment = Environment()
	evolution = 10000 # the number of time we introduce a new population
	fitness_func = FitnessFunction(-.51, .76, -.35, -.184)

	while environment.isActive():
		print(environment.getPieceConfigurations())
#		action = fitness_func.find_best_action(environment.getPieceConfigurations())
#		print(action)
#		environment.executeAction(action)
#		environment.render()

if __name__ == "__main__":
	main()
