#!/bin/bash

##################### loving herman is the name of the docker container running the NS3-LoRaRL simulator
##################### this needs to be generalized

if [ $# -ne "2" ]
then

        echo "Provide the arguments in sequence 1st #gateways, 2nd #clusters"
        return 1

fi


nrgateways="$1"
nrclusters="$2"

rm -f tempdata
rm -f triptimes.txt
python clustering01.py $2> tempdata


### This is pointing to the home directory. It may vary depending on the system and python paths
rm -f ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/triptimes.txt
cp triptimes.txt ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/

IFS="
"
nrnodes=`cat tempdata|grep COUNTER|awk -F";" {'print $2'}`



rm -f coordinates_xy.txt
for (( i=0;i<$nrclusters;i++ ))
{
  echo "cluster$i-FIM" >> coordinates_xy.txt
  cat tempdata |grep "CLUSTER; $i ;" > clusterx
  cat clusterx | tr -d " " | awk -F";" {'print $7","$8",200,"int(rand()*5)'} >> coordinates_xy.txt
  rm -f clusterx
}



for (( i=0;i<$nrclusters;i++ ))
{
        let "j=$i+1"
        #echo "Looking coordinates for cluster $i"
 	cat coordinates_xy.txt |sed -n "/cluster$i-FIM/,/cluster$j-FIM/p" |sed "/cluster$i-FIM/d" |sed "/cluster$j-FIM/d" > cluster$j-coordinates.dat
}


rm -f in_edge_exp.txt
echo "[indicates clusters with edge connectivity]" >> in_edge_exp.txt
for ((i=1;i<=$nrclusters;i++))
{
	let "isedge = $i % 4"
	#echo "Checking if $i is edge $isedge"
	if [ $isedge -eq "0" ]
	then
	  echo -n "$i " >> in_edge_exp.txt
	fi

}

rm -f in_delay_exp.txt
echo [traveling time in ms from ci to cj] > in_delay_exp.txt
cat tempdata | tr -d " " | grep CLUSTERX | awk -F";" {'print $5'} >> in_delay_exp.txt

rm -f in_cluster_exp.txt
echo "[id of devices (0...) covered by cluster identified by first column; cluster can have no devices]" > in_cluster_exp.txt
echo 0 >> in_cluster_exp.txt
for (( i=0;i<$nrclusters;i++ ))
{
   let "k = $i + 1"
   echo -n "$k " >> in_cluster_exp.txt
   
   for j in `cat tempdata | grep NODE  | tr -d " "`
   do
    no=`echo $j | grep "CLUSTER;$i;" | awk -F";" {'print $2'}`
    if [ ! -z $no ] 
     then 
	
	#echo "$no is in the cluster $k"
	echo -n "$no " >> in_cluster_exp.txt
	
     
     fi
   done
  echo >> in_cluster_exp.txt 
}
let "finalcluster = $nrclusters + 1"
echo $finalcluster >> in_cluster_exp.txt
let "totalclusters = $finalcluster + 1"



rm -f  in_spectrum_exp.txt 

echo "[device, cluster, pkt(i)*ToA(i,c)/PRR(i,c)*NoCl(i,c)]" > in_spectrum_exp.txt

#for (( i=0;i<$nrnodes;i++ ))
#{
#        linenr=`grep " $i " in_cluster_exp.txt | cut -d" " -f1`
	
#	for (( j=0;j<10;j++ ))
#	{
#	 if [ $j -eq $linenr ] 
#          then
#		 echo "$i $j PLACEHOLDERSPECTRUM_$j" >> in_spectrum_exp.txt
#	  else
#		 echo "$i $j 0"  >> in_spectrum_exp.txt
#          fi 
#        }
#}

break='0'
for (( i=0;i<$nrnodes;i++ ))
{
        linenr=`grep " $i " in_cluster_exp.txt | cut -d" " -f1`
	
	for (( j=0;j<$totalclusters;j++ ))
	{
	 if [ $j -eq $linenr ] 
          then
		if [ $break -ne $j ]
		then
	 	  echo "$i $j PLACEHOLDERSPECTRUM_$j" >> in_spectrum_exp.txt
		  let "break=$j"
	  	else
		 echo "$i $j 0"  >> in_spectrum_exp.txt
		fi
	  else
		 echo "$i $j 0"  >> in_spectrum_exp.txt
          fi 
        }
}


rm -f startup_ext.txt
nodes_number_before=0

for (( i=1;i<$finalcluster;i++ ))
{

	nodes_number=`wc -l cluster$i-coordinates.dat | tr ' ' '\n' | head -1`
	if [ $nodes_number -gt "0" ]
	then 
		docker cp cluster$i-coordinates.dat loving_hermann:/ns3/nodes.csv
		TOA=`docker exec loving_hermann /bin/sh -c "cd /ns3;./waf  --run  \"src/lorawan/examples/complete-network-example --rlagent=0 --radius=2000 --nDevices=$nodes_number --simulationTime=3600 --training=false  --pkgbasesize=18\" | grep \"has come\" | tail -n 1 |cut -d\" \" -f6"`
		echo $TOA
		sed -i "s/PLACEHOLDERSPECTRUM_$i/$TOA/g" in_spectrum_exp.txt  
	else
		sed -i "s/PLACEHOLDERSPECTRUM_$i/0/g" in_spectrum_exp.txt  
	fi
	

	for (( j=0;j<$nodes_number_before;j++ ))
	{

	 echo -n "0 " >> startup_ext.txt

	}
        for (( j=0;j<$nodes_number;j++ ))
	{

		bootuptime=`docker exec loving_hermann /bin/sh -c "cd /ns3;./waf  --run  \"src/lorawan/examples/complete-network-example --rlagent=0 --radius=2000 --nDevices=$nodes_number --simulationTime=3600 --training=false  --pkgbasesize=18\" | grep -w \"... $j\" | head -1|cut -d\" \" -f10 | cut -d\".\" -f1"`
		echo -n "$bootuptime " >> startup_ext.txt
		#echo "this is the node $j with value"
		#let "startk = $startk + $j"	
	}
	let "startk = $nodes_number+$nodes_number_before"
	for (( k=startk;k<$nrnodes;k++))
	{
		echo -n "0 " >> startup_ext.txt
	
	}
	echo ""	

	let "nodes_number_before=$nodes_number+$nodes_number_before"
}

rm -f ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/startup_ext.txt
cp startup_ext.txt ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/

sed -i "s/PLACEHOLDERSPECTRUM_0/0/g" in_spectrum_exp.txt  
sed -i "s/PLACEHOLDERSPECTRUM_9/0/g" in_spectrum_exp.txt  

#rm -f cluster{0..7}-coordinates.dat

#rm -f tempdata


nrclusters=`cat in_cluster_exp.txt |wc -l`
let "nrclusters = $nrclusters - 3"


rm -f vertices_ext.txt

for ((i=1;i<=$nrclusters;i++))
{
 
 #check=`grep $i in_edge_exp.txt`
 let "validate=$i%4" 
 
 if [ $validate -eq "0" ]
 then
   echo "0" >> vertices_ext.txt
 else
   echo "1" >> vertices_ext.txt
 fi

}

rm -f ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/vertices_ext.txt
cp vertices_ext.txt ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/

rm -f targets_ext.txt 
echo "NR NODES $nrnodes"
cat coordinates_xy.txt | grep "," | cut -d"," -f4 > targets_ext.txt
rm -f ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/targets_ext.txt
cp targets_ext.txt ~zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/

#GAMETIME=`./thmax in_cluster_exp.txt in_edge_exp.txt in_delay_exp.txt in_spectrum_exp.txt 10 1 $nrnodes 5000 output.lp && cplex < commands  | grep tauMAX |awk -F" " {'print $2'} | cut -d"." -f1`



#./thmax in_cluster_exp.txt in_edge_exp.txt in_delay_exp.txt in_spectrum_exp.txt $totalclusters $nrgateways $nrnodes 2700 output.lp 
####
./thmax in_cluster_exp.txt in_edge_exp.txt in_delay_exp.txt in_spectrum_exp.txt $totalclusters $nrgateways $nrnodes 2700 output.lp && cplex < commands > tauMAX-output
###
#./thmax in_cluster_exp.txt in_edge_exp.txt in_delay_exp.txt in_spectrum_exp.txt 10 1 $nrnodes 2700 output.lp && cplex < commands 

tau=`cat tauMAX-output |grep tauMAX |awk -F" " {'print $2'} |cut -d"." -f1`
volume=`cat coordinates_xy.txt | grep -v cluster |awk '{split($0,a,","); sum += a[4]} END {print sum}'`

echo "The game time optimized is $tau and the volume of data transferred is $volume for $nrnodes nodes, and $nrgateways gateways"

