/*
 * packet-tag-f2v.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_PACKET_TAG_F2V_H_
#define SCRATCH_VANET_CS_VFC_PACKET_TAG_F2V_H_

#include <vector>

#include "custom-type.h"
#include "ns3/tag.h"

namespace ns3 {
namespace vanet {

class PacketTagF2v : public Tag
{
public:

  enum class NextActionType:uint8_t
  {
    NOT_SET	= 0,
    V2F		= 1
  };

  PacketTagF2v (void);

  void SetCurrentEdgeType (EdgeType type);
  EdgeType GetCurrentEdgeType (void) const;

  void SetNextActionType (PacketTagF2v::NextActionType type);
  PacketTagF2v::NextActionType GetNextActionType (void) const;

  void SetFogId(uint32_t fogId);
  uint32_t GetFogId(void);

  void SetRsuWaitingServedIdxs (std::vector<uint32_t> rsuWaitingServedIdxs);
  std::vector<uint32_t> GetRsuWaitingServedIdxs (void) const;

  void SetDataIdxs (std::vector<uint32_t> dataIdxs);
  std::vector<uint32_t> GetDataIdxs (void);

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
  uint8_t m_currentEdgeType;
  uint8_t m_nextActionType; // next action type
  uint32_t m_fogId;
  std::vector<uint32_t> m_rsuWaitingServedIdxs; // index of rsu waiting to be served
  std::vector<uint32_t> m_dataIdxs;
};

} // namespace vanet
} // namespace ns3

#endif /* SCRATCH_VANET_CS_VFC_PACKET_TAG_F2V_H_ */
