/*
 * packet-tag-c2v.cc
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#include "packet-tag-c2v.h"

namespace ns3 {
namespace vanet {

PacketTagC2v::PacketTagC2v (void)
{
}

void
PacketTagC2v::SetReqsIds (std::vector<uint32_t> reqsIds)
{
  m_reqsIds.assign(reqsIds.begin(), reqsIds.end());
}

std::vector<uint32_t>
PacketTagC2v::GetReqsIds (void)
{
  return m_reqsIds;
}

NS_OBJECT_ENSURE_REGISTERED (PacketTagC2v);

TypeId
PacketTagC2v::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PacketTagC2v")
    .SetParent<Tag> ()
    .AddConstructor<PacketTagC2v> ()
  ;
  return tid;
}
TypeId
PacketTagC2v::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t
PacketTagC2v::GetSerializedSize (void) const
{
  return sizeof (uint32_t) + sizeof (uint32_t) * m_reqsIds.size();
}
void
PacketTagC2v::Serialize (TagBuffer i) const
{
  uint32_t size = m_reqsIds.size();
  i.WriteU32(size);
  for (uint32_t reqId : m_reqsIds)
    {
      i.WriteU32 (reqId);
    }
}
void
PacketTagC2v::Deserialize (TagBuffer i)
{
  uint32_t size = i.ReadU32 ();
  for (uint32_t j = 0; j < size; j++)
    {
      m_reqsIds.push_back(i.ReadU32 ());
    }
}
void
PacketTagC2v::Print (std::ostream &os) const
{
  os << "reqs=";
  uint32_t size = m_reqsIds.size();
  for (uint32_t i = 0; i < size; i++)
    {
      os << " " << m_reqsIds[i];
    }
}

} // namespace vanet
} // namespace ns3
