/*
 * packet-header.cc
 *
 *  Created on: Aug 2, 2018
 *      Author: haha
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
  : m_type (0)
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
    case MessageType::DATA:
      m_type = 2;
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
      ret = MessageType::DATA;
      break;
    default:
      NS_FATAL_ERROR ("Unknown Content-Type: " << m_type);
      break;
    }
  return ret;
}

TypeId
PacketHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TypeHeader")
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
  os << "(type=" << m_type << ")";
}
uint32_t
PacketHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 1;
}

void
PacketHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  i.WriteU8 (m_type);
}
uint32_t
PacketHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);
  Buffer::Iterator i = start;
  m_type = i.ReadU8 ();
  return GetSerializedSize ();
}

} // namespace vanet
} // namespace ns3
