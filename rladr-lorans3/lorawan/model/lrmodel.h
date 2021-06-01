#include <cstdint>
#include "stdio.h"
#include "idxnodes.h"


#ifndef LRMODEL_H
#define LRMODEL_H

namespace ns3 { namespace lorawan {


extern float ALPHA;
extern float GAMMA;
extern float EPS;

extern bool training;

extern int freq_on_air[3];
extern int st_freq;
extern int nd_freq;
extern int rd_freq;

extern double** Q;

//extern double compute_reward (int nodeid );


extern int get_action (int f, int sf, int tx);

extern void get_reward();

extern int sf_from_action(int action);

extern int tx_from_action(int action);

extern int chose_action(int nodeid);

extern double fr_from_action(int action);

extern void rlprocess(int nodeid, int rectype, uint8_t received_sf, double received_tx);

extern void release_freq(int freq, int nodeid);

extern void alloc_freq(int freq);

extern int sort_freq();

extern int rdaction();

} }



#endif
