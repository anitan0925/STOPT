/* -*- coding:utf-8 -*- */
#pragma once

#include <unordered_map>
#include <iostream>
#include <stopt/core/common.hpp>
#include <stopt/linear_model/vrsgd/gradient.hpp>

namespace stopt
{
  template < class ParamType, class DataType >
  class SmoothedSVM : public Gradient< ParamType, DataType >
  {
  public:
    typedef Gradient< ParamType, DataType > gradient_type;
    typedef typename gradient_type::label_type label_type;
    typedef typename gradient_type::value_type value_type;
    typedef typename gradient_type::param_type param_type;
    typedef typename gradient_type::data_chunk_type data_chunk_type;
    typedef typename gradient_type::feature_type feature_type;

    inline void gradient( param_type &grad, param_type &param, const typename data_chunk_type::const_iterator &sitr, const typename data_chunk_type::const_iterator &eitr )
    {
      grad.reset();

      int64_t data_chunk_size = eitr - sitr;
      value_type denom;
      probability_type probs;

      gradient_type::eval_count += data_chunk_size;


    }


    inline value_type loss( param_type &param, const typename data_chunk_type::const_iterator &sitr, const typename data_chunk_type::const_iterator &eitr )
    {
      value_type loss_ave = 0;
      int64_t n = eitr - sitr;

      return loss_ave;
    }
  };
}
