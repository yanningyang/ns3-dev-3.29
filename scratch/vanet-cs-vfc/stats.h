/*
 * stats.h
 *
 *  Created on: Jul 26, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_STATS_H_
#define SCRATCH_VANET_CS_VFC_STATS_H_

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"

using namespace ns3;

struct RequestStatus
{
  bool		completed	= false;
  double 	submitTime	= -1.0;
  double	satisfiedTime	= -1.0;
};

class RequestStats
{
public:

  RequestStats ();

  uint32_t GetSubmittedReqs ();

  void IncSubmittedReqs (uint32_t reqs);

  void SetSubmittedReqs (uint32_t submittedReqs);

  uint32_t GetSatisfiedReqs ();

  void IncSatisfiedReqs ();

  void SetSatisfiedReqs (uint32_t satisfiedReqs);

  uint32_t GetBroadcastPkts ();

  void IncBroadcastPkts ();

  void SetBroadcastPkts (uint32_t broadcastPkts);

  double GetCumulativeDelay ();

  void IncCumulativeDelay (double delay);

  void SetCumulativeDelay (double cumulativeDelay);

private:
  uint32_t m_SubmittedReqs; ///< packets submitted
  uint32_t m_SatisfiedReqs; ///< packets satisfied
  uint32_t m_BroadcastPkts; ///< packets broadcast by BS
  double m_CumulativeDelay; ///< cumulative delay
};

/**
 * \ingroup wave
 * \brief The RoutingStats class manages collects statistics
 * on routing data (application-data packet and byte counts)
 * for the vehicular network
 */
class RoutingStats
{
public:
  /**
   * \brief Constructor
   * \return none
   */
  RoutingStats ();

  /**
   * \brief Returns the number of bytes received
   * \return the number of bytes received
   */
  uint32_t GetRxBytes ();

  /**
   * \brief Returns the cumulative number of bytes received
   * \return the cumulative number of bytes received
   */
  uint32_t GetCumulativeRxBytes ();

  /**
   * \brief Returns the count of packets received
   * \return the count of packets received
   */
  uint32_t GetRxPkts ();

  /**
   * \brief Returns the cumulative count of packets received
   * \return the cumulative count of packets received
   */
  uint32_t GetCumulativeRxPkts ();

  /**
   * \brief Increments the number of (application-data)
   * bytes received, not including MAC/PHY overhead
   * \param rxBytes the number of bytes received
   * \return none
   */
  void IncRxBytes (uint32_t rxBytes);

  /**
   * \brief Increments the count of packets received
   * \return none
   */
  void IncRxPkts ();

  /**
   * \brief Sets the number of bytes received.
   * \param rxBytes the number of bytes received
   * \return none
   */
  void SetRxBytes (uint32_t rxBytes);

  /**
   * \brief Sets the number of packets received
   * \param rxPkts the number of packets received
   * \return none
   */
  void SetRxPkts (uint32_t rxPkts);

  /**
   * \brief Returns the number of bytes transmitted
   * \return the number of bytes transmitted
   */
  uint32_t GetTxBytes ();

  /**
   * \brief Returns the cumulative number of bytes transmitted
   * \return none
   */
  uint32_t GetCumulativeTxBytes ();

  /**
   * \brief Returns the number of packets transmitted
   * \return the number of packets transmitted
   */
  uint32_t GetTxPkts ();

  /**
   * \brief Returns the cumulative number of packets transmitted
   * \return the cumulative number of packets transmitted
   */
  uint32_t GetCumulativeTxPkts ();

  /**
   * \brief Increment the number of bytes transmitted
   * \param txBytes the number of addtional bytes transmitted
   * \return none
   */
  void IncTxBytes (uint32_t txBytes);

  /**
   * \brief Increment the count of packets transmitted
   * \return none
   */
  void IncTxPkts ();

  /**
   * \brief Sets the number of bytes transmitted
   * \param txBytes the number of bytes transmitted
   * \return none
   */
  void SetTxBytes (uint32_t txBytes);

  /**
   * \brief Sets the number of packets transmitted
   * \param txPkts the number of packets transmitted
   * \return none
   */
  void SetTxPkts (uint32_t txPkts);

private:
  uint32_t m_RxBytes; ///< reeive bytes
  uint32_t m_cumulativeRxBytes; ///< cumulative receive bytes
  uint32_t m_RxPkts; ///< receive packets
  uint32_t m_cumulativeRxPkts; ///< cumulative receive packets
  uint32_t m_TxBytes; ///< transmit bytes
  uint32_t m_cumulativeTxBytes; ///< cumulative transmit bytes
  uint32_t m_TxPkts; ///< transmit packets
  uint32_t m_cumulativeTxPkts; ///< cumulative transmit packets
};


/**
 * \ingroup wave
 * \brief The WifiPhyStats class collects Wifi MAC/PHY statistics
 */
class WifiPhyStats : public Object
{
public:
  /**
   * \brief Gets the class TypeId
   * \return the class TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   * \return none
   */
  WifiPhyStats ();

  /**
   * \brief Destructor
   * \return none
   */
  virtual ~WifiPhyStats ();

  /**
   * \brief Returns the number of bytes that have been transmitted
   * (this includes MAC/PHY overhead)
   * \return the number of bytes transmitted
   */
  uint32_t GetTxBytes ();

  /**
   * \brief Callback signiture for Phy/Tx trace
   * \param context this object
   * \param packet packet transmitted
   * \param mode wifi mode
   * \param preamble wifi preamble
   * \param txPower transmission power
   * \return none
   */
  void PhyTxTrace (std::string context, Ptr<const Packet> packet, WifiMode mode, WifiPreamble preamble, uint8_t txPower);

  /**
   * \brief Callback signiture for Phy/TxDrop
   * \param context this object
   * \param packet the tx packet being dropped
   * \return none
   */
  void PhyTxDrop (std::string context, Ptr<const Packet> packet);

  /**
   * \brief Callback signiture for Phy/RxDrop
   * \param context this object
   * \param packet the rx packet being dropped
   * \return none
   */
  void PhyRxDrop (std::string context, Ptr<const Packet> packet);

private:
  uint32_t m_phyTxPkts; ///< phy transmit packets
  uint32_t m_phyTxBytes; ///< phy transmit bytes
};


#endif /* SCRATCH_VANET_CS_VFC_STATS_H_ */
