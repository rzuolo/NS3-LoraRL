#!/bin/bash


##### Defining some style formats
#################################
RED='\033[0;31m'
NC='\033[0m' # No Color
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'

spin[0]="-"
spin[1]="\\"
spin[2]="|"
spin[3]="/"
#################################


## Function to parse yml and assign attributes to variables
## It is possible to add a prefix to the variables by using the 2nd positional argument, eg. "config_"
## Cloned from https://gist.github.com/pkuczynski/8665367 
parse_yaml() {
   local prefix=$2
   local s='[[:space:]]*' w='[a-zA-Z0-9_]*' fs=$(echo @|tr @ '\034')
   sed -ne "s|^\($s\)\($w\)$s:$s\"\(.*\)\"$s\$|\1$fs\2$fs\3|p" \
        -e "s|^\($s\)\($w\)$s:$s\(.*\)$s\$|\1$fs\2$fs\3|p"  $1 |
   awk -F$fs '{
      indent = length($1)/2;
      vname[indent] = $2;
      for (i in vname) {if (i > indent) {delete vname[i]}}
      if (length($3) > 0) {
         vn=""; for (i=0; i<indent; i++) {vn=(vn)(vname[i])("_")}
         printf("%s%s%s=\"%s\"\n", "'$prefix'",vn, $2, $3);
      }
   }'
}

## Check if the arguments file is present and parse it
## Quit otherwise
if [ ! -f arguments.yml ]; then
   echo -e "${RED}\033[5m No arguments file "arguments.yml" provided... Exiting :-(\033[0m${NC}"
   kill -INT $$
else
   eval $(parse_yaml arguments.yml "")
fi

## Check if the nodes positions file is present and extract the number of nodes from it
## Quit otherwise
if [ ! -f nodes.csv ]; then
   echo -e "${RED}\033[5m No nodes file "nodes.csv" provided... Exiting :-(\033[0m${NC}"
   kill -INT $$
else
   lnumber=`cat nodes.csv | sed '/^[[:space:]]*$/d'| wc -l`
fi


###############################################################################################################
######## Abort in case it overflows a static limit imposed by the NS3 code                                  ###
####### This is very poor coding, thus it has to be upgraded to a dynamic allocation (in the NS3 part of it)###
###############################################################################################################
if [ $lnumber -gt 4000 ]
then
   echo -e "${RED} \033[5mYou have exceeded the limit of 4000 nodes... aborting\033[0m${NC}"
   kill -INT $$
fi


## Reset performance indicator
maxperf=0.000000 

## Clear all previous experiences
rm -f array.txt
rm -rf performance-records
rm -rf temp-array-store
rm -rf workbench

## Create temporary folder for holding 
## QTable values, experiences and handlers
mkdir temp-array-store
mkdir performance-records
mkdir workbench 

###############################################################################
###############################################################################
#### Run training 
###############################################################################
###############################################################################
sed -i "s/PLACEHOLDER/$rladr_algorithm/g" src/lorawan/model/adr-component.cc
i=0
while [ $i -lt $training_max_rounds ]
 do 
  ./waf  --run  "src/lorawan/examples/complete-network-example  --rlagent=$rladr_mode --radius=$context_buildings_radius --nDevices=$lnumber --simulationTime=$training_round_time --training=true --gamma=$training_gamma --epsilon=$training_epsilon --alpha=$training_alpha --pkgbasesize=$context_pkg_size"| grep Result | tr '\n' '\0' | awk -F" " {'print $6'} > performance-records/perf-$i; perf=`cat performance-records/perf-$i`;clear;echo "";echo -e "#####${BLUE}Iterative Learning Process${NC}###########";echo -e "$perf    optimal goodput => ${GREEN} $maxperf ${NC}";echo "#########################################"; echo "${spin[$(($i % 4))]}" ;(( $(echo "$perf > $maxperf" |bc -l) )) && maxperf=$perf && cp array.txt temp-array-store/max-array.txt
  let "i = $i + 1"
done
sed -i "s/EnumValue (AdrComponent::$rladr_algorithm)/EnumValue (AdrComponent::PLACEHOLDER)/g" src/lorawan/model/adr-component.cc


###############################################################################
###############################################################################
#### Simulation function - Called by different ADR test-validation executions 
###############################################################################
###############################################################################
simulation()
{

	rladr_algorithm=$1
	rladr_mode=$2
	modality=$3
	title=$4
	cp temp-array-store/max-array.txt array.txt
	echo "$title" > workbench/$modality-energy-plot.csv
	echo "$title" > workbench/$modality-goodput-plot.csv
	sed -i "s/PLACEHOLDER/$rladr_algorithm/g" src/lorawan/model/adr-component.cc
	i=0
	while [ $i -lt $context_max_rounds ]
 		do 
  			./waf  --run  "src/lorawan/examples/complete-network-example --rlagent=$rladr_mode --radius=$context_buildings_radius --nDevices=$lnumber --simulationTime=$training_round_time --training=false --gamma=$rladr_gamma --epsilon=$rladr_epsilon --alpha=$rladr_alpha --pkgbasesize=$context_pkg_size"| grep Result > workbench/round-$i
  			goodput=`cat workbench/round-$i | cut -d" " -f6` 
  			energy=`cat workbench/round-$i | cut -d" " -f5` 
  			clear
		        echo ""
			echo -e "${YELLOW}###############$title Simulation##################### ${NC}"
			echo -e "Round $i ${BLUE} Goodput $goodput ${NC} ${GREEN}Energy $energy${NC}"
			echo -e "${YELLOW}##################################################### ${NC}"
  			echo -e "${RED} ${spin[$(($i % 4))]} ${NC}"
			echo "$goodput" >> workbench/$modality-goodput-plot.csv
  			echo "$energy" >> workbench/$modality-energy-plot.csv
  			let "i = $i + 1"
		done
	sed -i "s/EnumValue (AdrComponent::$rladr_algorithm)/EnumValue (AdrComponent::PLACEHOLDER)/g" src/lorawan/model/adr-component.cc

}


#### Run RLADR test 
simulation AVERAGE 1 adrrl ADR-RL
#### Run RLADR test 
simulation AVERAGE 0 adravg ADR-AVG
#### Run RLADR test 
simulation MAXIMUM 0 adrmax ADR-MAX
#### Run RLADR test 
simulation MINIMUM 0 adrmin ADR-MIN


output_dir="results_$(date +%Y%m%d_%H%M)"
mkdir /tmp/$output_dir

##########################################################
###### Compile the results into files for further printing
###### Files are stored in /tmp directory
##########################################################
paste workbench/adrrl-energy-plot.csv workbench/adravg-energy-plot.csv workbench/adrmax-energy-plot.csv workbench/adrmin-energy-plot.csv | tr "\t" "," > /tmp/$output_dir/energy_results.csv 
paste workbench/adrrl-goodput-plot.csv workbench/adravg-goodput-plot.csv workbench/adrmax-goodput-plot.csv workbench/adrmin-goodput-plot.csv | tr "\t" "," > /tmp/$output_dir/goodput_results.csv


##########################################################
###### Call a python side script for printing everything
###### with pandas and mathplotlib
##########################################################
python printing.py $output_dir



clear
echo ""
echo -e "${YELLOW}##################################################### ${NC}"
echo -e "${BLUE} The simulation has finished!${NC}"
echo -e "${BLUE} You can find the plots and results at${NC} ${RED} /tmp/$output_dir ${NC}"
echo -e "${YELLOW}##################################################### ${NC}"
echo -e "${RED} Have a nice day :-) !!! ${NC}"



