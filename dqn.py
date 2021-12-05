from os import environ
from build.tetris_environment import Environment, ActionSpace
import numpy as np
import tensorflow as tf
import random
from collections import deque

def calculate_fitness(old_observation, new_observation):
	# Heuristic from: https://codemyroad.wordpress.com/2013/04/14/tetris-ai-the-near-perfect-player/
	# Fitness is the -.51 * Height + .76 * Lines - .36 * Holes - .18 * Bumpiness
	# 1) We calculate the aggregate height, which is pretty much just how many blocks
	# are present
	aggregate_height = new_observation.tolist().count(1)
	# 2) We calculate the number of complete lines. We can find this by finding the change
	# in the number of blocks between observations and dividing by the number of blocks that
	# are present in a row
	complete_lines = (old_observation.tolist().count(1) - new_observation.tolist().count(1)) / old_observation.shape[0]
	# 3) We calculate the number of holes, or the number of emtpy spaces with blocks above them.
	holes = 0
	for col in range(new_observation.shape[1]):
		for row in range(new_observation.shape[0]):
			if row != 0 and new_observation[row][col] == 0 and new_observation[row-1][col] == 1:
				holes += 1
	# 4) We calculate the bumpiness, which is the absolute value of the difference in consecutive heights
	bumpiness = 0
	for col in range(1, new_observation.shape[1]):
		c1 = new_observation[:, col-1].tolist().count(1)
		c2 = new_observation[:, col].tolist().count(1)
		bumpiness += abs(c1 - c2)
	
	# Return the weighted sum of these values as the fitness
	return -.51 * aggregate_height + .76 * complete_lines -.36 * holes -.18 * bumpiness

def step(environment, observation, action):
	"""
	@params1: environment: The Tetris environment
	@params2: observation: The current observation
	@returns (new_observatio, reward, done)
	"""
	print(action)
	environment.executeAction(action)
	new_observation = environment.getObservationSpace()
	reward = calculate_fitness(observation, new_observation)
	done = not environment.isActive()
	return (new_observation, reward, done)

def agent(state_shape, action_shape):
	"""
	Agent maps x-states to y-actions
	"""
	learning_rate = .001
	init = tf.keras.initializers.HeUniform()
	model = tf.keras.Sequential()
	model.add(tf.keras.layers.Dense(24, input_shape=state_shape, activation="relu", kernel_initializer=init))
	model.add(tf.keras.layers.Dense(12, activation="relu", kernel_initializer=init))
	model.add(tf.keras.layers.Dense(action_shape, activation="relu", kernel_initializer=init))
	model.compile(loss=tf.keras.losses.Huber(),
		optimizer=tf.keras.optimizers.Adam(lr=learning_rate),
		metrics=['accuracy'])
	return model

def train(replay_memory, model, done):
	learning_rate = .7
	discount_factor = .618

	if len(replay_memory) < 1000:
		return

	batch_size = 64 * 2
	mini_batch = random.sample(replay_memory, batch_size)

	current_states = np.array([transition[0] for transition in mini_batch])
	current_qs_list = model.predict(current_states)

	new_current_states = np.array([transition[3] for transition in mini_batch])
	future_qs_list = model.predict(new_current_states)

	X = []
	Y = []
	for index, (observation, action, reward, new_observation, done) in enumerate(mini_batch):
		if not done:
			max_future_q = reward + discount_factor * np.max(future_qs_list[index])
		else:
			max_future_q = reward

		current_qs = current_qs_list[index]
		current_qs[action] = (1 - learning_rate) * current_qs[action] + learning_rate * max_future_q

		X.append(observation)
		Y.append(current_qs)

	model.fit(np.array(X), np.array(Y), batch_size= batch_size, shuffle=True)


train_episodes = 300
test_episodes = 100

def main():
	epsilon = 1
	max_epsilon = 1
	min_epsilon = .01
	decay = .01

	environment = Environment()
	print(environment.getObservationSpace().shape)
	print(ActionSpace.getActionSpace().shape)
	# 1) Initialize the target and the main models
	# Main model updates every 4 steps
	main = agent(environment.getObservationSpace().shape, ActionSpace.getActionSpace().shape[0])
	# Target  models updates every 100 steps
	target = agent(environment.getObservationSpace().shape, ActionSpace.getActionSpace().shape[0])
	target.set_weights(main.get_weights())

	replay_memory = deque(maxlen=50_000)

	target_update_counter = 0
	# X (states), Y (actions)
	X = []
	Y = []

	steps_to_update_target_model = 0
	for episode in range(train_episodes):
		total_training_rewards = 0
		observation = environment.getObservationSpace()
		done = False
		while not done:
			steps_to_update_target_model += 1
			if True:
				environment.render()

			if np.random.rand() <= epsilon: # Explore
				action = ActionSpace.sample()
			else: # Exploit the best known action
				action_id = np.argmax(main.predict(observation))
				if action_id == 0:
					action = ActionSpace.none
				elif action_id == 1:
					action = ActionSpace.left
				elif action_id == 2:
					action = ActionSpace.right
				elif action_id == 3:
					action = ActionSpace.rotate

			(new_observation, reward, done) = step(environment, observation, action)
			replay_memory.append([observation, action, reward, new_observation, done])

			# 3) Update the main network using the Bellman Equation 
			if steps_to_update_target_model % 4 == 0 or done:
				train(replay_memory, main, done)

			observation = new_observation
			total_training_rewards += reward

			if done:
				print(f"Total training rewards: {total_training_rewards} after n = {episode} steps, with final reward {reward}")
				total_training_rewards += 1

				if steps_to_update_target_model >= 100:
					print("Copying main network weights to the target network weights")
					target.set_weights(main.get_weights())
					steps_to_update_target_model = 0

				# Reset the environment
				environment.reset()

		epsilon = min_epsilon + (max_epsilon - min_epsilon) * np.exp(-decay * episode)
	main.save("main")
	target.save("target")

if __name__ == "__main__":
	main()
