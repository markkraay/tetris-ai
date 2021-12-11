import numpy as np
import math

class FitnessFunction:
	def __init__(self, aggregate_height_w, complete_lines_w, holes_w, bumpiness_w):
		self.aggregate_height_w = aggregate_height_w
		self.complete_lines_w = complete_lines_w
		self.holes_w = holes_w
		self.bumpiness_w = bumpiness_w
		self.previous_observation = None
		self.fitness = 0 # Equal to the total number of lines cleared
	
	def __lt__(self, other):
		return self.fitness < other.fitness

	def __repr__(self):
		return f"""
		Aggregate Height: {self.aggregate_height_w}
		Complete Lines: {self.complete_lines_w}
		Holes: {self.holes_w}
		Bumpiness: {self.bumpiness_w}
		"""

	@staticmethod 
	def get_dims(observation):
		if isinstance(observation, np.ndarray):
			return (observation.shape[0], observation.shape[1])
		return (0, 0)

	@staticmethod
	def fill_in_curve(observation):
		# First fill in the area under the curve
		(rows, cols) = FitnessFunction.get_dims(observation)
		for col in range(cols):
			under_curve = False
			for row in range(rows):
				# Fill in the area under the lines
				if observation[row][col] == 1 or under_curve:
					under_curve = True
					observation[row][col] = 1
		return observation
	
	def get_array(self):
		return np.array([self.aggregate_height_w, self.complete_lines_w, self.holes_w, self.bumpiness_w])

	def get_aggregate_height(self, observation):
		observation = self.fill_in_curve(observation)
		return observation.sum()

	def get_complete_lines(self, observation):
		complete_lines = 0
		if self.previous_observation is not None:
			complete_lines = max(int((self.previous_observation.sum() - observation.sum()) / self.previous_observation.shape[1]), 0)
		self.fitness += complete_lines
		return complete_lines

	def get_holes(self, observation):
		holes = 0
		observation = 1 - observation
		(rows, cols) = self.get_dims(observation)
		# Filling in the areas that can touch the top of the grid because they are not holes.
		def fill_in(r, c):
			if (r >= 0 and r < rows and c >= 0 and 
					c < cols and observation[r][c] == 1):
				observation[r][c] = 0
				fill_in(r+1, c)
				fill_in(r-1, c)
				fill_in(r, c+1)
				fill_in(r, c-1)
			return
		for c in range(cols):
			fill_in(0, c)
		for c in range(cols):
			for r in range(rows):
				if observation[r][c] == 1:
					holes += 1
		return holes

	def get_bumpiness(self, observation):
		observation = self.fill_in_curve(observation)
		bumpiness = 0
		for col in range(1, observation.shape[1]):
			c1 = observation[:, col-1].sum()
			c2 = observation[:, col].sum()
			bumpiness += abs(c1 - c2)
		return bumpiness

	def calculate_fitness(self, aggregate_height, complete_lines, holes, bumpiness):
		return (self.aggregate_height_w * aggregate_height +
			self.complete_lines_w * complete_lines + 
			self.holes_w * holes + 
			self.bumpiness_w * bumpiness)
	
	def find_best_path(self, observations):
		best_score = -math.inf
		best_observation = None
		for observation in observations:
			state = observation[0]
			height = self.get_aggregate_height(state)
			lines = self.get_complete_lines(state)
			holes = self.get_holes(state)
			bumpiness = self.get_bumpiness(state)
			fitness = self.calculate_fitness(height, lines, holes, bumpiness)
			if fitness > best_score:
				best_score = fitness
				best_observation = observation
		self.previous_observation = best_observation[0]
		return best_observation[1] # The action sequence
	
	def crossover(self, other):
		weights = (
			self.get_array() * self.fitness +
			other.get_array() * other.fitness
		)
		return FitnessFunction(weights[0], weights[1], weights[2], weights[3])


