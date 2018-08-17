/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
 * Copyright (c) 2013 Dalian University of Technology
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Author: Junling Bu <linlinjavaer@gmail.com>
 *
 */
/**
 * This example shows basic construction of an 802.11p node.  Two nodes
 * are constructed with 802.11p devices, and by default, one node sends a single
 * packet to another node (the number of packets and interval between
 * them can be configured by command-line arguments).  The example shows
 * typical usage of the helper classes for this mode of WiFi (where "OCB" refers
 * to "Outside the Context of a BSS")."
 */

#include <iostream>

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wifi-net-device.h"
#include "ns3/netanim-module.h"
#include "ns3/lte-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"

using namespace ns3;

#define NS2_MOBILITY 1

NS_LOG_COMPONENT_DEFINE ("VanetDemoCase3");

class StatsTag : public Tag
{
public:
  StatsTag (void)
    : m_packetId (0),
      m_sendTime (Seconds (0)),
      m_packetType(0),
	  m_nodeId(0)
  {
  }
  StatsTag (uint32_t packetId, Time sendTime,uint32_t pactetType,uint32_t nodeid)
    : m_packetId (packetId),
      m_sendTime (sendTime),
	  m_packetType(pactetType),
	  m_nodeId(nodeid)
  {
  }
  virtual ~StatsTag (void)
  {
  }

  uint32_t GetPacketId (void)
  {
    return m_packetId;
  }
  Time GetSendTime (void)
  {
    return m_sendTime;
  }

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

	uint32_t getPacketType() const {
		return m_packetType;
	}

	void setPackettType(uint32_t packetType) {
		m_packetType = packetType;
	}

	uint32_t getNodeId() const {
		return m_nodeId;
	}

	void setNodeId(uint32_t nodeId) {
		this->m_nodeId = nodeId;
	}

private:
  uint32_t m_packetId;
  Time m_sendTime;
  uint32_t m_packetType;
  uint32_t m_nodeId;
};
TypeId
StatsTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::StatsTag")
    .SetParent<Tag> ()
    .AddConstructor<StatsTag> ()
  ;
  return tid;
}
TypeId
StatsTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
StatsTag::GetSerializedSize (void) const
{
  return 3*sizeof (uint32_t) + sizeof (uint64_t);
}
void
StatsTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_packetId);
  i.WriteU64 (m_sendTime.GetMicroSeconds ());
  i.WriteU32 (m_packetType);
  i.WriteU32(m_nodeId);
}
void
StatsTag::Deserialize (TagBuffer i)
{
  m_packetId = i.ReadU32 ();
  m_sendTime = MicroSeconds (i.ReadU64 ());
  m_packetType = i.ReadU32 ();
  m_nodeId=  i.ReadU32 ();
}
void
StatsTag::Print (std::ostream &os) const
{
  os << "packet=" << m_packetId << " sendTime=" << m_sendTime<< "  packetType="<<m_packetType;
}

static inline std::string
PrintReceivedPacket (Ptr<Socket> socket, Ptr<Packet> packet, Address srcAddress)
{
  std::ostringstream oss;

  oss << Simulator::Now ().GetSeconds () << " node " << socket->GetNode ()->GetId ();

  if (InetSocketAddress::IsMatchingType (srcAddress))
    {
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (srcAddress);
      oss << " received one packet from " << addr.GetIpv4 ();
      uint8_t buffer[packet->GetSize()];
      packet->CopyData(buffer, packet->GetSize());
      oss << ". data: " << buffer;
    }
  else
    {
      oss << " received one packet!";
    }
  return oss.str ();
}

void
ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address srcAddress;
  while ((packet = socket->RecvFrom (srcAddress)))
    {
//      uint32_t RxRoutingBytes = packet->GetSize ();

      NS_LOG_UNCOND (PrintReceivedPacket (socket, packet, srcAddress));
    }
}

uint32_t pktCount = 0;
void
TwoAddressTrace (std::string context, Ptr<const Packet> packet, const Address & srcAddr, const Address & destAddr)
{
  pktCount++;
  std::cout << "count: " << pktCount << std::endl;

  std::ostringstream oss;

  oss << Simulator::Now ().GetSeconds () << ", clock: " << (double)clock() / CLOCKS_PER_SEC;
  if (InetSocketAddress::IsMatchingType (srcAddr))
    {
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (srcAddr);
      oss << " src: " << addr.GetIpv4 ();
    }
  if (InetSocketAddress::IsMatchingType (destAddr))
    {
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (destAddr);
      oss << " dest: " << addr.GetIpv4 ();
    }

  uint8_t buffer[packet->GetSize() + 1];
  packet->CopyData(buffer, packet->GetSize());
  buffer[packet->GetSize()] = '\0';
  oss << ". data: " << buffer;

  NS_LOG_UNCOND(oss.str ());
}

static void
GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval )
{
  if (pktCount > 0)
    {
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pktSize,pktCount - 1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}

uint32_t m_dataSize = 0; //!< packet payload size (must be equal to m_size)
uint8_t *m_data = 0; //!< packet payload data
void
SetFill (std::string fill)
{
  uint32_t dataSize = fill.size () + 1;

  if (dataSize != m_dataSize)
    {
      delete [] m_data;
      m_data = new uint8_t [dataSize];
      m_dataSize = dataSize;
    }

  memcpy (m_data, fill.c_str (), dataSize);
}

void
SendData (Ptr<Socket> socket, std::string data)
{
  SetFill(data);
  Ptr<Packet> packet = Create<Packet> (m_data, m_dataSize);
  socket->Send (packet);
//  socket->Close ();
}

void
SendData2 (Ptr<Socket> socket, uint32_t dataSize)
{
  std::cout << "send time " << Now().GetSeconds() << ", clock: " << (double)(clock()) / CLOCKS_PER_SEC << std::endl;

  Ptr<Packet> packet = Create<Packet> (dataSize);
  socket->Send (packet);
//  socket->Close ();
}

int main (int argc, char *argv[])
{
  double simTime = 5.01;
  uint32_t numOfRsu = 3;
  uint32_t numOfObu = 25;
  uint32_t numOfEnb = 1;
  std::string phyMode ("OfdmRate6MbpsBW10MHz");
  uint32_t packetSize = 1000; // bytes
  uint32_t numPackets = 10;
  double interval = 1.0; // seconds
  bool verbose = false;
  double rsuTxp = 20.0; // 26:250m, 20:140m
  double enbY = 1000.0;
  double rsu1Y = 100.0;
  double obu1Y = rsu1Y + 120.0;
  bool useCa = false;  //whether to use carrier aggregation

  std::vector<uint16_t> database;
  for (uint16_t i = 0; i < 10; i++)
    {
      database.push_back(i);
    }

  CommandLine cmd;

  cmd.AddValue ("numOfRsu", "number of rsu", numOfRsu);
  cmd.AddValue ("numOfObu", "number of obu", numOfObu);
  cmd.AddValue ("numOfEnb", "number of eNodeb", numOfEnb);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.AddValue ("rsuTxp", "rsu Transmition Power", rsuTxp);
  cmd.AddValue ("obu1Y", "obu1Y", obu1Y);
  cmd.AddValue ("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.AddValue ("simTime", "time of simulation", simTime);
  cmd.Parse (argc, argv);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  // Parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }

  //Set Non-unicastMode rate to unicast mode
//  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",StringValue (phyMode));

  NodeContainer nodes;
  NodeContainer rsuNodes;
  NodeContainer obuNodes;

  NetDeviceContainer rsu_devices_80211p;
  NetDeviceContainer obu_devices_80211p;
  NetDeviceContainer enbLteDevs;
  NetDeviceContainer ueLteDevs;

  Ipv4InterfaceContainer rsu_iface_80211p;
  Ipv4InterfaceContainer obu_iface_80211p;
  Ipv4InterfaceContainer ueIpIface;

  InternetStackHelper internet;
  Ipv4AddressHelper ipv4AddrHelper;

  MobilityHelper mobility;

  rsuNodes.Create (numOfRsu);
  obuNodes.Create (numOfObu);
  nodes.Add(rsuNodes);
  nodes.Add(obuNodes);

  for (uint32_t i = 0; i < rsuNodes.GetN (); ++i)
    {
      std::ostringstream oss;
      oss << "rsu_" << i;
      Names::Add (oss.str(), rsuNodes.Get(i));
    }
  for (uint32_t i = 0; i < obuNodes.GetN (); ++i)
    {
      std::ostringstream oss;
      oss << "obu_" << i;
      Names::Add (oss.str(), obuNodes.Get(i));
    }

  //----------------------------------------------------------------------------------------------------//
  //						802.11p							//
  //----------------------------------------------------------------------------------------------------//
  // The below set of helpers will help us to put together the wifi NICs we want
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wifiPhy.SetChannel (channel);
  // ns-3 supports generate a pcap trace
  wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11);
  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
  if (verbose)
    {
      wifi80211p.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
    }

  wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (phyMode),
                                      "ControlMode",StringValue (phyMode));

  // Set Tx Power
  wifiPhy.Set ("TxPowerStart",DoubleValue (rsuTxp));
  wifiPhy.Set ("TxPowerEnd", DoubleValue (rsuTxp));

  rsu_devices_80211p = wifi80211p.Install (wifiPhy, wifi80211pMac, rsuNodes);
  obu_devices_80211p = wifi80211p.Install (wifiPhy, wifi80211pMac, obuNodes);

  for (uint32_t i = 0; i < rsuNodes.GetN (); ++i)
    {
      Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(rsu_devices_80211p.Get(i));
      device->GetPhy()->SetTxPowerStart(rsuTxp);
      device->GetPhy()->SetTxPowerEnd(rsuTxp);
    }

  Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator> ();
  for (uint32_t i = 0; i < rsuNodes.GetN (); ++i)
    {
      positionAlloc1->Add (Vector (i * 500.0, rsu1Y, 0.0));
    }
  mobility.SetPositionAllocator (positionAlloc1);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (rsuNodes);

#if NS2_MOBILITY
  // Create Ns2MobilityHelper with the specified trace log file as parameter
  Ns2MobilityHelper ns2 = Ns2MobilityHelper ("/home/haha/ns3.29-dev-workspace/workspace/vanet-cs-vfc/mobility/mobility.tcl");
  ns2.Install (obuNodes.Begin(), obuNodes.End()); // configure movements for each OBU node, while reading trace file
#else
  Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();
  double y_offset = 50.0;
  uint32_t nInLine = 20.0;
  for (uint32_t i = 0; i < obuNodes.GetN (); ++i)
    {
      if ((i % nInLine) == 0)
	{
	  obu1Y += y_offset;
	}
      positionAlloc2->Add (Vector ((i % nInLine) * 100.0, obu1Y, 0.0));
    }
  mobility.SetPositionAllocator (positionAlloc2);
//  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (obuNodes);
//  for (uint32_t i = 0; i < obuNodes.GetN (); ++i)
//    {
//      Ptr<Node> obu = obuNodes.Get(i);
//      obu->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (20, 0, 0));
//    }
#endif

  internet.Install (nodes);

//  ipv4AddrHelper.SetBase ("192.168.1.0", "255.255.255.0");
//  rsu_iface_80211p = ipv4AddrHelper.Assign (rsu_devices_80211p);
//  obu_iface_80211p = ipv4AddrHelper.Assign (obu_devices_80211p);

  //----------------------------------------------------------------------------------------------------//
  //						LTE							//
  //----------------------------------------------------------------------------------------------------//
//  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity",StringValue ("80"));

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  ipv4AddrHelper.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4AddrHelper.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(numOfEnb);
  ueNodes.Add(obuNodes);

  Ptr<ListPositionAllocator> positionAlloc3 = CreateObject<ListPositionAllocator> ();
  positionAlloc3->Add (Vector (450.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc3);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (remoteHostContainer);

  positionAlloc3 = CreateObject<ListPositionAllocator> ();
  positionAlloc3->Add (Vector (650.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc3);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (pgw);

  positionAlloc3 = CreateObject<ListPositionAllocator> ();
  double x_enb = 0.0;
  double y_enb = enbY;
  for (uint32_t i = 0; i < enbNodes.GetN (); ++i)
    {
      if (i % 2 == 0)
	{
	  x_enb = 1000;
	  y_enb += (i == 0) ? 0 : 1000;
	}
      else
	{
	  x_enb += 1000;
	}
      positionAlloc3->Add (Vector (x_enb, y_enb, 0.0));
    }
  mobility.SetPositionAllocator (positionAlloc3);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);

  // Install LTE Devices to the nodes
  enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  ueIpIface = epcHelper->AssignUeIpv4Address (ueLteDevs);

  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

//  // Attach UEs to eNodeB
//  uint32_t start_j = 0;
//  for (uint32_t i = 0; i < enbNodes.GetN(); i++)
//    {
//      uint32_t j = 0;
//      for (j = start_j; (j < start_j + 200 && j < ueNodes.GetN()); j++)
//	{
//	  lteHelper->Attach (ueLteDevs.Get(j), enbLteDevs.Get(i));  // side effect: the default EPS bearer will be activated
//	}
//      start_j = j;
//    }
  lteHelper->Attach(ueLteDevs);

  // Must be placed after "epcHelper->AssignUeIpv4Address", otherwise UE's upload line will be invalid, why??????
  ipv4AddrHelper.SetBase ("10.2.0.0", "255.255.0.0");
  rsu_iface_80211p = ipv4AddrHelper.Assign (rsu_devices_80211p);
  obu_iface_80211p = ipv4AddrHelper.Assign (obu_devices_80211p);

//  PointToPointHelper p2p;
//  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
//  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
//  std::vector<Ipv4InterfaceContainer> p2pIpIfaceVector;
//  for (uint32_t i = 0; i < rsuNodes.GetN(); i++)
//    {
//      NodeContainer rsuToRemote = NodeContainer (rsuNodes.Get(i), remoteHost);
//      NetDeviceContainer p2pDevices = p2p.Install (rsuToRemote);
//      std::ostringstream oss;
//      oss << "10.1." << i << ".0";
//      ipv4AddrHelper.SetBase (oss.str().c_str(), "255.255.255.0");
//      p2pIpIfaceVector.push_back(ipv4AddrHelper.Assign (p2pDevices));
//    }

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1234;
  uint16_t ulPort = 2000;
  uint16_t v2vPort = 3000;
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
//  for (uint32_t i =0; i < ueNodes.GetN (); i++)
//    {
//      InetSocketAddress dlLocal = InetSocketAddress (Ipv4Address::GetAny (), dlPort);
//      Ptr<Socket> recvSink = Socket::CreateSocket (ueNodes.Get (i), tid);
//      recvSink->Bind (dlLocal);
//      recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
//    }

  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (ueIpIface.GetAddress(u), dlPort));
      serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
      PacketSinkHelper obuPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (obu_iface_80211p.GetAddress(u), v2vPort));
      serverApps.Add (obuPacketSinkHelper.Install (ueNodes.Get(u)));
    }
  PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (remoteHostAddr, ulPort));
  serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/RxWithAddresses", MakeCallback (&TwoAddressTrace));
  serverApps.Start (Seconds (0.01));

#if 0
  // Activate Dedicated Eps Bearer
  Ptr<EpcTft> tft = Create<EpcTft> ();
  EpcTft::PacketFilter dlpf;
  dlpf.localPortStart = dlPort;
  dlpf.localPortEnd = dlPort;
  tft->Add (dlpf);
  EpcTft::PacketFilter ulpf;
  ulpf.remotePortStart = ulPort;
  ulpf.remotePortEnd = ulPort;
  tft->Add (ulpf);
  EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  lteHelper->ActivateDedicatedEpsBearer (ueLteDevs, bearer, tft);
#endif

#if 0
  Ptr<Socket> obuSource = Socket::CreateSocket (obuNodes.Get(0), tid);
  InetSocketAddress obuRemote = InetSocketAddress (obu_iface_80211p.GetAddress(1), v2vPort);
  obuSource->Connect (obuRemote);
  for (uint32_t i = 1; i < 5; ++i)
    {
      Simulator::Schedule(Seconds(i), &SendData, obuSource, "obu-test");
    }
#endif

#if 1
//  for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
//    {
//      Ptr<Socket> dlSource = Socket::CreateSocket (remoteHost, tid);
//      InetSocketAddress dlRemote = InetSocketAddress (ueIpIface.GetAddress(i), dlPort);
//      dlSource->SetAllowBroadcast(true);
//      dlSource->Connect (dlRemote);
//      Simulator::Schedule(Seconds(1 + i * 0.0), &SendData, dlSource, "lte-test-dl");
//    }
    Ptr<Socket> dlSource = Socket::CreateSocket (remoteHost, tid);
    InetSocketAddress dlRemote = InetSocketAddress (Ipv4Address("255.255.255.255"), dlPort);
    dlSource->SetAllowBroadcast(true);
    dlSource->Connect (dlRemote);
    Simulator::Schedule(Seconds(1), &SendData, dlSource, "lte-test-dl");
#endif

#if 0
  for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
    {
      if (i != 0 ) continue;
      Ptr<Socket> ulSource = Socket::CreateSocket (ueNodes.Get(i), tid);
      InetSocketAddress ulRemote = InetSocketAddress (remoteHostAddr, ulPort);
      ulSource->Connect (ulRemote);
      for (uint32_t j = 0; j < 1000; j++)
	{
	  Simulator::Schedule(Seconds(1 + j * 0.001), &SendData2, ulSource, 1000);
	}
    }
#endif

//  Simulator::Schedule(Seconds(1), &SendData, ulSource, "obu0 request data");
//  Simulator::Schedule(Seconds(1), &SendData, ulSource, "BS schedule rsu1");
//  Simulator::Schedule(Seconds(1), &SendData, ulSource, "rsu1 get data form obu1");
//  Simulator::Schedule(Seconds(1), &SendData, ulSource, "rsu1 deliver data to BS");
//  Simulator::Schedule(Seconds(1), &SendData, ulSource, "BS deliver data to rsu0");
//  Simulator::Schedule(Seconds(1), &SendData, ulSource, "rsu0 deliver data to obu0");

  //----------------------------------------------------------------------------------------------------//
  //						Animation						//
  //----------------------------------------------------------------------------------------------------//
  AnimationInterface anim ("wireless-animation.xml");
  uint32_t resId1 = anim.AddResource("/home/haha/Pictures/icon/car-003.png");
  uint32_t resId2 = anim.AddResource("/home/haha/Pictures/icon/cellular-001.png");
  uint32_t resId3 = anim.AddResource("/home/haha/Pictures/icon/rsu-001.png");
  uint32_t resId4 = anim.AddResource("/home/haha/Pictures/icon/pgw.png");
  uint32_t resId5 = anim.AddResource("/home/haha/Pictures/icon/cloud.png");

  for (uint32_t i = 0; i < rsuNodes.GetN(); ++i)
    {
      Ptr<Node> rsu = rsuNodes.Get(i);
      anim.UpdateNodeDescription (rsu, "RSU");
      anim.UpdateNodeSize(rsu->GetId(), 50, 50);
      anim.UpdateNodeImage(rsu->GetId(), resId3);
    }
  for (uint32_t i = 0; i < obuNodes.GetN(); ++i)
    {
      Ptr<Node> obu = obuNodes.Get(i);
      anim.UpdateNodeDescription (obu, "OBU");
      anim.UpdateNodeSize(obu->GetId(), 50, 50);
      anim.UpdateNodeImage(obu->GetId(), resId1);
    }

  for (uint32_t i = 0; i < enbNodes.GetN(); ++i)
    {
      Ptr<Node> enb = enbNodes.Get(i);
      anim.UpdateNodeDescription (enb, "eNB");
      anim.UpdateNodeSize(enb->GetId(), 50, 50);
      anim.UpdateNodeImage(enb->GetId(), resId2);
    }

  anim.UpdateNodeDescription (remoteHost, "Remote Host");
  anim.UpdateNodeSize(remoteHost->GetId(), 50, 50);
  anim.UpdateNodeImage(remoteHost->GetId(), resId5);

  anim.UpdateNodeDescription (pgw, "MME/PGW");
  anim.UpdateNodeSize(pgw->GetId(), 50, 50);
  anim.UpdateNodeImage(pgw->GetId(), resId4);

  anim.EnablePacketMetadata ();

  Simulator::Stop(Seconds(simTime));

  clock_t startTime, endTime;
  startTime = clock();
  std::cout << "simulation start time " << Now().GetSeconds() << ", clock: " << (double)(startTime) / CLOCKS_PER_SEC << std::endl;

  Simulator::Run ();

  endTime = clock();
  std::cout << "simulation end time " << Now().GetSeconds() << ", clock: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << std::endl;

  Simulator::Destroy ();

  return 0;
}
