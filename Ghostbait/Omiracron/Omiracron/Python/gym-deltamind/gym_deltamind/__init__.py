from gym.envs.registration import register

register(
    id='deltamind-v0',
    entry_point='gym_deltamind.envs:DeltamindEnv',
)