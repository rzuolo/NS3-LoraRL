
import numpy as np
import random
import sys
from numpy import asarray
from numpy import savetxt

### Q[SF,NewSF]
###  6*(12-SF)+step is the line (state)
###  step is the column

def maxAction(Q, state, actions):
    values = np.array([Q[state,a] for a in actions])
    action = np.argmax(values)
    return actions[action]


if __name__ == '__main__':

    actionspace = [0,1,2,3,4,5]

    Q = np.load("data.csv.npy").tolist()
    #action = maxAction(Q,observation,env.actionspace) if rand < (1-EPS) else env.actionSpaceSample()

    SF=int(sys.argv[1])
    step=int(sys.argv[2])

    if step > 5:
        over = step - 5
        step = 5
    #print(Q)
    #print("O meu SF é ",SF," e o meu step é ",step)
    state= (6*(12 - int(SF)))+int(step)
    action = maxAction(Q, state, actionspace)
    print(12 - action)
    #print(over)

    #Q[observation,action] = Q[observation,action] + ALPHA*(reward + GAMMA*Q[observation_,action_] - Q[observation,action])
    #observation = observation_
