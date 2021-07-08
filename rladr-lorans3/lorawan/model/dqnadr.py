import numpy as np
from numpy import random as rnd
import random
from matplotlib import pyplot as plt
import pickle
import torch
import torch.nn as nn
import torch.autograd as autograd
import torch.nn.functional as F
from collections import deque
from time import time
from os import system
import os

class DQN(nn.Module):
    def __init__(self, envstate_dim, action_dim):
        super(DQN, self).__init__()
        self.input_dim = envstate_dim
        self.output_dim = action_dim
        self.huber_loss = F.smooth_l1_loss
        torch.manual_seed(2)
        #self.apply(init_weights)

        self.ff = nn.Sequential(
            nn.Linear(self.input_dim, 672),
            nn.ReLU(),
            nn.Linear(672, 1344),
            nn.ReLU(),
            nn.Linear(1344, 1344),
            nn.ReLU(),
            nn.Linear(1344, 672),
            nn.ReLU(),
            nn.Linear(672, self.output_dim),
        )

        #self.optimizer = torch.optim.SGD(self.parameters(),lr=0.09, momentum=0)
        self.optimizer = torch.optim.Adam(self.parameters(),lr=0.0001)
        
    #def weights_init(m):
    #    classname = m.__class__.__name__
    #    if classname.find('Linear') != -1:
    #        torch.init.kaiming_normal(self.linear[-1].weight)


    #def init_weights():
    #    for dim in range(1,6):
            #self.linear.append(nn.Linear(pre_dim, dim, bias=False))
            #self.bn.append(nn.BatchNorm1d(dim))
            #self.relu.append(nn.ReLU(inplace=True))
    #       init.kaiming_normal(self.linear[-1].weight)
            #self.nLayers += 1
            #pre_dim = dim
           


    def forward(self, state):
        qvals = self.ff(state)
        return qvals

class ReplayMemory:
    memory = None
    counter = 0

    def __init__(self, size):
        self.memory = deque(maxlen=size)

    def append(self, element):
        self.memory.append(element)
        self.counter += 1
        #print("Deque ",self.memory, " Element ",element)

    def sample(self, n):
        return random.sample(self.memory, n)


def decisionmaker(statein):

    if os.path.exists("model-store"):
        agent = torch.load("model-store")
    else:
        agent=DQN(672,48)
        torch.save(agent, "model-store")
        print("Is it the first round? Model not found!")

    if os.path.exists("label-store"):
        label = torch.load("label-store")
    else:
        label = DQN(672,48)
        print("Is it the first round? Label not found!")
        torch.save(label, "label-store")

    #for p in agent.parameters():
    #    if p.requires_grad:
    #        print(p.name, p.data)


    state = np.zeros(672)
    state[statein] = 1
    qvals = agent.forward(torch.FloatTensor(state)) # forward run through the policy network
    action = np.argmax(qvals.detach().numpy()) # need to detach from auto
    return action, 0


def remember(state1,state2,act1,reward):
    if os.path.exists("memory-store"):
        with open('memory-store', 'rb') as memory_file:
            memory = pickle.load(memory_file)
    else:
        memory = ReplayMemory(2000)
    
    mem_state1 = np.zeros(672)
    mem_state1[state1] = 1
    
    mem_state2 = np.zeros(672)
    mem_state2[state2] = 1


    #Add to memory and store it on disk
    memory.append({'state':mem_state1,'action':act1,'reward':reward,'next_state':mem_state2})
    with open('memory-store', 'w+b') as memory_file:
        pickle.dump(memory, memory_file)
    

def train(state1,state2,act1,reward):
    print("This is the training process ",state1, state2, act1, reward)
    
    if os.path.exists("memory-store"):
        with open('memory-store', 'rb') as memory_file:
            memory = pickle.load(memory_file)
    else:
        print("Error, no memory to work with")
        return 1
    
    if os.path.exists("model-store"):
        agent = torch.load("model-store")
    else:
        print("Error, no DQN model to work with")
        return 1
    
    if os.path.exists("label-store"):
        label = torch.load("label-store")
    else:
        print("Error, no DQN training model to work with")
        return 1
    
    batch = memory.sample(10) # here, batch size equals 10
    states = torch.FloatTensor(np.array(list(map(lambda x: x['state'], batch))))
    actions = torch.LongTensor(np.array(list(map(lambda x: x['action'], batch))))
    rewards = torch.FloatTensor(np.array(list(map(lambda x: x['reward'], batch))))
    next_states = torch.FloatTensor(np.array(list(map(lambda x: x['next_state'], batch))))

    #:print(states,agent.input_dim)
    curr_Q = agent.forward(states).gather(1,actions.unsqueeze(1)) # calculate the current Q(s,a) estimates
    next_Q = label.forward(next_states) # calculate Q'(s,a) (EV)
    
    max_next_Q = torch.max(next_Q,1)[0] # equivalent of taking a greedy action
    expected_Q = rewards + 0.8 * max_next_Q # Calculate total Q(s,a)
    loss = agent.huber_loss(curr_Q, expected_Q.unsqueeze(1)) # unsqueeze is really
    agent.optimizer.zero_grad()
    loss.backward()
    agent.optimizer.step()    
   
    torch.save(agent, "model-store")
    torch.save(label, "label-store")
