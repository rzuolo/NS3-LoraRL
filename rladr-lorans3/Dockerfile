FROM rzuolo/rladr-lorans3
COPY automation-interface/* /ns3/ 
COPY ns3/wscript /ns3/ 
#A code in python for testing the Python embedded setup 
#COPY ns3/examples/tutorial/hello-simulator.cc /ns3/examples/tutorial/ 
ADD lorawan/ /ns3/src/lorawan
CMD ["/bin/bash","-c","cd /ns3/ ; source ./rladr-simulations.sh"]
#If troubleshoot is needed, the image can start with an infinite loop
#Attach to the running container to carry out troubelshoot 
#CMD ["/bin/bash","-c", "while :; do echo \"nda\"; sleep 10000000000;done"]

