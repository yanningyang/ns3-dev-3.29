/*
 * packet-header.h
 *
 *  Created on: Aug 2, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_PACKET_HEADER_H_
#define SCRATCH_VANET_CS_VFC_PACKET_HEADER_H_


#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3 {
namespace vanet {

class PacketHeader : public Header
{
public:
  PacketHeader ();

  enum class MessageType:uint8_t
  {
    NOT_SET	= 0,
    REQUEST	= 1,
    DATA_C2V	= 2,
    DATA_V2F	= 3,
    DATA_F2F	= 4,
    DATA_F2V	= 5
  };

  /**
   * \param type the packet type
   */
  void SetType (PacketHeader::MessageType type);
  /**
   * \return the packet type
   */
  PacketHeader::MessageType GetType (void) const;

  void SetBroadcastId (uint32_t broadcastId);
  uint32_t GetBroadcastId ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint8_t m_type; //!< packet type
  uint32_t m_broadcastId; //!< broadcast id
};

} // namespace vanet
} // namespace ns3


#endif /* SCRATCH_VANET_CS_VFC_PACKET_HEADER_H_ */
