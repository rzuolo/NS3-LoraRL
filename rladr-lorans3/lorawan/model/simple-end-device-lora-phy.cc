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

#include <algorithm>
#include "ns3/simple-end-device-lora-phy.h"
#include "ns3/simulator.h"
#include "ns3/lora-tag.h"
#include "ns3/log.h"

//##########tests
#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/idxnodes.h"
#include "ns3/lrmodel.h"
//##########tests


namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("SimpleEndDeviceLoraPhy");

NS_OBJECT_ENSURE_REGISTERED (SimpleEndDeviceLoraPhy);

TypeId
SimpleEndDeviceLoraPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleEndDeviceLoraPhy")
    .SetParent<EndDeviceLoraPhy> ()
    .SetGroupName ("lorawan")
    .AddConstructor<SimpleEndDeviceLoraPhy> ();

  return tid;
}

// Initialize the device with some common settings.
// These will then be changed by helpers.
SimpleEndDeviceLoraPhy::SimpleEndDeviceLoraPhy ()
{
}

SimpleEndDeviceLoraPhy::~SimpleEndDeviceLoraPhy ()
{
}


///**************tests
//Function to print a follow-up status of transmissions and performance

void ScoreBoard()
{
	std::system ("clear");
	std::cout << "##########################"<<std::endl;
	std::cout << "\033[1;33mPower:\t"<< global_power <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;33mEpsilon:\t"<< EPS <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;33mDevices:\t"<< nDevices <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;33mAvg Distance:\t"<< avgDistance <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;31mFailed:\t\t"<< int (global_sent - global_received) <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;34mSucceeded:\t"<< int (global_received) <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;32mGoodput:\t"<< float(global_received)/float(global_sent) <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;
	std::cout << "\033[1;32mTime Elapsed:\t"<< Simulator::Now () <<std::endl;
	std::cout << "\033[0m##########################"<<std::endl;


}
///

void
SimpleEndDeviceLoraPhy::Send (Ptr<Packet> packet, LoraTxParameters txParams,
                              double frequencyMHz, double txPowerDbm)
{
  NS_LOG_FUNCTION (this << packet << txParams << frequencyMHz << txPowerDbm);

  NS_LOG_INFO ("Current state: " << m_state);



  //**** tests


  trialdevices[m_device->GetNode ()->GetId ()].pkg_on_air++ ;



  if ( rlagent == 0 or trialdevices[m_device->GetNode ()->GetId ()].success == 0 or trialdevices[m_device->GetNode()->GetId()].previous_attempt == false )
  {
	 

	  //int SFP[6] = {7,8,9,10,11,12};
  	  //double FCP[3] = {868.1,868.3,868.5};
  	  //int TXP[8] = {2, 4, 6, 8, 10, 12, 14, 16};
        
	//if (trialdevices[m_device->GetNode ()->GetId ()].pkg_on_air < 2 )
	//{
	//  txParams.sf = 7; 
        //  txPowerDbm =  2;
	//}
        //frequencyMHz =  868.1;

          trialdevices[m_device->GetNode ()->GetId ()].state_ii[0] = txParams.sf;
	  trialdevices[m_device->GetNode ()->GetId ()].state_ii[1] = 16 - txPowerDbm ;

	  
	  //trialdevices[m_device->GetNode ()->GetId ()].state_ii[2] = ((uint32_t(frequencyMHz*10))-8680) ;

	  if (rlagent == 1 or rlagent == 4)
	  {
	   //std::cout<<"Attention: A random selection will be performed"<<std::endl;
	   double FCP[3] = {868.1,868.3,868.5};
	   frequencyMHz = FCP[(rdaction() % 3)];
	   trialdevices[m_device->GetNode ()->GetId ()].state_ii[2]=((uint32_t(frequencyMHz*10))-8680) ;
	  }
	  else if ( (trialdevices[m_device->GetNode ()->GetId ()].success + trialdevices[m_device->GetNode ()->GetId ()].fail) < 21 )
	 {
	
	  	txParams.sf = 7; 
          	txPowerDbm =  14;

	 }
   }
  else
  {   //receive tx and sf from the action selected on the gateway


	if ( rlagent == 3)
        {
	  	txParams.sf = 7; 
          	txPowerDbm =  2;

            //int SFP[6] = {7,8,9,10,11,12};
            //txParams.sf = SFP[(rand() % 6)];
            double FCP[3] = {868.1,868.3,868.5};
            frequencyMHz = FCP[(rand() % 3)];
            //int TXP[8] = {16, 14, 12, 10, 8, 6, 4, 2};
            //txPowerDbm = TXP[(rand() % 8)];

        }
        else
        {
	  trialdevices[m_device->GetNode ()->GetId ()].state_i[0] = trialdevices[m_device->GetNode ()->GetId ()].state_ii[0];
          trialdevices[m_device->GetNode ()->GetId ()].state_i[1] = trialdevices[m_device->GetNode ()->GetId ()].state_ii[1];
	  trialdevices[m_device->GetNode ()->GetId ()].state_i[2] = trialdevices[m_device->GetNode ()->GetId ()].state_ii[2];
	  trialdevices[m_device->GetNode ()->GetId ()].state_ii[0] = sf_from_action(trialdevices[m_device->GetNode ()->GetId ()].action_ii) ;
	  trialdevices[m_device->GetNode ()->GetId ()].state_ii[1] = tx_from_action(trialdevices[m_device->GetNode ()->GetId ()].action_ii);
	  trialdevices[m_device->GetNode ()->GetId ()].state_ii[2] = ((int((fr_from_action(trialdevices[m_device->GetNode ()->GetId ()].action_ii))*10))-8680);

	  txParams.sf = trialdevices[m_device->GetNode ()->GetId ()].state_ii[0];
	  frequencyMHz = ((double(trialdevices[m_device->GetNode ()->GetId ()].state_ii[2])/10)+868.0);
	  txPowerDbm = 16 - trialdevices[m_device->GetNode ()->GetId ()].state_ii[1];
	    

	  //txParams.sf = 7; 
	  //txPowerDbm  = 2; 
	}  
}

  // frequencies allocated by index
  // 868.1 is 0
  // 868.3 is 1
  // 868.5 is 2
  //std::cout<<"\n\n\n\n\n";
  //std::cout<<" frequency selected to be sent " << frequencyMHz << std::endl;
        if ( frequencyMHz == 868.1)
   {
           //std::cout<<" allocating 868.1 before "<< freq_on_air[0] <<std::endl;
           alloc_freq(0);
           //std::cout<<" allocating 868.1 after "<< freq_on_air[0] <<std::endl;
   }
        else
   {
                if (frequencyMHz == 868.3)
                {
           		//std::cout<<" allocating 868.3 before "<< freq_on_air[1] <<std::endl;
                        alloc_freq(1);
           		//std::cout<<" allocating 868.3 after "<< freq_on_air[1] <<std::endl;
                }
                else
                {
           		//std::cout<<" allocating 868.5 before"<< freq_on_air[2] <<std::endl;
                        alloc_freq(2);
           		//std::cout<<" allocating 868.5 after"<< freq_on_air[2] <<std::endl;
                }
    }


   //std::system ("clear");

   //ScoreBoard();
  //txParams.sf = 12;
  //frequencyMHz = 868.3;
  //txPowerDbm = 16;

  //**** tests



  // We must be either in STANDBY or SLEEP mode to send a packet
  if (m_state != STANDBY && m_state != SLEEP)
    {
      NS_LOG_INFO ("Cannot send because device is currently not in STANDBY or SLEEP mode");
      return;
    }

  // Compute the duration of the transmission
  Time duration = GetOnAirTime (packet, txParams);


  // We can send the packet: switch to the TX state
  SwitchToTx (txPowerDbm);

  // Tag the packet with information about its Spreading Factor
  LoraTag tag;
  packet->RemovePacketTag (tag);
  tag.SetSpreadingFactor (txParams.sf);
  packet->AddPacketTag (tag);


  // Send the packet over the channel
  NS_LOG_INFO ("Sending the packet in the channel");
  m_channel->Send (this, packet, txPowerDbm, txParams, duration, frequencyMHz);

  //****tests
  //std::cout<<"Sending TxPwr "<< txPowerDbm << " "<<int(txParams.sf) << " "<< frequencyMHz<<std::endl;
  //****tests
  //std::cout<<"Sending "<< trialdevices[m_device->GetNode ()->GetId ()].state_ii[1] << " "<<trialdevices[m_device->GetNode ()->GetId ()].state_ii[0] << " "<< trialdevices[m_device->GetNode ()->GetId ()].state_ii[2]<<std::endl;

       //trialdevices[m_device->GetNode ()->GetId ()].success_rate = (trialdevices[m_device->GetNode ()->GetId ()].success *100) /(trialdevices[m_device->GetNode ()->GetId ()].fail + trialdevices[m_device->GetNode ()->GetId ()].success);

       //std::cout<<"Statistics"<< trialdevices[m_device->GetNode ()->GetId ()].success << ","<<trialdevices[m_device->GetNode ()->GetId ()].fail<<std::endl;


  // Schedule the switch back to STANDBY mode.
  // For reference see SX1272 datasheet, section 4.1.6
  Simulator::Schedule (duration, &EndDeviceLoraPhy::SwitchToStandby, this);

  // Schedule the txFinished callback, if it was set
  // The call is scheduled just after the switch to standby in case the upper
  // layer wishes to change the state. This ensures that it will find a PHY in
  // STANDBY mode.
  if (!m_txFinishedCallback.IsNull ())
    {
      Simulator::Schedule (duration + NanoSeconds (10),
                           &SimpleEndDeviceLoraPhy::m_txFinishedCallback, this,
                           packet);
    }


  // Call the trace source
  if (m_device)
    {
      m_startSending (packet, m_device->GetNode ()->GetId ());
    }
  else
    {
      m_startSending (packet, 0);
    }


        //**************************tests
  	  	global_sent+=1;
        /*NS_LOG_DEBUG ("Starting Header Removal at SimplEndLoraPhy Class");   
        Ptr<Packet> tempkt = packet->Copy(); 
        NS_LOG_DEBUG ("Copy of Packet: " << tempkt);
        NS_LOG_DEBUG ("Original Packet: " << packet);
        LorawanMacHeader macHdr;
        tempkt->RemoveHeader (macHdr);
        LoraFrameHeader frameHdr;
        tempkt->RemoveHeader (frameHdr);
        std::ostream retrv(nullptr);
        std::stringbuf str;
        retrv.rdbuf(&str);
        tempkt->CopyData(&retrv,tempkt->GetSize());
        NS_LOG_DEBUG ("################Size as of SimpleEndLoraPhy Class #####################  "<< tempkt->GetSize() );
        NS_LOG_DEBUG ("################Content as of SimpleEndLoraPhy Class ####################  "<< str.str() );
        */
     	//ApplyNecessaryOptions (frameHdr);
        //tempkt->AddHeader (frameHdr);
        //ApplyNecessaryOptions (macHdr);
        //tempkt->AddHeader (macHdr);

  	  	//std::cout<<"Printing on sending"<<std::endl;
  	    //Flashout(m_device->GetNode ()->GetId ());
  	  	//std::getchar();
        //**************************tests


}

void
SimpleEndDeviceLoraPhy::StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                                      uint8_t sf, Time duration, double frequencyMHz)
{

  NS_LOG_FUNCTION (this << packet << rxPowerDbm << unsigned (sf) << duration <<
                   frequencyMHz);

  
  
  
  
        //**************************tests
       /* NS_LOG_DEBUG ("Starting Header Removal at SimplEndLoraPhy StartReceive Class");   
        Ptr<Packet> tempkt = packet->Copy(); 
        NS_LOG_DEBUG ("Copy of Packet: " << tempkt);
        NS_LOG_DEBUG ("Original Packet: " << packet);
        LorawanMacHeader macHdr;
        tempkt->RemoveHeader (macHdr);
        LoraFrameHeader frameHdr;
        tempkt->RemoveHeader (frameHdr);
        std::ostream retrv(nullptr);
        std::stringbuf str;
        retrv.rdbuf(&str);
        tempkt->CopyData(&retrv,tempkt->GetSize());
        NS_LOG_DEBUG ("################Size as of SimpleEndLoraPhy StartReceive Class #####################  "<< tempkt->GetSize() );
        NS_LOG_DEBUG ("################Content as of SimpleEndLoraPhy StartReceive Class ####################  "<< str.str() );
        */

  	  	//std::cout<< "O device recebendo é este . . . :" << m_device->GetNode ()->GetId () <<std::endl;



        //ApplyNecessaryOptions (frameHdr);
        //tempkt->AddHeader (frameHdr);
        //ApplyNecessaryOptions (macHdr);
        //tempkt->AddHeader (macHdr);
        //**************************tests


  
  // Notify the LoraInterferenceHelper of the impinging signal, and remember
  // the event it creates. This will be used then to correctly handle the end
  // of reception event.
  //
  // We need to do this regardless of our state or frequency, since these could
  // change (and making the interference relevant) while the interference is
  // still incoming.

  Ptr<LoraInterferenceHelper::Event> event;
  event = m_interference.Add (duration, rxPowerDbm, sf, packet, frequencyMHz);

  //****************tests
   //std::cout<<"duration "<< duration <<"sf "<< int(sf) << "rxpower "<<  rxPowerDbm <<" frequency "<< frequencyMHz << std::endl;
   //event = m_interference.Add ( Time(1000000000000), double(-200.0), 8, packet, 868.3);
   //event = m_interference.Add ( Time(1000000000000), double(-200.0), 8, packet, 868.1);
   //event = m_interference.Add ( Time(1000000000000), double(-200.0), 8, packet, 868.5);

   //*****************tests

  // Switch on the current PHY state
  switch (m_state)
    {
    // In the SLEEP, TX and RX cases we cannot receive the packet: we only add
    // it to the list of interferers and do not schedule an EndReceive event for
    // it.
    case SLEEP:
      {
        NS_LOG_INFO ("Dropping packet because device is in SLEEP state");
        break;
      }
    case TX:
      {
        NS_LOG_INFO ("Dropping packet because device is in TX state");
        break;
      }
    case RX:
      {
        NS_LOG_INFO ("Dropping packet because device is already in RX state");
        break;
      }
    // If we are in STANDBY mode, we can potentially lock on the currently
    // incoming transmission
    case STANDBY:
      {
        // There are a series of properties the packet needs to respect in order
        // for us to be able to lock on it:
        // - It's on frequency we are listening on
        // - It uses the SF we are configured to look for
        // - Its receive power is above the device sensitivity for that SF

        // Flag to signal whether we can receive the packet or not
        bool canLockOnPacket = true;

        // Save needed sensitivity
        double sensitivity = EndDeviceLoraPhy::sensitivity[unsigned(sf) - 7];

        // Check frequency
        //////////////////
        if (!IsOnFrequency (frequencyMHz))
          {
            NS_LOG_INFO ("Packet lost because it's on frequency " <<
                         frequencyMHz << " MHz and we are listening at " <<
                         m_frequency << " MHz");

            // Fire the trace source for this event.
            if (m_device)
              {
                m_wrongFrequency (packet, m_device->GetNode ()->GetId ());
              }
            else
              {
                m_wrongFrequency (packet, 0);
              }

            canLockOnPacket = false;
          }

        // Check Spreading Factor
        /////////////////////////
        if (sf != m_sf)
          {
            NS_LOG_INFO ("Packet lost because it's using SF" << unsigned(sf) <<
                         ", while we are listening for SF" << unsigned(m_sf));

            // Fire the trace source for this event.
            if (m_device)
              {
                m_wrongSf (packet, m_device->GetNode ()->GetId ());
              }
            else
              {
                m_wrongSf (packet, 0);
              }

            canLockOnPacket = false;
          }

        // Check Sensitivity
        ////////////////////
        if (rxPowerDbm < sensitivity)
          {
            NS_LOG_INFO ("Dropping packet reception of packet with sf = " <<
                         unsigned(sf) << " because under the sensitivity of " <<
                         sensitivity << " dBm");

            // Fire the trace source for this event.
            if (m_device)
              {
                m_underSensitivity (packet, m_device->GetNode ()->GetId ());
              }
            else
              {
                m_underSensitivity (packet, 0);
              }

            canLockOnPacket = false;
          }

        // Check if one of the above failed
        ///////////////////////////////////
        if (canLockOnPacket)
          {
            // Switch to RX state
            // EndReceive will handle the switch back to STANDBY state
            SwitchToRx ();

            // Schedule the end of the reception of the packet
            NS_LOG_INFO ("Scheduling reception of a packet. End in " <<
                         duration.GetSeconds () << " seconds");

            Simulator::Schedule (duration, &LoraPhy::EndReceive, this, packet,
                                 event);

            // Fire the beginning of reception trace source
            m_phyRxBeginTrace (packet);
          }
      }
    }

}

void
SimpleEndDeviceLoraPhy::EndReceive (Ptr<Packet> packet,
                                    Ptr<LoraInterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << packet << event);

  // Automatically switch to Standby in either case
  SwitchToStandby ();

  // Fire the trace source
  m_phyRxEndTrace (packet);

  //*********************tests
  //std::cout<<"Testing interference on device "<< m_device->GetNode ()->GetId ()<<std::endl;
  //*********************tests
  // Call the LoraInterferenceHelper to determine whether there was destructive
  // interference on this event.
  bool packetDestroyed = m_interference.IsDestroyedByInterference (event);

  // Fire the trace source if packet was destroyed
  if (packetDestroyed)
    {
      NS_LOG_INFO ("Packet destroyed by interference");

      if (m_device)
        {
          m_interferedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_interferedPacket (packet, 0);
        }

      // If there is one, perform the callback to inform the upper layer of the
      // lost packet
      if (!m_rxFailedCallback.IsNull ())
        {
          m_rxFailedCallback (packet);
        }

    }
  else
    {
      NS_LOG_INFO ("Packet received correctly");


      //#################################tests###############

          //**************************tests
          //NS_LOG_DEBUG ("Starting Header ******* *******  Removal");   
          //Ptr<Packet> tempkt = packet->Copy(); 
          //NS_LOG_DEBUG ("PacketCopied: " << tempkt);
          //NS_LOG_DEBUG ("OriginalPacket: " << packet);
          //LorawanMacHeader macHdr;
          //packet->RemoveHeader (macHdr);
          //LoraFrameHeader frameHdr;
          //packet->RemoveHeader (frameHdr);
          //std::ostream retrv(nullptr);
          //std::stringbuf str;
          //retrv.rdbuf(&str);
          //packet->CopyData(&retrv,packet->GetSize());
          //NS_LOG_DEBUG ("#####?&&&&&&###########Size Received "<< packet->GetSize() );
          //NS_LOG_DEBUG ("#####?&&&&&&###########Content Received "<< str.str() );
       //   ApplyNecessaryOptions (frameHdr);
      //    tempkt->AddHeader (frameHdr);
      //    ApplyNecessaryOptions (macHdr);
     //     tempkt->AddHeader (macHdr);
        //**************************tests




      if (m_device)
        {
          m_successfullyReceivedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_successfullyReceivedPacket (packet, 0);
        }

      // If there is one, perform the callback to inform the upper layer
      if (!m_rxOkCallback.IsNull ())
        {
          m_rxOkCallback (packet);
        }

    }
}
}
}
