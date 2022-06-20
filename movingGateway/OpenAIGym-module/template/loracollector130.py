#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Feb  3 15:43:48 2022

@author: zuolo
"""

import numpy as np 
from gym import Env,spaces


class LoraCollector130(Env):
  
    def __init__(self):
                
        #self.observation_space = spaces.MultiDiscrete([8,8,8,8,8,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,500,500,500,500,500,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME,PLACEHOLDERGAMETIME])   # 6 gateways 
        PLACEHOLDEROBSPACE
        
        self.action_space = spaces.MultiDiscrete([PLACEHOLDERGATEWAYS,PLACEHOLDERACTIONS]) #3 gateways
                        
        # Define elements present inside the environment
        self.elements = []
       
        # Maximum flags for common timer, episode, airbourn cargo, etc
        self.max_time = PLACEHOLDERGAMETIME
        self.max_tick = PLACEHOLDERFRESH
        self.max_cargo = 29999
        self.period =  PLACEHOLDERPERIOD  #app period in timesteps    
        #####################################################
        #####################################################
        ### Number of vertices in the graph
        ### 1 means it is a cluster for collecting data
        ### 0 means is a depot to store/upload data
        self.vertices = np.loadtxt("vertices_ext.txt").reshape(PLACEHOLDERACTIONS)
        #self.vertices = [1,1,1,1,0,0,0]
        # Msg sending frequency for each node at every cluster-vertex
        # This also determines the number of nodes in the system
        #self.node_freq = [ [590,1500,600,0,0,0,0,0,0],
        #                   [0,0,0,299,2100,0,0,0,0],
        #                   [0,0,0,0,0,328,2621,780,0],
        #                   [0,0,0,0,0,0,0,0,1200]]
        
        # Msg sending frequency bootup time
        self.node_freq = np.loadtxt("startup_ext.txt").reshape(PLACEHOLDERACTIONS,PLACEHOLDERNODES)
        
        #self.node_freq = [ [20,15,40,0,0,0,0,0,0],
        #                   [0,0,0,20,40,0,0,0,0],
        #                   [0,0,0,0,0,80,90,15,0],
        #                   [0,0,0,0,0,0,0,0,10]]
   


        # The number of nodes can be extracted from the frequencies array
        self.nodes = len(self.node_freq[0]) 

        ####################################################
        ####################################################
        #### Timesptes table, considering step of 5 seconds
        ####################################################
        ####################################################
        #590 = 9.3 minutes  118 timesteps
        #1500 = 25 minutes   300 timesteps
        #600  = 10 minutes   120 timesteps
        #299  = 4.9 minutes  59.8 timesteps
        #2100 = 35 minutes   420 timesteps
        #328  = 5.4 minutes  65.6 timesteps
        #2621 = 43.6 minutes 524.2 timesteps
        #780 = 13 minutes    156 timesteps
        #1200 = 20 minutes   240 timesteps
        
        #2*(8+12+52+52+12+32+8+12+20) = 416
        # The amount of bytes sent per message for each node
        # The number of indices here needs to match the length of node_freq
        #self.node_payload = [8,12,52,52,12,32,8,12,20]
        #self.node_payload = [2,4,8,10,12,14,16,18,20]
        #self.node_payload = [10,100,10,100,10,100,10,100,10]
        self.node_payload = [10]*self.nodes
        
        # Number of packets to be collected for each node
        self.node_target = np.loadtxt("targets_ext.txt").reshape(self.nodes)
        #self.node_target = [1,2,3,2,4,2,5,3,3]

        ##################################################################
        ############### Matrix for movement duration #####################
        ############### Graph vertices travel times  #####################
        ############### Times are given in timesteps #####################

        self.move_duration = np.loadtxt("triptimes.txt").reshape(PLACEHOLDERACTIONS,PLACEHOLDERACTIONS)
        starting_cluster = [-1]*PLACEHOLDERACTIONS
        self.move_duration = np.vstack((self.move_duration,starting_cluster))
        #self.move_duration = [ 
        #                [0,288,504,576,115,518,432],
        #                [288,0,216,115,115,173,56],
        #                [504,216,0,144,230,115,84],
        #                [576,115,144,0,461,29,230],
        #                [115,115,230,461,0,20,20],
        #                [518,173,115,29,20,0,20],
        #                [432,56,86,230,20,20,0],
        #                [-1,-1,-1,-1,-1,-1,-1] 
        #               ]
        
        ### used to determine if a node has been visited at game over
        self.checked = [0]*self.nodes        
    
    def reset(self):
        
        self.actions_available = PLACEHOLDERACTIONS
        self.gateways_available = PLACEHOLDERGATEWAYS

        #initial state starts from the last - and neutral - state
        #this state is not avalaible in the range of agents actions.
        #for instance, on an environment with 7 vertices, the initial is 7, 
        #whilst from 0-6 is the range of avaible vertices, therefore actions. 
        #self.state = (7,7,7,0,0,0,0,0,0,0,0,0)  #3 gateways
        #self.state = (7,7,7,0,0,0,0,0,0,0,0,0,0)
        self.state = [PLACEHOLDERMASK]
        #########################################################################
        #### Create all counters and flags according with the scale of gateways 
        #########################################################################
        self.buffer = [0]*self.gateways_available
        self.game_over = [0]*self.gateways_available
        self.earliest = [PLACEHOLDERGAMETIME+1]*self.gateways_available
        self.total_reward = 0 
        self.visited = [0]*self.nodes        
        
        return np.array(self.state, dtype=np.int32)
    
    def mask(self):
        self.state = [PLACEHOLDERMASK]
        return np.array(self.state, dtype=np.int32)
    
    
    #######################################################
    ######### update the earliest timer seen for a gateway
    #######################################################
    def tick_update(self,time,gw):
        if time < self.earliest[gw]: 
            self.earliest[gw] = time
            
                
    #######################################################
    ######### check if the time limit has been breached
    #######################################################
    def tick_check(self,time,gw):
        if time-self.earliest[gw] >= self.max_tick:
            return 0
        else:
            return 1

    ########################################################
    ######## Check if the node is upstreaming data
    ######## at that time and position
    ########################################################
    def sender_active(self,gw,time,node,vertex):
        #print("LOG4 Node is ",node, " and time is ",time, " ", (self.node_freq[vertex][node])+self.period, " and pos is ", vertex)
        #print ("LOG4 This is the freq ",(node_freq[node-1]*60)/5)
        

#        if ( (self.node_freq[vertex][node] != 0)  and ((time*5) > (self.node_freq[vertex][node])+self.period)  and (   ((time*5) % (self.node_freq[vertex][node])) <= 5) ):
        if ( (self.node_freq[vertex][node] != 0)  and ((time*5) > ((self.node_freq[vertex][node])+self.period))  and (   ((time*5) % (self.period) <= 5) )):
        #if ( time % node_freq[node-1]) <= (time-5):
            if self.visited[node] < self.node_target[node]:
                self.buffer[gw] = self.buffer[gw] + self.node_payload[node]
                
                if self.buffer[gw] > self.max_cargo:
                    self.buffer[gw] = 30000
                    self.game_over[gw] = 1 
#                    print ("LOG4 Buffer overflow")
                self.visited[node] = 1 + self.visited[node]
                self.checked[node] = 1 
                print("LOG4 The sender interval is active ",node," Gateway ", gw ," payload ",self.node_payload[node]," time ",time, " position ",self.state[gw]," visitado ", self.visited[node], " target ", self.node_target[node])
#                return self.node_payload[node]
                self.tick_update(time,gw)
                return 10
            else:
                return 0
                #return -1*self.node_payload[node]
        else:
            return 0
            #return 0
       

    ########################################################
    ######## Check if the node is at a upstreaming depot
    ######## reward if arriving on time, penalize otherwise
    ########################################################
    def dump_buffer(self,gw,time):
        
#        print("LOG Dumping buffer ",self.buffer)
        if self.buffer[gw] == 0:
            return 0     
        else:
            #return 10
            if self.tick_check(time,gw) == 1 :
                print("LOG4 The gateway is dumping            Gateway ", gw ," payload ",self.buffer[gw]      ," time ",time, " position ",self.state[gw])
                self.earliest[gw] = PLACEHOLDERGAMETIME+1
                return self.buffer[gw]
            else:
                self.earliest[gw] = PLACEHOLDERGAMETIME+1
                print("LOG4 The buffer EXPIRADO", self.buffer[gw], " position ",self.state[gw], " Gateway ", gw )
                return 0     
                #return -1*(self.buffer[gw])
    

    def action_breakdown(self,action,gateways,actions):

        combo = pow(actions,gateways)

        if int(action) > (combo-1):
            print("Error: Action is out of bounds")

        result = [0] * (gateways+1)

        for i in range(1,gateways+1):
              comp1 = action
              comp2 = 0
              for j in range(i,0,-1):
                  comp2 = comp2 + result[j]*(actions**(gateways-j))

              comp3 = (combo/(actions**(i)))

              result[i] = int(((comp1 - (comp2)))/comp3)

              #print("Resultado ",i," ",result[i])

        answer = result[1:gateways+1]
        return answer
       
    ######################################################
    ######################################################
    #move the gateways, update their states
    #apply rewards and return the resulting environment
    ######################################################
    ######################################################
    def step(self, action):
        
        #going = self.action_breakdown(action,self.gateways_available,self.actions_available)

        going,turn = action 
        gain = 0

        timecoming = [0]*self.gateways_available
        coming = [0]*self.gateways_available
        time_going = 0 
        pos_going = 0
        
       
        timecoming[turn] = self.state[turn+self.gateways_available] #update the current time
        coming[turn] = self.state[turn]    #update the current position
        pos_going,time_going = self.move(turn,coming[turn],going) #moving to new position and time
        #print("LOG7 timecoming ",timecoming[turn]," poscoming ", coming[turn])
        #print("LOG7 timegoing ",time_going," posgoing ", pos_going)

        if np.count_nonzero(self.game_over) == self.gateways_available:
            over = 1
            return np.asarray(self.state),gain,over, None       
       
        ##updating the state with the new positions and times
        
        self.state[turn] = pos_going 
        self.state[turn+self.gateways_available] = time_going
                
        #if coming != going:
        #    print ("coming from pos time LOG4",coming,timecoming," to pos time ",self.state[0],self.state[1])
        
        ###############################################
        ###############################################
        ### Reading all new times from the states,
        ### running through the existing gateways.
        ### Check their position and time
        ### in relation to all nodes (second loop).
        ### Verify if they will be fecth or dump. 
        ### Apply rewards and scores accoringly.
        ###############################################
        ###############################################
        time = [0]*self.gateways_available
        
        time[turn] = self.state[turn+self.gateways_available]
        position_idx = self.state[turn] 
         
        for node in range(self.nodes): 
          
            if self.vertices[position_idx] == 1:
                if time[turn] < PLACEHOLDERGAMETIME:
                     gain  = gain + self.sender_active(turn,time[turn],node,position_idx)
            else:
             if self.vertices[position_idx] == 0:
               if time[turn] < PLACEHOLDERGAMETIME:
                     gain = gain + self.dump_buffer(turn,time[turn])
                     self.buffer[turn] = 0 

            
        
        ##convert it back into the state format and store it
        self.state[turn+(2*self.gateways_available)] = self.buffer[turn]
        self.state[turn+(3*self.gateways_available)] = self.earliest[turn]
        self.state[len(self.state)-1] = turn
        
        
        
        ##update the reward
        reward = gain
        #if gain > 0:
            #print("LOG4 This is the supposed reward ",reward)
        self.total_reward = self.total_reward + reward
        
        ##confirm the end of the game
#        if np.count_nonzero(self.game_over) == self.gateways_available:
#            over = 1
#        else:
#            over = 0
#        
        if (self.game_over[turn] == 1):
            over = 1
        else:
            over = 0
#       
        
        #print("LOG5 before returing this is the reward ", reward, "Total_Reward ",self.total_reward ," Gateway ", turn, " Visited ",self.visited, " Game over ",self.game_over )
        ## return the conext and new environment state
        return np.asarray(self.state),reward,over, None 
    
   

    def move(self,gw,coming,going):
              
        new_state_idx = going
        if coming == going:
#            print("Do nothing, stays at same position")
            new_state_idx = coming
        
        new_time_idx = self.state[gw+self.gateways_available] + self.trip_time(coming,going) + 1    
      
        if new_time_idx > self.max_time:
            print (" LOG4 Time is up ",new_time_idx)
            new_time_idx = PLACEHOLDERGAMETIME
            self.game_over[gw] = 1       
            
        return new_state_idx,new_time_idx
    
    
    def trip_time(self,prevstate,state):
        print("STATES ", int(prevstate)," " ,int(state) )    
        return (self.move_duration[int(prevstate)][int(state)]) 
