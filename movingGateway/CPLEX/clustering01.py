#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May  2 13:48:41 2022

@author: zuolo
"""

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May  2 12:39:42 2022

@author: zuolo
"""

import random
import math
import sys
import matplotlib.pyplot as plt 
import numpy as np 
from mpl_toolkits.axisartist import SubplotZero 


if len(sys.argv) != 2:
    print("Provide the number of clusters as argument ",sys.argv )
    sys.exit() 

def generate_point(limit,center_x,center_y):
    return round(random.uniform(-limit,limit),2)+center_x, round(random.uniform(-limit,limit),2)+center_y


def generate_point_gauss(mean_x, mean_y, deviation_x, deviation_y):
    return round(random.gauss(mean_x, deviation_x),2), round(random.gauss(mean_y, deviation_y),2)


def validate_center(cluster_centers,center_z,center_w):
    point_attempt = np.array((center_z,center_w))
    if ( (len(cluster_centers) == 0)):
        return 1
    for point in cluster_centers:
        #print("eh o point",point,cluster_centers)
        center_x  = point[0]
        center_y  = point[1]
        point_cluster = np.array((center_x,center_y))
        dst = round(np.linalg.norm(point_cluster - point_attempt),2)
        #print("dst ",dst)
        if (  dst < (20) or dst >(100)):
            return 0
    return 1


cluster_mean_x = 0
cluster_mean_y = 0
cluster_deviation_x = 200
cluster_deviation_y = 200
#point_deviation_x = 3
#point_deviation_y = 3

number_of_clusters = int(sys.argv[1])


points_per_cluster = []
maxrad = []

for i in range(0,number_of_clusters):
    print("cluster ",i+1)
    if ((i+1)%4) == 0:
        points_per_cluster.append(0)
    else:
        points_per_cluster.append(random.randint(8,15))
    maxrad.append(20)

#points_per_cluster = [4,3,4,8,2,4,3,5]
print("points per cluster ",points_per_cluster, " maxrad ",maxrad)

#maxrad = [2,3,4,1,2,4,3,5]
##maxrad = [20,20,20,20,20,20,20,20]

cluster_centers = []
attempt = 0


cluster_centers.append(generate_point_gauss(cluster_mean_x, cluster_mean_y, cluster_deviation_x, cluster_deviation_y))

for i in range(number_of_clusters-1):
    
    point_attempt = generate_point_gauss(cluster_mean_x, cluster_mean_y, cluster_deviation_x, cluster_deviation_y)
    attempt = validate_center(cluster_centers,point_attempt[0],point_attempt[1])
    
    
    print("point attempt",point_attempt,point_attempt[0])
    while ( attempt != 1):
        #print ("Looking for a new cluster ...")
        point_attempt = generate_point_gauss(cluster_mean_x, cluster_mean_y, cluster_deviation_x, cluster_deviation_y)
        attempt = validate_center(cluster_centers,point_attempt[0],point_attempt[1])
    
    cluster_centers.append(point_attempt)
        


#points = [generate_point(center_x, center_y, point_deviation_x, point_deviation_y) for center_x, center_y in cluster_centers  for i in range(points_per_cluster)]

idx = 0


print("CLUSTERX 10000",";",10000,";",10000,";",10000,";",10000,";",10000,";",10000)
for cter in range(0,number_of_clusters):
    print("CLUSTERX 0",";",0,";",0,";",0,";",0,";",0,";",0)
print("CLUSTERX 10000",";",10000,";",10000,";",10000,";",10000,";",10000,";",10000)
    
a_file = open("triptimes.txt", "w")
for center_x, center_y in cluster_centers:
    
    #print ("Cluster center ",idx," ",center_x," ",center_y," ")
    point_a = np.array((center_x,center_y))
    
    print("CLUSTERX 10000",";",10000,";",10000,";",10000,";",10000,";",10000,";",10000)
    for index in range(0,(number_of_clusters)):
        
        #if index != idx:
            center_z, center_w = cluster_centers[index]
            point_b = np.array((center_z,center_w))
            distance = round(np.linalg.norm(point_a - point_b),2)
            #Assuming the constant speed of the drone
            #43km/h = 8.3x10-6 km/ms or 0.0083 km/s
            #distance_delay = round((distance / 0.0000083),2)
            #distance_delay = round((distance / 0.083),2)
            #distance_step = round(((distance / 0.0083)/5),2)
            distance_delay = round((distance / 8.3),2)
            distance_step = math.ceil(round(((distance / 8.3)/5),2))
            #print("Distance between cluster ",idx, " and cluster ",index," is ",distance, center_x," ",center_y," ",idx)
            print("CLUSTERX ;",idx,";",index,";",round(distance,2),";",int(distance_delay),";",round(distance_step,2),";",center_x,";",center_y)
            print(int(distance_step))
            a_file.write(str(int(distance_step))+"\n")
    idx = idx +1
    print("CLUSTERX 10",";",0,";",0,";",0,";",0,";",0,";",0)
a_file.close()

    
#points = [generate_point(center_x, center_y, point_deviation_x, point_deviation_y) for center_x, center_y in cluster_centers  for i in range(points_per_cluster)]

idx = 0
for index in range(0,(number_of_clusters+2)):
    print("CLUSTERX 10000",";",10000,";",10000,";",10000,";",10000,";",10000,";",10000)

counter = 0
for center_x, center_y in cluster_centers:
    if points_per_cluster[idx] != 0:
        points = [ generate_point(maxrad[idx],center_x,center_y) for i in range(points_per_cluster[idx]) ]
        data =np.array(points)
        x, y = data.T
        plt.scatter(x, y)
    
        point_a = np.array((center_x,center_y))
        for zet in data:
#    print (zet)
         x, y = zet.T
         #center_w = round(1000 * (x - center_x),2)
         #center_z = round(1000 * (y - center_y),2)
         center_w = round((x - center_x),2)
         center_z = round( (y - center_y),2)
         point_b = np.array((x,y))
         distance = round(np.linalg.norm(point_a - point_b),2)
     #print("Cluster ",idx," x ",x," y ",y," ",distance)
         print("NODE;",counter, ";CLUSTER;",idx,";",round(x,2),";",round(y,2),";",round(center_w,2),";",round(center_z,2),";",round(distance,2))
         counter = counter +1
    idx = idx +1

print("COUNTER;",counter)


#print ("Points are points ",points)


#data =np.array(points)
#x, y = data.T

#for i in points:
#    print(i)
#plt.scatter(x, y)


#plt.show()
