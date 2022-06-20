#!/bin/bash


if [ $# -ne "7" ]
then

	echo "Provide the arguments in sequence 1st #gateways, 2nd #actions, 3rd AppPeriod (Usually 200), 4th #nodesi, 5h Gametime, 6th freshness, 7th real"
	return 1

fi 


let "INPUTFEAT= ($1*4)+1"
let "OUTPUTFEAT= $2"
let "PERIOD= $3"
let "NRNODES= $4"
let "ACTIONMAX = $2 - 1"
cp template/LoraGatewayGame.py .
sed -i "s/PLACEHOLDERGATEWAYS/$1/g" LoraGatewayGame.py
sed -i "s/PLACEHOLDERACTIONS/$2/g" LoraGatewayGame.py
sed -i "s/PLACEHOLDERINPUTFEAT/$INPUTFEAT/g" LoraGatewayGame.py
sed -i "s/PLACEHOLDEROUTPUTFEAT/$OUTPUTFEAT/g" LoraGatewayGame.py
sed -i "s/PLACEHOLDERMINUS/$ACTIONMAX/g" LoraGatewayGame.py

REAL="$7"

lista0="[]"
rm -f ho

        for l in $lista0
        do
                i=0
                while [ $i -lt $1 ]
                do
                        echo -n "$l" >> ho
                        let "i=$i+1"
                        echo -n "," >> ho
                done
        done
        #var0=`echo -n $var2",[]"`
        var0=$(cat ho|sed 's/.$//')
rm -f ho

sed -i "s/PLACEHOLDERFORPLACES/$var0/g" LoraGatewayGame.py



### here, also assuming the local home directory for the python OpenAIGym module
### this is likely different on other systems 
cp ~/.local/lib/python3.8/site-packages/gym/envs/classic_control/template/loracollector130.py ~/.local/lib/python3.8/site-packages/gym/envs/classic_control/loracollector130.py

TARGET="/home/zuolo/.local/lib/python3.8/site-packages/gym/envs/classic_control/loracollector130.py"


sed -i "s/PLACEHOLDERGATEWAYS/$1/g" $TARGET
sed -i "s/PLACEHOLDERACTIONS/$2/g" $TARGET
sed -i "s/PLACEHOLDERPERIOD/$3/g" $TARGET
sed -i "s/PLACEHOLDERNODES/$4/g" $TARGET


let "gameinsteps = $5/5"

sed -i "s/PLACEHOLDERGAMETIME/$gameinsteps/g" $TARGET
sed -i "s/PLACEHOLDERFRESH/$6/g" $TARGET


rm -f startup_ext.txt
cp ../CPLEX/startup_ext.txt .

rm -f vertices_ext.txt
cp ../CPLEX/vertices_ext.txt . 

rm -f targets_ext.txt
cp ../CPLEX/targets_ext.txt .

rm -f triptimes.txt
cp ../CPLEX/triptimes.txt .


let "VARRAND= ($2-1)"
sed -i "s/VARRAND/$VARRAND/g" $TARGET

IFS=":"

let "head=$2+1"
lista1="$head:2160:500:2160"
rm -f gaba

	for j in $lista1
	do
		i=0
		while [ $i -lt $1 ]
		do
  			echo -n "$j" >> gaba
			let "i=$i+1"
			echo -n "," >> gaba
		done
	done
	var1=$(cat gaba)
	var1=`echo -n $var1$1`
rm -f gaba 

lista2="$2:0:0:0"
rm -f hey

	for k in $lista2
	do
		i=0
		while [ $i -lt $1 ]
		do
  			echo -n "$k" >> hey
			let "i=$i+1"
			echo -n "," >> hey
		done
	done	
	var2=$(cat hey)
	var2=`echo -n $var2"0"`
rm -f hey

sed -i "s/PLACEHOLDEROBSPACE/self.observation_space = spaces.MultiDiscrete([$var1])/g" $TARGET
sed -i "s/PLACEHOLDERINITSTATE/self.observation_space = self.state[$var2]/g" $TARGET
sed -i "s/PLACEHOLDERMASK/$var2/g" $TARGET

echo "This is $REAL "

if [ "$REAL" != "real" ]
then
	#rm -f model-store label-store memory-store keiser
	rounds=1
	i=0
	while [ $i -lt $rounds ]
	do 
		echo "ROUND $i"
		python LoraGatewayGame.py nada |grep LOG4 |grep SAR
		#python LoraGatewayGame.py nada |grep LOG4 
		#python LoraGatewayGame.py nada 
		let "i=i+1"
	done
else

	rounds=3
	i=0
	while [ $i -lt $rounds ]
	do 
		echo "ROUND $i"
		python LoraGatewayGame.py real |grep LOG4 |grep SAR
		#python LoraGatewayGame.py nada 
		let "i=i+1"
	done


fi
