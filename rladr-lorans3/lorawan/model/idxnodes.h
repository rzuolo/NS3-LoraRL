/*
 * idxnodes.h
 *
 *  Created on: 6 de mai de 2020
 *      Author: zuolo
 */
#include <cstdint>

#ifndef IDXNODES_H
#define IDXNODES_H


namespace ns3 {
namespace lorawan {

//struct to hold infor pertaining the simulation
//TxPower per node, SF per node, Frequency utilized, MDP state, etc
struct LabDevices{

	int state_i[3] = {0,0,0}; // 'SF' , 'TXPwr', 'Freq'
	int state_ii[3] = {0,0,0};

	double pwr_i = 0.0;
	double pwr_ii = 0.0;
	double pwr_total = 0.0;
	
	double max_pwr  = 0.00001;

	double instant_reward_i = 0.0;
	double instant_reward_ii = 0.0;


	double snr_i = 0.0;
	double snr_ii = 0.0;

	int mdp_state_i = 0;
	int mdp_state_ii = 0;

	double reward_total_i = 0.0;
	double reward_total_ii = 0.0;

	int id = 0;
	int nwkaddr = 0;
	int success = 0;
	int fail = 0;

	float distance = 0.0;

	int last_size = 0;

	int action_i = 0;
	int action_ii = 0;


	int pkg_on_air = 0;

	uint64_t last_interval = 0;

	bool previous_attempt = true;

	float success_rate = 0;

	int fail_for_under_threshold = 0;
	int fail_for_interference = 0;
	int fail_for_no_demodulator = 0;
	int fail_for_transmitting = 0;

	bool random_selection = false;

	};


//static int portion;

extern double global_power;
extern double min_diff_power_total_overt_all_ts;
extern int global_sent;
extern int global_received;
extern int pkgbasesize;
extern int rlagent;
extern int settled;
extern int nDevices;

extern double avgDistance;

extern struct LabDevices trialdevices[4000];

int FindNode (int addr);
void Flashout (int idx);


extern int global_under_threshold;
extern int global_interference ;
extern int global_no_demodulator ;
extern int global_transmitting ;


} //namespace ns3
}

#endif /* SRC_LORAWAN_MODEL_IDXNODES_H_ */
