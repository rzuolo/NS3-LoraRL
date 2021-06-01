/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Davide Magrin <magrinda@dei.unipd.it>
 */

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ns3/periodic-sender.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/lora-net-device.h"

//***********tests
#include "ns3/lora-packet-tracker.h"
#include "ns3/lora-helper.h"
#include "ns3/idxnodes.h"
#include "ns3/basic-energy-source-helper.h"
#include "ns3/lora-radio-energy-model-helper.h"
#include "ns3/device-energy-model.h"

//***********tests


//#include <typeinfo>
//#include  "/usr/include/python3.7/Python.h"

namespace ns3 {
namespace lorawan {


NS_LOG_COMPONENT_DEFINE ("PeriodicSender");

NS_OBJECT_ENSURE_REGISTERED (PeriodicSender);

TypeId
PeriodicSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PeriodicSender")
    .SetParent<Application> ()
    .AddConstructor<PeriodicSender> ()
    .SetGroupName ("lorawan")
    .AddAttribute ("Interval", "The interval between packet sends of this app",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&PeriodicSender::GetInterval,
                                     &PeriodicSender::SetInterval),
                   MakeTimeChecker ());
    //.AddAttribute ("PacketSizeRandomVariable", "The random variable that determines the shape of the packet size, in bytes",
    //              StringValue ("ns3::UniformRandomVariable[Min=0,Max=10]"),
    //              MakePointerAccessor (&PeriodicSender::m_pktSizeRV),
    //              MakePointerChecker <RandomVariableStream>());
  return tid;
}

PeriodicSender::PeriodicSender ()
  : m_interval (Seconds (10)),
  m_initialDelay (Seconds (1)),
  m_basePktSize (10),
  m_pktSizeRV (0)

{
  //*tests*****
  //NS_LOG_FUNCTION_NOARGS ();
  srand(clock());
  //NS_LOG_DEBUG ("Am I doing something here? " << clock() );
  //*tests*****
}

PeriodicSender::~PeriodicSender ()
{

}

void
PeriodicSender::SetInterval (Time interval)
{
 //*******************tests
	srand(clock());
//	interval = Time(rand()%600000000);
 //********************tests

  NS_LOG_FUNCTION (this << interval);
  m_interval = interval;
}

Time
PeriodicSender::GetInterval (void) const
{
  NS_LOG_FUNCTION (this);
  return m_interval;
}

void
PeriodicSender::SetInitialDelay (Time delay)
{
  NS_LOG_FUNCTION (this << delay);
  m_initialDelay = delay;
}


void
PeriodicSender::SetPacketSizeRandomVariable (Ptr <RandomVariableStream> rv)
{
  m_pktSizeRV = rv;
}


void
PeriodicSender::SetPacketSize (uint8_t size)
{
  m_basePktSize = size;
}


void
PeriodicSender::SendPacket (void)
{
  NS_LOG_FUNCTION (this);

  //*****tests******
  //int size = 20;
    int size = pkgbasesize;
  //******tests******



  // Create and send a new packet
  Ptr<Packet> packet;
  if (m_pktSizeRV)
    {
      int randomsize = m_pktSizeRV->GetInteger ();
      packet = Create<Packet> (m_basePktSize + randomsize);
    }
  else
    {

  	//*tests*****
  	//*tests*****
  	//*tests*****
  	//*tests*****
  	//*tests*****

	///int retv;
	//*************Option 1*************** Uses a dynamic Python with filestream pipe
	//int size = m_basePktSize + SchcGen();	
	//*************Option 2************** Uses a dynamic inline Python call by retv afore typed 
	//retv = system("cd /home/zuolo/src/my-schc/ && /usr/bin/python3 process-rules.py");
	//int size = m_basePktSize + (retv/256);
  	//************Option 3*************** Uses a static header of 80
	//int size = m_basePktSize + 80;
	//int size = m_basePktSize + 40;



	NS_LOG_DEBUG ("The header sizer will be "<< size );
	
	//before it was packet = Create<Packet> (m_basePktSize)
	//packet = Create<Packet> (m_basePktSize);
	
	//int rule_id = ((random()%8)+48);

    //int SizeP[6] = {57,12,19,12,22,32};
    //size = SizeP[(rand() % 6)];

     //size = 12;

	//NS_LOG_DEBUG ("The rule ID will be "<< SizeP[size]);
	//NS_LOG_DEBUG ("Length "<< std::to_string(size).length() );
	
	Packet::EnablePrinting();
	
	//uint8_t strg[std::to_string(rule_id).length()] = std::to_string(rule_id);
	//uint8_t strg[std::to_string(rule_id).length()] = { uint8_t(rule_id) } ;

	//uint8_t strg[std::to_string(SizeP[size]).length()] = { uint8_t(SizeP[size]) } ;
	
	//size=std::to_string((rule_id)-48).length() ;
	
	//uint8_t strg[size] = {0};
	//packet = Create<Packet> (strg, size);

	packet = Create<Packet> (size);
	//NS_LOG_DEBUG ("Printing the rule number "<< strg);
	
	std::ostream retrv(nullptr);
	std::stringbuf str;
    retrv.rdbuf(&str);
	
	//uint8_t resul;




	packet->CopyData(&retrv,packet->GetSize());
	//packet->CopyData(&resul,packet->GetSize());
	NS_LOG_INFO ("The package size will be "<< packet->GetSize() );
	NS_LOG_DEBUG ("The package content will be "<< str.str() );
	//NS_LOG_DEBUG ("The package content will be "<< resul );
	NS_LOG_DEBUG ("The package content will be "<< packet->ToString() );
	
	//packet = Create<Packet> (size);
	
  	//*tests*****
  	//*tests*****
  	//*tests*****
  	//*tests*****
    }



  m_mac->Send (packet);


  //*tests*****
  srand(clock());
  //std::cout<<"Now this is the node sending something ... " << m_node->GetId()<<std::endl;
  //srand(time(0));

  NS_LOG_INFO ("Current interval "<< m_interval );
  //std::cout<<"Current interval"<< m_interval<<std::endl;
  //m_interval = Time((rand()%180000000000))*1000;


 //switch (m_node->GetId()%3)
 // {
 // Random inter arrival packet
 // case 0:
 //m_interval = Time(uint32_t((((rand()%300)+10)*60)))*1000000000;
//	  break;
   // Exponential inter arrival packet
//  case 1:
     m_interval = ExpStep(m_interval, 2, Time("21600s"));
//	  break;
  // Fixed inter arrival period
//  case 2:
//  m_interval = Minutes(10);
//  }
  NS_LOG_INFO ("Proposed interval "<< m_interval );


  //std::cout<<"Next interval "<< m_interval.GetSeconds() << " s " << " Node #" << m_node->GetId() << std::endl;


  /*
  LoraHelper helper = LoraHelper ();
  helper.DoPrintGlobalPerformance;

  helper.EnablePacketTracking (); // Output filename
  LoraPacketTracker &tracker3 = helper.GetPacketTracker ();
  Time stop = Seconds (200);
  std::cout << tracker3.PrintPhyPacketsPerGw (Seconds (0), stop + Hours (1),700) << std::endl;
  */

  trialdevices[int(m_node->GetId())].last_size = size;
  trialdevices[int(m_node->GetId())].last_interval = uint64_t(m_interval.GetTimeStep());

   //std::cout << "this is the interval "<< m_interval <<std::endl;
  //std::cout<<"Now this is the node sending something ... " << m_node->GetId() <<" "<<trialdevices[int(m_node->GetId())].last_size<<" "<< trialdevices[int(m_node->GetId())].id << " success "<< trialdevices[int(m_node->GetId())].success << " fail " << trialdevices[int(m_node->GetId())].fail << std::endl;

  // Assumes there's only one device
  //Ptr<LoraNetDevice> loraNetDevice = m_node->GetDevice (0)->GetObject<LoraNetDevice> ();
 //Ptr<EndDeviceLoraPhy> loraPhy = loraNetDevice->GetPhy()->GetObject<EndDeviceLoraPhy> ();


  //*tests*****
  


  // Schedule the next SendPacket event
  m_sendEvent = Simulator::Schedule (m_interval, &PeriodicSender::SendPacket,
                                     this);
  


  NS_LOG_DEBUG ("Sent a packet of size " << packet->GetSize ());

}


//***************************************Tests	
//***************************************Tests	
//***************************************Tests	
int
PeriodicSender::SchcGen (void)
{
	std::string cmd="cd /home/zuolo/src/my-schc/ && /usr/bin/python3 process-rules.py";
	std::string data;
    	FILE * stream;
    	const int max_buffer = 256;
    	char buffer[max_buffer];
    	cmd.append(" 2>&1");

    	stream = popen(cmd.c_str(), "r");
    	if (stream) 
	{
      		while(!feof(stream))
		{
        		if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
          		pclose(stream);
      		}
	}

 return stoi(data);
}	



//***************************************Tests	
//***************************************Tests	
//***************************************Tests	
//***************************************Tests	
//***************************************Tests	

void
PeriodicSender::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // Make sure we have a MAC layer
  if (m_mac == 0)
    {
      // Assumes there's only one device
      Ptr<LoraNetDevice> loraNetDevice = m_node->GetDevice (0)->GetObject<LoraNetDevice> ();

      m_mac = loraNetDevice->GetMac ();
      NS_ASSERT (m_mac != 0);
    }

  // Schedule the next SendPacket event
  Simulator::Cancel (m_sendEvent);
  NS_LOG_DEBUG ("Starting up application with a first event with a " <<
                m_initialDelay.GetSeconds () << " seconds delay");
  m_sendEvent = Simulator::Schedule (m_initialDelay,
                                     &PeriodicSender::SendPacket, this);
  NS_LOG_DEBUG ("Event Id: " << m_sendEvent.GetUid ());
}

void
PeriodicSender::StopApplication (void)
{
  //NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}


//***** tests
Time
PeriodicSender::ExpStep(Time crt_interval, int power, Time threshold )
{

    double period = crt_interval.GetSeconds();

    if (period > 1) { period = period/1000; }
    period = exp(period)*1000;

    if (period > threshold.GetSeconds())
    {
    	srand(clock());
    	period = (rand()%1000);
    	//std::cout<<"trouble reset " << MilliSeconds(period)<<std::endl;
    	return crt_interval = MilliSeconds(period);
    }

    //std::cout<< "trouble "<< period<< std::endl;

    return crt_interval = Seconds(period) ;

    //*******************************tests
}

}
}
