/* -*- coding:utf-8 -*- */
#pragma once

#include <stdint.h>

namespace stopt
{
  typedef int32_t int_type;
  typedef double real_type;
  
  template <typename T>
  struct TypeRank
  {
    enum {rank = 0};
  };
  
#define DEFINE_TYPE_RANK(T, value) \
  template <>			   \
  struct TypeRank<T>		   \
  {				   \
    enum {rank = value};	   \
  };

  //---------------------------------------------------
  DEFINE_TYPE_RANK(int8_t, 100);
  DEFINE_TYPE_RANK(uint8_t, 200);
  DEFINE_TYPE_RANK(int16_t, 300);
  DEFINE_TYPE_RANK(uint16_t, 400);
  DEFINE_TYPE_RANK(int32_t, 500);
  DEFINE_TYPE_RANK(uint32_t, 600);
  DEFINE_TYPE_RANK(int64_t, 700);
  DEFINE_TYPE_RANK(uint64_t, 800);
  DEFINE_TYPE_RANK(float, 800);
  DEFINE_TYPE_RANK(double, 900);
  DEFINE_TYPE_RANK(long double, 1000);
  
  template <bool flag, typename S, typename T>
  struct STATIC_IFELSE{
    typedef S result_type;
  };
  
  template <typename S, typename T>
  struct STATIC_IFELSE<false, S, T>
  {
    typedef T result_type;
  };

  template <class S, class T>
  struct TypePromote{
    typedef typename STATIC_IFELSE< (TypeRank<S>::rank >=
				     TypeRank<T>::rank), S, T >::result_type result_type;
  };
}
