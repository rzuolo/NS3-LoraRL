
/*
 * This script simulates a complex scenario with multiple gateways and end
 * devices. The metric of interest for this script is the throughput of the
 * network.
 */

#include "ns3/end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/gateway-lorawan-mac.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/lora-helper.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
#include "ns3/periodic-sender-helper.h"
#include "ns3/command-line.h"
#include "ns3/network-server-helper.h"
#include "ns3/correlated-shadowing-propagation-loss-model.h"
#include "ns3/building-penetration-loss.h"
#include "ns3/building-allocator.h"
#include "ns3/buildings-helper.h"
#include "ns3/forwarder-helper.h"
#include <algorithm>
#include <ctime>

//****tests
#include "ns3/idxnodes.h"
#include "ns3/basic-energy-source-helper.h"
#include "ns3/lora-radio-energy-model-helper.h"
#include "ns3/device-energy-model.h"
#include "ns3/lrmodel.h"
#include <algorithm>
#include <iterator>
#include <fstream>
#include <curses.h>

 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/mobility-module.h"
 #include "ns3/config-store.h"
 #include <ns3/buildings-helper.h>
 #include <ns3/hybrid-buildings-propagation-loss-model.h>
 #include <ns3/constant-position-mobility-model.h>


//****tests


using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("ComplexLorawanNetworkExample");

//****************tests
std::string adrType = "ns3::AdrComponent";

//****************tests

// Network settings
//tests taking nDevices to idxnodes
//int nDevices = 200;
//tests taking nDevices to idxnodes
int nGateways = 1;
double radius = 7500;
double simulationTime = 600;



// Channel model
//bool realisticChannelModel = false;
bool realisticChannelModel = true;

int appPeriodSeconds = 600;

// Output control
bool print = true;





//***test

//// Function to print each node with their attributes
void PrintNodes (int lastNode)
{

	for (int i=0;i<lastNode;i++)
		{
			std::cout<<"Node address "<<trialdevices[i].nwkaddr<< " " << trialdevices[i].success <<" "<< trialdevices[i].fail <<" " << trialdevices[i].pwr_total <<" " << trialdevices[i].distance << std::endl;
			std::cout<<"Node distance "<<trialdevices[i].distance << std::endl;
			std::cout<<"Under threshold "<<trialdevices[i].fail_for_under_threshold << " Interference " << int(trialdevices[i].fail_for_interference) <<" Transmitting "<< trialdevices[i].fail_for_transmitting  <<" Demodulators " << trialdevices[i].fail_for_no_demodulator << std::endl;
		}

	std::cout<<"############################################################################################################################################################################"<<std::endl;
	std::cout<<"Result: Under threshold "<< global_under_threshold << " Interference " << global_interference <<" Transmitting "<< global_transmitting  <<" Demodulators " << global_no_demodulator << std::endl;

}

 
//// At the end of the simulation, stores the Q matrix in the hard disk as array.txtfor future use
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

//// Loads array.txt file from disk to continue feeding the Q matrix
void LoadQ(double **Q)
{

	    std::ifstream file("array.txt");
	    if(file.is_open())
	    {

	    	std::string line;
	    	int j=0;

	    	while (std::getline(file, line))
	    	    {
	    	        std::stringstream ss(line);


	    	        for (int i=0;i<48;i++)
	    	        {
	    	        	std::string value;
	    	        	std::getline(ss,value,'|');
	    	        	Q[j][i] = std::stod(value);
	    	        	//std::cout<<"\""<<vaule<<"\"";
	    	        }

	    	        j++;
	    	    }
	    }
	    else //if the file is not present
	    {

	    		 for(int i = 0; i < ((672));i++)
	    		    {
	    		      for (int j=0; j<48;j++)
	    		      {
	    		    	  Q[i][j] = 0;
	    			  }
	    		  }

	    }


	    file.close();
}



/*void hurricane()
{
	std::cout<<"Here I am"<<std::endl;
	std::getchar();

	 EventId id = Simulator::Schedule (Seconds (600.0), &hurricane);
}*/


//****test





int
main (int argc, char *argv[])
{


  CommandLine cmd;
  cmd.AddValue ("nDevices", "Number of end devices to include in the simulation", nDevices);
  cmd.AddValue ("radius", "The radius of the area to simulate", radius);
  cmd.AddValue ("simulationTime", "The time for which to simulate", simulationTime);
  cmd.AddValue ("appPeriod",
                "The period in seconds to be used by periodically transmitting applications",
                appPeriodSeconds);
  cmd.AddValue ("print", "Whether or not to print various informations", print);
  //** tests
  cmd.AddValue ("pkgbasesize", "Package size provided as argument", pkgbasesize);
  cmd.AddValue ("rlagent", "Activate reinforcement learning agent", rlagent);
  cmd.AddValue ("settled", "Position of single node experiment", settled);
  cmd.AddValue ("training", "Whether it is a training/learning round", training);
  cmd.AddValue ("alpha", "RL Alpha value", ALPHA);
  cmd.AddValue ("gamma", "RL Gamma value", GAMMA);
  cmd.AddValue ("epsilon", "RL epsilon value", EPS);
  //** tests

  cmd.Parse (argc, argv);


  // Set up logging
  // LogComponentEnable ("ComplexLorawanNetworkExample", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraChannel", LOG_LEVEL_INFO);
  // LogComponentEnable("LoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("GatewayLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraInterferenceHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLorawanMac", LOG_LEVEL_ALL);

  // LogComponentEnable("ClassAEndDeviceLorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("GatewayLorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannel", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraPhyHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LorawanMacHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("PeriodicSenderHelper", LOG_LEVEL_ALL);
  //  LogComponentEnable("PeriodicSender", LOG_LEVEL_INFO);
  // LogComponentEnable("LorawanMacHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraFrameHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkScheduler", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkServer", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkStatus", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkController", LOG_LEVEL_ALL);

  //  LogComponentEnable("NetworkControllerComponent", LOG_LEVEL_ALL);
  //  LogComponentEnable("LoraPacketTracker", LOG_LEVEL_ALL);
  
  //  LogComponentEnable("EndDeviceStatus", LOG_LEVEL_ALL);
  //  LogComponentEnable("LoraNetDevice", LOG_LEVEL_ALL);
  //  LogComponentEnable("SimpleEndDeviceLoraPhy", LOG_LEVEL_DEBUG);
  //  LogComponentEnable("SimpleGatewayLoraPhy", LOG_LEVEL_DEBUG);
  //   LogComponentEnable ("LoraRadioEnergyModel", LOG_LEVEL_ALL);

 //     LogComponentEnable("AdrComponent", LOG_LEVEL_ALL);

  /***********
   *  Setup  *
   ***********/

  //**** test
  if (rlagent == 1)
  {
	  for(int i = 0; i < ((672));i++)
	  	  {Q[i] = new double[48];}

	  // Comment this out if you want to start every simulation
	  // with an empty Q matrix

	  /*for(int i = 0; i < ((672));i++)
	    {
	      for (int j=0; j<48;j++)
	      {
	    	  Q[i][j] = 0;
		  }
	  }*/



	  //Otherwise load Q matrix stored from previous simulations
	  LoadQ(Q);




  //Print out on the screen what is the initial Q matrix
  /* std::cout<<"\n"<<"---------------Starting Q Matrix------------------------------------------------\n";
  for(int i = 0; i < ((672));i++)
  {
	  for (int j=0; j<48;j++)
	  {
		  std::cout<<Q[i][j]<<"|";
	  }
	  std::cout<<"\n"<<"---------------------------------------------------------------------------\n";
  } */

   //read anything just to pause the screen
  //std::getchar();

  }


  //******test




  // Create the time value from the period
  Time appPeriod = Seconds (appPeriodSeconds);

  // Mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator", "rho", DoubleValue (radius),
                                 "X", DoubleValue (0.0), "Y", DoubleValue (0.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  /************************
   *  Create the channel  *
   ************************/

  // Create the lora channel object
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 7.7);

  if (realisticChannelModel)
    {
      // Create the correlated shadowing component
      Ptr<CorrelatedShadowingPropagationLossModel> shadowing =
          CreateObject<CorrelatedShadowingPropagationLossModel> ();

      // Aggregate shadowing to the logdistance loss
      loss->SetNext (shadowing);

      // Add the effect to the channel propagation loss
      Ptr<BuildingPenetrationLoss> buildingLoss = CreateObject<BuildingPenetrationLoss> ();

      shadowing->SetNext (buildingLoss);
    }

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

  /************************
   *  Create the helpers  *
   ************************/

  // Create the LoraPhyHelper
  LoraPhyHelper phyHelper = LoraPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the LorawanMacHelper
  LorawanMacHelper macHelper = LorawanMacHelper ();

  // Create the LoraHelper
  LoraHelper helper = LoraHelper ();
  helper.EnablePacketTracking (); // Output filename
  // helper.EnableSimulationTimePrinting ();

  //Create the NetworkServerHelper
  NetworkServerHelper nsHelper = NetworkServerHelper ();

  //Create the ForwarderHelper
  ForwarderHelper forHelper = ForwarderHelper ();

  /************************
   *  Create End Devices  *
   ************************/

  // Create a set of nodes
  NodeContainer endDevices;
  endDevices.Create (nDevices);

  // Assign a mobility model to each node
  mobility.Install (endDevices);

  // Make it so that nodes are at a certain height > 0
  
  //***tests start
 
  std::fstream nds;
   nds.open("nodes.csv",std::ios::in);

   if (nds.is_open())
   {
      std::string tp;
      std::vector<std::string> row;
      std::string word;
      while(getline(nds, tp))
      {
	      std::stringstream s(tp);
                while (getline(s, word, ','))
                {
                row.push_back(word);
                }

      }

      //Confirming that the CSV has been imported in a multiple of 3 (which follows the expected format)
      if( ((row.size())%3) != 0 || ( (int(row.size()/3)) != nDevices ) )
        {
		std::cout << "There is a problem with the nodes CSV input. Please, re-validate it!\n";
		std::exit(-1);
        }

      //Print out the number of nodes
      std::cout<<"The number of nodes is "<< (row.size()+1)/3  <<std::endl;

      
      
      //Prepare to create nodes
      NodeContainer::Iterator j = endDevices.Begin ();
      Ptr<Node> node = *j;
      Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      Vector position = mobility->GetPosition ();

      //Reading the coordinates of each node (every 3 elements)
      for (std::size_t i = 0; i < row.size(); i=i+3)
              {
        	
        	mobility = (*j)->GetObject<MobilityModel> ();
        	position = mobility->GetPosition ();
        	
		position.x=std::stod(row[i]);
        	position.y=std::stod(row[i+1]);
        	position.z=std::stod(row[i+2]);

        	mobility->SetPosition (position);
        	trialdevices[node->GetId()].distance=sqrt((pow(position.x,2)+(pow(position.y,2))));
        	std::cout<<"distance "<<trialdevices[node->GetId()].distance<<" node->GetId() "<< node->GetId() <<std::endl;
        	avgDistance = (avgDistance + trialdevices[node->GetId()].distance);
 		
		if (i < (row.size()-3) )
		{
			j++;
			node = *j;
		}

	      }
      nds.close();
   }

 
 
 
 
 
 
  /* if (nDevices == 3)
  {

   	NodeContainer::Iterator j = endDevices.Begin ();
      	
      	Ptr<Node> node = *j;
	Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      	Vector position = mobility->GetPosition ();
      	position.z=0;
      	position.x=-settled;
      	position.y=0;
	//position.z = 1.2;
	//position.x = 5500;
	//position.y = 6;
	//position.x = 50;
	//position.y = 87;
	mobility->SetPosition (position);
	trialdevices[node->GetId()].distance=sqrt((pow(position.x,2)+(pow(position.y,2))));
        std::cout<<"distance "<<trialdevices[node->GetId()].distance<<" node->GetId() "<< node->GetId() <<std::endl;
        avgDistance = (avgDistance + trialdevices[node->GetId()].distance);
	j++;

      	node = *j;
	mobility = (*j)->GetObject<MobilityModel> ();
	position = mobility->GetPosition ();
      	position.z=0;
      	position.x=settled;
      	position.y=0;
      	//position.z = 1.2;
	//position.x = 2500;
	//position.y = 60;
	//position.x = 800;
	//position.y = 1834;
	mobility->SetPosition (position);
        trialdevices[node->GetId()].distance=sqrt((pow(position.x,2)+(pow(position.y,2))));
        std::cout<<"distance "<<trialdevices[node->GetId()].distance<<" node->GetId() "<< node->GetId() <<std::endl;
        avgDistance = (avgDistance + trialdevices[node->GetId()].distance);
	j++;
	
      	node = *j;
	mobility = (*j)->GetObject<MobilityModel> ();
	position = mobility->GetPosition ();
      	position.z=0;
      	position.x=0;
      	position.y=settled;
      	//position.z = 1.2;
	//position.x = 2500;
	//position.y = 1659;
	//position.x = 3000;
	//position.y = 90;
	mobility->SetPosition (position);
        trialdevices[node->GetId()].distance=sqrt((pow(position.x,2)+(pow(position.y,2))));
        std::cout<<"distance "<<trialdevices[node->GetId()].distance<<" node->GetId() "<< node->GetId() <<std::endl;
        avgDistance = (avgDistance + trialdevices[node->GetId()].distance);

  }
  else
  {
  
  	if (nDevices == 1)
	{

	 NodeContainer::Iterator j = endDevices.Begin ();

         Ptr<Node> node = *j;
         Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
         Vector position = mobility->GetPosition ();

       	 node = *j;
         mobility = (*j)->GetObject<MobilityModel> ();
         position = mobility->GetPosition ();
         position.z = 3;
         position.x = settled;
         position.y = 0;
         mobility->SetPosition (position);
         trialdevices[node->GetId()].distance=sqrt((pow(position.x,2)+(pow(position.y,2))));
         std::cout<<"distance "<<trialdevices[node->GetId()].distance<<" node->GetId() "<< node->GetId() <<std::endl;
         avgDistance = (avgDistance + trialdevices[node->GetId()].distance);
       	 j++; 


	}
	else
	{
   		for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    		{
      		Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      		Vector position = mobility->GetPosition ();
      		position.z = 1.2;
      		Ptr<Node> node = *j;
      		//position.x = radius;
      		//position.x = (node->GetId())*10 ;
      		//position.y = sqrt( (pow(radius,2))-(pow(position.x,2))) ;
      		//position.x = 810;
      		//position.y = 0;
      		position.x = 2500;
      		position.y = 1659;
      		trialdevices[node->GetId()].distance=sqrt((pow(position.x,2)+(pow(position.y,2))));
      		std::cout<<"distance "<<trialdevices[node->GetId()].distance<<" node->GetId() "<< node->GetId() <<std::endl;
      		//std::cout<<"Distance "<<  sqrt((pow(position.x,2)+(pow(position.y,2))))<< " Position "<< mobility->GetPosition () << std::endl;
      		avgDistance = (avgDistance + trialdevices[node->GetId()].distance);
      		mobility->SetPosition (position);
    		}	
	}
  } */

  //***tests
  avgDistance = (avgDistance/nDevices);
  //*****tests


  // Create the LoraNetDevices of the end devices
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen =
      CreateObject<LoraDeviceAddressGenerator> (nwkId, nwkAddr);

  // Create the LoraNetDevices of the end devices
  macHelper.SetAddressGenerator (addrGen);
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LorawanMacHelper::ED_A);
  //helper.Install (phyHelper, macHelper, endDevices);
  // *************tests
  NetDeviceContainer EndNetDevices = helper.Install (phyHelper, macHelper, endDevices);
  // *************tests


  // Now end devices are connected to the channel



  // Connect trace sources
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<Node> node = *j;
      Ptr<LoraNetDevice> loraNetDevice = node->GetDevice (0)->GetObject<LoraNetDevice> ();
      Ptr<LoraPhy> phy = loraNetDevice->GetPhy ();
  
      // tests *****************************************
      // adding confirmation message - default is unconfirmed 
      Ptr<LorawanMac> loraNetMac = node->GetDevice (0)->GetObject<LoraNetDevice> ()->GetMac ();
      Ptr<ClassAEndDeviceLorawanMac> edLorawanMac = loraNetMac->GetObject<ClassAEndDeviceLorawanMac> ();
      //edLorawanMac->SetMType (LorawanMacHeader::CONFIRMED_DATA_UP);

      LoraDeviceAddress edAddr = edLorawanMac->GetDeviceAddress();

      trialdevices[node->GetId()].id=node->GetId();
      trialdevices[node->GetId()].nwkaddr = edAddr.GetNwkAddr();
      trialdevices[node->GetId()].fail = 0;
      trialdevices[node->GetId()].success = 0;
     // std::cout<< "Checking indices "<<node->GetId() << " " <<trialdevices[node->GetId()].id << " " << edAddr.GetNwkAddr() <<std::endl;
    }



     //for (int i= 0; i<nDevices; i++){
     //	  std::cout << "Scanning the devices structures items #.. " <<  trialdevices[i].id << std::endl;
     //  }




  // ********************************************tests

  /*********************
   *  Create Gateways  *
   *********************/

  // Create the gateway nodes (allocate them uniformely on the disc)
  NodeContainer gateways;
  gateways.Create (nGateways);

  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  // Make it so that nodes are at a certain height > 0
  allocator->Add (Vector (0.0, 0.0, 15.0));
  mobility.SetPositionAllocator (allocator);
  mobility.Install (gateways);

  // Create a netdevice for each gateway
  phyHelper.SetDeviceType (LoraPhyHelper::GW);
  macHelper.SetDeviceType (LorawanMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);

  /**********************
   *  Handle buildings  *
   **********************/

  /**** tests *
   Ptr<ConstantPositionMobilityModel> mmEnb = CreateObject<ConstantPositionMobilityModel> ();
   mmEnb->SetPosition (Vector (150.0, 0.0, 0.0));
   Ptr<Building> building1 = CreateObject<Building> ();
   building1->SetBoundaries (Box (-40, 40, -40, 40, -20, 20));
  
   building1->SetBuildingType (Building::Residential);
   building1->SetExtWallsType (Building::ConcreteWithWindows);
      
   Ptr<MobilityBuildingInfo> buildingInfoEnb = CreateObject<MobilityBuildingInfo> ();
   mmEnb->AggregateObject (buildingInfoEnb); // operation usually done by BuildingsHelper::Install
   */
   // cancel shadowing effect
 
  /* 
   double x_min = 0.0;
   double x_max = 2000.0;
   double y_min = 0.0;
   double y_max = 20.0;
   double z_min = -40.0;
   double z_max = 40.0;
   Ptr<Building> b = CreateObject <Building> ();
   b->SetBoundaries (Box (x_min, x_max, y_min, y_max, z_min, z_max));
   b->SetBuildingType (Building::Residential);
   b->SetExtWallsType (Building::StoneBlocks);
   b->SetNFloors (3);
   b->SetNRoomsX (3);
   b->SetNRoomsY (2);
   **** tests */
 

  
    double xLength = 130;
    double deltaX = 32;
    double yLength = 64;
    double deltaY = 17;
  //double xLength = 130;
  //double deltaX = 100;
  //double yLength = 64;
  //double deltaY = 100;
  int gridWidth = 2 * radius / (xLength + deltaX);
  int gridHeight = 2 * radius / (yLength + deltaY);
  if (realisticChannelModel == false)
    {
      gridWidth = 0;
      gridHeight = 0;
    }
  Ptr<GridBuildingAllocator> gridBuildingAllocator;


  gridBuildingAllocator = CreateObject<GridBuildingAllocator> ();
  gridBuildingAllocator->SetAttribute ("GridWidth", UintegerValue (gridWidth));
  gridBuildingAllocator->SetAttribute ("LengthX", DoubleValue (xLength));
  gridBuildingAllocator->SetAttribute ("LengthY", DoubleValue (yLength));
  gridBuildingAllocator->SetAttribute ("DeltaX", DoubleValue (deltaX));
  gridBuildingAllocator->SetAttribute ("DeltaY", DoubleValue (deltaY));
  gridBuildingAllocator->SetAttribute ("Height", DoubleValue (6));
  gridBuildingAllocator->SetBuildingAttribute ("NRoomsX", UintegerValue (2));
  gridBuildingAllocator->SetBuildingAttribute ("NRoomsY", UintegerValue (4));
  gridBuildingAllocator->SetBuildingAttribute ("NFloors", UintegerValue (2));
  

  //***** tests
  //gridBuildingAllocator->SetBuildingAttribute ("ExtWallsType", UintegerValue (3));
  //***** tests
  gridBuildingAllocator->SetAttribute (
      "MinX", DoubleValue (-gridWidth * (xLength + deltaX) / 2 + deltaX / 2));
  gridBuildingAllocator->SetAttribute (
      "MinY", DoubleValue (-gridHeight * (yLength + deltaY) / 2 + deltaY / 2));
   
  BuildingContainer bContainer = gridBuildingAllocator->Create (gridWidth * gridHeight);
  //BuildingContainer bContainer = gridBuildingAllocator->Create (352);
  //BuildingContainer bContainer = gridBuildingAllocator->Create (1);
  

  BuildingsHelper::Install (endDevices);
  BuildingsHelper::Install (gateways);
  BuildingsHelper::MakeMobilityModelConsistent ();

  // Print the buildings
  if (print)
    {
      std::ofstream myfile;
      myfile.open ("buildings.txt");
      std::vector<Ptr<Building>>::const_iterator it;
      int j = 1;
      for (it = bContainer.Begin (); it != bContainer.End (); ++it, ++j)
        {
          Box boundaries = (*it)->GetBoundaries ();
          myfile << "set object " << j << " rect from " << boundaries.xMin << "," << boundaries.yMin
                 << " to " << boundaries.xMax << "," << boundaries.yMax  <<std::endl;
        }
      myfile.close ();
    }

  /**********************************************
    *  Set up the end device's spreading factor  *
   **********************************************/

  macHelper.SetSpreadingFactorsUp (endDevices, gateways, channel);

  NS_LOG_DEBUG ("Completed configuration");

  /*********************************************
   *  Install applications on the end devices  *
   *********************************************/

  Time appStopTime = Seconds (simulationTime);
  PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
  appHelper.SetPeriod (Seconds (appPeriodSeconds));
  appHelper.SetPacketSize (23);

  Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0), "Max", DoubleValue (10));
  ApplicationContainer appContainer = appHelper.Install (endDevices);

  appContainer.Start (Seconds (0));
  appContainer.Stop (appStopTime);

  /**************************
   *  Create Network Server  *
   ***************************/

  // Create the NS node
  NodeContainer networkServer;
  networkServer.Create (1);

  // Create a NS for the network
  nsHelper.SetEndDevices (endDevices);
  nsHelper.SetGateways (gateways);

  //**********tests
  nsHelper.EnableAdr(true);
  nsHelper.SetAdr (adrType);
  //**********tests

  nsHelper.Install (networkServer);


  //Create a forwarder for each gateway
  forHelper.Install (gateways);


  /////////////////////////////////////
  /****************** tests adding power consumption measurement*/

  BasicEnergySourceHelper basicSourceHelper;
  LoraRadioEnergyModelHelper radioEnergyHelper;

  // configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (10000)); // Energy in J
  basicSourceHelper.Set ("BasicEnergySupplyVoltageV", DoubleValue (3.3));

  radioEnergyHelper.Set ("StandbyCurrentA", DoubleValue (0.0014));
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.028));
  radioEnergyHelper.Set ("SleepCurrentA", DoubleValue (0.0000015));
  radioEnergyHelper.Set ("RxCurrentA", DoubleValue (0.0112));

  radioEnergyHelper.SetTxCurrentModel ("ns3::ConstantLoraTxCurrentModel",
                                       "TxCurrent", DoubleValue (0.028));

  // install source on EDs' nodes
  EnergySourceContainer sources = basicSourceHelper.Install (endDevices);
  Names::Add ("/Names/EnergySource", sources.Get (0));

  // install device model
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install(EndNetDevices, sources);

  ///////////////////////////////////

  /* tests */



  ///****
  //EventId id = Simulator::Schedule (Seconds (30.0), &hurricane);
  ///***

  ////////////////
  // Simulation //
  ////////////////

  Simulator::Stop (appStopTime + Hours (1));


  NS_LOG_INFO ("Running simulation...");
  Simulator::Run ();

  Simulator::Destroy ();

  ///////////////////////////
  // Print results to file //
  ///////////////////////////
  NS_LOG_INFO ("Computing performance metrics...");

  //****** tests
  // std::cout<<"###Results: Power spent in the environment "<< global_power << std::endl;



  /* for (int i=0;i<=nDevices;i++)
   {

	   std::cout<<"Device index "<< trialdevices[i].nwkaddr <<std::endl;
	   std::cout<<"Success transmissions "<< trialdevices[i].success <<std::endl;
	   std::cout<<"Failed transmissions "<< trialdevices[i].fail <<std::endl;

   }*/
  //*****  tests
  LoraPacketTracker &tracker = helper.GetPacketTracker ();
  //std::cout << "###Results: " << tracker.CountMacPacketsGlobally (Seconds (0), appStopTime + Hours (1)) << std::endl;
  
  //LoraPacketTracker &tracker2 = helper.GetPacketTracker ();
  //std::cout << tracker2.PrintPhyPacketsPerGw (Seconds (0), appStopTime + Hours (1),700) << std::endl;


  std::cout<< std::endl << global_power << " " << tracker.CountMacPacketsGlobally (Seconds (0), appStopTime + Hours (1)) <<std::endl;

  float indicator = float(global_received)/float(global_sent);
  float power_indicator = float(global_power/global_received);

  std::cout<<"Result "<<global_received<<" "<< global_sent <<" "<<global_power<< " "<< power_indicator  <<" "<< indicator<< " "<<avgDistance<< " " << freq_on_air[0] << " "<< freq_on_air[1] <<" "<<freq_on_air[2] <<std::endl;



  //read anything just to pause the screen
  //std::getchar();


  if (rlagent == 1)
  {

	 /* std::cout<<"\n"<<"---------------------------------------------------------------------------\n";
	  for(int i = 0; i < ((672));i++)
	  {
		  for (int j=0; j<48;j++)
		  {
			  std::cout<<Q[i][j]<<"|";
		  }
		  std::cout<<"\n"<<"---------------------------------------------------------------------------\n";
	  }*/

	if (training)
	{
		SaveQ(Q);
	}
  }

  //***tests
  //PrintNodes(nDevices);
  //***tests
  return 0;
}
