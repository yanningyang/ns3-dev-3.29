/*
 * BytesConvertionUtil.hpp
 *
 *  Created on: Jul 29, 2018
 *      Author: haha
 */

#ifndef SCRATCH_TEST2_UTILS_HPP_
#define SCRATCH_TEST2_UTILS_HPP_

#include <iostream>
#include "ns3/core-module.h"

using namespace std;

typedef unsigned char uint8_t;

namespace utils
{
  template<typename T = const char *>
  static T Bytes2T(uint8_t *bytes);

  template<typename T = const char *>
  static uint8_t * T2Bytes(T u);

  template<typename T = const char *>
  static T Bytes2T(uint8_t *bytes)
  {
      T res = 0;
      int n = sizeof(T);
      memcpy(&res, bytes, n);
      return res;
  }

  // note that uint8_t* b must deleted after being used
  template<typename T = const char *>
  static uint8_t * T2Bytes(T u)
  {
      int n = sizeof(T);
      uint8_t* b = new uint8_t[n];
      memcpy(b, &u, n);
      return b;
  }
}


#endif /* SCRATCH_TEST2_UTILS_HPP_ */
