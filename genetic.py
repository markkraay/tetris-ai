from build.tetris_environment import Environment, ActionSpace
import math
import numpy as np
import random

np.warnings.filterwarnings('ignore', category=np.VisibleDeprecationWarning)

class FitnessFunction():
	def __init__(self, a_h_w, c_l_w, h_w, b_w):
		self.aggregate_height_w = a_h_w
		self.complete_lines_w = c_l_w
		self.holes_w = h_w
		self.bumpiness_w = b_w
		self.old_observation = None

	def get_fitness(self, observation):
		# Generate a new fitness measurement
		aggregate_height = observation.tolist().count(1)
		complete_lines = 0
		if self.old_observation is not None:
			complete_lines = (self.old_observation.tolist().count(1) - observation.tolist().count(1)) / self.old_observation.shape[0]
		bumpiness = 0
		for col in range(1, observation.shape[1]):
			c1 = observation[:, col-1].tolist().count(1)
			c2 = observation[:, col].tolist().count(1)
			bumpiness += abs(c1 - c2)

		# Find the number of holes
		holes = 0
		observation = 1 - observation
		def fill_in(r, c):
			if r >= 0 and r < observation.shape[0] and c >= 0 and c < observation.shape[1] and observation[r][c] == 1:
				observation[r][c] = 0
				fill_in(r+1, c)
				fill_in(r-1, c)
				fill_in(r, c+1)
				fill_in(r, c-1)
			return
		for c in range(observation.shape[1]):
			fill_in(0, c)
		for c in range(observation.shape[1]):
			for r in range(observation.shape[0]):
				if observation[r][c] == 1:
					holes += 1

		print(f"Complete lines: {complete_lines}\nBumpiness: {bumpiness}\nHoles: {holes}\nHeight: {aggregate_height}")
		return (self.aggregate_height_w * aggregate_height +
			self.complete_lines_w * complete_lines +
			self.holes_w * holes +
			self.bumpiness_w * bumpiness)

	def find_best_actions(self, observations):
		# Using the weights, we have to test which piece configuration
		# results in the highest score.
		best_score = -math.inf 
		best_observation = None # A image of the board and the action sequence
		for observation in observations:
			fitness = self.get_fitness(np.array(observation[0]))
			if fitness > best_score:
				best_score = fitness
				best_observation = observation
		self.old_observation = np.array(best_observation[0])
		return best_observation[1] # Action sequence

def main():
	environment = Environment()
	environment.reset()
	evolution = 10000 # the number of time we introduce a new population
	fitness_func = FitnessFunction(-.51, .76, -.35, -.184)

	while environment.isActive():
		configs = environment.getPieceConfigurations()
		actions = fitness_func.find_best_actions(configs)
		for action in actions:
			environment.executeAction(action)
			environment.render()

if __name__ == "__main__":
	main()
