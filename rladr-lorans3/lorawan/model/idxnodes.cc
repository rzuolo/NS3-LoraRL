
#include "idxnodes.h"
#include <cstdint>


#include <stdio.h>
#include <iostream>

namespace ns3 { namespace lorawan {

//Global variables
//Default values assignment
struct LabDevices trialdevices[4000];

int nDevices = 200;

double global_power = 0.0;
double min_diff_power_total_overt_all_ts = 0.0;
int global_sent = 0;
int global_received = 0;
int pkgbasesize = 20;
int rlagent = 0;
int settled = 100;

double avgDistance = 0 ;

int global_under_threshold = 0;
int global_interference = 0 ;
int global_no_demodulator = 0 ;
int global_transmitting = 0 ;

//Search for the node according to its index in the trialdevices array
int FindNode (int addr)
{

	for (int i=0;i<4000;i++)
	{
		if (trialdevices[i].nwkaddr == addr) return i;

	}

	return 0;
}

//Print out on the screen the progress of execution
void Flashout(int idx)
{

	std::cout<<"########################################################"<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" I am the node " <<idx<< " sending the pkt #" << trialdevices[idx].pkg_on_air<< " is random "<< trialdevices[idx].random_selection <<std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" state_i[0] "<< trialdevices[idx].state_i[0]<< std::endl;
	std::cout<<" state_i[1] "<< trialdevices[idx].state_i[1]<< std::endl;
	std::cout<<" state_i[2] "<< trialdevices[idx].state_i[2]<< std::endl;
	std::cout<<" state_ii[0] "<< trialdevices[idx].state_ii[0]<< std::endl;
	std::cout<<" state_ii[1] "<< trialdevices[idx].state_ii[1]<< std::endl;
	std::cout<<" state_ii[2] "<< trialdevices[idx].state_ii[2]<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" pwr_i "<< trialdevices[idx].pwr_i<< std::endl;
	std::cout<<" pwr_ii "<< trialdevices[idx].pwr_ii<< std::endl;
	std::cout<<" pwr_total "<< trialdevices[idx].pwr_total<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" instant_reward_i "<< trialdevices[idx].instant_reward_i<< std::endl;
	std::cout<<" instant_reward_ii "<< trialdevices[idx].instant_reward_ii<< std::endl;
	std::cout<<" snr_i "<< trialdevices[idx].snr_i<< std::endl;
	std::cout<<" snr_ii "<< trialdevices[idx].snr_ii<< std::endl;
	std::cout<<" reward_total_i "<< trialdevices[idx].reward_total_i<< std::endl;
	std::cout<<" reward_total_ii "<< trialdevices[idx].reward_total_ii<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" action_i "<< trialdevices[idx].action_i<< std::endl;
	std::cout<<" action_ii "<< trialdevices[idx].action_ii<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" mdp_state_i "<< trialdevices[idx].mdp_state_i<< std::endl;
	std::cout<<" mdp_state_ii "<< trialdevices[idx].mdp_state_ii<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" success "<< trialdevices[idx].success<< std::endl;
	std::cout<<" fail "<< trialdevices[idx].fail<< std::endl;
	std::cout<<" distance "<< trialdevices[idx].distance<< std::endl;
	std::cout<<" pkg_on_air "<< trialdevices[idx].pkg_on_air<< std::endl;
	std::cout<<"########################################################"<< std::endl;
	std::cout<<" previous_attempt "<< trialdevices[idx].previous_attempt<< std::endl;
	std::cout<<" success_rate "<< trialdevices[idx].success_rate<< std::endl;
	std::cout<<"########################################################"<< std::endl;


}


} }

