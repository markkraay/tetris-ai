from random import sample
import numpy as np
from ga.FitnessFunction import FitnessFunction

class Population:
	def __init__(self, members=None, distribution=None, *populations):
		self.members = []
		if isinstance(distribution, np.ndarray):
			self.members = [FitnessFunction(vector[0], vector[1], vector[2], vector[3]) for vector in distribution]
		elif isinstance(members, list):
			self.members = members
		else: 
			self.members = [member for population in populations for member in population.members]
	
	def reproduce(self, percentage: float, to: float):
		"""
			Select the top @param1 percentage of the population at random,
			and the two fittest mates are paired up in this subpool for crossover
			to produce new offspring, repeating the process until @param2 individuals
			are created.
		"""
		offspring_pool = []
		while len(offspring_pool) < int(len(self.members) * to):
			parents = sorted(sample(self.members, int(len(self.members) * percentage)), reverse=True)
			# try:
			parent1 = parents[0]
			parent2 = parents[1]
			offspring_pool.append(parent1.crossover(parent2))
			# xcept:
				# raise ValueError("The percentage * members is two small. Consider increasing the percentage.")
		return Population(offspring_pool)
	
	def mutate(self, chance: float):
		"""
			Mutate a @param1 percentage of the population.
		"""
		mutated = []
		non_mutated = []
		for member in self.members:
			if np.random.rand() < chance:
				# Select random parameter to adjust
				parameter = np.random.randint(0, 4)
				# Parameter adjustment
				adjustment = np.random.uniform(-.2, .2)
				if parameter == 0:
					member.aggregate_height_w += adjustment
				elif parameter == 1:
					member.bumpiness_w += adjustment
				elif parameter == 2:
					member.complete_lines_w += adjustment
				elif parameter == 3:
					member.holes_w += adjustment
				mutated.append(member)
			else:
				non_mutated.append(member)
		return (Population(mutated), Population(non_mutated))
	
	def kill(self, percentage: float):
		"""
			Return the population with the worst @param1 percentage of the population removed.
		"""
		remaining = sorted(self.members, reverse=True)[0: int(len(self.members) * (1 - percentage))]
		return Population(remaining)