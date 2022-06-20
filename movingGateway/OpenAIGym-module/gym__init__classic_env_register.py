from gym.envs.registration import (
    registry,
    register,
    make,
    spec,
    load_env_plugins as _load_env_plugins,
)

# Hook to load plugins from entry points
_load_env_plugins()


# Classic
# ----------------------------------------

register(
    id="LoraCollector-v130",
    entry_point="gym.envs.classic_control:LoraCollector130",
    max_episode_steps=10000,
    reward_threshold=500.0,
)
        
