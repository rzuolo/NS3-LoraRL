#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Feb  6 20:37:44 2022

@author: zuolo
"""

# if gpu is to be used

import gym
import random
#import math
import torch
import torch.nn as nn
#import torch.optim as optim
#from torch.autograd import Variable
import torch.nn.functional as F
import os
import pickle
from colorama import Fore, Style
from collections import namedtuple, deque
import numpy as np
import sys 
from datetime import datetime 

random.seed(datetime.now())

# hyper parameters
EPISODES = 30
EPS_START = 1
EPS_END = 0.05  # e-greedy threshold end value
EPS_DECAY = 300
GAMMA = 0.9  # Q-learning discount factor
LR = 0.1  # NN optimizer learning rate
HIDDEN_LAYER = 128  # NN hidden layer size
BATCH_SIZE = 64  # Q-learning batch size
use_cuda = torch.cuda.is_available()
FloatTensor = torch.cuda.FloatTensor if use_cuda else torch.FloatTensor
LongTensor = torch.cuda.LongTensor if use_cuda else torch.LongTensor
ByteTensor = torch.cuda.ByteTensor if use_cuda else torch.ByteTensor
Tensor = FloatTensor

### This is the variable that holds the max reward across the rounds
keiser = 0 

class ReplayMemory:
    memory = None
    counter = 0

    def __init__(self, size):
        self.memory = deque(maxlen=size)

    def append(self, *args):
        self.memory.append(Transition(*args))
        self.counter += 1
        #print("Deque ",self.memory, " Element ",element)

    def sample(self, n):
        return random.sample(self.memory, n)

class Network(nn.Module):
    def __init__(self):
        nn.Module.__init__(self)
        self.ff = nn.Sequential(
            nn.Linear(13, 64),
            nn.ReLU(),
            nn.Linear(64, 512),
            nn.ReLU(),
            nn.Linear(512, 512),
            nn.ReLU(),
            nn.Linear(512,8),
        )

        torch.manual_seed(2)
        self.init_weights()

        self.huber_loss = F.smooth_l1_loss
        #self.optimizer = optim.SGD(self.parameters(),lr=0.001, momentum=0)
        self.optimizer = torch.optim.Adam(self.parameters(),lr=0.001)
#       self.ff = nn.Sequential( nn.Linear(env.observation_space.shape[0], 256),nn.ReLU(),nn.Linear(256,256),nn.ReLU(),nn.Linear(256,256),nn.ReLU(),nn.Linear(256,7),)

    def forward(self, x):
        qvals = self.ff(x)
        return qvals


    def init_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Linear):
                torch.nn.init.xavier_uniform(m.weight)
                torch.nn.init.constant_(m.bias,0)


#def select_action(eps_threshold,state):
def select_action(eps_threshold):
    global steps_done

    sample = random.random()
    #eps_threshold = 0.5
    steps_done += 1

    if sys.argv[1] == "real" :
        var = decisionmaker()
        #print(" LOG4 ", Fore.GREEN, steps_done," not random ",Fore.YELLOW, " action var", var ,Style.RESET_ALL, sample)
        return var 
        
    if sample > eps_threshold:
        var = decisionmaker()
        print(" LOG8 ", Fore.GREEN, steps_done," not random ",Fore.YELLOW, " action var", var ,Style.RESET_ALL, sample)
        return var 
    else:
        #var = LongTensor([[random.randrange(3)]])
        var = random.randint(0,7)
        #var = LongTensor([[random.randrange(3)]])
        print(" LOG8 ", Fore.RED, steps_done," random ", Fore.YELLOW, " action ", int(var), Style.RESET_ALL, sample)
        return var


def decisionmaker():
               
    qvals = agent.forward(torch.FloatTensor(env.state)) # forward run through the policy network
    action = np.argmax(qvals.detach().numpy()) # need to detach from auto
    print("LOG8 selecting from model ", action)
    #print("LOG4 3 tensor selecting action ",float(qvals))
    #print("LOG4 3 tensor selecting action ",int(action))
    return action


def run_single_actor(bogus, real, head, e, environment,agent,label,memory,state,list_of_actions):
 
       state = environment.mask()
       state[len(state)-1] = head
       environment.state[len(state)-1] = head
       
       
       steps = 0
       prev_action = 1000
       bias = 0
       curr_action = 1000
       action = 0
       total_reward = 0
       done = 0
       
       while done != 1:

            prev_action = action
            reward = 0
            
            if  real == "real" :
                action = int(decisionmaker())
            else:
                action = int(select_action(0.5))
            
            curr_action = action 
            
            if curr_action == prev_action and real != "real":
                bias = bias + 1
            else:
                bias = 0
        
            if bias > 5 and real != "real":
                bias = 0 
                action = int(select_action(0.9))
            
            action2=[action,head]
        
            print("BASEADO STATE ", state)
            i = 1
            while i < 101:
                next_state, reward, done, _ = environment.step(action2)
                i += 1
                if reward > 0:
                    total_reward = total_reward + reward
                    #print("LOG4 Aborting on i ",i," reward ",reward, " total_reward ",total_reward, " done ",done, " state ", state ," next_state ", next_state)    
                    i = 301
            
            list_of_actions.append(action)
            
            if done == 1:
                reward = -1

            if real != "real":
                remember( FloatTensor([state]), FloatTensor([next_state]), FloatTensor([action]), FloatTensor([reward]) )

            print("BASEADO STATE ", state, next_state)
            state = next_state
            steps += 1

            #print("REPORT LOG4 this is the final sate at the round # ", head, " state ",state, " env.state ", environment.state, " next-state ",next_state)                         
            if real != "real":
                agent, label = learn(e)
      
       #### the ending state of environment.state seems to be allways the same ????????
       print (" LOG4 nobogus Total reward for turn ", head, " is ",total_reward, " round ", head ," real ", real, " bogus set ", bogus)
       
#       bogus_reward = 0
#       
#       if bogus == "nope":
#            print(" IAMGOINGTOSTAARTITNOW LOG4 bogus reward ", " bogus set ", bogus)                      
#            bogus_reward = run_single_actor("bogus","real", head, e, environment,agent,label,memory,state,list_of_actions)   
#            #bogus_reward = run_episode("bogus", "real", head, e, environment,agent,label,memory)             
#            print(" LOG4 bogus reward ",bogus_reward, " round ", head ," real ", real, " bogus set ", bogus)                      
#       
       #print (" LOG4 nobogus Total reward for turn ", head, " is ",total_reward, " round ", head ," real ", real, " bogus reward ", bogus_reward, " bogus set ", bogus)
       return total_reward,agent,label,memory
    
        
            
def ScaledFloatFrame(action_taken):
    scaled_action = (((action_taken - (-256)) * (343 - 1 - 0)) / (256 - (-256))) + 0
    return int(scaled_action)
    
def remember(mem_state1,mem_state2,act1,reward):
    memory.append(mem_state1,act1,reward,mem_state2)
    
def learn(e):
    if memory.counter < 20:
        return agent, label
    
    #print("LOG4 let's learn some stuff")
    transitions = memory.sample(20)
    batch = Transition(*zip(*transitions))
    non_final_next_states = torch.cat([s for s in batch.next_state if s is not None])
    state_batch = torch.cat(batch.state)
    action_batch = torch.cat(batch.action)
    reward_batch = torch.cat(batch.reward)
    action_batch = action_batch.type(torch.int64)
    
    
    print(" forward ",agent.forward(state_batch))
    print(" action ",action_batch)
    print(" action_squeeze ",action_batch.unsqueeze(1))
    curr_Q = agent.forward(state_batch).gather(1,action_batch.unsqueeze(1)) # calculate the current Q(s,a) estimates
    
    next_Q = label.forward(non_final_next_states) # calculate Q'(s,a) (EV)
    max_next_Q = torch.max(next_Q,1)[0] # equivalent of taking a greedy action
    
    expected_Q = reward_batch + 0.8 * max_next_Q # Calculate total Q(s,a)
    
    loss = agent.huber_loss(curr_Q, expected_Q.unsqueeze(1)) # unsqueeze is really
   
     
    agent.optimizer.zero_grad()
    loss.backward() 


    for param in agent.parameters():
        param.grad.data.clamp_(-1, 1)

        
    agent.optimizer.step()    

 
    if e % 2 == 0:    
        label.load_state_dict(agent.state_dict())
 
            
    return agent, label



###########################################################################################################
###########################################################################################################
###########################################################################################################
###########################################################################################################
##############################   Main Program
###########################################################################################################
###########################################################################################################
###########################################################################################################
###########################################################################################################
    
Transition = namedtuple('Transition', ('state', 'action', 'reward', 'next_state'))
env = gym.make('LoraCollector-v130')

if os.path.exists("keiser"):
    f = open('keiser', 'r')
    keiser = f.read() 
    keiser=float(keiser)
    f.close()
else:
    keiser = 0 
    
if os.path.exists("model-store"):
   agent = torch.load("model-store")
else:
   print("Error, no DQN model to work with")
   agent = Network()        

if use_cuda:
    agent.cuda()
    
if os.path.exists("label-store"):
   label = torch.load("label-store")
else:
   print("Error, no DQN training model to work with")
   label = Network()             

if os.path.exists("memory-store"):
        with open('memory-store', 'rb') as memory_file:
            memory = pickle.load(memory_file)
else:
        memory = ReplayMemory(2000)

steps_done = 0
head = 0
episode_durations = []



for e in range(EPISODES):
    head = 0
    state = env.reset()
    list_of_actions = ([],[],[])
    list_of_actions2 = ([],[],[])
    
    if len(list_of_actions) == 0:
        list_of_actions = list([[]])
        list_of_actions2 = list([[]])

    episode_result = 0
    episode_result2 = 0
    
    
    reward2 = [0] * 3
    reward = [0] * 3


    r = list(range(3))
    random.shuffle(r)

    for worker in r:
        reward[worker],agent,label,memory = run_single_actor("nope", sys.argv[1], worker, e, env,agent,label,memory,state,list_of_actions[worker])

    for worker in r:
        episode_result = reward[worker] + episode_result

    state = env.reset()
  
    for worker in r:
        reward2[worker],agent,label,memory = run_single_actor("bogus", "real", worker, e, env,agent,label,memory,state,list_of_actions2[worker])
    
    for worker in r:
        episode_result2 = reward2[worker] + episode_result2
        
    if ( episode_result >= keiser ):
        print(" LOG4 SAR saving the sun")
        torch.save(agent, "model-store")
        torch.save(label, "label-store")
        with open('memory-store', 'w+b') as memory_file:
            pickle.dump(memory, memory_file)

    if (episode_result > keiser ):
        keiser = episode_result
        file = open("keiser", "w")
        file.write(str(keiser))
        file.close()
        print("SARILHO LOG4 Saving the best status ",keiser)
        
    env.total_reward = episode_result
                
            
    #if ( bogus != "bogus" ):
    #print("REPORT LOG4 episode result ", episode_result, "episodes ",e , " EnvReward ", environment.total_reward, " Keiser ", keiser  )
    

    print("LOG4 ============================================================================================================================ SAR",)
    print("LOG4 ============================================================================================================================ SAR",)
    print("REPORT LOG4 episode", e, " episode result ", episode_result2 , " EnvReward ", env.total_reward, " Keiser ", keiser )
    print("LOG4 ============================================================================================================================ SAR",)
    for i in range(3):
        print("REPORT LOG4  actions  ",i ," ", len(list_of_actions[i]), list_of_actions[i], " reward ", reward[i])
    print("LOG4 ============================================================================================================================ SAR",)
    for i in range(3):
        print("REPORT LOG4  actions  ",i," ",len(list_of_actions2[i]), list_of_actions2[i], " reward ", reward2[i])
    print("LOG4 ============================================================================================================================ SAR",)

    if np.count_nonzero(env.checked) > 0:
            for i in range(len(env.checked)):
                if env.checked[i] == 0:
                    print("LOG4 REPORT the node ",i," hasn't been visited")



















env.close()
