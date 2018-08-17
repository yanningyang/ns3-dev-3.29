/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Dalian University of Technology
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
 * Author: Junling Bu <linlinjavaer@gmail.com>
 */
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/command-line.h"
#include "ns3/mobility-model.h"
#include <iostream>
#include <algorithm>
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/object-map.h"
#include "ns3/regular-wifi-mac.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/llc-snap-header.h"
#include "ns3/wave-net-device.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wave-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WaveMultipleChannel");

class StatsTag : public Tag
{
public:
  StatsTag (void)
    : m_packetId (0),
      m_sendTime (Seconds (0))
  {
  }
  StatsTag (uint32_t packetId, Time sendTime)
    : m_packetId (packetId),
      m_sendTime (sendTime)
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

private:
  uint32_t m_packetId;
  Time m_sendTime;
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
  return sizeof (uint32_t) + sizeof (uint64_t);
}
void
StatsTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_packetId);
  i.WriteU64 (m_sendTime.GetMicroSeconds ());
}
void
StatsTag::Deserialize (TagBuffer i)
{
  m_packetId = i.ReadU32 ();
  m_sendTime = MicroSeconds (i.ReadU64 ());
}
void
StatsTag::Print (std::ostream &os) const
{
  os << "packet=" << m_packetId << " sendTime=" << m_sendTime;
}

/**
 * (1) some nodes are in constant speed mobility, every node sends
 * two types of packets.
 * the first type is a small size with 200 bytes which models beacon
 * and safety message, this packet is broadcasted to neighbor nodes
 * (there will no ACK and retransmission);
 * the second type is a big size with 1500 bytes which models information
 * or entertainment message, this packet is sent to individual destination
 * node (this may cause ACK and retransmission).
 *
 * (2) Here are four configurations:
 * a - the first is every node sends packets randomly in SCH1 channel with continuous access.
 * b - the second is every node sends packets randomly with alternating access.
 *   Safety packets are sent to CCH channel, and non-safety packets are sent to SCH1 channel.
 * c - the third is similar to the second. But Safety packets will be sent randomly in CCH interval,
 *   non-safety packets will be sent randomly in SCH interval.
 *   This is the best situation of configuration 2 which models higher layer be aware of lower layer.
 * d - the fourth is also similar to the second. But Safety packets will be sent randomly in SCH interval,
 *   non-safety packets will be sent randomly in CCH interval.
 *   This is the worst situation of configuration 2 which makes packets get high queue delay.
 *
 * (3) Besides (2), users can also configures send frequency and nodes number.
 *
 * (4) The output is the delay of safety packets, the delay of non-safety packets, the throughput of
 * safety packets, and the throughput of safety packets.
 *
 * REDO: the packets are sent from second 0 to second 99, and the simulation will be stopped at second 100.
 * But when simulation is stopped, some packets may be queued in wave mac queue. These queued packets should
 * not be used for stats, but here they will be treated as packet loss.
 */
const static uint16_t IPv4_PROT_NUMBER = 0x0800;
const static uint16_t WSMP_PROT_NUMBER = 0x88DC;

// the transmission range of a device should be decided carefully,
// since it will affect the packet delivery ratio
// we suggest users use wave-transmission-range.cc to get this value
#define Device_Transmission_Range 250

class MultipleChannelExperiment
{
public:
  MultipleChannelExperiment (void);

  bool Configure (int argc, char **argv);
  void Usage (void);
  void Run (void);
  void Stats (void);

private:
  void CreateWaveNodes (void);

  // we treat WSMP packets as safety message
  void SendWsmpPackets (Ptr<WaveNetDevice> sender, uint32_t channelNumber);
  // we treat IP packets as non-safety message
  void SendIpPackets (Ptr<WaveNetDevice> sender);
  bool Receive (Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t mode, const Address &sender);

  void ConfigurationA (void);
  void ConfigurationB (void);
  void ConfigurationC (void);
  void ConfiguartionD (void);

  void InitStats (void);
  void Stats (uint32_t randomNumber);
  void StatQueuedPackets (void);

  NodeContainer nodes;
  NetDeviceContainer devices;
  uint32_t nodesNumber;
  uint32_t frequencySafety;
  uint32_t frequencyNonSafety;
  uint32_t simulationTime;
  uint32_t sizeSafety;
  uint32_t sizeNonSafety;

  Ptr<UniformRandomVariable> rngSafety;
  Ptr<UniformRandomVariable> rngNonSafety;
  Ptr<UniformRandomVariable> rngOther;

  uint32_t safetyPacketID;
  // we will check whether the packet is received by the neighbors
  // in the transmission range
  std::map<uint32_t, std::vector<uint32_t> *> broadcastPackets;

  uint32_t nonSafetyPacketID;

  struct SendIntervalStat
  {
    uint32_t        sendInCchi;
    uint32_t        sendInCguardi;
    uint32_t    sendInSchi;
    uint32_t    sendInSguardi;
  };

  SendIntervalStat sendSafety;
  SendIntervalStat sendNonSafety;
  uint32_t receiveSafety;     // the number of packets
  uint32_t receiveNonSafety;  // the number of packets
  uint32_t queueSafety;
  uint32_t queueNonSafety;
  uint64_t timeSafety;        // us
  uint64_t timeNonSafety;     // us

  bool createTraceFile;
  std::ofstream outfile;
};

MultipleChannelExperiment::MultipleChannelExperiment (void)
  : nodesNumber (20),           // 20 nodes
    frequencySafety (10),       // 10Hz, 100ms send one safety packet
    frequencyNonSafety (10),    // 10Hz, 100ms send one non-safety packet
    simulationTime (10),       // make it run 100s
    sizeSafety (200),           // 100 bytes small size
    sizeNonSafety (1500),       // 1500 bytes big size
    safetyPacketID (0),
    nonSafetyPacketID (0),
    receiveSafety (0),
    receiveNonSafety (0),
    queueSafety (0),
    queueNonSafety (0),
    timeSafety (0),
    timeNonSafety (0),
    createTraceFile (false)
{
}

bool
MultipleChannelExperiment::Configure (int argc, char **argv)
{
  CommandLine cmd;
  cmd.AddValue ("nodes", "Number of nodes.", nodesNumber);
  cmd.AddValue ("time", "Simulation time, s.", simulationTime);
  cmd.AddValue ("sizeSafety", "Size of safety packet, bytes.", sizeSafety);
  cmd.AddValue ("sizeNonSafety", "Size of non-safety packet, bytes.", sizeNonSafety);
  cmd.AddValue ("frequencySafety", "Frequency of sending safety packets, Hz.", frequencySafety);
  cmd.AddValue ("frequencyNonSafety", "Frequency of sending non-safety packets, Hz.", frequencyNonSafety);
  cmd.AddValue ("createTraceFile", "create trace file with 4 different configuration", createTraceFile);

  cmd.Parse (argc, argv);
  return true;
}
void
MultipleChannelExperiment::Usage (void)
{
  std::cout << "usage:"
		    << "./waf --run=\"wave-multiple-channel --nodes=20 --time=100 --sizeSafety=200 "
		    		"--sizeNonSafety=1500 --frequencySafety=10 --frequencyNonSafety=10 \""
            << std::endl;
}

void
MultipleChannelExperiment::CreateWaveNodes (void)
{
  NS_LOG_FUNCTION (this);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (17);

  nodes = NodeContainer ();
  nodes.Create (nodesNumber);

  // Create static grid
  // here we use static mobility model for two reasons
  // (a) since the mobility model of ns3 is mainly used for MANET, we can not ensure
  // whether they are suitable for VANET. From some papers, considering real traffic
  // pattern is required for VANET simulation.
  // (b) Here is no network layer protocol used to avoid causing the impact on MAC layer,
  // which will cause packets can not be routed. So if two nodes is far from each other,
  // the packets will not be received by phy layer, but here we want to show the impact of
  // MAC layer of 1609.4, like low PDR caused by contending at the guard interval of channel.
  // So we need every node is in transmission range.
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator");
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  for (NodeContainer::Iterator i = nodes.Begin (); i != nodes.End (); ++i)
    {
      Ptr<Node> node = (*i);
      Ptr<MobilityModel> model = node->GetObject<MobilityModel> ();
      Vector pos = model->GetPosition ();
      NS_LOG_DEBUG ( "position: " << pos);
    }

  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
  WaveHelper waveHelper = WaveHelper::Default ();
  devices = waveHelper.Install (wifiPhy, waveMac, nodes);

  // Enable WAVE logs.
  // WaveHelper::LogComponentEnable();
  // or waveHelper.LogComponentEnable();

  for (uint32_t i = 0; i != devices.GetN (); ++i)
    {
      devices.Get (i)->SetReceiveCallback (MakeCallback (&MultipleChannelExperiment::Receive,this));
    }
}

bool
MultipleChannelExperiment::Receive (Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t mode, const Address &sender)
{
  NS_LOG_FUNCTION (this << dev << pkt << mode << sender);

  StatsTag tag;
  bool result;
  result = pkt->FindFirstMatchingByteTag (tag);
  if (!result)
    {
      NS_FATAL_ERROR ("the packet here shall have a stats tag");
    }
  Time now = Now ();
  Time sendTime = tag.GetSendTime ();
  uint32_t packetId = tag.GetPacketId ();

  if (createTraceFile)
  outfile << "Time = " << std::dec << now.GetMicroSeconds () << "us, receive packet: "
          << " protocol = 0x" << std::hex << mode << std::dec
          << " id = " << packetId << " sendTime = " << sendTime.GetMicroSeconds ()
          << " type = " << (mode == WSMP_PROT_NUMBER ? "SafetyPacket" : "NonSafetyPacket")
          << std::endl;

  if (mode != WSMP_PROT_NUMBER)
    {
	  receiveNonSafety++;
	  timeNonSafety += (now - sendTime).GetMicroSeconds ();
	  return true;
    }


  // get current node id
    Ptr<Node> node = dev->GetNode();
    uint32_t nodeId = node->GetId();

    std::map<uint32_t, std::vector<uint32_t> *>::iterator i;
    i = broadcastPackets.find(packetId);
    if (i == broadcastPackets.end())
    {
  	  // why this packet is received but cannot be stat
  	  // two reason:
  	  // (i) the time the source node has no neighbors indeed, the received time
  	  //     the source node has neighbors again because of mobility characteristic
  	  // (ii) the transmission range we used is the range where the receivers can
  	  //      receive a packet with 95% probability. So there will be a case that
  	  //      in given transmission range source node has neighbors, but this packet
  	  //      can still be received by nodes out of the transmission range.
  	  // However, here we will take this packet not received.
  	  NS_LOG_DEBUG ("the node [" << nodeId << " out of source node's transmission range"
  	 			    " receive the packet [" << packetId << "].");
  		 return true;
    }
    std::vector<uint32_t> * neighbors = i->second;
    std::vector<uint32_t>::iterator j;
    j = std::find((*neighbors).begin()+1, (*neighbors).end(), nodeId);
    if (j == (*neighbors).end())
    {
  	  NS_LOG_DEBUG ("the node [" << nodeId << " out of source node's transmission range"
  	 	 			    " receive the packet [" << packetId << "].");
  		 return true;
    }

    NS_ASSERT ((*neighbors).size() > 1 && (*neighbors)[0] != 0);
    // since the packet is received by one neighbor successfully
    // decease the current size.
    (*neighbors)[0]--;
    // if becomes 0, it indicates all neighbors receive packets successfully
    if ((*neighbors)[0] == 0)
    {
    	receiveSafety++;
    	timeSafety += (now - sendTime).GetMicroSeconds ();

    	(*neighbors).clear();
    	delete neighbors;
    	broadcastPackets.erase(i);
    	return true;
    }

  return true;
}
// although WAVE devices support send IP-based packets, here we
// simplify ip routing protocol and application. Actually they will
// make delay and through of safety message more serious.
void
MultipleChannelExperiment::SendIpPackets (Ptr<WaveNetDevice> sender)
{
  NS_LOG_FUNCTION (this << sender);

  Time now = Now ();
  Ptr<Packet> packet = Create<Packet> (sizeNonSafety);
  StatsTag tag = StatsTag (nonSafetyPacketID++, now);
  packet->AddByteTag (tag);

  uint32_t i = rngOther->GetInteger (0, nodesNumber - 1);
  Address dest = devices.Get (i)->GetAddress ();
  // we randomly select a node  as destination node
  // however if the destination is unluckily sender itself, we select again
  if (dest == sender->GetAddress ())
    {
      if (i != 0)
        {
          i--;
        }
      else
        {
          i++;
        }
      dest = devices.Get (i)->GetAddress ();
    }

  bool result = false;
  result = sender->Send (packet, dest, IPv4_PROT_NUMBER);
  if (createTraceFile)
    {
      if (result)
        outfile << "Time = " << now.GetMicroSeconds () << "us,"
        		  << " unicast IP packet:  ID = " << tag.GetPacketId ()
                  << ", dest = " << dest << std::endl;
      else
        outfile << "unicast IP packet fail" << std::endl;
    }

  Ptr<ChannelCoordinator> coordinator = sender->GetChannelCoordinator ();
  if (coordinator->IsCchInterval ())
    {
      if (coordinator->IsGuardInterval ())
        {
          sendNonSafety.sendInCguardi++;
        }
      else
        {
          sendNonSafety.sendInCchi++;
        }
    }
  else
    {
      if (coordinator->IsGuardInterval ())
        {
          sendNonSafety.sendInSguardi++;
        }
      else
        {
          sendNonSafety.sendInSchi++;
        }
    }
}

void
MultipleChannelExperiment::SendWsmpPackets (Ptr<WaveNetDevice> sender, uint32_t channelNumber)
{
  NS_LOG_FUNCTION (this << sender << channelNumber);
  Time now = Now ();
  Ptr<Packet> packet = Create<Packet> (sizeSafety);
  StatsTag tag = StatsTag (safetyPacketID++, now);
  packet->AddByteTag (tag);
  const Address dest = Mac48Address::GetBroadcast ();
  Ptr<Node> src = sender->GetNode();
  Ptr<MobilityModel> model_src = src->GetObject<MobilityModel> ();
  Vector pos_src = model_src->GetPosition ();

  NodeContainer::Iterator i = nodes.Begin ();
  std::vector<uint32_t> * neighbors = new std::vector<uint32_t>;
  // the first elem is used to set the neighbors size
  // when it is received by one neighbor, it will decrease.
  // if it becomes 0 finally, it indicates the packet is broadcasted successfully.
  // otherwise it indicates that the packet is only received by some neighbors unsuccessfully
  (*neighbors).push_back (0);
  for (; i != nodes.End (); ++i)
  {
	  if ((*i) == src)
		  continue;
	  Ptr<Node> d = (*i);
	  Ptr<MobilityModel> model_d = d->GetObject<MobilityModel> ();
	  Vector pos_d = model_d->GetPosition ();

	  if (CalculateDistance (pos_d, pos_src) < Device_Transmission_Range )
	  {
		  (*neighbors).push_back (d->GetId());
		  continue;
	  }
  }
  if (neighbors->size() == 1)
    {
	  NS_LOG_WARN ("it may be strange that this node[" << src->GetId()
			  <<"] has no neighbors when it is sending a packet[" <<
			  tag.GetPacketId() << "].");
	}
  (*neighbors)[0] =  (*neighbors).size() - 1;
  broadcastPackets.insert(std::make_pair(tag.GetPacketId(), neighbors));

  TxInfo info = TxInfo (channelNumber);
  bool result = false;
  result = sender->SendX (packet, dest, WSMP_PROT_NUMBER, info);
  if (createTraceFile)
    {
     if (result)
       outfile << "Time = " << now.GetMicroSeconds () << "us, broadcast WSMP packet: ID = " << tag.GetPacketId () << std::endl;
     else
       outfile << "broadcast WSMP packet fail" << std::endl;
    }

  Ptr<ChannelCoordinator> coordinator = sender->GetChannelCoordinator ();
  if (coordinator->IsCchInterval ())
    {
      if (coordinator->IsGuardInterval ())
        {
          sendSafety.sendInCguardi++;
        }
      else
        {
          sendSafety.sendInCchi++;
        }

    }
  else
    {
      if (coordinator->IsGuardInterval ())
        {
          sendSafety.sendInSguardi++;
        }
      else
        {
          sendSafety.sendInSchi++;
        }
    }
}

void
MultipleChannelExperiment::InitStats (void)
{
	 // used for sending packets randomly
	  rngSafety = CreateObject<UniformRandomVariable> ();
	  rngSafety->SetStream (1);
	  rngNonSafety = CreateObject<UniformRandomVariable> ();
	  rngNonSafety->SetStream (2);
	  rngOther = CreateObject<UniformRandomVariable> ();
	  rngOther->SetStream (3);

	  broadcastPackets.clear ();
	  safetyPacketID = 0;
	  nonSafetyPacketID = 0;
	  receiveSafety = 0;
	  receiveNonSafety = 0;
	  queueSafety = 0;
	  queueNonSafety = 0;
	  timeSafety = 0;
	  timeNonSafety = 0;
	  sendSafety.sendInCchi = 0;
	  sendSafety.sendInCguardi = 0;
	  sendSafety.sendInSchi = 0;
	  sendSafety.sendInSguardi = 0;
	  sendNonSafety.sendInCchi = 0;
	  sendNonSafety.sendInCguardi = 0;
	  sendNonSafety.sendInSchi = 0;
	  sendNonSafety.sendInSguardi = 0;

  std::map<uint32_t, std::vector<uint32_t> *>::iterator i;
  for (i = broadcastPackets.begin(); i != broadcastPackets.end();++i)
  {
	  i->second->clear();
	  delete i->second;
	  i->second = 0;
  }
  broadcastPackets.clear();

  Simulator::ScheduleDestroy (&MultipleChannelExperiment::StatQueuedPackets, this);
}



// when simulation is stopped, we need to stats the queued packets
// so the real transmitted packets will be (sends - queues).
void
MultipleChannelExperiment::StatQueuedPackets ()
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer::Iterator i;
  for (i = devices.Begin (); i != devices.End (); ++i)
    {
	  Ptr<WaveNetDevice> device = DynamicCast<WaveNetDevice>(*i);
	  Ptr<RegularWifiMac> rmac = DynamicCast<RegularWifiMac>(device->GetMac (172));

	  PointerValue ptr;

	  // for WAVE devices, the DcaTxop will not be used.
	  // rmac->GetAttribute ("DcaTxop", ptr);
	  // Ptr<DcaTxop> dcaTxop = ptr.Get<DcaTxop> ();

	  rmac->GetAttribute ("VO_QosTxop", ptr);
	  Ptr<QosTxop> vo_edcaTxopN = ptr.Get<QosTxop> ();
	  //Ptr<WaveQosTxop> wave_vo = DynamicCast<WaveQosTxop>(vo_edcaTxopN);
	  //StatSingleQueue (wave_vo);

	  rmac->GetAttribute ("VI_QosTxop", ptr);
	  Ptr<QosTxop> vi_edcaTxopN = ptr.Get<QosTxop> ();
	  //Ptr<WaveQosTxop> wave_vi = DynamicCast<WaveQosTxop>(vi_edcaTxopN);
	  //StatSingleQueue (wave_vi);

	  rmac->GetAttribute ("BE_QosTxop", ptr);
	  Ptr<QosTxop> be_edcaTxopN = ptr.Get<QosTxop> ();
	  //Ptr<WaveQosTxop> wave_be = DynamicCast<WaveQosTxop>(be_edcaTxopN);
	  ///StatSingleQueue (wave_be);

	  rmac->GetAttribute ("BK_QosTxop", ptr);
	  Ptr<QosTxop> bk_edcaTxopN = ptr.Get<QosTxop> ();
	 // Ptr<WaveQosTxop> wave_bk = DynamicCast<WaveQosTxop>(bk_edcaTxopN);
	  //StatSingleQueue (wave_bk);
  }
}

void
MultipleChannelExperiment::ConfigurationA (void)
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer::Iterator i;
  for (i = devices.Begin (); i != devices.End (); ++i)
    {
      Ptr<WaveNetDevice> sender = DynamicCast<WaveNetDevice> (*i);
      SchInfo schInfo = SchInfo (SCH1, false, 0xff);
      Simulator::Schedule (Seconds (0.0), &WaveNetDevice::StartSch, sender, schInfo);
      TxProfile profile = TxProfile (SCH1);
      Simulator::Schedule (Seconds (0.0), &WaveNetDevice::RegisterTxProfile, sender, profile);
      for (uint32_t time = 0; time != simulationTime; ++time)
        {
          for (uint32_t sends = 0; sends != frequencySafety; ++sends)
            {
              Simulator::Schedule (Seconds (rngSafety->GetValue (time, time + 1)), &MultipleChannelExperiment::SendWsmpPackets, this, sender, SCH1);
            }
          for (uint32_t sends = 0; sends != frequencyNonSafety; ++sends)
            {
              Simulator::Schedule (Seconds (rngNonSafety->GetValue (time, time + 1)), &MultipleChannelExperiment::SendIpPackets, this, sender);
            }
        }
    }
}
void
MultipleChannelExperiment::ConfigurationB (void)
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer::Iterator i;
  for (i = devices.Begin (); i != devices.End (); ++i)
    {
      Ptr<WaveNetDevice> sender = DynamicCast<WaveNetDevice> (*i);
      SchInfo schInfo = SchInfo (SCH1, false, 0x0);
      Simulator::Schedule (Seconds (0.0),&WaveNetDevice::StartSch,sender,schInfo);
      TxProfile profile = TxProfile (SCH1);
      Simulator::Schedule (Seconds (0.0), &WaveNetDevice::RegisterTxProfile, sender, profile);

      for (uint32_t time = 0; time != simulationTime; ++time)
        {
          for (uint32_t sends = 0; sends != frequencySafety; ++sends)
            {
              Simulator::Schedule (Seconds (rngSafety->GetValue (time, time + 1)), &MultipleChannelExperiment::SendWsmpPackets, this, sender, CCH);
            }
          for (uint32_t sends = 0; sends != frequencyNonSafety; ++sends)
            {
              Simulator::Schedule (Seconds (rngNonSafety->GetValue (time, time + 1)), &MultipleChannelExperiment::SendIpPackets, this, sender);
            }
        }
    }
}
void
MultipleChannelExperiment::ConfigurationC (void)
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer::Iterator i;
  for (i = devices.Begin (); i != devices.End (); ++i)
    {
      Ptr<WaveNetDevice> sender = DynamicCast<WaveNetDevice> (*i);
      SchInfo schInfo = SchInfo (SCH1, false, 0x0);
      Simulator::Schedule (Seconds (0.0),&WaveNetDevice::StartSch,sender,schInfo);
      TxProfile profile = TxProfile (SCH1);
      Simulator::Schedule (Seconds (0.0), &WaveNetDevice::RegisterTxProfile, sender, profile);

      Ptr<ChannelCoordinator> coordinator = sender->GetChannelCoordinator ();
      for (uint32_t time = 0; time != simulationTime; ++time)
        {
          for (uint32_t sends = 0; sends != frequencySafety; ++sends)
            {
              Time t = Seconds (rngSafety->GetValue (time, time + 1));
              // if the send time is not at CCHI, we will calculate a new time
              if (!coordinator->IsCchInterval (t))
                {
                  t = t + coordinator->NeedTimeToCchInterval (t)
                    +  MicroSeconds (rngOther->GetInteger (0, coordinator->GetCchInterval ().GetMicroSeconds () - 1));
                }
              Simulator::Schedule (t, &MultipleChannelExperiment::SendWsmpPackets, this, sender, CCH);
            }
          for (uint32_t sends = 0; sends != frequencyNonSafety; ++sends)
            {

              Time t = Seconds (rngNonSafety->GetValue (time, time + 1));
              // if the send time is not at CCHI, we will calculate a new time
              if (!coordinator->IsSchInterval (t))
                {
                  t =  t
                    + coordinator->NeedTimeToSchInterval (t)
                    +  MicroSeconds (rngOther->GetInteger (0, coordinator->GetSchInterval ().GetMicroSeconds () - 1));
                }
              Simulator::Schedule (t, &MultipleChannelExperiment::SendIpPackets, this, sender);
            }
        }
    }
}
void
MultipleChannelExperiment::ConfiguartionD (void)
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer::Iterator i;
  for (i = devices.Begin (); i != devices.End (); ++i)
    {
      Ptr<WaveNetDevice> sender = DynamicCast<WaveNetDevice> (*i);
      SchInfo schInfo = SchInfo (SCH1, false, 0x0);
      Simulator::Schedule (Seconds (0.0),&WaveNetDevice::StartSch,sender,schInfo);
      TxProfile profile = TxProfile (SCH1);
      Simulator::Schedule (Seconds (0.0), &WaveNetDevice::RegisterTxProfile, sender, profile);

      Ptr<ChannelCoordinator> coordinator = sender->GetChannelCoordinator ();
      for (uint32_t time = 0; time != simulationTime; ++time)
        {
          for (uint32_t sends = 0; sends != frequencySafety; ++sends)
            {
              Time t = Seconds (rngSafety->GetValue (time, time + 1));
              if (!coordinator->IsSchInterval (t))
                {
                  t =  t + coordinator->NeedTimeToSchInterval (t)
                    +  MicroSeconds (rngOther->GetInteger (0, coordinator->GetSchInterval ().GetMicroSeconds () - 1));

                }
              Simulator::Schedule (t, &MultipleChannelExperiment::SendWsmpPackets, this, sender, CCH);
            }
          for (uint32_t sends = 0; sends != frequencyNonSafety; ++sends)
            {

              Time t = Seconds (rngNonSafety->GetValue (time, time + 1));
              // if the send time is not at CCHI, we will calculate a new time
              if (!coordinator->IsCchInterval (t))
                {
                  t =  t + coordinator->NeedTimeToCchInterval (t)
                    +  MicroSeconds (rngOther->GetInteger (0, coordinator->GetCchInterval ().GetMicroSeconds () - 1));

                }
              Simulator::Schedule (t, &MultipleChannelExperiment::SendIpPackets, this, sender);
            }
        }
    }
}
void
MultipleChannelExperiment::Run (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("simulation configuration arguments: ");

  {
    NS_LOG_UNCOND ("configuration A:");
    if (createTraceFile)
      outfile.open ("config-a");
    InitStats ();
    CreateWaveNodes ();
    ConfigurationA ();
    Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();
    Simulator::Destroy ();
    Stats ();
    if (createTraceFile)
      outfile.close ();
  }
  {
    NS_LOG_UNCOND ("configuration B:");
    if (createTraceFile)
      outfile.open ("config-b");
    InitStats ();
    CreateWaveNodes ();
    ConfigurationB ();
    Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();
    Simulator::Destroy ();
    Stats ();
    if (createTraceFile)
      outfile.close ();
  }
  {
    NS_LOG_UNCOND ("configuration C:");
    if (createTraceFile)
      outfile.open ("config-c");
    InitStats ();
    CreateWaveNodes ();
    ConfigurationC ();
    Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();
    Simulator::Destroy ();
    Stats ();
    if (createTraceFile)
      outfile.close ();
  }
  {
    NS_LOG_UNCOND ("configuration D:");
    if (createTraceFile)
      outfile.open ("config-d");
    InitStats ();
    CreateWaveNodes ();
    ConfiguartionD ();
    Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();
    Simulator::Destroy ();
    Stats ();
    if (createTraceFile)
      outfile.close ();
  }
}
void
MultipleChannelExperiment::Stats (void)
{
  NS_LOG_FUNCTION (this);
  // first show stats information
  NS_LOG_UNCOND (" safety packet: ");
  NS_LOG_UNCOND ("  sends = " << safetyPacketID);
  NS_LOG_UNCOND ( "  CGuardI CCHI SGuardI SCHI " );
  NS_LOG_UNCOND ("  " << sendSafety.sendInCguardi  << " "
                      << sendSafety.sendInCchi  << " "
                      << sendSafety.sendInSguardi  << " "
                      << sendSafety.sendInSchi);
  NS_LOG_UNCOND ("  receives = " << receiveSafety);
  NS_LOG_UNCOND ("  queues = " << queueSafety);

  NS_LOG_UNCOND (" non-safety packet: ");
  NS_LOG_UNCOND ("  sends = " << nonSafetyPacketID);
  NS_LOG_UNCOND ("  CGuardI CCHI SGuardI SCHI ");
  NS_LOG_UNCOND ("  " << sendNonSafety.sendInCguardi  << " "
                      << sendNonSafety.sendInCchi  << " "
                      << sendNonSafety.sendInSguardi  << " "
                      << sendNonSafety.sendInSchi);
  NS_LOG_UNCOND ("  receives = " << receiveNonSafety);
  NS_LOG_UNCOND ("  queues = " << queueNonSafety);

  // second show performance result
  NS_LOG_UNCOND (" performance result:");
  // stats PDR (packet delivery ratio)
  double safetyPDR = receiveSafety / (double)(safetyPacketID - queueSafety);
  double nonSafetyPDR = receiveNonSafety / (double)(nonSafetyPacketID - queueNonSafety);
  NS_LOG_UNCOND ("  safetyPDR = " << safetyPDR << " , nonSafetyPDR = " << nonSafetyPDR);
  // stats average delay
  double delaySafety = timeSafety / receiveSafety / 1000.0;
  double delayNonSafety = timeNonSafety / receiveNonSafety / 1000.0;
  NS_LOG_UNCOND ("  delaySafety = " << delaySafety << "ms"
                                    << " , delayNonSafety = " << delayNonSafety << "ms");
  // stats average throughout
  double throughoutSafety = receiveSafety * sizeSafety * 8 / simulationTime / 1000.0;
  double throughoutNonSafety = receiveNonSafety * sizeNonSafety * 8 / simulationTime / 1000.0;
  NS_LOG_UNCOND ("  throughoutSafety = " << throughoutSafety << "kbps"
                                         << " , throughoutNonSafety = " << throughoutNonSafety << "kbps");
}

int
main (int argc, char *argv[])
{
  //LogComponentEnable ("WaveMultipleChannel", LOG_LEVEL_DEBUG);

  MultipleChannelExperiment experiment;
  if (experiment.Configure (argc, argv))
    {
      experiment.Run ();
    }
  else
    {
      experiment.Usage ();
    }

  return 0;
}
