/*
 * packet-header.h
 *
 *  Created on: Aug 2, 2018
 *      Author: haha
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
    DATA	= 2
  };

  /**
   * \param type the packet type
   */
  void SetType (PacketHeader::MessageType type);
  /**
   * \return the packet type
   */
  PacketHeader::MessageType GetType (void) const;

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
};

} // namespace vanet
} // namespace ns3


#endif /* SCRATCH_VANET_CS_VFC_PACKET_HEADER_H_ */
