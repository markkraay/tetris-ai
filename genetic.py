from build.environment import Environment, ActionSpace
import random

class FitnessFunction():
	def __init__(self, a_h_w, c_l_w, h_w, b_w):
		r = lambda: random.random()
		self.fitness = 0
		self.aggregate_height_w = a_h_w or r()
		self.complete_lines_w = c_l_w or r()
		self.holes_w = h_w or r()
		self.bumpiness_w = b_w or r()

	def score(self, observation):
		# Generate a new fitness measurement
		self.fitness += new_fitness

		# TODO: Calculate the score of the configuration

	def find_best_action(self, observations):
		# Using the weights, we have to test which piece configuration
		# results in the highest score.
		
		# TODO: Return the best action
	
	# TODO: Find a course of actions that will get to this position


def main():
	environment = Environment()
	evolution = 10000 # the number of time we introduce a new population
	fitness_func = FitnessFunction(-.51, .76, -.35, -.184)

	while environment.isActive():
		action = fitness_func.find_best_action(environment.getPieceConfigurations())
		print(action)
		environment.executeAction(action)
		environment.render()

if __name__ == "__main__":
