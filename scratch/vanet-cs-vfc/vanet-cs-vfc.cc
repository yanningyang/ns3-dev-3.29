/*
 * vanet-cs-vfc.cc
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#include "vanet-cs-vfc.h"

using namespace ns3;

//std::vector<uint32_t> VanetCsVfcExperiment::globalDB;
//std::map<uint32_t, uint32_t> VanetCsVfcExperiment::vehId2IndexMap;
//std::vector<bool> VanetCsVfcExperiment::vehsEnterFlag;
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::vehsReqs;
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::vehsCaches;
//std::vector<std::map<uint32_t, RequestStas>> VanetCsVfcExperiment::vehsReqsStas;
//std::map<uint32_t, uint32_t> VanetCsVfcExperiment::vehIdx2FogIdxMap;
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::fogCluster;
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::fogsReqs;
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::fogsCaches;
//
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::vehsReqsInCloud;
//std::vector<std::set<uint32_t>> VanetCsVfcExperiment::vehsCachesInCloud;
//std::vector<std::map<uint32_t, RequestStas>> VanetCsVfcExperiment::vehsReqsStasInCloud;
//std::vector<ns3::Vector3D> VanetCsVfcExperiment::vehsMobInfoInCloud;
//
//uint32_t VanetCsVfcExperiment::receive_count = 0;

//std::map<uint32_t, std::set<uint32_t>> VanetCsVfcExperiment::vehsReqs;
//std::map<uint32_t, std::set<uint32_t>> VanetCsVfcExperiment::vehsCaches;
//std::map<uint32_t, std::set<uint32_t>> VanetCsVfcExperiment::fogCluster;
//std::map<uint32_t, std::set<uint32_t>> VanetCsVfcExperiment::fogsReqs;
//std::map<uint32_t, std::set<uint32_t>> VanetCsVfcExperiment::fogsCaches;

VanetCsVfcExperiment::VanetCsVfcExperiment ():
    m_protocol (0),
    m_dlPort (1000),
    m_ulPort (2000),
    m_v2IPort (3000),
    m_i2IPort (4000),
    m_i2VPort (5000),
    m_protocolName ("NONE"),
    m_nObuNodes (288),
    m_obuNodes (),
    m_obuTxp (137), // 450m
    m_nRsuNodes (16),
    m_rsuNodes (),
    m_rsuTxp (137), // 450m
    m_ueNodes (),
    m_enbNodes (),
    m_remoteHostNodes (),
    m_bsTxp (180),
    m_phyMode ("OfdmRate6MbpsBW10MHz"),
    m_traceMobility (false),
    m_workspacePath ("/home/haha/ns3.29-dev-workspace/workspace/vanet-cs-vfc"),
    m_mobilityFile (m_workspacePath + "/mobility" + "/mobility.tcl"),
    m_mobLogFile (m_workspacePath + "/mobility.log"),
    m_trName (m_workspacePath + "/vanet-cs-vfc.mob"),
    m_animFile (m_workspacePath + "/vanet-cs-vfc-animation.xml"),
    m_log (1),
    m_TotalSimTime (361.01),
    m_rate ("2048bps"),
    m_wavePacketSize (200),
    m_nonSafetySize (1500),
    m_waveInterval (0.1),
    m_verbose (0),
    m_routingTables (0),
    m_asciiTrace (0),
    m_pcap (0)
{
  m_wifiPhyStats = CreateObject<WifiPhyStats> ();

  // set to non-zero value to enable
  // simply uncond logging during simulation run
  m_log = 1;

  globalDB.resize(Gloabal_DB_Size);
  vehsEnterFlag.resize(m_nObuNodes, false);

  vehsInitialReqs.resize(m_nObuNodes);
  vehsInitialCaches.resize(m_nObuNodes);
  vehsReqs.resize(m_nObuNodes);
  vehsReqsStatus.resize(m_nObuNodes);
  vehsCaches.resize(m_nObuNodes);

  vehsReqsInCloud.resize(m_nObuNodes);
  vehsReqsStasInCloud.resize(m_nObuNodes);
  vehsCachesInCloud.resize(m_nObuNodes);
  vehsMobInfoInCloud.resize(m_nObuNodes);

  fogCluster.resize(m_nRsuNodes);
  fogsCaches.resize(m_nRsuNodes);
  fogsReqs.resize(m_nRsuNodes);

  isFirstSubmit.resize(m_nObuNodes, true);
  isDecoding = false;

  datasNeededForDecodingPerClique.resize(m_nObuNodes);

  currentBroadcastId = 0;

  receive_count = 0;

  // Initializing global data base
  for (uint32_t i = 0; i < Gloabal_DB_Size; i++)
    {
      globalDB[i] = i;
    }

  // Initializing vehicle cache and request set
#if 1
  Ptr<UniformRandomVariable> urv = CreateObject<UniformRandomVariable>();
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      urv->SetStream(i);
      uint32_t minLocalSize = Gloabal_DB_Size / 5;
      uint32_t maxLocalSize = minLocalSize * 1.5;
      uint32_t localSize = (uint32_t)(urv->GetValue(minLocalSize, maxLocalSize));
      std::set<uint32_t> vehReq(globalDB.begin(), globalDB.end());
      std::set<uint32_t> vehCache;
      for (uint32_t j = 1; j <= localSize; j++)
	{
	  uint32_t cacheIdx = (uint32_t)(urv->GetValue(0, Gloabal_DB_Size));
	  uint32_t data = globalDB[cacheIdx];
	  vehCache.insert(data);
	  vehReq.erase(data);
	}
      vehsInitialCaches[i] = vehCache;
      vehsInitialReqs[i] = vehReq;
    }
#endif

#if 0
  // for testing
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
//      uint32_t localSize = (uint32_t)(Gloabal_DB_Size / 5);
      uint32_t localSize = (uint32_t)(Gloabal_DB_Size - 2);
      std::set<uint32_t> vehReq(globalDB.begin(), globalDB.end());
      std::set<uint32_t> vehCache;
      for (uint32_t j = 1; j <= localSize; j++)
	{
	  uint32_t cacheIdx = j - 1;
	  uint32_t data = globalDB[cacheIdx];
	  vehCache.insert(data);
	  vehReq.erase(data);
	}
      vehsCaches[i] = vehCache;
      vehsReqs[i] = vehReq;
    }
#endif

#if Print_Vehicle_Initial_Request
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      cout << "veh:" << i << ", init reqs:";
      for(uint32_t req : vehsInitialReqs[i])
	{
	  cout << " " << req;
	}
      cout << endl;
    }
#endif

#if Print_Vehicle_Initial_Cache
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      cout << "veh:" << i << ", init caches:";
      for(uint32_t cache : vehsInitialCaches[i])
	{
	  cout << " " << cache;
	}
      cout << endl;
    }
#endif
}

void
VanetCsVfcExperiment::Simulate (int argc, char **argv)
{
  NS_LOG_UNCOND ("Start Simulate...");

  SetDefaultAttributeValues ();
  ParseCommandLineArguments (argc, argv);
  ConfigureNodes ();
//  ConfigureChannels ();
  ConfigureDevices ();
//  ConfigureInternet ();
//  ConfigureMobility ();
  ConfigureApplications ();
  ConfigureTracing ();
  RunSimulation ();
  ProcessOutputs ();
}

void
VanetCsVfcExperiment::SetDefaultAttributeValues ()
{
  // handled in constructor
}

void
VanetCsVfcExperiment::ParseCommandLineArguments (int argc, char **argv)
{
  CommandSetup (argc, argv);
  SetupScenario ();

  ConfigureDefaults ();
}

void
VanetCsVfcExperiment::ConfigureNodes ()
{
  m_obuNodes.Create (m_nObuNodes);
  m_rsuNodes.Create (m_nRsuNodes);

  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      vehId2IndexMap.insert(make_pair(m_obuNodes.Get(i)->GetId(), i));
    }
  for (uint32_t i = 0; i < m_nRsuNodes; i++)
    {
      fogId2FogIdxMap.insert(make_pair(m_rsuNodes.Get(i)->GetId(), i));
    }
}

void
VanetCsVfcExperiment::ConfigureChannels ()
{
  // todo
}

void
VanetCsVfcExperiment::ConfigureDevices ()
{
  SetupDevices ();
  // devices are set up in SetupAdhocDevices(),
  // called by ConfigureChannels()

  // every device will have PHY callback for tracing
  // which is used to determine the total amount of
  // data transmitted, and then used to calculate
  // the MAC/PHY overhead beyond the app-data								// get the raw pointer
  Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/Tx", MakeCallback (&WifiPhyStats::PhyTxTrace, m_wifiPhyStats)); // @suppress("Invalid arguments")
  // TxDrop, RxDrop not working yet.  Not sure what I'm doing wrong.
//  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback (&WifiPhyStats::PhyTxDrop, m_wifiPhyStats));
//  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback (&WifiPhyStats::PhyRxDrop, m_wifiPhyStats));
}

void
VanetCsVfcExperiment::ConfigureInternet ()
{
  // todo
}

void
VanetCsVfcExperiment::ConfigureMobility ()
{
  SetupObuMobilityNodes ();
  SetupRsuMobilityNodes ();
}
void
VanetCsVfcExperiment::ConfigureApplications ()
{
  // config trace to capture app-data (bytes) for
  // routing data, subtracted and used for
  // routing overhead
  std::ostringstream oss;
  oss.str ("");
  oss << "/NodeList/*/ApplicationList/*/$ns3::OnOffApplication/Tx";
  Config::Connect (oss.str (), MakeCallback (&VanetCsVfcExperiment::OnOffTrace, this));

  ApplicationContainer serverApps;
  for (uint32_t i = 0; i < m_nObuNodes; ++i)
    {
#if Lte_Enable
      PacketSinkHelper obuLteDlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (m_ueInterface.GetAddress(i), m_dlPort));
      serverApps.Add (obuLteDlPacketSinkHelper.Install (m_ueNodes.Get(i)));
#else
//      PacketSinkHelper obuDsrcDlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (m_ueInterface.GetAddress(i), m_dlPort));
      PacketSinkHelper obuDsrcDlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress ("10.2.0.0", m_dlPort));
      serverApps.Add (obuDsrcDlPacketSinkHelper.Install (m_ueNodes.Get(i)));
#endif
//      PacketSinkHelper obuDsrcPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (m_obuInterfaces.GetAddress(i), m_i2VPort));
      PacketSinkHelper obuDsrcPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress ("10.3.0.0", m_i2VPort));
      serverApps.Add (obuDsrcPacketSinkHelper.Install (m_obuNodes.Get(i)));
    }
  for (uint32_t i = 0; i < m_nRsuNodes; ++i)
    {
      PacketSinkHelper rsuDsrcPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (m_rsu80211pInterfaces.GetAddress(i), m_v2IPort));
      serverApps.Add (rsuDsrcPacketSinkHelper.Install (m_rsuNodes.Get(i)));
      PacketSinkHelper rsuCsmaPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (m_rsuCsmaInterfaces.GetAddress(i), m_i2IPort));
      serverApps.Add (rsuCsmaPacketSinkHelper.Install (m_rsuNodes.Get(i)));
    }
#if Cloud_Enable
  PacketSinkHelper remoteHostPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (m_remoteHostAddr, m_ulPort));
  serverApps.Add (remoteHostPacketSinkHelper.Install (m_remoteHost));
#endif
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/RxWithAddresses", MakeCallback (&VanetCsVfcExperiment::ReceivePacketWithAddr, this));
  serverApps.Start (Seconds (0.01));
}

void
VanetCsVfcExperiment::ConfigureTracing ()
{
//  WriteCsvHeader ();//todo
  SetupTraceFile ();
  SetupLogging ();
}

void
VanetCsVfcExperiment::ConfigureAnim ()
{
  AnimationInterface anim (m_animFile);
  uint32_t resId1 = anim.AddResource("/home/haha/Pictures/icon/car-003.png");
  uint32_t resId3 = anim.AddResource("/home/haha/Pictures/icon/rsu-001.png");
  uint32_t resId5 = anim.AddResource("/home/haha/Pictures/icon/cloud.png");

  for (uint32_t i = 0; i < m_nObuNodes; ++i)
    {
      Ptr<Node> obu = m_obuNodes.Get(i);
//      anim.UpdateNodeDescription (obu, "OBU");
      anim.UpdateNodeSize(obu->GetId(), 50, 50);
      anim.UpdateNodeImage(obu->GetId(), resId1);
    }
  for (uint32_t i = 0; i < m_nRsuNodes; ++i)
    {
      Ptr<Node> rsu = m_rsuNodes.Get(i);
//      anim.UpdateNodeDescription (rsu, "RSU");
      anim.UpdateNodeSize(rsu->GetId(), 50, 50);
      anim.UpdateNodeImage(rsu->GetId(), resId3);
    }
#if Lte_Enable
  uint32_t resId2 = anim.AddResource("/home/haha/Pictures/icon/cellular-001.png");
  uint32_t resId4 = anim.AddResource("/home/haha/Pictures/icon/pgw.png");
  for (uint32_t i = 0; i < m_enbNodes.GetN(); ++i)
    {
      Ptr<Node> enb = m_enbNodes.Get(i);
//      anim.UpdateNodeDescription (enb, "eNB");
      anim.UpdateNodeSize(enb->GetId(), 50, 50);
      anim.UpdateNodeImage(enb->GetId(), resId2);
    }

//  anim.UpdateNodeDescription (m_pgw, "MME/PGW");
  anim.UpdateNodeSize(m_pgw->GetId(), 50, 50);
  anim.UpdateNodeImage(m_pgw->GetId(), resId4);
#endif

//  anim.UpdateNodeDescription (m_remoteHost, "Remote Host");
  anim.UpdateNodeSize(m_remoteHost->GetId(), 50, 50);
  anim.UpdateNodeImage(m_remoteHost->GetId(), resId5);

  anim.SetStartTime (Seconds(1));
  anim.SetStopTime (Seconds(10));
  anim.EnablePacketMetadata ();
//  anim.EnableIpv4RouteTracking ("routingtable-wireless.xml", Seconds (0), Seconds (5), Seconds (0.25));
//  anim.EnableWifiMacCounters (Seconds (0), Seconds (10));
//  anim.EnableWifiPhyCounters (Seconds (0), Seconds (10));
}

void
VanetCsVfcExperiment::RunSimulation ()
{
#if Total_Time_Spent_stas
  clock_t startTime, endTime;
  startTime = clock();
  std::cout << "simulation start time " << Now() << " -- " << (double)(startTime) / CLOCKS_PER_SEC << std::endl;
#endif

  Run ();

#if Print_Vehicle_Final_Request
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      cout << "veh:" << i << ", final reqs:";
      for(uint32_t req : vehsReqs[i])
	{
	  cout << " " << req;
	}
      cout << endl;
    }
#endif

#if Print_Vehicle_Final_Cache
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      cout << "veh:" << i << ", final caches:";
      for(uint32_t cache : vehsCaches[i])
	{
	  cout << " " << cache;
	}
      cout << endl;
    }
#endif

  std::cout << "receive_count: " << receive_count << std::endl;

  std::cout << "SubmittedReqs: " << m_requestStats.GetSubmittedReqs() << std::endl;
  std::cout << "SatisfiedReqs: " << m_requestStats.GetSatisfiedReqs() << std::endl;
  std::cout << "BroadcastPkts: " << m_requestStats.GetBroadcastPkts() << std::endl;
  std::cout << "BroadcastPkts: " << m_requestStats.GetCumulativeDelay() << std::endl;
  std::cout << "ASD: " << (m_requestStats.GetCumulativeDelay() / 1e+6) / m_requestStats.GetSatisfiedReqs() << "ms" << std::endl;
  std::cout << "SR: " << (double)m_requestStats.GetSatisfiedReqs() / m_requestStats.GetSubmittedReqs() << std::endl;
  std::cout << "BE: " << m_requestStats.GetSatisfiedReqs() / m_requestStats.GetBroadcastPkts() << std::endl;

#if Total_Time_Spent_stas
  endTime = clock();
  std::cout << "simulation end time " << Now() << " -- " << (double)(endTime - startTime) / CLOCKS_PER_SEC << std::endl;
#endif

  Simulator::Destroy ();
}

void
VanetCsVfcExperiment::ProcessOutputs ()
{
  NS_LOG_UNCOND ("Process Outputs...");
  m_os.close (); // close log file
}

void
VanetCsVfcExperiment::Run ()
{
  Simulator::Stop (Seconds (m_TotalSimTime));

#if Output_Animation
  ConfigureAnim ();
#endif

  Simulator::Schedule(Seconds(0.01), &VanetCsVfcExperiment::LoopPerSecond, this);

  Simulator::Run ();
//  Simulator::Destroy ();
}

// Prints actual position and velocity when a course change event occurs
void
VanetCsVfcExperiment::CourseChange (std::string context, Ptr<const MobilityModel> mobility)
{
  Ptr<Node> obu = mobility->GetObject<Node> ();
  uint32_t obuId = obu->GetId();
  uint32_t obuIdx = vehId2IndexMap.at(obuId);

  // if the vehicle enters the observed area for the first time, it will initiate a request to the BS
  if (vehsEnterFlag[obuIdx] == false)
    {
//      UploadVehicleInfo(obu);
      vehsEnterFlag[obuIdx] = true;
    }
}

void
VanetCsVfcExperiment::CourseChange (std::ostream *os, std::string context, Ptr<const MobilityModel> mobility)
{
  Vector pos_obu = mobility->GetPosition (); // Get position
  Vector vel_obu = mobility->GetVelocity (); // Get velocity

  pos_obu.z = 1.5;

  // Prints position and velocities
  *os << Simulator::Now () << " POS: x=" << pos_obu.x << ", y=" << pos_obu.y
      << ", z=" << pos_obu.z << "; VEL:" << vel_obu.x << ", y=" << vel_obu.y
      << ", z=" << vel_obu.z << std::endl;
}

void
VanetCsVfcExperiment::LoopPerSecond()
{
#if Upload_Enable
  UploadAllVehiclesInfo();
#endif
  SubmitAllRequests();
  UpdateAllFogCluster();
  Simulator::Schedule(Seconds(1.0), &VanetCsVfcExperiment::LoopPerSecond, this);
}

void
VanetCsVfcExperiment::SubmitRequests(Ptr<Node> obu)
{
  uint32_t obuId = obu->GetId();
  uint32_t obuIdx = vehId2IndexMap.at(obuId);
  if (isFirstSubmit[obuIdx])
    {
      vehsReqs[obuIdx].insert(vehsInitialReqs[obuIdx].begin(), vehsInitialReqs[obuIdx].end());
      vehsCaches[obuIdx].insert(vehsInitialCaches[obuIdx].begin(), vehsInitialCaches[obuIdx].end());

      RequestStatus reqStatus;
      for (uint32_t reqIdx : vehsReqs[obuIdx])
	{
	  reqStatus.completed = false;
	  reqStatus.submitTime = Now().GetDouble();
	  vehsReqsStatus[obuIdx].insert(make_pair(reqIdx, reqStatus));
	}

      m_requestStats.IncSubmittedReqs(vehsReqs[obuIdx].size());

      isFirstSubmit[obuIdx] = false;
    }
}

void
VanetCsVfcExperiment::SubmitAllRequests()
{
  for (NodeContainer::Iterator i = m_obuNodes.Begin (); i != m_obuNodes.End (); ++i)
    {
      Ptr<Node> obu = (*i);
      uint32_t obuId = obu->GetId();
      uint32_t obuIdx = vehId2IndexMap.at(obuId);

      if (vehsEnterFlag[obuIdx] == true)
        {
	  SubmitRequests(obu);
        }
    }
}

void
VanetCsVfcExperiment::UploadVehicleInfo(Ptr<Node> obu)
{
  Ptr<MobilityModel> model = obu->GetObject<MobilityModel> ();
  Vector pos_obu = model->GetPosition ();

  uint32_t obuId = obu->GetId();
  uint32_t obuIdx = vehId2IndexMap.at(obuId);

  /**
   * vehicle id (uint32_t):		4 * byte
   * vehicle pos x (double):		8 * byte
   * vehicle pos y (double):		8 * byte
   * num of requsets (uint32_t):	4 * byte
   * vehicle request (uint32_t):	4 * byte * vehsReqs[obuIdx].size()
   * num of chaches (uint32_t):		4 * byte
   * vehicle cache (uint32_t):		4 * byte * vehsCaches[obuIdx].size()
   */
  uint32_t dataSize = 4 + 8 * 2 + 4 + 4 * vehsReqs[obuIdx].size() + 4 + 4 * vehsCaches[obuIdx].size();
  vanet::ByteBuffer bytes(dataSize);
  bytes.WriteU32(obuId);
  bytes.WriteDouble(pos_obu.x);
  bytes.WriteDouble(pos_obu.y);

  // the number of requests
  bytes.WriteU32(vehsReqs[obuIdx].size());
  for (uint32_t reqData : vehsReqs[obuIdx])
    {
      RequestStatus reqStats;
      reqStats.submitTime = Now().GetSeconds();
      vehsReqsStatus[obuIdx].insert(make_pair(reqData, reqStats));

      bytes.WriteU32(reqData);
    }

  // the number of cache
  bytes.WriteU32(vehsCaches[obuIdx].size());
  for (uint32_t cacheData : vehsCaches[obuIdx])
    {
      bytes.WriteU32(cacheData);
    }

  Ptr<UdpSender> sender = CreateObject<UdpSender>();
  sender->SetNode(obu);
  sender->SetRemote(m_remoteHostAddr, m_ulPort);
  sender->Start();
  using vanet::PacketHeader;
  PacketHeader header;
  header.SetType(PacketHeader::MessageType::REQUEST);
  sender->SetHeader(header);
  sender->SetFill(bytes.GetBufferData(), dataSize);

  Simulator::Schedule(Now(), &UdpSender::Send, sender);
}

void
VanetCsVfcExperiment::UploadAllVehiclesInfo()
{
  for (NodeContainer::Iterator i = m_obuNodes.Begin (); i != m_obuNodes.End (); ++i)
    {
      Ptr<Node> obu = (*i);
      uint32_t obuId = obu->GetId();
      uint32_t obuIdx = vehId2IndexMap.at(obuId);

      if (vehsEnterFlag[obuIdx] == true)
        {
	  UploadVehicleInfo(obu);
        }
    }
}

void
VanetCsVfcExperiment::UpdateAllFogCluster()
{
  // update vehicle set in every fog
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      uint32_t obuIdx = i;
#if Upload_Enable
      Vector pos_obu = vehsMobInfoInCloud[obuIdx];
#else
      Ptr<Node> obu = m_obuNodes.Get(i);
      Ptr<MobilityModel> obuMobility = obu->GetObject<MobilityModel> ();
      Vector pos_obu = obuMobility->GetPosition ();
#endif
      for (uint32_t j = 0; j < m_nRsuNodes; j++)
	{
	  Ptr<Node> rsu = m_rsuNodes.Get(j);
	  Ptr<MobilityModel> rsuMobility = rsu->GetObject<MobilityModel> ();
	  Vector pos_rsu = rsuMobility->GetPosition ();
	  if (CalculateDistance (pos_obu, pos_rsu) > Device_Transmission_Range )
	    {
	      fogCluster[j].erase(obuIdx);
	    }
	  else
	    {
	      fogCluster[j].insert(obuIdx);
	      vehIdx2FogIdxMap[obuIdx] = j;
	    }
	}
    }

  UpdateAllFogData();

#if Print_Fog_Cluster
  PrintFogCluster();
#endif

#if Construct_Graph
  ConstructGraphAndBroadcast();
#endif
}

void
VanetCsVfcExperiment::UpdateAllFogData()
{
#if Print_Vehicle_Request
  for (uint32_t i = 0; i < m_nObuNodes; i++)
    {
      cout << "veh:" << i << ", reqs:";
      for(uint32_t req : vehsReqs[i])
	{
	  cout << " " << req;
	}
      cout << endl;
    }
#endif

  // update fog request set cache set
  for (uint32_t i = 0; i < m_nRsuNodes; i++)
    {
      fogsCaches[i].clear();
      fogsReqs[i].clear();
      std::set<uint32_t> vehSet = fogCluster[i];
      for (uint32_t veh : vehSet)
	{
#if Upload_Enable
	  fogsCaches[i].insert(vehsCachesInCloud[veh].begin(), vehsCachesInCloud[veh].end());
	  fogsReqs[i].insert(vehsReqsInCloud[veh].begin(), vehsReqsInCloud[veh].end());
#else
	  fogsCaches[i].insert(vehsCaches[veh].begin(), vehsCaches[veh].end());
	  fogsReqs[i].insert(vehsReqs[veh].begin(), vehsReqs[veh].end());
#endif
	}
    }
}

void
VanetCsVfcExperiment::PrintFogCluster()
{
  if (m_log != 0)
  {
    std::cout << Simulator::Now() << endl;
    for (uint32_t i = 0; i < m_nRsuNodes; i++)
      {
	std::cout << "fogCluster[" << i << "]:";
#if 1
	for (uint32_t id : fogCluster[i])
	  {
	    std::cout << " " << id;
	  }
	std::cout << endl;
#endif
#if 0
	std::cout << endl;
	std::cout << "req:";
	for (uint32_t req : fogsReqs[i])
	  {
	    std::cout << " " << req;
	  }
	std::cout << endl;
	std::cout << "cache:";
	for (uint32_t cache : fogsCaches[i])
	  {
	    std::cout << " " << cache;
	  }
	std::cout << endl;
#endif
      }
    std::cout << endl;
  }
}

void
VanetCsVfcExperiment::ConstructGraphAndBroadcast ()
{
#if Construct_Graph_And_Find_Clique_Time_stas
  clock_t startTime, endTime;
  startTime = clock();
#endif
  // update fog request set cache set
  vector<VertexNode> vertices;
  for (uint32_t i = 0; i < m_nRsuNodes; i++)
    {
      for (uint32_t req : fogsReqs[i])
	{
	  VertexNode vn(i, req);
	  vertices.push_back(vn);
	}
    }

  if (vertices.empty()) return;

  graph = GraphMatrix<VertexNode> (vertices, vertices.size());

  // add edge using condition 1, 2, 3
  VertexNode vn1;
  VertexNode vn2;
  EdgeNode<> en = {.type = EdgeType::NOT_SET, .weight = 1};
  for (uint32_t i = 0; i < m_nRsuNodes; i++)
    {
      vn1.fogIndex = i;
      for (uint32_t req1 : fogsReqs[i])
	{
	  vn1.reqDataIndex = req1;
	  vn1.genName();
	  for (uint32_t j = i + 1; j < m_nRsuNodes; j++)
	    {
	      vn2.fogIndex = j;
	      if (fogsReqs[j].count(req1)) // condition 1
		{
		  vn2.reqDataIndex = req1;
		  vn2.genName();
		  en.type = EdgeType::CONDITION_1;
		  graph.addEdge(vn1, vn2, en);
		}
	      for (uint32_t req2 : fogsReqs[j])
		{
		  vn2.reqDataIndex = req2;
		  vn2.genName();
		  if ((fogsCaches[j].count(req1) && fogsCaches[i].count(req2))) // condition 2
		    {
		      en.type = EdgeType::CONDITION_2;
		      graph.addEdge(vn1, vn2, en);
		    }
		  else if ((fogsReqs[j].count(req1) && fogsCaches[i].count(req2))
		      || (fogsCaches[j].count(req1) && fogsReqs[i].count(req2))) // condition 3
		    {
		      en.type = EdgeType::CONDITION_3;
		      graph.addEdge(vn1, vn2, en);
		    }
		}
	    }
	}
    }
#if Print_Edge
  graph.printEdge();
#endif

#if Search_Clique
//  std::vector<std::vector<VertexNode>> cliques = graphM.getCliquesWithBA(Num_Cliques);
  cliques.clear();
  cliques = graph.getCliques(Num_Cliques);
#if Print_Cliques
  std::cout << "sim time:" << Simulator::Now().GetSeconds() << ", ";
  graph.printCliques(cliques);
#endif
#endif

#if Construct_Graph_And_Find_Clique_Time_stas
  endTime = clock();
  cout << "sim time: " << Now().GetSeconds() << ", clock: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << endl;
#endif

  // broadcast clique to vehicles
  for (std::vector<VertexNode> clique : cliques)
    {
      broadcastId2cliqueMap.clear();
      currentBroadcastId++;
      broadcastId2cliqueMap.insert(make_pair(currentBroadcastId, clique));
    }

  fogIdx2FogReqInCliqueMaps.clear();
  std::map<uint32_t, std::vector<VertexNode>>::iterator iter = broadcastId2cliqueMap.begin();
  for (; iter != broadcastId2cliqueMap.end(); iter++)
    {
      std::vector<VertexNode> clique = iter->second;

      m_requestStats.IncBroadcastPkts();

      std::map<uint32_t, uint32_t> fogIdx2FogReqInCliqueMap;
      std::set<uint32_t> broadcastData;
      for (VertexNode vertex : clique)
	{
	  broadcastData.insert(vertex.reqDataIndex);
	  fogIdx2FogReqInCliqueMap.insert(make_pair(vertex.fogIndex, vertex.reqDataIndex));
	}
      fogIdx2FogReqInCliqueMaps.insert(make_pair(iter->first, fogIdx2FogReqInCliqueMap));

      if (broadcastData.size() == 0) continue;

      Ptr<UdpSender> sender = CreateObject<UdpSender>();
      sender->SetNode(m_remoteHost);
      sender->SetRemote(Ipv4Address ("10.2.255.255"), m_dlPort);
      sender->Start();
      using vanet::PacketHeader;
      PacketHeader header;
      header.SetType(PacketHeader::MessageType::DATA_C2V);
      header.SetBroadcastId(iter->first);
      sender->SetHeader(header);

      using vanet::PacketTagC2v;
      PacketTagC2v *pktTag = new PacketTagC2v();
      std::vector<uint32_t> reqsIds;
      reqsIds.assign(broadcastData.begin(), broadcastData.end());
      pktTag->SetReqsIds(reqsIds);
      sender->SetPacketTag(pktTag);

//      Simulator::ScheduleNow (&UdpSender::Send, sender);
      Simulator::ScheduleNow (&VanetCsVfcExperiment::ResetStatusAndSend, this, sender);
    }
}

void
VanetCsVfcExperiment::ResetStatusAndSend (Ptr<UdpSender> sender)
{
  isDecoding = false;
  sender->Send();
}

void
VanetCsVfcExperiment::Decode (bool isEncoded, uint32_t broadcastId)
{
  isDecoding = true;
  if (!isEncoded)
    {
//      for (std::vector<VertexNode> clique : cliques)
//	{
//	  for (VertexNode vertex : clique)
//	    {
//	      for (uint32_t obuIdx : fogCluster[vertex.fogIndex])
//		{
//		  if (vehsReqs[obuIdx].count(vertex.reqDataIndex))
//		    {
//		      vehsReqs[obuIdx].erase(vertex.reqDataIndex);
//		      vehsCaches[obuIdx].insert(vertex.reqDataIndex);
//
//		      RecordStats(obuIdx, vertex.reqDataIndex);
//		    }
//		}
//	    }
//	}

      if (broadcastId2cliqueMap.count(broadcastId) == 0) return;
      for (VertexNode vertex : broadcastId2cliqueMap.at(broadcastId))
	{
	  for (uint32_t obuIdx : fogCluster[vertex.fogIndex])
	    {
	      if (vehsReqs[obuIdx].count(vertex.reqDataIndex))
		{
		  vehsReqs[obuIdx].erase(vertex.reqDataIndex);
		  vehsCaches[obuIdx].insert(vertex.reqDataIndex);

		  RecordStats(obuIdx, vertex.reqDataIndex);
		}
	    }
	}

//      for (uint32_t i = 0; i < m_nObuNodes; i++)
//	{
//	  if (vehsReqs[i].count(data))
//	    {
//	      vehsReqs[i].erase(data);
//	      vehsCaches[i].insert(data);
//	    }
//	}
    }
#if 1
  else
    {
      std::set<uint32_t> fogIdxsInCliques; // fog nodes index covered by a clique
      if (broadcastId2cliqueMap.count(broadcastId) == 0) return;
      std::vector<VertexNode> clique = broadcastId2cliqueMap.at(broadcastId);
      for (VertexNode vertex : clique)
	{
	  fogIdxsInCliques.insert(vertex.fogIndex);
	}
      uint32_t size = clique.size();

      for (uint32_t i = 0; i < size; i++)
	{
	  VertexNode vertex = clique[i];
	  for (uint32_t obuIdx : fogCluster[vertex.fogIndex])
	    {
	      if (vehsReqs[obuIdx].count(vertex.reqDataIndex))
		{
		  map<uint32_t, std::set<uint32_t>>::iterator it = datasNeededForDecodingPerClique[obuIdx].find(broadcastId);
		  NS_ASSERT (it != datasNeededForDecodingPerClique[obuIdx].end());
		  it->second.erase(vertex.reqDataIndex);

		  for (uint32_t data : it->second)
		    {
		      if (vehsCaches[obuIdx].count(data))
			{
			  it->second.erase(data);
			}
		      else if (fogsCaches[vertex.fogIndex].count(data))
			{
			  for (uint32_t obuIdx1 : fogCluster[vertex.fogIndex])
			    {
			      if (obuIdx == obuIdx1) continue;
			      if (vehsCaches[obuIdx1].count(data))
				{
				  Ptr<UdpSender> sender = CreateObject<UdpSender>();
				  sender->SetNode(m_obuNodes.Get(obuIdx1));
				  sender->SetRemote(m_rsu80211pInterfaces.GetAddress(vertex.fogIndex), m_v2IPort);
				  sender->Start();
				  using vanet::PacketHeader;
				  PacketHeader header;
				  header.SetType(PacketHeader::MessageType::DATA_V2F);
				  header.SetBroadcastId(broadcastId);
				  sender->SetHeader(header);

				  using vanet::PacketTagV2f;
				  PacketTagV2f *pktTagV2f = new PacketTagV2f();
				  pktTagV2f->SetCurrentEdgeType(EdgeType::NOT_SET);
				  pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2V);
				  std::vector<uint32_t> dataIdxs;
				  dataIdxs.push_back(data);
				  pktTagV2f->SetDataIdxs(dataIdxs);
				  sender->SetPacketTag(pktTagV2f);

				  Simulator::ScheduleNow (&UdpSender::Send, sender);

				  break;
				}
			    }
			}
		    }
		}
	    }
	}

      for (uint32_t i = 0; i < size; i++)
	{
	  VertexNode vertex1 = clique[i];
	  for (uint32_t j = i + 1; j < size; j++)
	    {
	      VertexNode vertex2 = clique[j];
	      EdgeType edgeType = graph.getEdgeType(vertex1, vertex2);
	      switch(edgeType)
	      {
		case EdgeType::CONDITION_1:
		  {
		    bool flag = false;
		    for (uint32_t k = 0; k < m_nRsuNodes; k++)
		      {
			if (flag) break; //avoid sending packet repeatly

			if (k == vertex1.fogIndex
			    || k == vertex2.fogIndex
			    || 0 == fogIdxsInCliques.count(k)) continue;

			if (fogsCaches[k].count(vertex1.reqDataIndex))
			  {
			    for (uint32_t obuIdx : fogCluster[k])
			      {

				if (vehsCaches[obuIdx].count(vertex1.reqDataIndex))
				  {
				    Ptr<UdpSender> sender = CreateObject<UdpSender>();
				    sender->SetNode(m_obuNodes.Get(obuIdx));
				    sender->SetRemote(m_rsu80211pInterfaces.GetAddress(k), m_v2IPort);
				    sender->Start();
				    using vanet::PacketHeader;
				    PacketHeader header;
				    header.SetType(PacketHeader::MessageType::DATA_V2F);
				    header.SetBroadcastId(broadcastId);
				    sender->SetHeader(header);

				    using vanet::PacketTagV2f;
				    PacketTagV2f *pktTagV2f = new PacketTagV2f();
				    pktTagV2f->SetCurrentEdgeType(EdgeType::CONDITION_1);
				    pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2F);
				    std::vector<uint32_t> rsuWaitingServedIdxs;
				    rsuWaitingServedIdxs.push_back(vertex1.fogIndex);
				    rsuWaitingServedIdxs.push_back(vertex2.fogIndex);
				    pktTagV2f->SetRsuWaitingServedIdxs(rsuWaitingServedIdxs);
				    std::vector<uint32_t> dataIdxs;
				    dataIdxs.push_back(vertex1.reqDataIndex);
				    pktTagV2f->SetDataIdxs(dataIdxs);
				    sender->SetPacketTag(pktTagV2f);

				    Simulator::ScheduleNow (&UdpSender::Send, sender);

				    flag = true;
				    break;
				  }
			      }
			  }
		      }
		    break;
		  }
		case EdgeType::CONDITION_2:
		  {
		    bool flag1 = false;
		    for (uint32_t obuIdx : fogCluster[vertex1.fogIndex])
		      {
			if (flag1) break; //avoid sending packet repeatly

			if (vehsCaches[obuIdx].count(vertex2.reqDataIndex))
			  {
			    Ptr<UdpSender> sender = CreateObject<UdpSender>();
			    sender->SetNode(m_obuNodes.Get(obuIdx));
			    sender->SetRemote(m_rsu80211pInterfaces.GetAddress(vertex1.fogIndex), m_v2IPort);
			    sender->Start();
			    using vanet::PacketHeader;
			    PacketHeader header;
			    header.SetType(PacketHeader::MessageType::DATA_V2F);
			    header.SetBroadcastId(broadcastId);
			    sender->SetHeader(header);

			    using vanet::PacketTagV2f;
			    PacketTagV2f *pktTagV2f = new PacketTagV2f();
			    pktTagV2f->SetCurrentEdgeType(EdgeType::CONDITION_2);
			    pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2V);
			    std::vector<uint32_t> dataIdxs;
			    dataIdxs.push_back(vertex2.reqDataIndex);
			    pktTagV2f->SetDataIdxs(dataIdxs);
			    sender->SetPacketTag(pktTagV2f);

			    Simulator::ScheduleNow (&UdpSender::Send, sender);

			    flag1 = true;
			    break;
			  }
		      }

		    bool flag2 = false;
		    for (uint32_t obuIdx : fogCluster[vertex2.fogIndex])
		      {
			if (flag2) break; //avoid sending packet repeatly

			if (vehsCaches[obuIdx].count(vertex1.reqDataIndex))
			  {
			    Ptr<UdpSender> sender = CreateObject<UdpSender>();
			    sender->SetNode(m_obuNodes.Get(obuIdx));
			    sender->SetRemote(m_rsu80211pInterfaces.GetAddress(vertex2.fogIndex), m_v2IPort);
			    sender->Start();
			    using vanet::PacketHeader;
			    PacketHeader header;
			    header.SetType(PacketHeader::MessageType::DATA_V2F);
			    header.SetBroadcastId(broadcastId);
			    sender->SetHeader(header);

			    using vanet::PacketTagV2f;
			    PacketTagV2f *pktTagV2f = new PacketTagV2f();
			    pktTagV2f->SetCurrentEdgeType(EdgeType::CONDITION_2);
			    pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2V);
			    std::vector<uint32_t> dataIdxs;
			    dataIdxs.push_back(vertex1.reqDataIndex);
			    pktTagV2f->SetDataIdxs(dataIdxs);
			    sender->SetPacketTag(pktTagV2f);

			    Simulator::ScheduleNow (&UdpSender::Send, sender);

			    flag2 = true;
			    break;
			  }
		      }
		    break;
		  }
		case EdgeType::CONDITION_3:
		  {
		    bool flag1 = false;
		    for (uint32_t obuIdx : fogCluster[vertex1.fogIndex])
		      {
			if (flag1) break; //avoid sending packet repeatly

			if (vehsCaches[obuIdx].count(vertex2.reqDataIndex))
			  {
			    Ptr<UdpSender> sender = CreateObject<UdpSender>();
			    sender->SetNode(m_obuNodes.Get(obuIdx));
			    sender->SetRemote(m_rsu80211pInterfaces.GetAddress(vertex1.fogIndex), m_v2IPort);
			    sender->Start();
			    using vanet::PacketHeader;
			    PacketHeader header;
			    header.SetType(PacketHeader::MessageType::DATA_V2F);
			    header.SetBroadcastId(broadcastId);
			    sender->SetHeader(header);

			    using vanet::PacketTagV2f;
			    PacketTagV2f *pktTagV2f = new PacketTagV2f();
			    pktTagV2f->SetCurrentEdgeType(EdgeType::CONDITION_3);
			    pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2V);
			    std::vector<uint32_t> rsuWaitingServedIdxs;
			    rsuWaitingServedIdxs.push_back(vertex2.fogIndex);
			    pktTagV2f->SetRsuWaitingServedIdxs(rsuWaitingServedIdxs);
			    std::vector<uint32_t> dataIdxs;
			    dataIdxs.push_back(vertex2.reqDataIndex);
			    pktTagV2f->SetDataIdxs(dataIdxs);
			    sender->SetPacketTag(pktTagV2f);

			    Simulator::ScheduleNow (&UdpSender::Send, sender);

			    flag1 = true;
			    break;
			  }
		      }

		    bool flag2 = false;
		    for (uint32_t obuIdx : fogCluster[vertex2.fogIndex])
		      {
			if (flag2) break; //avoid sending packet repeatly

			if (vehsCaches[obuIdx].count(vertex1.reqDataIndex))
			  {
			    Ptr<UdpSender> sender = CreateObject<UdpSender>();
			    sender->SetNode(m_obuNodes.Get(obuIdx));
			    sender->SetRemote(m_rsu80211pInterfaces.GetAddress(vertex2.fogIndex), m_v2IPort);
			    sender->Start();
			    using vanet::PacketHeader;
			    PacketHeader header;
			    header.SetType(PacketHeader::MessageType::DATA_V2F);
			    header.SetBroadcastId(broadcastId);
			    sender->SetHeader(header);

			    using vanet::PacketTagV2f;
			    PacketTagV2f *pktTagV2f = new PacketTagV2f();
			    pktTagV2f->SetCurrentEdgeType(EdgeType::CONDITION_3);
			    pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2V);
			    std::vector<uint32_t> dataIdxs;
			    dataIdxs.push_back(vertex1.reqDataIndex);
			    pktTagV2f->SetDataIdxs(dataIdxs);
			    sender->SetPacketTag(pktTagV2f);

			    Simulator::ScheduleNow (&UdpSender::Send, sender);

			    flag2 = true;
			    break;
			  }
		      }
		    break;
		  }
		default:
		  break;
	      }
	    }
	}
    }
#endif
}

void VanetCsVfcExperiment::RecordStats (uint32_t obuIdx, uint32_t dataIdx)
{
  std::map<uint32_t, RequestStatus>::iterator it = vehsReqsStatus[obuIdx].find(dataIdx);
  NS_ASSERT_MSG(it != vehsReqsStatus[obuIdx].end(), "iter == end()");
  if (it->second.completed) return;
  it->second.completed = true;
  it->second.satisfiedTime = Now().GetDouble();

  m_requestStats.IncSatisfiedReqs();
  m_requestStats.IncCumulativeDelay(it->second.satisfiedTime - it->second.submitTime);
}

void
VanetCsVfcExperiment::CommandSetup (int argc, char **argv)
{
  CommandLine cmd;

  // allow command line overrides
//  cmd.AddValue ("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
//  cmd.AddValue ("CSVfileName2", "The name of the CSV output file name2", m_CSVfileName2);
  cmd.AddValue ("totaltime", "Simulation end time", m_TotalSimTime);
  cmd.AddValue ("nodes", "Number of nodes (i.e. vehicles)", m_nObuNodes);
  cmd.AddValue ("txp", "Transmit power (dB), e.g. txp=7.5", m_obuTxp);
//  cmd.AddValue ("traceMobility", "Enable mobility tracing", m_traceMobility);
  cmd.AddValue ("phyMode", "Wifi Phy mode", m_phyMode);
  cmd.AddValue ("traceFile", "Ns2 movement trace file", m_mobilityFile);
  cmd.AddValue ("mobTraceFile", "Mobility Trace file", m_mobLogFile);
  cmd.AddValue ("rate", "Rate", m_rate);
  cmd.AddValue ("verbose", "0=quiet;1=verbose", m_verbose);
  cmd.AddValue ("bsm", "(WAVE) BSM size (bytes)", m_wavePacketSize);
  cmd.AddValue ("interval", "(WAVE) BSM interval (s)", m_waveInterval);

  cmd.AddValue ("asciiTrace", "Dump ASCII Trace data", m_asciiTrace);
  cmd.AddValue ("pcap", "Create PCAP files for all nodes", m_pcap);

  cmd.Parse (argc, argv);
}

void
VanetCsVfcExperiment::SetupTraceFile ()
{
  if (m_traceMobility)
    {
//      // open trace file for output
//      m_os.open (m_mobLogFile.c_str ());
//
//      // Configure callback for logging
//      Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
//		       MakeBoundCallback (&VanetCsVfcExperiment::CourseChange, &m_os));

      AsciiTraceHelper ascii;
      MobilityHelper::EnableAsciiAll (ascii.CreateFileStream (m_trName));
    }
}

void
VanetCsVfcExperiment::SetupLogging ()
{
  if (m_log != 0)
    {
//      // open trace file for output
//      m_os.open (m_mobLogFile.c_str ());

      // Enable logging from the ns2 helper
      LogComponentEnable ("Ns2MobilityHelper",LOG_LEVEL_DEBUG);

      Packet::EnablePrinting ();
    }
}

void
VanetCsVfcExperiment::ConfigureDefaults ()
{
  Config::SetDefault ("ns3::OnOffApplication::PacketSize",StringValue ("64"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (m_rate));

  //Set Non-unicastMode rate to unicast mode
//  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",StringValue (m_phyMode));
}

void
VanetCsVfcExperiment::SetupDevices ()
{
  YansWifiChannelHelper wifiChannel1 = YansWifiChannelHelper::Default ();
  wifiChannel1.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel1.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5.9e9));

  YansWifiPhyHelper wifiPhy1 =  YansWifiPhyHelper::Default ();
  Ptr<YansWifiChannel> channel1 = wifiChannel1.Create ();
  wifiPhy1.SetChannel (channel1);
  // ns-3 supports generate a pcap trace
  wifiPhy1.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11);
  NqosWaveMacHelper wifi80211pMac1 = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p1 = Wifi80211pHelper::Default ();
  if (m_verbose)
    {
      wifi80211p1.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
    }

  wifi80211p1.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (m_phyMode),
                                      "ControlMode",StringValue (m_phyMode));

  // OBU 802.11p
  wifiPhy1.Set ("TxPowerStart",DoubleValue (m_obuTxp));
  wifiPhy1.Set ("TxPowerEnd", DoubleValue (m_obuTxp));
  m_obuDevices = wifi80211p1.Install (wifiPhy1, wifi80211pMac1, m_obuNodes);

  // RSU 802.11p
  wifiPhy1.Set ("TxPowerStart",DoubleValue (m_rsuTxp));
  wifiPhy1.Set ("TxPowerEnd", DoubleValue (m_rsuTxp));
  m_rsu80211pDevices = wifi80211p1.Install (wifiPhy1, wifi80211pMac1, m_rsuNodes);

  // RSU CSMA
  CsmaHelper csmaHelper;
  csmaHelper.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));
  csmaHelper.SetChannelAttribute ("Delay", StringValue ("2ms"));
  m_rsuCsmaDevices = csmaHelper.Install(m_rsuNodes);

  InternetStackHelper internet;
  internet.Install(m_obuNodes);
  internet.Install(m_rsuNodes);

  Ipv4AddressHelper addressHelper;
//  // Assign Ip to OBUs and RSUs
//  NS_LOG_UNCOND ("Assigning IP addresses");
//  // we may have a lot of nodes, and want them all
//  // in same subnet, to support broadcast
//  addressHelper.SetBase ("10.2.0.0", "255.255.0.0");
//  m_obuInterfaces = addressHelper.Assign (m_obuDevices);
//  m_rsu80211pInterfaces = addressHelper.Assign (m_rsu80211pDevices);

  addressHelper.SetBase ("192.168.0.0", "255.255.0.0");
  m_rsuCsmaInterfaces = addressHelper.Assign (m_rsuCsmaDevices);
  // Tracing
//  wifiPhy.EnablePcap ("wave-simple-80211p", devices);

  // configure mobility
  SetupObuMobilityNodes ();
  SetupRsuMobilityNodes ();

#if Lte_Enable
  // -------------------------------------------LTE----------------------------------------------------
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity",UintegerValue (320));

  m_lteHelper = CreateObject<LteHelper> ();
  m_epcHelper = CreateObject<PointToPointEpcHelper> ();
  m_lteHelper->SetEpcHelper (m_epcHelper);

  m_pgw = m_epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  m_remoteHostNodes.Create (1);
  m_remoteHost = m_remoteHostNodes.Get (0);
  internet.Install (m_remoteHostNodes);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (m_pgw, m_remoteHost);
  addressHelper.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = addressHelper.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  m_remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (m_remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  m_enbNodes.Create(1);
  m_ueNodes.Add(m_obuNodes);

  // configure mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc3 = CreateObject<ListPositionAllocator> ();
  positionAlloc3->Add (Vector (1800.0, 1800.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc3);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_remoteHost);

  positionAlloc3 = CreateObject<ListPositionAllocator> ();
  positionAlloc3->Add (Vector (1800.0, 1600.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc3);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_pgw);

  positionAlloc3 = CreateObject<ListPositionAllocator> ();
  positionAlloc3->Add (Vector (1600, 1600, 0.0));
  mobility.SetPositionAllocator (positionAlloc3);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_enbNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer m_enbLteDevices = m_lteHelper->InstallEnbDevice (m_enbNodes);
  NetDeviceContainer m_ueLteDevices = m_lteHelper->InstallUeDevice (m_ueNodes);

  // Assign Ip to UE
  m_ueInterface = m_epcHelper->AssignUeIpv4Address (m_ueLteDevices);

  for (uint32_t u = 0; u < m_ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = m_ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (m_epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE to eNodeB
  m_lteHelper->Attach (m_ueLteDevices);  // side effect: the default EPS bearer will be activated
#else
  // Create a single RemoteHost
  m_remoteHostNodes.Create (1);
  m_remoteHost = m_remoteHostNodes.Get (0);

  m_ueNodes.Add(m_obuNodes);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (1600.0, 1600.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_remoteHost);

  // BS
  YansWifiChannelHelper wifiChannel2 = YansWifiChannelHelper::Default ();
  wifiChannel2.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel2.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

  YansWifiPhyHelper wifiPhy2 =  YansWifiPhyHelper::Default ();
  Ptr<YansWifiChannel> channel2 = wifiChannel2.Create ();
  wifiPhy2.SetChannel (channel2);
  // ns-3 supports generate a pcap trace
  wifiPhy2.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11);
  NqosWaveMacHelper wifi80211pMac2 = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p2 = Wifi80211pHelper::Default ();


  wifi80211p2.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (m_phyMode),
                                      "ControlMode",StringValue (m_phyMode));

  wifiPhy2.Set ("TxPowerStart",DoubleValue (m_bsTxp));
  wifiPhy2.Set ("TxPowerEnd", DoubleValue (m_bsTxp));
  NetDeviceContainer remoteDev = wifi80211p1.Install (wifiPhy2, wifi80211pMac2, m_remoteHostNodes);

  internet.Install (m_remoteHostNodes);
//  addressHelper.SetBase ("1.0.0.0", "255.0.0.0");
  addressHelper.SetBase ("10.2.0.0", "255.255.0.0");
  Ipv4InterfaceContainer internetIpIfaces = addressHelper.Assign (remoteDev);
  m_remoteHostAddr = internetIpIfaces.GetAddress (0);

//  Ipv4StaticRoutingHelper ipv4RoutingHelper;
//  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (m_remoteHost->GetObject<Ipv4> ());
//  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("10.2.0.0"), Ipv4Mask ("255.255.0.0"), 1);

  // OBU 802.11p NetDevice communicating with BS
  wifiPhy2.Set ("TxPowerStart",DoubleValue (m_obuTxp));
  wifiPhy2.Set ("TxPowerEnd", DoubleValue (m_obuTxp));
  NetDeviceContainer m_ueLteDevices = wifi80211p2.Install (wifiPhy2, wifi80211pMac2, m_ueNodes);
  m_ueInterface = addressHelper.Assign (m_ueLteDevices);
#endif

  // DSRC's ip address assignment must be placed after "epcHelper->AssignUeIpv4Address", otherwise UE's upload line will be invalid, why??????
  // Assign Ip to OBUs and RSUs
  // we may have a lot of nodes, and want them all
  // in same subnet, to support broadcast
  addressHelper.SetBase ("10.3.0.0", "255.255.0.0");
  m_rsu80211pInterfaces = addressHelper.Assign (m_rsu80211pDevices);
  m_obuInterfaces = addressHelper.Assign (m_obuDevices);
}

void
VanetCsVfcExperiment::AssignIpAddresses ()
{
  // todo
}
void
VanetCsVfcExperiment::SetupObuMobilityNodes ()
{
  // Create Ns2MobilityHelper with the specified trace log file as parameter
  Ns2MobilityHelper ns2 = Ns2MobilityHelper (m_mobilityFile);
  ns2.Install (m_obuNodes.Begin(), m_obuNodes.End()); // configure movements for each OBU node, while reading trace file

//  nodesMoving.resize (m_nObuNodes, 0);

  NodeContainer::Iterator i = m_obuNodes.Begin ();
  for (; i != m_obuNodes.End(); ++i)
    {
      Ptr<Node> node = (*i);
      uint32_t id = node->GetId();
      std::ostringstream oss;
      oss << "/NodeList/" << id << "/$ns3::MobilityModel/CourseChange";
      Config::Connect (oss.str(), MakeCallback (&VanetCsVfcExperiment::CourseChange, this));
    }
}

void
VanetCsVfcExperiment::SetupRsuMobilityNodes ()
{
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  int size = 4;
  double base = -300.0;
  double x = base, y = base, delta = 1000.0;
  for (int i = 0; i < size; i++)
    {
      x = x + delta;
      y = base;
      for (int j = 0; j < size; j++)
	{
	  y = y + delta;
	  positionAlloc->Add (Vector (x, y, 0.0));
	}
    }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_rsuNodes);
}

Ptr<Socket>
VanetCsVfcExperiment::SetupPacketSink (Ipv4Address addr, uint16_t port, Ptr<Node> node)
{
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback (&VanetCsVfcExperiment::ReceivePacket, this));

  return sink;
}

void
VanetCsVfcExperiment::SetupMessages ()
{
//  // Setup routing transmissions
//  OnOffHelper onoff ("ns3::UdpSocketFactory",Address ());
//  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
//  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
//
//  // Setup OBU sink on 80211p devices
//  for (uint32_t i = 0; i < m_nObuNodes; i++)
//    {
//      Ptr<Socket> sink = SetupPacketSink (Ipv4Address::GetAny (), m_port, m_obuNodes.Get (i));
//    }
//
//  // Setup RSU sink on CSMA devices
//  for (uint32_t i = 0; i < m_nRsuNodes; i++)
//    {
//      Ptr<Socket> sink = SetupPacketSink (m_rsuCsmaInterfaces.GetAddress(i), m_port, m_rsuNodes.Get (i));
//    }
//
//  // test RSU broadcast
//  Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
//  for (uint32_t i = 0; i < m_nRsuNodes; i++)
//    {
//      var->SetStream (i);
//      AddressValue remoteAddress (InetSocketAddress ("10.2.255.255", m_port));
//      onoff.SetAttribute ("Remote", remoteAddress);
//
//      ApplicationContainer temp = onoff.Install (m_rsuNodes.Get (i));
//      temp.Start (Seconds (var->GetValue (1.0,2.0)));
//      temp.Stop (Seconds (m_TotalSimTime));
//    }
//
//  // test LTE message
//  uint16_t dlPort = 1234;
//  for (uint32_t i =0; i < m_nRsuNodes; i++)
//    {
//      Ptr<Socket> sink = SetupPacketSink (Ipv4Address::GetAny(), dlPort, m_ueNodes.Get (i));
//    }
//  Ptr<UniformRandomVariable> var1 = CreateObject<UniformRandomVariable> ();
//  for (uint32_t i = 0; i < m_ueInterface.GetN(); i++)
//    {
//      var1->SetStream (i);
//      InetSocketAddress dest = InetSocketAddress (m_ueInterface.GetAddress(i), dlPort);
//      onoff.SetAttribute ("Remote", AddressValue(dest));
//      ApplicationContainer temp = onoff.Install (m_remoteHostNodes.Get(0));
//      temp.Start (Seconds (var1->GetValue (1.0,2.0)));
//      temp.Stop (Seconds (10));
//    }

  std::cout << "sim time:" << Simulator::Now().GetSeconds() << ", event: send data." << std::endl;

  Ptr<UdpSender> sender = CreateObject<UdpSender>();
  sender->SetNode(m_remoteHost);
  sender->SetRemote(Ipv4Address ("10.2.255.255"), m_dlPort);
  sender->Start();
  using vanet::PacketHeader;
  PacketHeader header;
  header.SetType(PacketHeader::MessageType::DATA_C2V);
  sender->SetHeader(header);

  /**
   * broadcast data size (uint32_t):	4 * byte
   * broadcast data (uint32_t):		4 * byte * broadcastData.size()
   */
  uint32_t dataSize = 4 + 4;
  vanet::ByteBuffer bytes(dataSize);
  bytes.WriteU32(1);
  bytes.WriteU32(static_cast<uint32_t>(Simulator::Now().GetSeconds()));
  sender->SetFill(bytes.GetBufferData(), dataSize);
  Simulator::ScheduleNow (&UdpSender::Send, sender);
}

static inline std::string
PrintReceivedPacket (Ptr<Socket> socket, Ptr<Packet> packet, Address srcAddress)
{
  std::ostringstream oss;

  oss << "time=" << Simulator::Now ().GetSeconds () << "s " << "node " << socket->GetNode ()->GetId ();

  if (InetSocketAddress::IsMatchingType (srcAddress))
    {
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (srcAddress);
      oss << " received one packet from " << addr.GetIpv4 ();
    }
  else
    {
      oss << " received one packet!";
    }
  return oss.str ();
}

void
VanetCsVfcExperiment::ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address srcAddress;
  while ((packet = socket->RecvFrom (srcAddress)))
    {
      uint32_t RxRoutingBytes = packet->GetSize ();
      m_routingStats.IncRxBytes (RxRoutingBytes);
      m_routingStats.IncRxPkts ();
      if (m_log != 0)
        {
          NS_LOG_UNCOND ("protocol=" << m_protocolName + " " + PrintReceivedPacket (socket, packet, srcAddress));
        }
    }
}

void
VanetCsVfcExperiment::ReceivePacketWithAddr (std::string context, Ptr<const Packet> packet, const Address & srcAddr, const Address & destAddr)
{
  size_t start = context.find("/", 0);
  start = context.find("/", start + 1);
  size_t end = context.find("/", start + 1);
  std::string nodeIdStr = context.substr(start + 1, end - start - 1);
  uint32_t nodeId = 0;
  sscanf(nodeIdStr.c_str(), "%d", &nodeId);

//  Ptr<Node> node = NodeList::GetNode(nodeId);

  std::ostringstream oss;
  oss << "sim time:" <<Simulator::Now ().GetSeconds () << ", clock: " << (double)(clock()) / CLOCKS_PER_SEC;
#if Print_Log_Header_On_Receive
  if (InetSocketAddress::IsMatchingType (srcAddr))
    {
      InetSocketAddress inetSrcAddr = InetSocketAddress::ConvertFrom (srcAddr);
      oss << " src: " << inetSrcAddr.GetIpv4 ();
    }
  if (InetSocketAddress::IsMatchingType (destAddr))
    {
      InetSocketAddress inetDestAddr = InetSocketAddress::ConvertFrom (destAddr);
      oss << " dest: " << inetDestAddr.GetIpv4 ();
    }
#endif
  using vanet::PacketHeader;
  PacketHeader recvHeader;
  Ptr<Packet> pktCopy = packet->Copy();
  pktCopy->RemoveHeader(recvHeader);
  if (recvHeader.GetType() == PacketHeader::MessageType::REQUEST)
    {
      uint32_t pktSize = pktCopy->GetSize();
      uint8_t buffer[pktSize];
      pktCopy->CopyData(buffer, pktSize);
      vanet::ByteBuffer bytes(buffer, pktSize);

      uint32_t obuId = bytes.ReadU32();
      uint32_t obuIdx = vehId2IndexMap.at(obuId);
      Vector3D obuPos (bytes.ReadDouble(), bytes.ReadDouble(), 0);
      vehsMobInfoInCloud[obuIdx] = obuPos;

      uint32_t reqsSize = bytes.ReadU32();
      for (uint32_t i = 0; i < reqsSize; i++)
	{
	  vehsReqsInCloud[obuIdx].insert(bytes.ReadU32());
	}
      uint32_t cachesSize = bytes.ReadU32();
      for (uint32_t i = 0; i < cachesSize; i++)
	{
	  vehsCachesInCloud[obuIdx].insert(bytes.ReadU32());
	}

#if Print_Received_Data_Cloud
      oss << " veh_id=" << obuId
	  << " veh_pos_x=" << obuPos.x
	  << " veh_pos_y=" << obuPos.y;
      oss << std::endl;
      oss << "requests:";
      for (uint32_t req : vehsReqsInCloud[obuIdx])
	{
	  oss << " " << req;
	}
      oss << std::endl;
      oss << "caches:";
      for (uint32_t cache : vehsCachesInCloud[obuIdx])
	{
	  oss << " " << cache;
	}
#endif
      NS_LOG_UNCOND(oss.str ());
    }
  else if (recvHeader.GetType() == PacketHeader::MessageType::DATA_C2V)
    {
//      receive_count++;

      using vanet::PacketTagC2v;
      PacketTagC2v pktTagC2v;
      pktCopy->RemovePacketTag(pktTagC2v);
      std::vector<uint32_t> reqIds = pktTagC2v.GetReqsIds();
      uint32_t broadcastDataNum = reqIds.size();

    //  std::ostringstream oss;
    //  oss << "sim time:" <<Simulator::Now ().GetSeconds ();
    //  oss << ", reqIds:";
    //  for (uint32_t reqId : reqIds)
    //    {
    //      oss << " " << reqId;
    //    }
    //  NS_LOG_UNCOND(oss.str());

      if (broadcastDataNum == 0) return;

      // Judging whether the broadcasted data is encoded or not
      bool isEncoded = broadcastDataNum == 1 ? false : true;

      if (!isDecoding)
	{
	  for (uint32_t i = 0; i < m_nObuNodes; i++)
	    {
	      std::set<uint32_t> datasNeeded;
	      datasNeeded.insert(reqIds.begin(), reqIds.end());
	      datasNeededForDecodingPerClique[i].insert(make_pair(recvHeader.GetBroadcastId(), datasNeeded));
	    }
	  Decode(isEncoded, recvHeader.GetBroadcastId());
	}
    }
  else if (recvHeader.GetType() == PacketHeader::MessageType::DATA_V2F)
    {
      receive_count++;
      using vanet::PacketTagV2f;
      PacketTagV2f pktTagV2f;
      pktCopy->RemovePacketTag(pktTagV2f);

//      std::cout << oss.str ();
//      std::cout << V2F "fogIdx:" << nodeId <<",";
//      pktTag.Print(std::cout);
//      std::cout << endl;

      EdgeType edgeType = pktTagV2f.GetCurrentEdgeType();
      PacketTagV2f::NextActionType nextAction = pktTagV2f.GetNextActionType();
      if (edgeType == EdgeType::NOT_SET)
	{
	  if (nextAction == PacketTagV2f::NextActionType::F2V)
	    {
	      Ptr<UdpSender> sender = CreateObject<UdpSender>();
	      sender->SetNode(NodeList::GetNode(nodeId));
	      sender->SetRemote(Ipv4Address("10.3.255.255"), m_i2VPort);
	      sender->Start();
	      using vanet::PacketHeader;
	      PacketHeader header;
	      header.SetType(PacketHeader::MessageType::DATA_F2V);
	      header.SetBroadcastId(recvHeader.GetBroadcastId());
	      sender->SetHeader(header);

	      using vanet::PacketTagF2v;
	      PacketTagF2v *pktTagF2v = new PacketTagF2v();
	      pktTagF2v->SetCurrentEdgeType(EdgeType::NOT_SET);
	      pktTagF2v->SetFogId(nodeId);
	      pktTagF2v->SetDataIdxs(pktTagV2f.GetDataIdxs());
	      sender->SetPacketTag(pktTagF2v);

	      Simulator::ScheduleNow (&UdpSender::Send, sender);
	    }
	}
      else if (edgeType == EdgeType::CONDITION_1)
	{
	  std::vector<uint32_t> rsuWaitingServedIdxs = pktTagV2f.GetRsuWaitingServedIdxs();
	  for (uint32_t rsuIdx : rsuWaitingServedIdxs)
	    {
	      Ptr<UdpSender> sender = CreateObject<UdpSender>();
	      sender->SetNode(NodeList::GetNode(nodeId));
	      sender->SetRemote(m_rsuCsmaInterfaces.GetAddress(rsuIdx), m_i2IPort);
	      sender->Start();
	      using vanet::PacketHeader;
	      PacketHeader header;
	      header.SetType(PacketHeader::MessageType::DATA_F2F);
	      header.SetBroadcastId(recvHeader.GetBroadcastId());
	      sender->SetHeader(header);

	      using vanet::PacketTagF2f;
	      PacketTagF2f *pktTagF2f = new PacketTagF2f();
	      pktTagF2f->SetCurrentEdgeType(EdgeType::CONDITION_1);
	      pktTagF2f->SetDataIdxs(pktTagV2f.GetDataIdxs());
	      sender->SetPacketTag(pktTagF2f);

	      Simulator::ScheduleNow (&UdpSender::Send, sender);
	    }
	}
//      else if (nextAction == PacketTagV2f::NextActionType::F2V)
      else if (edgeType == EdgeType::CONDITION_2)
	{
	  Ptr<UdpSender> sender = CreateObject<UdpSender>();
	  sender->SetNode(NodeList::GetNode(nodeId));
	  sender->SetRemote(Ipv4Address("10.3.255.255"), m_i2VPort);
	  sender->Start();
	  using vanet::PacketHeader;
	  PacketHeader header;
	  header.SetType(PacketHeader::MessageType::DATA_F2V);
	  header.SetBroadcastId(recvHeader.GetBroadcastId());
	  sender->SetHeader(header);

	  using vanet::PacketTagF2v;
	  PacketTagF2v *pktTagF2v = new PacketTagF2v();
	  pktTagF2v->SetCurrentEdgeType(EdgeType::CONDITION_2);
//	  pktTagF2v->SetPreActionType(PacketTagF2v::PreActionType::V2F);
	  pktTagF2v->SetFogId(nodeId);
	  pktTagF2v->SetDataIdxs(pktTagV2f.GetDataIdxs());
	  sender->SetPacketTag(pktTagF2v);

	  Simulator::ScheduleNow (&UdpSender::Send, sender);
	}
      else if (edgeType == EdgeType::CONDITION_3)
	{
	  if (nextAction == PacketTagV2f::NextActionType::F2V)
	    {
	      Ptr<UdpSender> sender = CreateObject<UdpSender>();
	      sender->SetNode(NodeList::GetNode(nodeId));
	      sender->SetRemote(Ipv4Address("10.3.255.255"), m_i2VPort);
	      sender->Start();
	      using vanet::PacketHeader;
	      PacketHeader header;
	      header.SetType(PacketHeader::MessageType::DATA_F2V);
	      header.SetBroadcastId(recvHeader.GetBroadcastId());
	      sender->SetHeader(header);

	      using vanet::PacketTagF2v;
	      PacketTagF2v *pktTagF2v = new PacketTagF2v();
	      pktTagF2v->SetCurrentEdgeType(EdgeType::CONDITION_3);
	      pktTagF2v->SetFogId(nodeId);
	      pktTagF2v->SetRsuWaitingServedIdxs(pktTagV2f.GetRsuWaitingServedIdxs());
	      pktTagF2v->SetDataIdxs(pktTagV2f.GetDataIdxs());
	      sender->SetPacketTag(pktTagF2v);

	      Simulator::ScheduleNow (&UdpSender::Send, sender);
	    }
	  else if (nextAction == PacketTagV2f::NextActionType::F2F)
	    {

	    }
	}

    }
  else if (recvHeader.GetType() == PacketHeader::MessageType::DATA_F2F)
    {
      receive_count++;
      using vanet::PacketTagF2f;
      PacketTagF2f pktTagF2f;
      pktCopy->RemovePacketTag(pktTagF2f);

//      std::cout << oss.str ();
//      std::cout << " F2F fogIdx:" << nodeId <<",";
//      pktTag.Print(std::cout);
//      std::cout << endl;

      EdgeType edgeType = pktTagF2f.GetCurrentEdgeType();
      if (edgeType == EdgeType::CONDITION_1)
	{
	  Ptr<UdpSender> sender = CreateObject<UdpSender>();
	  sender->SetNode(NodeList::GetNode(nodeId));
	  sender->SetRemote(Ipv4Address ("10.3.255.255"), m_i2VPort);
	  sender->Start();
	  using vanet::PacketHeader;
	  PacketHeader header;
	  header.SetType(PacketHeader::MessageType::DATA_F2V);
	  header.SetBroadcastId(recvHeader.GetBroadcastId());
	  sender->SetHeader(header);

	  using vanet::PacketTagF2v;
	  PacketTagF2v *pktTagF2v = new PacketTagF2v();
	  pktTagF2v->SetCurrentEdgeType(EdgeType::CONDITION_1);
	  pktTagF2v->SetFogId(nodeId);
	  pktTagF2v->SetDataIdxs(pktTagF2f.GetDataIdxs());
	  sender->SetPacketTag(pktTagF2v);

	  Simulator::ScheduleNow (&UdpSender::Send, sender);
	}
      else if (edgeType == EdgeType::CONDITION_3)
	{
	  Ptr<UdpSender> sender = CreateObject<UdpSender>();
	  sender->SetNode(NodeList::GetNode(nodeId));
	  sender->SetRemote(Ipv4Address ("10.3.255.255"), m_i2VPort);
	  sender->Start();
	  using vanet::PacketHeader;
	  PacketHeader header;
	  header.SetType(PacketHeader::MessageType::DATA_F2V);
	  header.SetBroadcastId(recvHeader.GetBroadcastId());
	  sender->SetHeader(header);

	  using vanet::PacketTagF2v;
	  PacketTagF2v *pktTagF2v = new PacketTagF2v();
	  pktTagF2v->SetCurrentEdgeType(EdgeType::CONDITION_3);
	  pktTagF2v->SetFogId(nodeId);
	  pktTagF2v->SetDataIdxs(pktTagF2f.GetDataIdxs());
	  sender->SetPacketTag(pktTagF2v);

	  Simulator::ScheduleNow (&UdpSender::Send, sender);
	}
    }
  else if (recvHeader.GetType() == PacketHeader::MessageType::DATA_F2V)
    {
      receive_count++;
      using vanet::PacketTagF2v;
      PacketTagF2v pktTagF2v;
      pktCopy->RemovePacketTag(pktTagF2v);

//      std::cout << oss.str ();
//      std::cout << " F2V obuIdx:" << nodeId <<",";
//      pktTagF2v.Print(std::cout);
//      std::cout << endl;

      uint32_t fogId = pktTagF2v.GetFogId();
      Ptr<Node> rsu = NodeList::GetNode(fogId);
      Ptr<MobilityModel> rsuMobility = rsu->GetObject<MobilityModel> ();
      Vector pos_rsu = rsuMobility->GetPosition ();

      Ptr<Node> obu = NodeList::GetNode(nodeId);
      Ptr<MobilityModel> obuMobility = obu->GetObject<MobilityModel> ();
      Vector pos_obu = obuMobility->GetPosition ();

      if (CalculateDistance (pos_obu, pos_rsu) > Device_Transmission_Range )
	{
	  return;
	}

      EdgeType edgeType = pktTagF2v.GetCurrentEdgeType();
      if (edgeType == EdgeType::NOT_SET)
	{
	  uint32_t fogIdx = fogId2FogIdxMap.at(pktTagF2v.GetFogId());
	  uint32_t obuIdx = vehId2IndexMap.at(nodeId);

	  if (fogIdx2FogReqInCliqueMaps.count(recvHeader.GetBroadcastId()) == 0) return;
	  uint32_t fogReqIdx = fogIdx2FogReqInCliqueMaps.at(recvHeader.GetBroadcastId()).at(fogIdx);
	  std::vector<uint32_t> dataIdxs = pktTagF2v.GetDataIdxs();
	  for (uint32_t dataIdx : dataIdxs)
	    {
	      if (vehsReqs[obuIdx].count(fogReqIdx) != 0)
		{
		  if (vehsCaches[obuIdx].count(dataIdx) == 0)
		    {
		      vehsCaches[obuIdx].insert(dataIdx);
		    }
		  if (vehsReqs[obuIdx].count(dataIdx) != 0)
		    {
		      vehsReqs[obuIdx].erase(dataIdx);
		      RecordStats(obuIdx, dataIdx);
		    }

		  map<uint32_t, std::set<uint32_t>>::iterator it = datasNeededForDecodingPerClique[obuIdx].find(recvHeader.GetBroadcastId());
		  NS_ASSERT (it != datasNeededForDecodingPerClique[obuIdx].end());
		  it->second.erase(dataIdx);
		  if (it->second.size() == 0)
		    {
		      if (vehsCaches[obuIdx].count(fogReqIdx) == 0)
			{
			  vehsCaches[obuIdx].insert(fogReqIdx);
			}
		      if (vehsReqs[obuIdx].count(fogReqIdx) != 0)
			{
			  vehsReqs[obuIdx].erase(fogReqIdx);
			  RecordStats(obuIdx, fogReqIdx);
			}
		    }
		}
	    }
	}
      else if (edgeType == EdgeType::CONDITION_1)
	{
	  std::vector<uint32_t> dataIdxs = pktTagF2v.GetDataIdxs();
	  for (uint32_t dataIdx : dataIdxs)
	    {
	      uint32_t obuIdx = vehId2IndexMap.at(nodeId);
	      if (vehsReqs[obuIdx].count(dataIdx) != 0)
		{
		  if (vehsCaches[obuIdx].count(dataIdx) == 0)
		    {
		      vehsCaches[obuIdx].insert(dataIdx);
		    }
		  if (vehsReqs[obuIdx].count(dataIdx) != 0)
		    {
		      vehsReqs[obuIdx].erase(dataIdx);
		      RecordStats(obuIdx, dataIdx);
		    }

		  map<uint32_t, std::set<uint32_t>>::iterator it = datasNeededForDecodingPerClique[obuIdx].find(recvHeader.GetBroadcastId());
		  NS_ASSERT (it != datasNeededForDecodingPerClique[obuIdx].end());
		  it->second.erase(dataIdx);
		}
	    }
	}
      else if (edgeType == EdgeType::CONDITION_2)
	{
	  uint32_t fogIdx = fogId2FogIdxMap.at(pktTagF2v.GetFogId());
	  uint32_t obuIdx = vehId2IndexMap.at(nodeId);

	  if (fogIdx2FogReqInCliqueMaps.count(recvHeader.GetBroadcastId()) == 0) return;
	  uint32_t fogReqIdx = fogIdx2FogReqInCliqueMaps.at(recvHeader.GetBroadcastId()).at(fogIdx);
	  std::vector<uint32_t> dataIdxs = pktTagF2v.GetDataIdxs();
	  for (uint32_t dataIdx : dataIdxs)
	    {
	      if (vehsReqs[obuIdx].count(fogReqIdx) != 0)
		{
		  if (vehsCaches[obuIdx].count(dataIdx) == 0)
		    {
		      vehsCaches[obuIdx].insert(dataIdx);
		    }
		  if (vehsReqs[obuIdx].count(dataIdx) != 0)
		    {
		      vehsReqs[obuIdx].erase(dataIdx);
		      RecordStats(obuIdx, dataIdx);
		    }

		  map<uint32_t, std::set<uint32_t>>::iterator it = datasNeededForDecodingPerClique[obuIdx].find(recvHeader.GetBroadcastId());
		  NS_ASSERT (it != datasNeededForDecodingPerClique[obuIdx].end());
		  it->second.erase(dataIdx);
		  if (it->second.size() == 0)
		    {
		      if (vehsCaches[obuIdx].count(fogReqIdx) == 0)
			{
			  vehsCaches[obuIdx].insert(fogReqIdx);
			}
		      if (vehsReqs[obuIdx].count(fogReqIdx) != 0)
			{
			  vehsReqs[obuIdx].erase(fogReqIdx);
			  RecordStats(obuIdx, fogReqIdx);
			}
		    }
		}
	    }
	}
      else if (edgeType == EdgeType::CONDITION_3)
	{
	  uint32_t fogIdx = fogId2FogIdxMap.at(pktTagF2v.GetFogId());
	  uint32_t obuIdx = vehId2IndexMap.at(nodeId);

	  if (fogIdx2FogReqInCliqueMaps.count(recvHeader.GetBroadcastId()) == 0) return;
	  uint32_t fogReqIdx = fogIdx2FogReqInCliqueMaps.at(recvHeader.GetBroadcastId()).at(fogIdx);
	  std::vector<uint32_t> dataIdxs = pktTagF2v.GetDataIdxs();
	  for (uint32_t dataIdx : dataIdxs)
	    {
	      if (vehsReqs[obuIdx].count(fogReqIdx) != 0)
		{
		  if (vehsCaches[obuIdx].count(dataIdx) == 0)
		    {
		      vehsCaches[obuIdx].insert(dataIdx);
		    }
		  if (vehsReqs[obuIdx].count(dataIdx) != 0)
		    {
		      vehsReqs[obuIdx].erase(dataIdx);
		      RecordStats(obuIdx, dataIdx);
		    }

		  map<uint32_t, std::set<uint32_t>>::iterator it = datasNeededForDecodingPerClique[obuIdx].find(recvHeader.GetBroadcastId());
		  NS_ASSERT (it != datasNeededForDecodingPerClique[obuIdx].end());
		  it->second.erase(dataIdx);
		  if (it->second.size() == 0)
		    {
		      if (vehsCaches[obuIdx].count(fogReqIdx) == 0)
			{
			  vehsCaches[obuIdx].insert(fogReqIdx);
			}
		      if (vehsReqs[obuIdx].count(fogReqIdx) != 0)
			{
			  vehsReqs[obuIdx].erase(fogReqIdx);
			  RecordStats(obuIdx, fogReqIdx);
			}
		    }

		  if (pktTagF2v.GetRsuWaitingServedIdxs().size() != 0)
		    {

		      Ptr<UdpSender> sender = CreateObject<UdpSender>();
		      sender->SetNode(NodeList::GetNode(nodeId));
		      sender->SetRemote(m_rsu80211pInterfaces.GetAddress(fogIdx), m_v2IPort);
		      sender->Start();
		      using vanet::PacketHeader;
		      PacketHeader header;
		      header.SetType(PacketHeader::MessageType::DATA_V2F);
		      header.SetBroadcastId(recvHeader.GetBroadcastId());
		      sender->SetHeader(header);

		      using vanet::PacketTagV2f;
		      PacketTagV2f *pktTagV2f = new PacketTagV2f();
		      pktTagV2f->SetCurrentEdgeType(EdgeType::CONDITION_3);
		      pktTagV2f->SetNextActionType(PacketTagV2f::NextActionType::F2F);
		      pktTagV2f->SetRsuWaitingServedIdxs(pktTagF2v.GetRsuWaitingServedIdxs());
		      pktTagV2f->SetDataIdxs(dataIdxs);
		      sender->SetPacketTag(pktTagV2f);

		      Simulator::ScheduleNow (&UdpSender::Send, sender);
		    }
		}
	    }
	}
    }
  NS_LOG_UNCOND(oss.str ());
}

void
VanetCsVfcExperiment::OnOffTrace (std::string context, Ptr<const Packet> packet)
{
  uint32_t pktBytes = packet->GetSize ();
  m_routingStats.IncTxBytes (pktBytes);
}

void
VanetCsVfcExperiment::SetupScenario ()
{
//  m_nObuNodes = 288;
//  m_obuTxp = 35.0; // 300m
//  m_nRsuNodes = 16;
//  m_rsuTxp = 35.0; // 31:300m, 35:400m, 36: 500m, 38: 600m, 45: 1000m
//
//  m_mobilityFile = m_workspacePath + "/mobility" + "/mobility.tcl";
//  m_mobLogFile = m_workspacePath + "/mobility.log";
//  m_TotalSimTime = 315.01;
}

int
main (int argc, char *argv[])
{
  VanetCsVfcExperiment experiment;
  experiment.Simulate (argc, argv);
  return 0;
}
