import pandas as pd
import seaborn as sns
import sys
from matplotlib import pyplot as plt
sns.set() 
import sys

#################################################
#### Printing the Whisker Boxes for Battery usage
#### Results are stored in the /tmp directory
#################################################
inputfile="/tmp/"+sys.argv[1]+"/energy_results.csv"
plot_data = pd.read_csv(inputfile)
box_plot=sns.boxplot(data=plot_data)
median_RL = round(plot_data['ADR-RL'].mean(),4)
median_AVG = round(plot_data['ADR-AVG'].mean(),4)
median_MAX = round(plot_data['ADR-MAX'].mean(),4)
median_MIN = round(plot_data['ADR-MIN'].mean(),4)
fig = plt.gcf()
# Change seaborn plot size
fig.set_size_inches(12, 8)
xtick = box_plot.get_xticks()
plt.ylabel("mj/received packet", size=14)
plt.xlabel("ADR Type", size=14)
plt.savefig("/tmp/"+ sys.argv[1]+"/Battery-Use.png")


#################################################
#### Printing the Whisker Boxes for Packet Goodput
#### Results are stored in the /tmp directory
#################################################
inputfile="/tmp/"+sys.argv[1]+"/goodput_results.csv"
plot_data = pd.read_csv(inputfile)
box_plot=sns.boxplot(data=plot_data)
median_RL = round(plot_data['ADR-RL'].mean(),4)
median_AVG = round(plot_data['ADR-AVG'].mean(),4)
median_MAX = round(plot_data['ADR-MAX'].mean(),4)
median_MIN = round(plot_data['ADR-MIN'].mean(),4)
fig = plt.gcf()
# Change seaborn plot size
fig.set_size_inches(12, 8)
xtick = box_plot.get_xticks()
mingoodput = (plot_data[['ADR-RL','ADR-AVG','ADR-MAX','ADR-MIN']].min().min()) - 0.05
plt.ylim(mingoodput,1.05)
plt.ylabel("% Packets received/Packets sent", size=14)
plt.xlabel("ADR Type", size=14)
plt.savefig("/tmp/"+ sys.argv[1]+"/Packets-Goodput.png")


