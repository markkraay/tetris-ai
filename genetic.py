from ga.FitnessFunction import FitnessFunction
from ga.Population import Population
from build.tetris_environment import Environment, ActionSpace
import os
import numpy as np

def main():
	environment = Environment()

	# Number of episodes
	# Each AI runs for 100 games; with at most 500 Tetris pieces. 
	population_size = 500
	number_of_generations = 500
	number_of_games = 100
	maximum_moves = 1_000
	target_lines_cleared = 50_000
	population = Population(distribution=np.random.normal(0, 1, size=(population_size, 4)))

	for generation in range(number_of_generations):
		# Score the population fitness function. 200 Games for each fitness function with a maximum of 500 moves
		for (i, member) in enumerate(population.members):
			print(f"""
				Generation: {generation}/{number_of_generations}
				Population Member: {i}/{population_size}
			 	{member}
			""")
			for _ in range(number_of_games):
				moves = 0
				environment.reset()
				while environment.isActive() and moves < maximum_moves:
					configs = environment.getPieceConfigurations()
					if len(configs) == 0:
						moves += 1
						environment.executeAction(ActionSpace.none)
						environment.render()
					else:
						actions = member.find_best_path(configs)
						for action in actions:
							moves += 1
							environment.executeAction(action)
							environment.render()
			if member.fitness > target_lines_cleared:
				print(member)
				break
		
		os.system('clear')
		print("Creating new population...")
		# Edit the population 
		reproduced = population.reproduce(.20, .30) # Generate a new group of reproduced members from the population, without altering the normal population
		mutated, reproduced = reproduced.mutate(.05) # Divides the reproduced population into two groups, one mutated
		population = population.kill(.3) # Remove the worst 30% from the population
		population = Population(reproduced, mutated, population)
	np.save("best_params", max(population.members).get_array())

if __name__ == "__main__":
	main()


	# while environment.isActive():
	# 	configs = environment.getPieceConfigurations()
	# 	if len(configs) == 0:
	# 		environment.executeAction(ActionSpace.none)
	# 		environment.render()
	# 	else:
	# 		actions = fitness_func.find_best_path(configs)
	# 		for action in actions:
	# 			environment.executeAction(action)
	# 			environment.render()
