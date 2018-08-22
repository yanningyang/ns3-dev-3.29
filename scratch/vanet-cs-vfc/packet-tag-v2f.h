/*
 * packet-tag-v2f.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_PACKET_TAG_V2F_H_
#define SCRATCH_VANET_CS_VFC_PACKET_TAG_V2F_H_

#include <vector>

#include "custom-type.h"
#include "ns3/tag.h"

namespace ns3 {
namespace vanet {

class PacketTagV2f : public Tag
{
public:

  enum class NextActionType:uint8_t
  {
    NOT_SET	= 0,
    F2F		= 1,
    F2V		= 2
  };

  PacketTagV2f (void);

  void SetCurrentEdgeType (EdgeType type);
  EdgeType GetCurrentEdgeType (void) const;

  void SetNextActionType (PacketTagV2f::NextActionType type);
  PacketTagV2f::NextActionType GetNextActionType (void) const;

  void SetRsuWaitingServedIdxs (std::vector<uint32_t> rsuWaitingServedIdxs);
  std::vector<uint32_t> GetRsuWaitingServedIdxs (void) const;

//  void SetDestRsuIdxs (std::vector<uint32_t> destRsuIdxs);
//  std::vector<uint32_t> GetDestRsuIdxs (void) const;

  void SetDataIdxs (std::vector<uint32_t> dataIdxs);
  std::vector<uint32_t> GetDataIdxs (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
  uint8_t m_currentEdgeType;
  uint8_t m_nextActionType; // next action type
  std::vector<uint32_t> m_rsuWaitingServedIdxs; // index of rsu waiting to be served
  std::vector<uint32_t> m_destRsuIdxs; // index of destination rsu
  std::vector<uint32_t> m_dataIdxs; // data index to be send
};

} // namespace vanet
} // namespace ns3

#endif /* SCRATCH_VANET_CS_VFC_PACKET_TAG_V2F_H_ */
