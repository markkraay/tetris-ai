from build.tetris_environment import Environment, ActionSpace
import math
import numpy as np
from time import sleep
import random

np.warnings.filterwarnings('ignore', category=np.VisibleDeprecationWarning)

class FitnessFunction():
	def __init__(self, a_h_w, c_l_w, h_w, b_w):
		self.aggregate_height_w = a_h_w
		self.complete_lines_w = c_l_w
		self.holes_w = h_w
		self.bumpiness_w = b_w
		self.old_observation = None

	def get_params(self, observation):
		# Generate a new fitness measurement
		complete_lines = 0
		if self.old_observation is not None:
			complete_lines = max(((self.old_observation.sum() - observation.sum()) / self.old_observation.shape[1]), 0)

		# Find the number of holes
		holes = 0
		holes_obs = 1 - observation
		def fill_in(r, c):
			if r >= 0 and r < holes_obs.shape[0] and c >= 0 and c < holes_obs.shape[1] and holes_obs[r][c] == 1:
				holes_obs[r][c] = 0
				fill_in(r+1, c)
				fill_in(r-1, c)
				fill_in(r, c+1)
				fill_in(r, c-1)
			return
		for c in range(holes_obs.shape[1]):
			fill_in(0, c)
		for c in range(holes_obs.shape[1]):
			for r in range(holes_obs.shape[0]):
				if holes_obs[r][c] == 1:
					holes += 1

		bumpiness = 0
		# First fill in the area under the curve
		for col in range(observation.shape[1]):
			under_curve = False
			for row in range(observation.shape[0]):
				# Fill in the area under the lines
				if observation[row][col] == 1 or under_curve:
					under_curve = True
					observation[row][col] = 1

		aggregate_height = observation.sum()
		for col in range(1, observation.shape[1]):
			c1 = observation[:, col-1].sum()
			c2 = observation[:, col].sum()
			bumpiness += abs(c1 - c2)

		return (aggregate_height, complete_lines, holes, bumpiness)

	def get_fitness(self, a_h, c_l, h, b):
		return (self.aggregate_height_w * a_h +
			self.complete_lines_w * c_l +
			self.holes_w * h +
			self.bumpiness_w * b)

	def find_best_actions(self, observations):
		# Using the weights, we have to test which piece configuration
		# results in the highest score.
		best_score = -math.inf 
		best_observation = None # A image of the board and the action sequence
		f_holes = 0
		f_lines = 0
		f_height = 0
		f_bumps = 0
		for observation in observations:
			(height, lines, holes, bumps) = self.get_params(np.array(observation[0]))
			fitness = self.get_fitness(height, lines, holes, bumps)
			if fitness > best_score:
				best_score = fitness
				best_observation = observation
				f_holes = holes
				f_lines = lines
				f_height = height
				f_bumps = bumps
		self.old_observation = np.array(best_observation[0])
		return best_observation[1] # Action sequence

def main():
	environment = Environment()
	environment.reset()
	evolution = 10000 # the number of time we introduce a new population
	fitness_func = FitnessFunction(-.51, .76, -.35, -.184)

	while environment.isActive():
		configs = environment.getPieceConfigurations()
		if len(configs) == 0:
			environment.executeAction(ActionSpace.none)
			environment.render()
		else:
			actions = fitness_func.find_best_actions(configs)
			for action in actions:
				environment.executeAction(action)
				environment.render()
	

if __name__ == "__main__":
	main()
