/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 University of Padova
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
 * Author: Matteo Perin <matteo.perin.2@studenti.unipd.it
 */

#include "ns3/adr-component.h"


//************ tests
#include "ns3/idxnodes.h"
#include "ns3/lrmodel.h"
//*************  tests



namespace ns3 {
namespace lorawan {

////////////////////////////////////////
// LinkAdrRequest commands management //
////////////////////////////////////////

NS_LOG_COMPONENT_DEFINE ("AdrComponent");

NS_OBJECT_ENSURE_REGISTERED (AdrComponent);

TypeId AdrComponent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AdrComponent")
    .SetGroupName ("lorawan")
    .AddConstructor<AdrComponent> ()
    .SetParent<NetworkControllerComponent> ()
    .AddAttribute ("MultipleGwCombiningMethod",
                   "Whether to average the received power of gateways or to use the maximum",
                   EnumValue (AdrComponent::PLACEHOLDER),
		   MakeEnumAccessor (&AdrComponent::tpAveraging),
                   MakeEnumChecker (AdrComponent::AVERAGE,
                                    "avg",
                                    AdrComponent::MAXIMUM,
                                    "max",
                                    AdrComponent::MINIMUM,
                                    "min"))
    .AddAttribute ("MultiplePacketsCombiningMethod",
                   "Whether to average SNRs from multiple packets or to use the maximum",
                   EnumValue (AdrComponent::PLACEHOLDER),
                   MakeEnumAccessor (&AdrComponent::historyAveraging),
                   MakeEnumChecker (AdrComponent::AVERAGE,
                                    "avg",
                                    AdrComponent::MAXIMUM,
                                    "max",
                                    AdrComponent::MINIMUM,
                                    "min"))
    .AddAttribute ("HistoryRange",
                   "Number of packets to use for averaging",
                   IntegerValue (21),
                   MakeIntegerAccessor (&AdrComponent::historyRange),
                   MakeIntegerChecker<int> (0, 100))
    .AddAttribute ("ChangeTransmissionPower",
                   "Whether to toggle the transmission power or not",
                   BooleanValue (true),
                   MakeBooleanAccessor (&AdrComponent::m_toggleTxPower),
                   MakeBooleanChecker ())
  ;
  return tid;
}



//****** tests

int computeSteps(int currsteps)
{
	int steps;

	if (currsteps <= 0)
	{
		steps = 0;
	}
	else
	{
		if (currsteps >= 13)
		{
			steps = 13;
		}
		else
		{
			steps = currsteps ;
		}
	}
    return steps;
}

//****** tests



AdrComponent::AdrComponent ()
{
}

AdrComponent::~AdrComponent ()
{
}

void AdrComponent::OnReceivedPacket (Ptr<const Packet> packet,
                                     Ptr<EndDeviceStatus> status,
                                     Ptr<NetworkStatus> networkStatus)
{
  NS_LOG_FUNCTION (this->GetTypeId () << packet << networkStatus);

  // We will only act just before reply, when all Gateways will have received
  // the packet, since we need their respective received power.
}

void
AdrComponent::BeforeSendingReply (Ptr<EndDeviceStatus> status,
                                  Ptr<NetworkStatus> networkStatus)
{



  NS_LOG_FUNCTION (this << status << networkStatus);

  Ptr<Packet> myPacket = status->GetLastPacketReceivedFromDevice ()->Copy ();
  LorawanMacHeader mHdr;
  LoraFrameHeader fHdr;
  //fHdr.SetAsUplink ();
  myPacket->RemoveHeader (mHdr);
  myPacket->RemoveHeader (fHdr);

  //***** tests
  fHdr.SetAdr(true);
  int nodeid = status->GetMac()->GetDevice()->GetNode()->GetId();
  //bool nodeReqstatus = fHdr.GetAdrAckReq();

  if (rlagent == 1 or rlagent == 4)
  {
  //*obtaining the SNR of the last received package
   auto it = status->GetReceivedPacketList ().rbegin ();
   double curr_SNR = RxPowerToSNR (GetReceivedPower (it->second.gwList));
   uint8_t currspreadingFactor = status->GetFirstReceiveWindowSpreadingFactor ();
   double reqcurr_SNR = treshold[SfToDr (currspreadingFactor)];
   double margincurr_SNR = curr_SNR - reqcurr_SNR;
   //converting into real steps to be used
   int currsteps = floor (margincurr_SNR / 3);


    //Get the SF used by the device
    uint8_t received_spreadingFactor = unsigned(status->GetFirstReceiveWindowSpreadingFactor ());

    //Get the device transmission power (dBm)
    double received_transmissionPower = status->GetMac ()->GetTransmissionPower ();


   
   //std::cout<<"current in the air     "<< margincurr_SNR << " "<<freq_on_air[0] <<" "<< freq_on_air[1] <<" " << freq_on_air[2] << std::endl;
   //std::cout<<"Returning           " << sort_freq()<< std::endl;


   //std::cout<<"kicked in with " << unsigned(received_spreadingFactor)  << " TX  " <<  received_transmissionPower << std::endl; 
  
   //If this is the first packet being received
   if (trialdevices[nodeid].success == 1 and EPS != 0 )
   {
	   trialdevices[nodeid].instant_reward_ii = double(((float(trialdevices[nodeid].success)/(float(trialdevices[nodeid].success)+float(trialdevices[nodeid].fail)))*100)/(((float(trialdevices[nodeid].pwr_ii))*100)/float(global_power)));
	   trialdevices[nodeid].snr_ii=computeSteps(currsteps);
	   trialdevices[nodeid].action_ii = rdaction();
	   trialdevices[nodeid].mdp_state_ii = ((48*trialdevices[nodeid].snr_ii)+(8*((trialdevices[nodeid].state_ii[0])-7))+ (((16-received_transmissionPower)-2)/2));
	   //std::cout<<"Action selected is envio 1 "<< trialdevices[nodeid].action_ii << std::endl;
   }


   if (trialdevices[nodeid].success == 2 and EPS != 0 )
   {
	   trialdevices[nodeid].instant_reward_i = trialdevices[nodeid].instant_reward_ii;
	   trialdevices[nodeid].instant_reward_ii = double(((float(trialdevices[nodeid].success)/(float(trialdevices[nodeid].success)+float(trialdevices[nodeid].fail)))*100)/(((float(trialdevices[nodeid].pwr_ii))*100)/float(global_power)));
	   trialdevices[nodeid].snr_i=trialdevices[nodeid].snr_ii;
	   trialdevices[nodeid].snr_ii=computeSteps(currsteps);
	   //update Q for the first time with this dev
	   //Q[trialdevices[nodeid].mdp_state_ii][trialdevices[nodeid].action_ii]= trialdevices[nodeid].instant_reward_ii - trialdevices[nodeid].instant_reward_i;
	   trialdevices[nodeid].mdp_state_i = trialdevices[nodeid].mdp_state_ii;
	   trialdevices[nodeid].mdp_state_ii = ((48*trialdevices[nodeid].snr_ii)+(8*((trialdevices[nodeid].state_ii[0])-7))+ (((16-received_transmissionPower)-2)/2));
	   trialdevices[nodeid].action_i = trialdevices[nodeid].action_ii;
	   trialdevices[nodeid].action_ii = rdaction();
	   //std::cout<<"Action selected is envio 2 "<< trialdevices[nodeid].action_ii << std::endl;
   }


   if (trialdevices[nodeid].success > 2 or EPS == 0)
   {

	   trialdevices[nodeid].snr_i=trialdevices[nodeid].snr_ii;
	   trialdevices[nodeid].snr_ii=computeSteps(currsteps);
	   //update Q
	  
	   if (rlagent == 1 or rlagent == 4 )
	   { 
	   	rlprocess(nodeid,0,received_spreadingFactor,received_transmissionPower);
	   }
	   trialdevices[nodeid].mdp_state_i = trialdevices[nodeid].mdp_state_ii;
	   trialdevices[nodeid].mdp_state_ii = ((48*trialdevices[nodeid].snr_ii)+(8*((trialdevices[nodeid].state_ii[0])-7))+(((16-received_transmissionPower)-2)/2));
	
	   //std::cout<<"STATE "<<trialdevices[nodeid].mdp_state_ii<<" "<<((received_transmissionPower-2)/2)<<" "<< received_transmissionPower <<std::endl;


	   trialdevices[nodeid].action_i = trialdevices[nodeid].action_ii;
	   //chose the best action for the next sending
	   trialdevices[nodeid].action_ii = chose_action(nodeid);
	   //std::cout<<"Action selected is "<< trialdevices[nodeid].action_ii << std::endl;
	   //std::cout<<"Which is the freq "<< fr_from_action(trialdevices[nodeid].action_ii) << std::endl;

   }

	  //std::cout << "Nodeid "<< trialdevices[nodeid].nwkaddr <<"Going from SF " <<  sf_from_action(trialdevices[nodeid].action_i) << " to "<<  sf_from_action(trialdevices[nodeid].action_ii) <<" TxPower from " << tx_from_action(trialdevices[nodeid].action_i) << " to " << tx_from_action(trialdevices[nodeid].action_ii) << " action "<< trialdevices[nodeid].action_i<<" " << trialdevices[nodeid].action_ii <<std::endl;
	  //std::cout << "Going from " << trialdevices[nodeid].action_i << " to "<< trialdevices[nodeid].action_ii  << std::endl;





   if (trialdevices[nodeid].success == 0)
   {
	   std::cout<<"Panic. Something went wrong"<<std::endl;
   }



  // frequencies allocated by index
  // 868.1 is 0
  // 868.3 is 1
  // 868.5 is 2
  /*
  std::cout<<" trialdevices[nodeid].action_ii " << trialdevices[nodeid].action_ii << std::endl;
	if ( fr_from_action(trialdevices[nodeid].action_ii) == 868.1)
   {
 	   std::cout<<" allocating 868.1 "<<std::endl;
  	   alloc_freq(0);
   }
	else
   {
		if (fr_from_action(trialdevices[nodeid].action_ii) == 868.3)
		{
 	   		std::cout<<" allocating 868.3 "<<std::endl;
			alloc_freq(1);
		}
		else
		{
 	   		std::cout<<" allocating 868.5 "<<std::endl;
			alloc_freq(2);
		}
	}
   */

  }//Finish the entire RL processing loop



  //Execute the ADR algorithm only if the request bit is set
 // if (fHdr.GetAdr ()) changing this to be compatible with the rl implementation
 if ((fHdr.GetAdr ()) and (rlagent == 0)) //***** tests
 {


	  if (int(status->GetReceivedPacketList ().size ()) < historyRange)
        {
          NS_LOG_ERROR ("Not enough packets received by this device (" << status->GetReceivedPacketList ().size () << ") for the algorithm to work (need " << historyRange << ")");


          //****** tests
           //std::cout << "Not enough packets for the node " << nodeid << " "<< status->GetReceivedPacketList ().size ()<< std::endl;
          //****** tests

        }
      else
        {
          NS_LOG_DEBUG ("New ADR request");

          //Get the SF used by the device
          uint8_t spreadingFactor = status->GetFirstReceiveWindowSpreadingFactor ();

          //Get the device transmission power (dBm)
          uint8_t transmissionPower = status->GetMac ()->GetTransmissionPower ();

          
	  //New parameters for the end-device
          uint8_t newDataRate;
          uint8_t newTxPower;

          //ADR Algorithm
          AdrImplementation (&newDataRate,
                             &newTxPower,
                             status);


          //**** tests

          //ADRLR(int(spreadingFactor),int(GetIndexTxPower(int(newTxPower))));

          //std::cout << "Node " << nodeid << " ADR "<<  fHdr.GetAdr() << " Req " << nodeReqstatus <<std::endl;
          //std::cout << "Going from SF nodeid "<< status->GetMac()->GetDevice()->GetNode()->GetId() <<" "<< int(spreadingFactor) << " to "<<  int(12-int(newDataRate)) << " TxPower from " << int(GetIndexTxPower(int(transmissionPower))) << " to " << int(GetIndexTxPower(int(newTxPower))) << std::endl;
          //trialdevices[nodeid].state_ii[0]=int(12-int(newDataRate));
          //trialdevices[nodeid].state_ii[1]=int(GetIndexTxPower(int(newTxPower)));

          //*obtaining the SNR of the last received package
          //auto it = status->GetReceivedPacketList ().rbegin ();
          //double curr_SNR = RxPowerToSNR (GetReceivedPower (it->second.gwList));
          //uint8_t currspreadingFactor = status->GetFirstReceiveWindowSpreadingFactor ();
          //double reqcurr_SNR = treshold[SfToDr (currspreadingFactor)];
          //double margincurr_SNR = curr_SNR - reqcurr_SNR;

          //std::cout<<"SNR Curr "<<curr_SNR<<" "<<reqcurr_SNR<<" "<<margincurr_SNR<<" "<<floor(margincurr_SNR / 3) <<std::endl	;

          //int currsteps = floor (margincurr_SNR / 3);
          //std::cout<<"SNR Received "<<int(currsteps)<<std::endl;;
   	  //std::cout<<"I am coming with the SNR-steps "<< currsteps<<std::endl;




          //**** tests


          // Change the power back to the default if we don't want to change it
          if (!m_toggleTxPower)
            {
              newTxPower = transmissionPower;
            }

          if (newDataRate != SfToDr (spreadingFactor) || newTxPower != transmissionPower)
            {
              //Create a list with mandatory channel indexes
              int channels[] = {0, 1, 2};
              std::list<int> enabledChannels (channels,
                                              channels + sizeof(channels) /
                                              sizeof(int));

              //Repetitions Setting
              const int rep = 1;

              NS_LOG_DEBUG ("Sending LinkAdrReq with DR = " << (unsigned)newDataRate << " and TP = " << (unsigned)newTxPower << " dBm");

              status->m_reply.frameHeader.AddLinkAdrReq (newDataRate,
                                                         GetTxPowerIndex (newTxPower),
                                                         enabledChannels,
                                                         rep);
              status->m_reply.frameHeader.SetAsDownlink ();
              status->m_reply.macHeader.SetMType (LorawanMacHeader::UNCONFIRMED_DATA_DOWN);

              status->m_reply.needsReply = true;
            }
          else
            {
              NS_LOG_DEBUG ("Skipped request");
            }
        }
    }
  else
    {
      // Do nothing
    }
}

void AdrComponent::OnFailedReply (Ptr<EndDeviceStatus> status,
                                  Ptr<NetworkStatus> networkStatus)
{
  NS_LOG_FUNCTION (this->GetTypeId () << networkStatus);
}

void AdrComponent::AdrImplementation (uint8_t *newDataRate,
                                      uint8_t *newTxPower,
                                      Ptr<EndDeviceStatus> status)
{

	   //***** tests
	  // std::cout<<"************ Real ADR  *******************"<<std::endl;
     	  //***** tests

   //Compute the maximum or median SNR, based on the boolean value historyAveraging
  double m_SNR = 0;
  switch (historyAveraging)
    {
    case AdrComponent::AVERAGE:
      m_SNR = GetAverageSNR (status->GetReceivedPacketList (),
                             historyRange);
      break;
    case AdrComponent::MAXIMUM:
      m_SNR = GetMaxSNR (status->GetReceivedPacketList (),
                         historyRange);
      break;
    case AdrComponent::MINIMUM:
      m_SNR = GetMinSNR (status->GetReceivedPacketList (),
                         historyRange);
    }

  NS_LOG_DEBUG ("m_SNR = " << m_SNR);
  //** tests
  //std::cout<<"The SNR starts with this " << m_SNR <<std::endl;
  //** tests

  //Get the SF used by the device
  uint8_t spreadingFactor = status->GetFirstReceiveWindowSpreadingFactor ();

  NS_LOG_DEBUG ("SF = " << (unsigned)spreadingFactor);

  //Get the device data rate and use it to get the SNR demodulation treshold
  double req_SNR = treshold[SfToDr (spreadingFactor)];

  NS_LOG_DEBUG ("Required SNR = " << req_SNR);

  //Get the device transmission power (dBm)
  double transmissionPower = status->GetMac ()->GetTransmissionPower ();

  NS_LOG_DEBUG ("Transmission Power = " << transmissionPower);

  //Compute the SNR margin taking into consideration the SNR of
  //previously received packets
  double margin_SNR = m_SNR - req_SNR;

  NS_LOG_DEBUG ("Margin = " << margin_SNR);

  //Number of steps to decrement the SF (thereby increasing the Data Rate)
  //and the TP.
  int steps = std::floor (margin_SNR / 3);

  NS_LOG_DEBUG ("steps = " << steps);

  //If the number of steps is positive (margin_SNR is positive, so its
  //decimal value is high) increment the data rate, if there are some
  //leftover steps after reaching the maximum possible data rate
  //(corresponding to the minimum SF) decrement the transmission power as
  //well for the number of steps left.
  //If, on the other hand, the number of steps is negative (margin_SNR is
  //negative, so its decimal value is low) increase the transmission power
  //(note that the SF is not incremented as this particular algorithm
  //expects the node itself to raise its SF whenever necessary).
  while (steps > 0 && spreadingFactor > min_spreadingFactor)
    {

      //***** tests
       //std::cout<<"Steps "<< steps <<std::endl;
       // std::cout<<"spreadingFactor "<< (unsigned)spreadingFactor <<std::endl;
       // std::cout<<"spreadingFactor "<< status->GetFirstReceiveWindowSpreadingFactor() <<std::endl;
        //***** tests

	  spreadingFactor--;
      steps--;
      NS_LOG_DEBUG ("Decreased SF by 1");

      //***** tests
        //std::cout<<"Steps "<< steps <<std::endl;
        //std::cout<<"spreadingFactor "<< (unsigned)spreadingFactor <<std::endl;
        //std::cout<<"spreadingFactor "<< status->GetFirstReceiveWindowSpreadingFactor() <<std::endl;
        //***** tests
    }
  while (steps > 0 && transmissionPower > min_transmissionPower)
    {
	  	  	  //***** tests
	          // std::cout<<"Steps "<< steps <<std::endl;
	          // std::cout<<"transmissionPower "<< transmissionPower <<std::endl;
	          //***** tests

	  transmissionPower -= 2;
      steps--;
      NS_LOG_DEBUG ("Decreased Ptx by 2");
      //***** tests
      //   std::cout<<"Steps "<< steps <<std::endl;
      //   std::cout<<"transmissionPower "<< transmissionPower <<std::endl;
        //***** tests
    }
  while (steps < 0 && transmissionPower < max_transmissionPower)
    {
        //***** tests
      //  std::cout<<"Steps "<< steps <<std::endl;
     //    std::cout<<"transmissionPower "<< transmissionPower <<std::endl;
        //***** tests

	  transmissionPower += 2;
      steps++;
      NS_LOG_DEBUG ("Increased Ptx by 2");
      //***** tests
       //  std::cout<<"Steps "<< steps <<std::endl;
      //   std::cout<<"transmissionPower "<< transmissionPower <<std::endl;
        //***** tests
     }


  	  	  //***** tests
           //std::cout<<"Steps "<< steps <<std::endl;
           //std::cout<<"transmissionPower "<< transmissionPower << "new data rate "<<  SfToDr (spreadingFactor) <<std::endl;
          //***** tests


  *newDataRate = SfToDr (spreadingFactor);
  *newTxPower = transmissionPower;



}

uint8_t AdrComponent::SfToDr (uint8_t sf)
{
  switch (sf)
    {
    case 12:
      return 0;
      break;
    case 11:
      return 1;
      break;
    case 10:
      return 2;
      break;
    case 9:
      return 3;
      break;
    case 8:
      return 4;
      break;
    default:
      return 5;
      break;
    }
}

double AdrComponent::RxPowerToSNR (double transmissionPower)
{
  //The following conversion ignores interfering packets
  return transmissionPower + 174 - 10 * log10 (B) - NF;
}

//Get the maximum received power (it considers the values in dB!)
double AdrComponent::GetMinTxFromGateways (EndDeviceStatus::GatewayList gwList)
{
  EndDeviceStatus::GatewayList::iterator it = gwList.begin ();
  double min = it->second.rxPower;

  for (; it != gwList.end (); it++)
    {
      if (it->second.rxPower < min)
        {
          min = it->second.rxPower;
        }
    }

  return min;
}

//Get the maximum received power (it considers the values in dB!)
double AdrComponent::GetMaxTxFromGateways (EndDeviceStatus::GatewayList gwList)
{
  EndDeviceStatus::GatewayList::iterator it = gwList.begin ();
  double max = it->second.rxPower;

  for (; it != gwList.end (); it++)
    {
      if (it->second.rxPower > max)
        {
          max = it->second.rxPower;
        }
    }

  return max;
}

//Get the maximum received power
double AdrComponent::GetAverageTxFromGateways (EndDeviceStatus::GatewayList gwList)
{
  double sum = 0;

  for (EndDeviceStatus::GatewayList::iterator it = gwList.begin (); it != gwList.end (); it++)
    {
      NS_LOG_DEBUG ("Gateway at " << it->first << " has TP " << it->second.rxPower);
      sum += it->second.rxPower;
    }

  double average = sum / gwList.size ();

  NS_LOG_DEBUG ("TP (average) = " << average);

  return average;
}

double
AdrComponent::GetReceivedPower (EndDeviceStatus::GatewayList gwList)
{
  switch (tpAveraging)
    {
    case AdrComponent::AVERAGE:
      return GetAverageTxFromGateways (gwList);
    case AdrComponent::MAXIMUM:
      return GetMaxTxFromGateways (gwList);
    case AdrComponent::MINIMUM:
      return GetMinTxFromGateways (gwList);
    default:
      return -1;
    }
}

// TODO Make this more elegant
double AdrComponent::GetMinSNR (EndDeviceStatus::ReceivedPacketList packetList,
                                int historyRange)
{
  double m_SNR;

  //Take elements from the list starting at the end
  auto it = packetList.rbegin ();
  double min = RxPowerToSNR (GetReceivedPower (it->second.gwList));

  for (int i = 0; i < historyRange; i++, it++)
    {
      m_SNR = RxPowerToSNR (GetReceivedPower (it->second.gwList));

      NS_LOG_DEBUG ("Received power: " << GetReceivedPower (it->second.gwList));
      NS_LOG_DEBUG ("m_SNR = " << m_SNR);

      if (m_SNR < min)
        {
          min = m_SNR;
        }
    }

  NS_LOG_DEBUG ("SNR (min) = " << min);

  return min;
}

double AdrComponent::GetMaxSNR (EndDeviceStatus::ReceivedPacketList packetList,
                                int historyRange)
{
  double m_SNR;

  //Take elements from the list starting at the end
  auto it = packetList.rbegin ();
  double max = RxPowerToSNR (GetReceivedPower (it->second.gwList));

  for (int i = 0; i < historyRange; i++, it++)
    {
      m_SNR = RxPowerToSNR (GetReceivedPower (it->second.gwList));

      NS_LOG_DEBUG ("Received power: " << GetReceivedPower (it->second.gwList));
      NS_LOG_DEBUG ("m_SNR = " << m_SNR);

      if (m_SNR > max)
        {
          max = m_SNR;
        }
    }

  NS_LOG_DEBUG ("SNR (max) = " << max);

  return max;
}

double AdrComponent::GetAverageSNR (EndDeviceStatus::ReceivedPacketList packetList,
                                    int historyRange)
{
  double sum = 0;
  double m_SNR;

  //Take elements from the list starting at the end
  auto it = packetList.rbegin ();
  for (int i = 0; i < historyRange; i++, it++)
    {
      m_SNR = RxPowerToSNR (GetReceivedPower (it->second.gwList));

      NS_LOG_DEBUG ("Received power: " << GetReceivedPower (it->second.gwList));
      NS_LOG_DEBUG ("m_SNR = " << m_SNR);

      sum += m_SNR;
    }

  double average = sum / historyRange;

  NS_LOG_DEBUG ("SNR (average) = " << average);

  return average;
}

int AdrComponent::GetTxPowerIndex (int txPower)
{
  if (txPower >= 16)
    {
      return 0;
    }
  else if (txPower >= 14)
    {
      return 1;
    }
  else if (txPower >= 12)
    {
      return 2;
    }
  else if (txPower >= 10)
    {
      return 3;
    }
  else if (txPower >= 8)
    {
      return 4;
    }
  else if (txPower >= 6)
    {
      return 5;
    }
  else if (txPower >= 4)
    {
      return 6;
    }
  else
    {
      return 7;
    }
}

//// *******************tests
int AdrComponent::GetIndexTxPower (int index)
{
  if (index == 0)
    {
      return 16;
    }
  else if (index == 1)
    {
      return 14;
    }
  else if (index ==  2)
    {
      return 12;
    }
  else if (index ==  3)
    {
      return 10;
    }
  else if (index ==  4)
    {
      return 8;
    }
  else if (index ==  5)
    {
      return 6;
    }
  else if (index ==  6)
    {
      return 4;
    }
  else
    {
      return 2;
    }
}


int AdrComponent::ADRLR (int SF, int step)
{

	//retv = system("cd /home/zuolo/src/my-schc/ && /usr/bin/python3 process-rules.py");
	//int size = m_basePktSize + (retv/256);

	std::string sf = std::to_string(int(SF));
	std::string st = std::to_string(int(step));

	std::string cmd = "cd /home/zuolo//eclipse-workspace/ns-3/src/lorawan/adr-rl && /usr/bin/python3 lora-game.py " + sf + " " + st;
	return system(cmd.c_str());
	//std::cout << "saida "<< size<<std::endl;
	//int size = (retv/256);

	//std::cout<<" Saida " << retv << std::endl;



}






//// *******************tests

}
}
