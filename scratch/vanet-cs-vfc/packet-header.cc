/*
 * packet-header.cc
 *
 *  Created on: Aug 2, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/header.h"
#include "ns3/simulator.h"
#include "packet-header.h"

namespace ns3 {
namespace vanet {

NS_LOG_COMPONENT_DEFINE ("TypeHeader");

NS_OBJECT_ENSURE_REGISTERED (PacketHeader);

PacketHeader::PacketHeader ()
  : m_type (0),
    m_broadcastId (0)
{
  NS_LOG_FUNCTION (this);
}

void
PacketHeader::SetType (PacketHeader::MessageType type)
{
  NS_LOG_FUNCTION (this << static_cast<uint16_t> (type));
  switch (type)
    {
    case MessageType::NOT_SET:
      m_type = 0;
      break;
    case MessageType::REQUEST:
      m_type = 1;
      break;
    case MessageType::DATA_C2V:
      m_type = 2;
      break;
    case MessageType::DATA_V2F:
      m_type = 3;
      break;
    case MessageType::DATA_F2F:
      m_type = 4;
      break;
    case MessageType::DATA_F2V:
      m_type = 5;
      break;
    default:
      NS_FATAL_ERROR ("Unknown Content-Type: " << static_cast<uint16_t> (type));
      break;
    }
}
PacketHeader::MessageType
PacketHeader::GetType (void) const
{
  NS_LOG_FUNCTION (this);
  MessageType ret;
  switch (m_type)
    {
    case 0:
      ret = MessageType::NOT_SET;
      break;
    case 1:
      ret = MessageType::REQUEST;
      break;
    case 2:
      ret = MessageType::DATA_C2V;
      break;
    case 3:
      ret = MessageType::DATA_V2F;
      break;
    case 4:
      ret = MessageType::DATA_F2F;
      break;
    case 5:
      ret = MessageType::DATA_F2V;
      break;
    default:
      NS_FATAL_ERROR ("Unknown Content-Type: " << m_type);
      break;
    }
  return ret;
}

void
PacketHeader::SetBroadcastId (uint32_t broadcastId)
{
  m_broadcastId = broadcastId;
}

uint32_t
PacketHeader::GetBroadcastId ()
{
  return m_broadcastId;
}

TypeId
PacketHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PacketHeader")
    .SetParent<Header> ()
    .SetGroupName("Vanet")
    .AddConstructor<PacketHeader> ()
  ;
  return tid;
}
TypeId
PacketHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
void
PacketHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  os << "(type=" << m_type << ", broadcastId=" << m_broadcastId << ")";
}
uint32_t
PacketHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return sizeof(uint8_t) + sizeof(uint32_t);
}

void
PacketHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_type);
  i.WriteU32 (m_broadcastId);
}

uint32_t
PacketHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_type = i.ReadU8 ();
  m_broadcastId = i.ReadU32();
  return GetSerializedSize ();
}

} // namespace vanet
} // namespace ns3
