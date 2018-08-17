/*
 * packet-tag-c2v.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_PACKET_TAG_C2V_H_
#define SCRATCH_VANET_CS_VFC_PACKET_TAG_C2V_H_

#include <vector>
#include "ns3/tag.h"

namespace ns3 {
namespace vanet {

class PacketTagC2v : public Tag
{
public:
  PacketTagC2v (void);

  void SetReqsIds (std::vector<uint32_t> reqsIds);

  std::vector<uint32_t> GetReqsIds (void);

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
  std::vector<uint32_t> m_reqsIds;
};

} // namespace vanet
} // namespace ns3

#endif /* SCRATCH_VANET_CS_VFC_PACKET_TAG_C2V_H_ */
