/* -*- coding:utf-8 -*- */
#pragma once

#include <vector>

namespace stopt
{
  template < class ParamType, class DataType >
  class Gradient
  {
  public:
    typedef ParamType param_type;
    typedef typename DataType::label_type label_type;
    typedef typename DataType::feature_type feature_type;
    typedef typename feature_type::key_type key_type;
    typedef typename feature_type::value_type value_type;
    typedef typename std::vector< DataType > data_chunk_type;

    bool biased_f;
    int64_t eval_count;

  public:
    Gradient() : eval_count(0), biased_f(false)
    {
    }

    inline int64_t get_eval_count()
    {
      return eval_count;
    }

    inline void set_eval_count( int64_t eval_count_ )
    {
      eval_count = eval_count_;
    }

    inline void set_biased_flag( bool biased_f_ )
    {
      biased_f = biased_f_;
    }
  };
}
