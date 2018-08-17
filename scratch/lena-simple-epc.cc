/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jaume.nin@cttc.cat>
 */

#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("EpcFirstExample");

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
//  Ptr<Node> node;
  Address srcAddress;
//  Address destAddress;

//  node = socket->GetNode ();
//  Ptr<NetDevice> device = node->GetDevice(i);
//  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
//  int32_t interface = ipv4->GetInterfaceForDevice (device);
//  destAddress = ipv4->GetAddress(interface, 0).GetLocal();
  while ((packet = socket->RecvFrom (srcAddress)))
    {
      NS_LOG_UNCOND (PrintReceivedPacket (socket, packet, srcAddress));
    }
}

void
TwoAddressTrace (std::string context, Ptr<const Packet> packet, const Address & srcAddr, const Address & destAddr)
{
  std::ostringstream oss;

  oss << Simulator::Now ().GetSeconds ();
  if (InetSocketAddress::IsMatchingType (destAddr))
      {
        InetSocketAddress addr = InetSocketAddress::ConvertFrom (destAddr);
        oss << " " << addr.GetIpv4 ();
      }

  if (InetSocketAddress::IsMatchingType (srcAddr))
    {
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (srcAddr);
      oss << " received one packet from " << addr.GetIpv4 ();
      uint8_t buffer[packet->GetSize() + 1];
      packet->CopyData(buffer, packet->GetSize());
      buffer[packet->GetSize()] = '\0';
      oss << ". data: " << buffer;
    }
  else
    {
      oss << " received one packet!";
    }
  NS_LOG_UNCOND(oss.str ());
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

int
main (int argc, char *argv[])
{

  uint16_t numberOfNodes = 2;
  double simTime = 3.1;
  double distance = 60.0;
  double ueToEnbDistance = 200.0;
  double interPacketInterval = 1000;
  bool useCa = false;

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("ueToEnbDistance", "Distance between eNB and UE [m]", ueToEnbDistance);
  cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
  cmd.AddValue("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.Parse(argc, argv);

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(numberOfNodes);
  ueNodes.Create(numberOfNodes);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfNodes; i++)
    {
      positionAlloc1->Add (Vector(distance * i, 0, 0));
    }
  Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator> ();
    for (uint16_t i = 0; i < numberOfNodes; i++)
      {
        positionAlloc2->Add (Vector(distance * i, ueToEnbDistance, 0));
      }
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc1);
  mobility.Install(enbNodes);
  mobility.SetPositionAllocator(positionAlloc2);
  mobility.Install(ueNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < numberOfNodes; i++)
      {
        lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
        // side effect: the default EPS bearer will be activated
      }


  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1234;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (ueIpIface.GetAddress(u), dlPort));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (remoteHostAddr, ulPort));
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));


      serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
      serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u)));

//      UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
//      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
//      dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
//
//      UdpClientHelper ulClient (remoteHostAddr, ulPort);
//      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
//      ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
//
//      UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
//      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
//      client.SetAttribute ("MaxPackets", UintegerValue(1000000));
//
//      clientApps.Add (dlClient.Install (remoteHost));
//      clientApps.Add (ulClient.Install (ueNodes.Get(u)));
//      if (u+1 < ueNodes.GetN ())
//        {
//          clientApps.Add (client.Install (ueNodes.Get(u+1)));
//        }
//      else
//        {
//          clientApps.Add (client.Install (ueNodes.Get(0)));
//        }
    }

  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/RxWithAddresses", MakeCallback (&TwoAddressTrace));

  serverApps.Start (Seconds (0.01));
//  clientApps.Start (Seconds (0.01));

    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
//    InetSocketAddress ulLocal = InetSocketAddress (Ipv4Address::GetAny (), ulPort);
//    Ptr<Socket> ulRecvSink = Socket::CreateSocket (remoteHost, tid);
//    ulRecvSink->Bind (ulLocal);
//    ulRecvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

    Ptr<Socket> dlSource = Socket::CreateSocket (remoteHost, tid);
    InetSocketAddress dlRemote = InetSocketAddress (ueIpIface.GetAddress(0), dlPort);
    dlSource->Connect (dlRemote);

    Simulator::Schedule(Seconds(1), &SendData, dlSource, "haha-dl");
    Simulator::Schedule(Seconds(2), &SendData, dlSource, "test-dl");
    Simulator::Schedule(Seconds(10), &SendData, dlSource, "hehe-dl");

    Ptr<Socket> ulSource = Socket::CreateSocket (ueNodes.Get(0), tid);
    InetSocketAddress ulRemote = InetSocketAddress (remoteHostAddr, ulPort);
    ulSource->Connect (ulRemote);

    Simulator::Schedule(Seconds(1), &SendData, ulSource, "haha-ul");
    Simulator::Schedule(Seconds(2), &SendData, ulSource, "test-ul");
    Simulator::Schedule(Seconds(10), &SendData, ulSource, "hehe-ul");

//  p2ph.EnablePcapAll("lena-epc-first");


  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy();
  return 0;

}

