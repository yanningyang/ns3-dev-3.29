/*
 * packet-tag-f2f.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_PACKET_TAG_F2F_H_
#define SCRATCH_VANET_CS_VFC_PACKET_TAG_F2F_H_

#include <vector>

#include "custom-type.h"
#include "ns3/tag.h"

namespace ns3 {
namespace vanet {

class PacketTagF2f : public Tag
{
public:
  PacketTagF2f (void);

  void SetCurrentEdgeType (EdgeType type);
  EdgeType GetCurrentEdgeType (void) const;

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
  std::vector<uint32_t> m_dataIdxs; // data index to be send
};

} // namespace vanet
} // namespace ns3

#endif /* SCRATCH_VANET_CS_VFC_PACKET_TAG_F2F_H_ */
