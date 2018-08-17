/*
 * packet-tag-v2f.cc
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#include "packet-tag-v2f.h"

namespace ns3 {
namespace vanet {

PacketTagV2f::PacketTagV2f (void)
{
}

void
PacketTagV2f::SetCurrentEdgeType (EdgeType type)
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
PacketTagV2f::GetCurrentEdgeType (void) const
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
PacketTagV2f::SetNextActionType (PacketTagV2f::NextActionType type)
{
  switch (type)
    {
    case NextActionType::NOT_SET:
      m_nextActionType = 0;
      break;
    case NextActionType::F2F:
      m_nextActionType = 1;
      break;
    case NextActionType::F2V:
      m_nextActionType = 2;
      break;
    default:
      NS_FATAL_ERROR ("Unknown NextAction-Type: " << static_cast<uint8_t> (type));
      break;
    }
}
PacketTagV2f::NextActionType
PacketTagV2f::GetNextActionType (void) const
{
  NextActionType ret;
  switch (m_nextActionType)
    {
    case 0:
      ret = NextActionType::NOT_SET;
      break;
    case 1:
      ret = NextActionType::F2F;
      break;
    case 2:
      ret = NextActionType::F2V;
      break;
    default:
      NS_FATAL_ERROR ("Unknown NextAction-Type: " << m_nextActionType);
      break;
    }
  return ret;
}

void
PacketTagV2f::SetRsuWaitingServedIdxs (std::vector<uint32_t> rsuWaitingServedIdxs)
{
  m_rsuWaitingServedIdxs.assign(rsuWaitingServedIdxs.begin(), rsuWaitingServedIdxs.end());
}

std::vector<uint32_t>
PacketTagV2f::GetRsuWaitingServedIdxs (void) const
{
  return m_rsuWaitingServedIdxs;
}

//void
//PacketTagV2f::SetDestRsuIdxs (std::vector<uint32_t> destRsuIdxs)
//{
//  m_destRsuIdxs.assign(destRsuIdxs.begin(), destRsuIdxs.end());
//}
//
//std::vector<uint32_t>
//PacketTagV2f::GetDestRsuIdxs (void) const
//{
//  return m_destRsuIdxs;
//}

void
PacketTagV2f::SetDataIdxs (std::vector<uint32_t> dataIdxs)
{
  m_dataIdxs.assign(dataIdxs.begin(), dataIdxs.end());
}

std::vector<uint32_t>
PacketTagV2f::GetDataIdxs (void) const
{
  return m_dataIdxs;
}

NS_OBJECT_ENSURE_REGISTERED (PacketTagV2f);

TypeId
PacketTagV2f::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PacketTagV2f")
    .SetParent<Tag> ()
    .AddConstructor<PacketTagV2f> ()
  ;
  return tid;
}
TypeId
PacketTagV2f::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
PacketTagV2f::GetSerializedSize (void) const
{
  return sizeof (uint8_t)
      + sizeof (uint8_t)
      + sizeof (uint32_t) + sizeof (uint32_t) * m_rsuWaitingServedIdxs.size()
      + sizeof (uint32_t) + sizeof (uint32_t) * m_destRsuIdxs.size()
      + sizeof (uint32_t) + sizeof (uint32_t) * m_dataIdxs.size();
}
void
PacketTagV2f::Serialize (TagBuffer i) const
{
  i.WriteU8(m_currentEdgeType);
  i.WriteU8(m_nextActionType);

  uint32_t size0 = m_rsuWaitingServedIdxs.size();
  i.WriteU32(size0);
  for (uint32_t rsuIdx : m_rsuWaitingServedIdxs)
    {
      i.WriteU32 (rsuIdx);
    }

  uint32_t size1 = m_destRsuIdxs.size();
  i.WriteU32(size1);
  for (uint32_t rsuIdx : m_destRsuIdxs)
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
PacketTagV2f::Deserialize (TagBuffer i)
{
  m_currentEdgeType = i.ReadU8();
  m_nextActionType = i.ReadU8();

  uint32_t size0 = i.ReadU32 ();
  for (uint32_t j = 0; j < size0; j++)
    {
      m_rsuWaitingServedIdxs.push_back(i.ReadU32 ());
    }

  uint32_t size1 = i.ReadU32 ();
  for (uint32_t j = 0; j < size1; j++)
    {
      m_destRsuIdxs.push_back(i.ReadU32 ());
    }

  uint32_t size2 = i.ReadU32 ();
  for (uint32_t j = 0; j < size2; j++)
    {
      m_dataIdxs.push_back(i.ReadU32 ());
    }
}
void
PacketTagV2f::Print (std::ostream &os) const
{
  os << "currentEdgeType=" << m_currentEdgeType;
  os << ", nextActionType=" << m_nextActionType;
  os << ", rsuWaitingServedIdxs=";
  for (uint32_t rsuIdx : m_rsuWaitingServedIdxs)
    {
      os << " " << rsuIdx;
    }
//  os << ", destRsuIdxs=";
//  for (uint32_t rsuIdx : m_destRsuIdxs)
//    {
//      os << " " << rsuIdx;
//    }
  os << ", dataIdxs=";
  for (uint32_t dataIdx : m_dataIdxs)
    {
      os << " " << dataIdx;
    }
}

} // namespace vanet
} // namespace ns3
