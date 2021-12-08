import numpy as np
from build.tetris_environment import Environment

env = Environment()
for config in env.getPieceConfigurations():
	print(np.array(config[0]))
