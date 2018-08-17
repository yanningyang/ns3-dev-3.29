#include <iostream>

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/ssid.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wifi-net-device.h"
#include "ns3/netanim-module.h"
#include "ns3/lte-module.h"
#include "ns3/point-to-point-helper.h"
#include "udp-sender.h"
#include "utils.hpp"
#include "byte-buffer.h"

#define NS2_MOBILITY			false
#define Wifi_80211p			true
#define Wifi_80211x			false

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WifiSimpleOcb");


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

uint32_t packet_count = 0;
void
TwoAddressTrace (std::string context, Ptr<const Packet> packet, const Address & srcAddr, const Address & destAddr)
{
  packet_count++;

  std::ostringstream oss;
  oss << "sim time:" <<Simulator::Now ().GetSeconds () << ", clock: " << (double)(clock()) / CLOCKS_PER_SEC;
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

  using vanet::PacketHeader;
  PacketHeader header;
  Ptr<Packet> pktCopy = packet->Copy();
  pktCopy->RemoveHeader(header);
  if (header.GetType() == PacketHeader::MessageType::DATA)
    {
//      packet_count++;
      uint32_t pktSize = pktCopy->GetSize();
      uint8_t buffer[pktSize];
      pktCopy->CopyData(buffer, pktSize);
      vanet::ByteBuffer bytes(buffer, pktSize);

      uint32_t cliqueSize = bytes.ReadU32();
      oss << " clique=";
      for (uint32_t i = 0; i < cliqueSize; i++)
	{
	  oss << " " << bytes.ReadU32();
	}
    }
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

void
SendData (Ptr<Socket> socket)
{
  Ptr<Packet> packet = Create<Packet> (100);
  socket->Send (packet);
}

void
BroadCastData (Ptr<Node> remoteHost)
{
  std::cout << "sim time:" << Simulator::Now().GetSeconds() << ", event: send data." << std::endl;

//  Ptr<UdpSender> sender = CreateObject<UdpSender>();
//  sender->SetNode(remoteHost);
//  sender->SetRemote(Ipv4Address ("10.1.255.255"), 80);
//  sender->Start();
//  using vanet::TypeHeader;
//  TypeHeader header;
//  header.SetType(TypeHeader::MessageType::DATA);
//  sender->SetHeader(header);
//
//  /**
//   * broadcast data size (uint32_t):	4 * byte
//   * broadcast data (uint32_t):		4 * byte * broadcastData.size()
//   */
//  uint32_t dataSize = 4 + 4;
//  vanet::ByteBuffer bytes(dataSize);
//  bytes.WriteU32(1);
//  bytes.WriteU32(static_cast<uint32_t>(Simulator::Now().GetSeconds()));
//  sender->SetFill(bytes.GetBufferData(), dataSize);
//  Simulator::Schedule(Seconds(Simulator::Now().GetSeconds() + 0.01), &UdpSender::Send, sender);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  for (uint32_t i = 0; i < 1; ++i)
    {
      Ptr<Socket> source = Socket::CreateSocket (remoteHost, tid);
      source->SetAllowBroadcast (true);
    //  InetSocketAddress remote = InetSocketAddress (interface_80211_obu.GetAddress (0), 80);
      InetSocketAddress remote = InetSocketAddress (Ipv4Address::GetBroadcast(), 80);
//      InetSocketAddress remote = InetSocketAddress ("10.1.255.255", 80);
      source->Connect (remote);
      Simulator::ScheduleNow (&SendData, source);
    }

//  Simulator::Schedule(Seconds(1.0), &BroadCastData, remoteHost);
}

int main (int argc, char *argv[])
{
  std::string phyMode ("OfdmRate6MbpsBW10MHz");
  std::string socketType ("ns3::UdpSocketFactory");
//  std::string socketType ("ns3::PacketSocketFactory");
  uint32_t packetSize = 1000; // bytes
  uint32_t numPackets = 20;
  double interval = 1.0; // seconds
  bool verbose = false;
  double txp = 29.0;
  double distance = 29.0;
  double intervalTime = 0.1;
  uint32_t nObu = 20;

  std::map<std::uint32_t, std::vector<std::uint8_t *>> cache;

  CommandLine cmd;

  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.AddValue ("txp", "txp", txp);
  cmd.AddValue ("distance", "distance", distance);
  cmd.AddValue ("intervalTime", "broadcast interval time", intervalTime);
  cmd.AddValue ("nObu", "number of obu", nObu);
  cmd.Parse (argc, argv);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  //Set Non-unicastMode rate to unicast mode
//  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",StringValue (phyMode));

  NodeContainer nodes;
  NodeContainer obuNodes;
  obuNodes.Create (nObu);
  NodeContainer rsuNodes;
  rsuNodes.Create (1);
  nodes.Add(rsuNodes);
  nodes.Add(obuNodes);

  NetDeviceContainer devices_80211_rsu;
  NetDeviceContainer devices_80211_obu;

  Ipv4InterfaceContainer interface_80211_rsu;
  Ipv4InterfaceContainer interface_80211_obu;

//  PacketSocketHelper packetSocket;

#if Wifi_80211p
  // The below set of helpers will help us to put together the wifi NICs we want
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5.9e9)); // txp 137 450m
//  wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel", "Frequency", DoubleValue (5.9e9), "HeightAboveZ", DoubleValue (1.5));

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
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
  wifiPhy.Set ("TxPowerStart",DoubleValue (29));
  wifiPhy.Set ("TxPowerEnd", DoubleValue (29));

  devices_80211_rsu = wifi80211p.Install (wifiPhy, wifi80211pMac, rsuNodes);
  devices_80211_obu = wifi80211p.Install (wifiPhy, wifi80211pMac, obuNodes);
#endif

#if Wifi_80211x
//  // give packet socket powers to nodes.
//  packetSocket.Install (obuNodes);
//  packetSocket.Install (rsuNodes);

  WifiHelper wifi;
  WifiMacHelper wifiMac;

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());

  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  phyMode = "OfdmRate6Mbps";
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));

  Ssid ssid = Ssid ("wifi-default");
  // setup stas.
  wifiMac.SetType ("ns3::StaWifiMac",
                   "ActiveProbing", BooleanValue (true),
                   "Ssid", SsidValue (ssid));
  devices_80211_obu = wifi.Install (wifiPhy, wifiMac, obuNodes);
  // setup ap.
  wifiMac.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));
  devices_80211_rsu =wifi.Install (wifiPhy, wifiMac, rsuNodes);
#endif

//  // Adjust Tx Power 1
//  Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(devices_80211p.Get(2));
//  device->GetPhy()->SetTxPowerStart(txp);
//  device->GetPhy()->SetTxPowerEnd(txp);

  // Adjust Tx Power 2
  Config::Set("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerStart", DoubleValue (txp));
  Config::Set("/NodeList/*/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerEnd", DoubleValue (txp));


#if NS2_MOBILITY
  // Create Ns2MobilityHelper with the specified trace log file as parameter
  Ns2MobilityHelper ns2 = Ns2MobilityHelper ("/home/haha/ns3.29-dev-workspace/workspace/vanet-cs-vfc/mobility/mobility.tcl");
  ns2.Install (obuNodes.Begin(), obuNodes.End()); // configure movements for each OBU node, while reading trace file

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (1600.0, 1600.0, 0.0));

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (rsuNodes);
#else
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (1600.0, 1600.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (rsuNodes);

  positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (1600.0, 1600.0 - distance, 0.0));
//  positionAlloc->Add (Vector (3000.0, 0.0, 0.0));
//  positionAlloc->Add (Vector (0.0, 3000.0, 0.0));

//  for (uint32_t i = 0; i < obuNodes.GetN (); ++i)
//    {
//      positionAlloc->Add (Vector (0.0 + i * 20, 0.0, 0.0));
//    }

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (obuNodes);
#endif

  InternetStackHelper internet;
  internet.Install (rsuNodes);
  internet.Install (obuNodes);

  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.0.0", "255.255.0.0");
  interface_80211_rsu = ipv4.Assign (devices_80211_rsu);
  interface_80211_obu = ipv4.Assign (devices_80211_obu);

  TypeId tid = TypeId::LookupByName (socketType);
//  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
//  for (uint32_t i =0; i < nodes.GetN(); i++)
//    {
//      Ptr<Socket> recvSink = Socket::CreateSocket (nodes.Get (i), tid);
//      recvSink->Bind (local);
//      recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
//    }

//  Ptr<Socket> source = Socket::CreateSocket (obuNodes.Get (0), tid);
//  source->SetAllowBroadcast (true);
//  InetSocketAddress remote = InetSocketAddress (interface_80211p_obu.GetAddress(1), 80);
//  source->Connect (remote);
//
//  Ptr<Socket> source1 = Socket::CreateSocket (obuNodes.Get (2), tid);
//  source1->SetAllowBroadcast (true);
//  InetSocketAddress remote2 = InetSocketAddress (interface_80211p_obu.GetAddress(1), 80);
//  source1->Connect (remote2);

//  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
//                                  Seconds (1.0), &GenerateTraffic,
//                                  source, packetSize, numPackets, interPacketInterval);

  ApplicationContainer serverApps;
//  PacketSinkHelper rsuPacketSinkHelper (socketType, InetSocketAddress (interface_80211_rsu.GetAddress(0), 80));
  PacketSinkHelper rsuPacketSinkHelper (socketType, InetSocketAddress (Ipv4Address::GetAny(), 80));
  serverApps.Add (rsuPacketSinkHelper.Install (rsuNodes.Get(0)));
  for (uint32_t i = 0; i < obuNodes.GetN (); ++i)
    {
//      PacketSinkHelper obuPacketSinkHelper (socketType, InetSocketAddress (interface_80211_obu.GetAddress(i), 80));
      PacketSinkHelper obuPacketSinkHelper (socketType, InetSocketAddress (Ipv4Address::GetAny(), 80));
//      PacketSinkHelper obuPacketSinkHelper (socketType, InetSocketAddress ("10.1.0.0", 80));
      serverApps.Add (obuPacketSinkHelper.Install (obuNodes.Get(i)));
    }
  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/RxWithAddresses", MakeCallback (&TwoAddressTrace));
  serverApps.Start (Seconds (0.01));

#if 0
  for (uint32_t i = 0; i < obuNodes.GetN (); ++i)
    {
//      Ptr<UdpSender> sender = CreateObject<UdpSender>();
//      sender->SetNode(obuNodes.Get (i));
//      sender->SetRemote(interface_80211p_rsu.GetAddress(0), 80);
//      sender->Start();
//      Simulator::Schedule(Seconds(1), &UdpSender::Send, sender);

      Ptr<Socket> source = Socket::CreateSocket (obuNodes.Get (i), tid);
      source->SetAllowBroadcast (true);
      InetSocketAddress remote = InetSocketAddress (interface_80211_rsu.GetAddress(0), 80);
      source->Connect (remote);
      Simulator::Schedule(Seconds(1 + i * 0.00), &SendData, source);
    }
#endif

#if 0
  for (uint32_t i = 0; i < 1; ++i)
    {
      Ptr<Socket> source = Socket::CreateSocket (rsuNodes.Get (0), tid);
      source->SetAllowBroadcast (true);
      InetSocketAddress remote = InetSocketAddress (interface_80211_obu.GetAddress (0), 80);
//      InetSocketAddress remote = InetSocketAddress ("10.1.255.255", 80);
      source->Connect (remote);
//      Simulator::Schedule(Seconds(Now().GetSeconds() + 0.01), &SendData, source);

      Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
				      Seconds (1.0), &GenerateTraffic,
				      source, packetSize, numPackets, interPacketInterval);
    }
#endif

  Simulator::Schedule(Seconds(0.01), &BroadCastData, rsuNodes.Get (0));


  // -----------------------------------------XOR-----------------------------------------------------
//  std::string a = "hahale()11*#&*&*";
//  std::string b = "wowo";
//  size_t size = std::max(a.length(), b.length()) + 1;
//  uint8_t c[size] = {0};
//  uint8_t d[size] = {0};
//
//  NS_LOG_UNCOND("d " << XOR(XOR(a, b, c), b, size, d));
//  NS_LOG_UNCOND("test " << ((12345 ^ 98765) ^ 12345));
//
//  NS_LOG_UNCOND("test float " << ((double)997 / 998));


  Simulator::Stop(Seconds(110));
  Simulator::Run ();
  Simulator::Destroy ();

  std::cout << "packet count: " << packet_count << std::endl;

  return 0;
}
