/*
 * packet-tag-f2v.cc
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#include "packet-tag-f2v.h"

namespace ns3 {
namespace vanet {

PacketTagF2v::PacketTagF2v (void)
{
}

void
PacketTagF2v::SetCurrentEdgeType (EdgeType type)
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
PacketTagF2v::GetCurrentEdgeType (void) const
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
PacketTagF2v::SetPreActionType (PacketTagF2v::PreActionType type)
{
  switch (type)
    {
    case PreActionType::NOT_SET:
      m_preActionType = 0;
      break;
    case PreActionType::F2F:
      m_preActionType = 1;
      break;
    case PreActionType::V2F:
      m_preActionType = 2;
      break;
    default:
      NS_FATAL_ERROR ("Unknown PreAction-Type: " << static_cast<uint8_t> (type));
      break;
    }
}
PacketTagF2v::PreActionType
PacketTagF2v::GetPreActionType (void) const
{
  PreActionType ret;
  switch (m_preActionType)
    {
    case 0:
      ret = PreActionType::NOT_SET;
      break;
    case 1:
      ret = PreActionType::F2F;
      break;
    case 2:
      ret = PreActionType::V2F;
      break;
    default:
      NS_FATAL_ERROR ("Unknown PreAction-Type: " << m_preActionType);
      break;
    }
  return ret;
}

void
PacketTagF2v::SetFogId(uint32_t fogId)
{
  m_fogId = fogId;
}

uint32_t
PacketTagF2v::GetFogId()
{
  return m_fogId;
}

void
PacketTagF2v::SetRsuWaitingServedIdxs (std::vector<uint32_t> rsuWaitingServedIdxs)
{
  m_rsuWaitingServedIdxs.assign(rsuWaitingServedIdxs.begin(), rsuWaitingServedIdxs.end());
}

std::vector<uint32_t>
PacketTagF2v::GetRsuWaitingServedIdxs (void) const
{
  return m_rsuWaitingServedIdxs;
}

void
PacketTagF2v::SetDataIdxs (std::vector<uint32_t> dataIdxs)
{
  m_dataIdxs.assign(dataIdxs.begin(), dataIdxs.end());
}

std::vector<uint32_t>
PacketTagF2v::GetDataIdxs (void)
{
  return m_dataIdxs;
}

NS_OBJECT_ENSURE_REGISTERED (PacketTagF2v);

TypeId
PacketTagF2v::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PacketTagF2v")
    .SetParent<Tag> ()
    .AddConstructor<PacketTagF2v> ()
  ;
  return tid;
}
TypeId
PacketTagF2v::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
PacketTagF2v::GetSerializedSize (void) const
{
  return sizeof (uint8_t)
      + sizeof (uint8_t)
      + sizeof (uint32_t)
      + sizeof (uint32_t) + sizeof (uint32_t) * m_rsuWaitingServedIdxs.size()
      + sizeof (uint32_t) + sizeof (uint32_t) * m_dataIdxs.size();
}
void
PacketTagF2v::Serialize (TagBuffer i) const
{
  i.WriteU8(m_currentEdgeType);
  i.WriteU8(m_preActionType);
  i.WriteU32(m_fogId);

  uint32_t size0 = m_rsuWaitingServedIdxs.size();
  i.WriteU32(size0);
  for (uint32_t rsuIdx : m_rsuWaitingServedIdxs)
    {
      i.WriteU32 (rsuIdx);
    }

  uint32_t size2 = m_dataIdxs.size();
  i.WriteU32(size2);
  for (uint32_t dataIdx : m_dataIdxs)
    {
      i.WriteU32 (dataIdx);
    }
}
void
PacketTagF2v::Deserialize (TagBuffer i)
{
  m_currentEdgeType = i.ReadU8();
  m_preActionType = i.ReadU8();
  m_fogId = i.ReadU32();

  uint32_t size0 = i.ReadU32 ();
  for (uint32_t j = 0; j < size0; j++)
    {
      m_rsuWaitingServedIdxs.push_back(i.ReadU32 ());
    }

  uint32_t size2 = i.ReadU32 ();
  for (uint32_t j = 0; j < size2; j++)
    {
      m_dataIdxs.push_back(i.ReadU32 ());
    }
}
void
PacketTagF2v::Print (std::ostream &os) const
{
  os << "currentEdgeType=" << m_currentEdgeType;
  os << ", preActionType=" << m_preActionType;
  os << ", fogId=" << m_fogId;
  os << ", rsuWaitingServedIdxs=";
  for (uint32_t rsuIdx : m_rsuWaitingServedIdxs)
    {
      os << " " << rsuIdx;
    }
  os << ", dataIdxs=";
  for (uint32_t dataIdx : m_dataIdxs)
    {
      os << " " << dataIdx;
    }
}

} // namespace vanet
} // namespace ns3
