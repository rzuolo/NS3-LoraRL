#include <cstdint>
#include "stdio.h"
#include <iostream>
#include "ns3/idxnodes.h"
#include <ctime>
#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <curses.h>
#include <Python.h>

namespace ns3 { namespace lorawan {

double** Q = new double*[672];

float ToA [6] = {66.8,123.4,226.3,411.6,823.3,1646.6};	

bool training = true;

float ALPHA = 0.7; // learning rate
float GAMMA = 0.8; // discount factor
float EPS = 0.7; // exploration factor


int freq_on_air[3] = {0,0,0} ;
int st_freq = 2 ;
int nd_freq = 1 ;
int rd_freq = 0;


//Lehmer adaptation to represent states for the arrangements
// ((st_freq*6) + (nd_freq*2) + (rd_freq*1));
// order 868.1 868.3 868.5 = (0*6)+(1*2)+(2*1) = 4
// order 868.1 868.5 868.3 = (0*6)+(2*2)+(1*1) = 5
// order 868.3 868.1 868.5 = (1*6)+(0*2)+(2*1) = 8
// order 868.5 868.1 868.3 = (2*6)+(0*2)+(1*1) = 13
// order 868.3 868.5 868.1 = (1*6)+(2*2)+(0*1) = 14
// order 868.5 868.3 868.1 = (2*6)+(1*2)+(0*1) = 10
int freq_states[] = {4,5,8,13,10,14};

/*double compute_reward (int nodeid )
//{



//	trialdevices[FindNode(nodeid)].last_action;
//	trialdevices[FindNode(nodeid)].snr_i;
	trialdevices[FindNode(nodeid)].snr_ii;


	//std::cout<<"Here it is "<<(100*(float(trialdevices[FindNode(nodeid)].pwr_ii)))<<std::endl;
	// sort of glboal rewarding
	//if (trialdevices[FindNode(nodeid)].pwr_ii == 0)
	//{
	//	return 0;
	//}
	//else
	//{
	//	return double(((float(global_received)/float(global_sent))*100)/(100*(float(trialdevices[FindNode(nodeid)].pwr_ii))/float(global_power)));
	//}

*/
//compute next_action


// select random action
int rdaction ( )
{
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    const boost::posix_time::time_duration td = now.time_of_day();
    const long hours        = td.hours();
    const long minutes      = td.minutes();
    const long seconds      = td.seconds();
    const long milliseconds = td.total_milliseconds() - ((hours * 3600 + minutes * 60 + seconds) * 1000);
    srand( int(milliseconds ));

    //srand(system("/usr/bin/od -vAn -N4 -t u4 < /dev/urandom"));    
    return (rand() % 48);

}

//Save Q value into file
void SaveQ (double **Q)
{

		std::ofstream myfile;
		myfile.open ("array.txt");

	      for(int i = 0; i < (672);i++)
		  {
			  for (int j=0; j<48;j++)
			  {

				  myfile << Q[i][j];
				  myfile << "|";
			  }
			  myfile << "\n";
		  }

		  myfile.close();
}

//Debug printing reward
void get_reward(int nodeid)
{
	//std::cout<<"this was the decision "<<trialdevices[FindNode(nodeid)].action_ii  <<" in the state "<< trialdevices[FindNode(nodeid)].snr_ii <<"with reward"<< trialdevices[FindNode(nodeid)].reward_total_ii << "by the node "<< nodeid<<std::endl;
}

//Decode Frequency from action index
double fr_from_action(int action)
{
	int fr_base = int(action/48);
	double FCP[3] = {868.1,868.3,868.5};

	return FCP[fr_base];
}
 
//Decode SF from action index
int sf_from_action(int action)
{

	int sftx = action;
	int sf = int(sftx/8);
	return sf+7;

}

//Decode TxPwr from action index
int tx_from_action(int action)
{

	return (2*((action%8)+1));


}

//find the max within the least used frequency interval
//  0-47  being freq 0 the least used
// 48-95  being freq 1  the least used
// 96-143 being freq 2 the least used
//
// quota is defined but not in use yet
// (define whether penalties will be applied for physical prohibited choices)
int find_max(int state,int border,int idx)
{
	double max = -1000;
	int maxid = 0;

	for(int i = 0;i < 48; i++)
    {
		 if( max < Q[state][i])
          {
	
			 max = Q[state][i];
			 maxid = i;
          }

    }


	/*if (Q[state][maxid] == 0)
	{
		maxid = rdaction();
	}*/

    std::cout<<"Acao escolhida "<<maxid<<" valor "<<Q[state][maxid]<<std::endl; 
    return maxid;
}

//Decode Action index from TxPwr and SF
int get_action (int sf, int tx)
{
	int action = 0;

	action = action+((8*sf)+tx);

	return action;

}

//Greedy selection
int chose_action(int nodeid)
{
	int chosen;
	srand( (unsigned)time( NULL ) );
	//srand(system("/usr/bin/od -vAn -N4 -t u4 < /dev/urandom"));	
	float p = (float) rand()/RAND_MAX;

	if (p < EPS)
	{
		//chosen = int(p*1000) % 48;
		chosen = rdaction();
		//std::cout<<"Random Choice "<< trialdevices[nodeid].mdp_state_ii<< " "<<chosen<<std::endl;
		trialdevices[nodeid].random_selection = true;
	}
	else
	{
		//change state condition
		//find the max in the range of the least used frequency
		
		if ( rlagent == 1 )
		{
			std::cout<<"Learned State "<< trialdevices[nodeid].mdp_state_ii<<std::endl;
			chosen=find_max(trialdevices[nodeid].mdp_state_ii,rd_freq, nodeid);
	  		trialdevices[nodeid].random_selection = false;
		}
		else
		{
			Py_Initialize();
			PyObject *pName;
        		PyObject *module;
		        pName = PyUnicode_DecodeFSDefault("dqnadr");
        		module = PyImport_Import(pName);
        		Py_DECREF(pName);
			//PyObject_Print(module,stdout,Py_PRINT_RAW);
			//std::cout<<std::endl;
			//std::cout<<"We are going to run the DQN on state "<< trialdevices[nodeid].mdp_state_ii <<std::endl;
        		//Py_DECREF(module);
		        PyObject *func = PyObject_GetAttrString(module,"decisionmaker");		
			PyObject *arg_state = Py_BuildValue("(i)",trialdevices[nodeid].mdp_state_ii);	
			PyObject *selected_action = PyEval_CallObject(func,arg_state);
			//PyObject *selected_action = PyEval_CallObject(func,NULL);
			//PyEval_CallObject(func,NULL);
			
			int *resulting_action =new int();
			int *resulting_output =new int();
			PyArg_ParseTuple(selected_action, "ii", resulting_action, resulting_output);

        		//Py_DECREF(modul);
        		//Py_DECREF(func);
			//std::cout<<"The returned action is "<< *resulting_action <<std::endl;
			chosen = *resulting_action;
	  		trialdevices[nodeid].random_selection = false;
			//Py_Finalize();			
		}
	}

        if (((EPS - 0.001) / global_sent) > 0)
        {
	    //std::cout<<"New EPS "<<EPS <<" "<<global_sent <<std::endl;
            EPS = EPS - ((EPS - 0.001) / global_sent);
        }
        else
        {
            EPS = 0;
        }
	
	//std::cout<<"Action taken is "<< chosen << " SF "<< sf_from_action(chosen) << " Tx  " << tx_from_action(chosen)  <<std::endl;
	
	return chosen;
}


//Run the RL process 
void rlprocess(int nodeid, int rectype, uint8_t received_sf, double received_tx)
{

	//std::cout<<"ALPHA "<< ALPHA<<" EPS "<< EPS << " GAMMA "<<GAMMA <<std::endl;

	switch (rectype)
	{
	case 0://received with success


		bool mismatch;

		mismatch = false; 
		
		//std::cout<<" SF supposed to be "<< sf_from_action(trialdevices[nodeid].action_ii) <<" but, in fact, it is "<< unsigned(received_sf) <<std::endl;
		//std::cout<<" TX supposed to be "<< tx_from_action(trialdevices[nodeid].action_ii) <<" but, in fact, it is "<< 16 - received_tx <<std::endl;

		//std::cout<<" "<< int(sf_from_action(trialdevices[nodeid].action_ii)) << " " << int(unsigned(received_sf)) << " " << int(tx_from_action(trialdevices[nodeid].action_ii)) <<" " <<  (16-received_tx)<<std::endl;
		//check if must penalize previous round
		if (      ((sf_from_action(trialdevices[nodeid].action_ii) != (int(unsigned(received_sf))))) or ((tx_from_action(trialdevices[nodeid].action_ii)) != (16-received_tx))  )     
		{ 
			mismatch = true; 
		}		

		//check if the battery used is the maximum so far

		if  (trialdevices[nodeid].pwr_ii > trialdevices[nodeid].max_pwr)  trialdevices[nodeid].max_pwr = trialdevices[nodeid].pwr_ii;
		
		//assign data rate and battery rate to local variables
		float prate; 
		prate = trialdevices[nodeid].pwr_ii/trialdevices[nodeid].max_pwr;
		float drate; 
		drate = (66.8/(ToA[trialdevices[nodeid].state_ii[0]-7]));
		
		
		//if last command reflects back, then give a reward, otherwise penalize
		if ( mismatch )
		{ 
			trialdevices[nodeid].reward_total_ii = -(drate*prate); 
			//trialdevices[nodeid].reward_total_ii = -1; 
		}
		else
		{
			trialdevices[nodeid].reward_total_ii = (drate)-(drate*prate);
			//trialdevices[nodeid].reward_total_ii = 0;
		}
		
		
		//if (trialdevices[nodeid].pkg_on_air > 1 and EPS != 0)
		//if (trialdevices[nodeid].pkg_on_air > 1 )
		if (trialdevices[nodeid].pkg_on_air > 1 and training and EPS != 0)
		{
			int state1 = int(trialdevices[nodeid].mdp_state_i);
			int state2 = int(trialdevices[nodeid].mdp_state_ii);
			int act1 = int(trialdevices[nodeid].action_i);
			int act2 = int(trialdevices[nodeid].action_ii);
		
			//std::cout<<"To verify "<<state1<<" "<<state2<<" "<<act1<<" "<<act2<<" "<<trialdevices[nodeid].reward_total_ii <<std::endl;	
			//std::cout<<"Supposedly running RL training "<<rlagent <<std::endl; 
			
			if ( rlagent == 1)
			{
				//std::cout<<"Supposedly running RL training "<<std::endl; 
				Q[state1][act1] = Q[state1][act1] + (ALPHA*(trialdevices[nodeid].reward_total_ii + ((GAMMA*Q[state2][act2]) - Q[state1][act1])));
			}
			else
			{
				//If using DQN, record every interaction into the memory
				//Every 10 rounds, run the optimizer
				Py_Initialize();
				PyObject *pName;
        			PyObject *module;
		        	pName = PyUnicode_DecodeFSDefault("dqnadr");
        			module = PyImport_Import(pName);
        			Py_DECREF(pName);
		        	PyObject *func = PyObject_GetAttrString(module,"remember");		
				PyObject *arg_bellman = Py_BuildValue("(iiif)",state1,state2,act1,trialdevices[nodeid].reward_total_ii);	
				PyEval_CallObject(func,arg_bellman);
				
				if ( ((trialdevices[nodeid].success + trialdevices[nodeid].fail) % 10) == 0)
				{	
		        		PyObject *func = PyObject_GetAttrString(module,"train");		
					PyObject *arg_bellman = Py_BuildValue("(iiif)",state1,state2,act1,trialdevices[nodeid].reward_total_ii);	
					PyEval_CallObject(func,arg_bellman);
				}
			}




		}
		break;
	case 1://received with interference or under threshold

		//trialdevices[nodeid].instant_reward_ii = 0;
		//trialdevices[nodeid].reward_total_ii = 0;

		break;


	}
}

//Update used carrier frequency - Decrease
void release_freq(int freq, int nodeid)
{

	if (trialdevices[nodeid].pkg_on_air > 1 )
	{
		freq_on_air[freq] = freq_on_air[freq] - 1;
	}


}
//Update used carrier frequency - Increase
void alloc_freq(int freq)
{
	freq_on_air[freq] = freq_on_air[freq] + 1;
}

//Sort the carrier frequencies used
int sort_freq()
{
	 int min = freq_on_air[0];
	 int max = freq_on_air[2];
	 //int mid = freq_on_air[1];
	 int index_min = 0; //start min with 868.1
	 int index_mid = 1; //start min with 868.3
	 int index_max = 2; //start min with 868.5


	 for (int i=0;i<3;i++)
	 {

		 	 if (freq_on_air[i] < min)
			 {
				 min = freq_on_air[i];
				 index_min = i;
			 }
			 else
			 {
				if (freq_on_air[i] > max)
		 	 	 	 {
		 	     	 	 max = freq_on_air[i];
		 	     	 	 index_max = i;
		 	 	 	 }
			 }


			 index_mid = 3 - index_max - index_min;
			 //mid = freq_on_air[index_mid];

	 }


	 st_freq = index_max;
	 nd_freq = index_mid;
	 rd_freq = index_min;
	

 	 return index_min;
}


} }




