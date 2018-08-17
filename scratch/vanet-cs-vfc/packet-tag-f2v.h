/*
 * packet-tag-f2v.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_PACKET_TAG_F2V_H_
#define SCRATCH_VANET_CS_VFC_PACKET_TAG_F2V_H_

#include <vector>
#include "ns3/tag.h"
#include "graph-edge-type.h"

namespace ns3 {
namespace vanet {

class PacketTagF2v : public Tag
{
public:

  enum class PreActionType:uint8_t
  {
    NOT_SET	= 0,
    F2F		= 1,
    V2F		= 2
  };

  PacketTagF2v (void);

  void SetCurrentEdgeType (EdgeType type);
  EdgeType GetCurrentEdgeType (void) const;

  void SetPreActionType (PacketTagF2v::PreActionType type);
  PacketTagF2v::PreActionType GetPreActionType (void) const;

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
  uint8_t m_preActionType; // previous action type
  uint32_t m_fogId;
  std::vector<uint32_t> m_rsuWaitingServedIdxs; // index of rsu waiting to be served
  std::vector<uint32_t> m_dataIdxs;
};

} // namespace vanet
} // namespace ns3

#endif /* SCRATCH_VANET_CS_VFC_PACKET_TAG_F2V_H_ */