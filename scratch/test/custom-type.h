/*
 * graph-edge-type.h
 *
 *  Created on: Aug 16, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_CUSTOM_TYPE_H_
#define SCRATCH_VANET_CS_VFC_CUSTOM_TYPE_H_

//#include <bits/stdint-uintn.h>
#include <string>
#include <sstream>
#include <ostream>

enum class EdgeType:uint8_t
{
  NOT_SET		= 0,
  CONDITION_1		= 1,
  CONDITION_2		= 2,
  CONDITION_3		= 3
};

struct ReqQueueItem
{
  uint32_t	vehIndex;
  uint32_t	reqDataIndex;
  std::string	name;

  ReqQueueItem()
  {
    this->vehIndex = 0;
    this->reqDataIndex = 0;
    this->name = "";
  }
  ReqQueueItem(const uint32_t& vehIndex, const uint32_t& _reqDataIndex)
      : vehIndex(vehIndex)
      , reqDataIndex(_reqDataIndex)
      , name("")
  {
    genName();
  }

  void genName()
  {
    std::ostringstream oss;
    oss << vehIndex << "-" << reqDataIndex;
    this->name = oss.str();
  }

  friend std::ostream & operator << (std::ostream &os, ReqQueueItem &reqItem)
    {
      os << reqItem.vehIndex << "-" << reqItem.reqDataIndex;
      return os;
    }

  bool operator == (const ReqQueueItem &reqItem) const
    {
      return (this->vehIndex == reqItem.vehIndex) && (this->reqDataIndex == reqItem.reqDataIndex);
    }
};

#endif /* SCRATCH_VANET_CS_VFC_CUSTOM_TYPE_H_ */
