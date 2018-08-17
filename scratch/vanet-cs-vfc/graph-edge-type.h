/*
 * graph-edge-type.h
 *
 *  Created on: Aug 16, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_GRAPH_EDGE_TYPE_H_
#define SCRATCH_VANET_CS_VFC_GRAPH_EDGE_TYPE_H_

#include <bits/stdint-uintn.h>

enum class EdgeType:uint8_t
{
  NOT_SET		= 0,
  CONDITION_1		= 1,
  CONDITION_2		= 2,
  CONDITION_3		= 3
};

#endif /* SCRATCH_VANET_CS_VFC_GRAPH_EDGE_TYPE_H_ */
