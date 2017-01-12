/* -*- coding:utf-8 -*- */
#pragma once

#include <unordered_map>
#include <iostream>
#include <stopt/core/common.hpp>
#include <stopt/linear_model/vrsgd/gradient.hpp>

namespace stopt
{
  template < class ParamType, class DataType >
  class LogReg : public Gradient< ParamType, DataType >
  {
  public:
    typedef Gradient< ParamType, DataType > gradient_type;
    typedef typename gradient_type::label_type label_type;
    typedef typename gradient_type::value_type value_type;
    typedef typename gradient_type::param_type param_type;
    typedef typename gradient_type::data_chunk_type data_chunk_type;
    typedef typename gradient_type::feature_type feature_type;
    typedef std::unordered_map<label_type,value_type> probability_type;

    inline void gradient( param_type &grad, param_type &param, const typename data_chunk_type::const_iterator &sitr, const typename data_chunk_type::const_iterator &eitr )
    {
      grad.reset();

      int64_t data_chunk_size = eitr - sitr;
      value_type denom;
      probability_type probs;

      gradient_type::eval_count += data_chunk_size;

      for( typename data_chunk_type::const_iterator data_itr=sitr; data_itr!=eitr; ++data_itr ){
	label_type label = data_itr->first;
	const feature_type &feature = data_itr->second;

	probability( feature, param, probs, denom );

	for( typename param_type::iterator param_itr=param.begin(); param_itr!=param.end(); ++param_itr ){
	  value_type p = probs[param_itr->first]/denom;

	  label_type loop_label = param_itr->first;

	  if( loop_label==label ){
	    grad[loop_label].plus_weight( (p-1.)/(float)data_chunk_size, feature );
	    if( gradient_type::biased_f ){
	      grad[loop_label].plus_bias( (p-1.)/(float)data_chunk_size );
	    }else{
	      grad[loop_label].bias = 0;
	    }
	  }else{
	    grad[loop_label].plus_weight( p/(float)data_chunk_size, feature );
	    if( gradient_type::biased_f ){
	      grad[loop_label].plus_bias  ( p/(float)data_chunk_size );
	    }else{
	      grad[loop_label].bias = 0;
	    }
	  }
	}
      }
    }

    inline value_type probability( const feature_type &feature, param_type &param, probability_type &probs, value_type &denom )
    {
      value_type val = 0.;
      value_type max_val = 0.;
      probs.clear();

      for( typename param_type::iterator param_itr=param.begin(); param_itr!=param.end(); ++param_itr ){
	val = param_itr->second.value(feature);
	probs.insert( std::pair< label_type, value_type >( param_itr->first, val ) );
	if( val > max_val ){ max_val = val; }
      } 

      denom = 0.;
      for( typename probability_type::iterator prob_itr=probs.begin(); prob_itr!=probs.end(); ++prob_itr ){
	val = exp( prob_itr->second - max_val );
	denom += val;
	prob_itr->second = val;
      }
      return max_val;
    }

    inline value_type loss( param_type &param, const typename data_chunk_type::const_iterator &sitr, const typename data_chunk_type::const_iterator &eitr )
    {
      value_type loss_ave = 0;
      int64_t n = eitr - sitr;

      for( typename data_chunk_type::const_iterator itr=sitr; itr!=eitr; ++itr ){
	label_type label = itr->first;
	const feature_type &feature = itr->second;

	probability_type probs;
	double denom;

	typename param_type::param_type &target_param = param[label];
	double val = target_param.value( feature );
	double max_val = probability( feature, param, probs, denom );

	loss_ave -= ( (val-max_val) - log(denom) ) / n;
      }
      
      return loss_ave;
    }
  };
}
