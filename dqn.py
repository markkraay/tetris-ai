from os import environ
from build.tetris_environment import Environment, ActionSpace
import numpy as np
import tensorflow as tf
import random
from collections import deque

def calculate_reward(original_observation, new_observation):
	# For this implementation, there will be a positive reward if the number of blocks
	# are removed from one episode to the next (20). There will be a negative reward if the 
	# agent takes an action and it doesn't remove the number of blocks (-1)
	original_num_blocks = original_observation.tolist().count(1)
	new_num_blocks = new_observation.tolist().count(1)
	if new_num_blocks < original_num_blocks:
		return 20
	return -1

def step(environment, observation, action):
	"""
	@params1: environment: The Tetris environment
	@params2: observation: The current observation
	@returns (new_observatio, reward, done)
	"""
	print(action)
	environment.executeAction(action)
	new_observ = environment.getObservationSpace()
	reward = calculate_reward(observation, new_observ)
	done = not environment.isActive()
	return (new_observ, reward, done)

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

def get_qs(model, state, step):
	return model.predict(state.reshape([1, state.shape[0]]))[0] # This will be changed

def train(env, replay_memory, model, target_model, done):
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
				train(environment, replay_memory, main, target, done)

			observation = new_observation
			total_training_rewards += reward

		epsilon = min_epsilon + (max_epsilon - min_epsilon) * np.exp(-decay * episode)
	main.save("main")
	target.save("target")

if __name__ == "__main__":
	main()
