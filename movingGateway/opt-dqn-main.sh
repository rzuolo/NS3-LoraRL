#!/bin/bash


if [ $# -ne "2" ]
then

        echo "Provide the arguments in sequence 1st #gateways, 2nd #clusters"
        return 1

fi



cd CPLEX
source call-clustering.sh $1 $2 >> /dev/null 2>&1




tau=`cat tauMAX-output |grep tauMAX |awk -F" " {'print $2'} |cut -d"." -f1`
volume=`cat coordinates_xy.txt | grep -v cluster |awk '{split($0,a,","); sum += a[4]} END {print sum}'`
nrnodes=`cat tempdata|grep COUNTER|awk -F";" {'print $2'}`

cd ../DQN


rm -f model-store label-store memory-store keiser


if [ -z $tau ]
then
	#echo "No solution found, running DQN for 3 hours"
	#source main.sh  $1 $2 200 $nrnodes 11340 2500 nada  >> /dev/null 2>&1  #5percent
	#source main.sh  $1 $2 200 $nrnodes 11880 2500 nada  >> /dev/null 2>&1  #10percent
	#source main.sh  $1 $2 200 $nrnodes 12420 2500 nada  >> /dev/null 2>&1  #15percent
	source main.sh  $1 $2 200 $nrnodes 10800 2500 nada  >> /dev/null 2>&1  #15percent
else
	#a=`echo "$tau*0.15" | bc `
	#a=`echo "$tau*0.10" | bc `
	#a=`echo "$tau*0.05" | bc `
	#a=${a%.*}
	#let "tau= $tau + $a"
	source main.sh  $1 $2 200 $nrnodes $tau 2500 nada >> /dev/null 2>&1
fi

cd ..
let "maxOPT = $volume * 20"

maxDQN=`cat DQN/keiser`

let "performance = ($maxDQN * 100)/ $maxOPT"

#echo "05percentFinal result for nodes $nrnodes and volume $volume and tau $tau is this: ModelOPT $maxOPT, DQN $maxDQN and its performance is $performance"
#echo "10percentFinal turn $i result for nodes $nrnodes and volume $volume and tau $tau is this: ModelOPT $maxOPT, DQN $maxDQN and its performance is $performance"
#echo "15percentFinal result for nodes $nrnodes and volume $volume and tau $tau is this: ModelOPT $maxOPT, DQN $maxDQN and its performance is $performance"
let "avg = $maxOPT/$nrnodes"
echo "Final result for nodes $nrnodes AVGpernode $avg and volume $volume and tau $tau is this: ModelOPT $maxOPT, DQN $maxDQN and its performance is $performance"


