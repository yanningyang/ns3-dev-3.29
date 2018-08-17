/*
 * packet-tag-f2f.cc
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#include "packet-tag-f2f.h"

namespace ns3 {
namespace vanet {

PacketTagF2f::PacketTagF2f (void)
{
}

void
PacketTagF2f::SetCurrentEdgeType (EdgeType type)
{
  switch (type)
    {
    case EdgeType::NOT_SET:
    m_currentEdgeType = 0;
      break;
    case EdgeType::CONDITION_1:
    m_currentEdgeType = 1;
      break;
    case EdgeType::CONDITION_2:
    m_currentEdgeType = 2;
      break;
    case EdgeType::CONDITION_3:
    m_currentEdgeType = 3;
      break;
    default:
      NS_FATAL_ERROR ("Unknown CurrentEdge-Type: " << static_cast<uint16_t> (type));
      break;
    }
}
EdgeType
PacketTagF2f::GetCurrentEdgeType (void) const
{
  EdgeType ret;
  switch (m_currentEdgeType)
    {
    case 0:
      ret = EdgeType::NOT_SET;
      break;
    case 1:
      ret = EdgeType::CONDITION_1;
      break;
    case 2:
      ret = EdgeType::CONDITION_2;
      break;
    case 3:
      ret = EdgeType::CONDITION_3;
      break;
    default:
      NS_FATAL_ERROR ("Unknown CurrentEdge-Type: " << m_currentEdgeType);
      break;
    }
  return ret;
}

void
PacketTagF2f::SetDataIdxs (std::vector<uint32_t> dataIdxs)
{
  m_dataIdxs.assign(dataIdxs.begin(), dataIdxs.end());
}

std::vector<uint32_t>
PacketTagF2f::GetDataIdxs (void)
{
  return m_dataIdxs;
}

NS_OBJECT_ENSURE_REGISTERED (PacketTagF2f);

TypeId
PacketTagF2f::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PacketTagF2f")
    .SetParent<Tag> ()
    .AddConstructor<PacketTagF2f> ()
  ;
  return tid;
}
TypeId
PacketTagF2f::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
PacketTagF2f::GetSerializedSize (void) const
{
  return sizeof (uint8_t) + sizeof (uint32_t) + sizeof (uint32_t) * m_dataIdxs.size();
}
void
PacketTagF2f::Serialize (TagBuffer i) const
{
  i.WriteU8(m_currentEdgeType);
  uint32_t size2 = m_dataIdxs.size();
  i.WriteU32(size2);
  for (uint32_t dataIdx : m_dataIdxs)
    {
      i.WriteU32 (dataIdx);
    }
}
void
PacketTagF2f::Deserialize (TagBuffer i)
{
  m_currentEdgeType = i.ReadU8();
  uint32_t size2 = i.ReadU32 ();
  for (uint32_t j = 0; j < size2; j++)
    {
      m_dataIdxs.push_back(i.ReadU32 ());
    }
}
void
PacketTagF2f::Print (std::ostream &os) const
{
  os << "currentEdgeType=" << m_currentEdgeType;
  os << ", dataIdxs=";
  for (uint32_t dataIdx : m_dataIdxs)
    {
      os << " " << dataIdx;
    }
}

} // namespace vanet
} // namespace ns3
