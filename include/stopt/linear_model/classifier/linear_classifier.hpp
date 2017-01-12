/* -*- coding:utf-8 -*- */
#pragma once

#include <stopt/core/common.hpp>
#include <unordered_map>

namespace stopt
{
  template < class ParamType >
  class LinearClassifier
  {
  public:
    typedef ParamType param_type;
    typedef typename param_type::label_type label_type;
    
    class result_type
    {
    public:
      typedef std::unordered_map< label_type, real_type > values_type;
      typedef std::unordered_map< label_type, real_type > probabilities_type;
      label_type predicted_label;
      values_type values;
      probabilities_type probabilities;
    };

    param_type param_;

    inline void set_param( param_type &param )
    {
      param_ = param;
    }

    template < class data_type >
    inline result_type predict( const data_type &x, const bool probability_f=false )
    {
      result_type result;
      label_type ymax;
      real_type maxval=0, tmpval;      

      for( typename param_type::iterator itr=param_.begin(); itr!=param_.end(); ++itr ){
	tmpval = itr->second.value(x);
	result.values.insert( std::pair< label_type, real_type >( itr->first, tmpval ) );

	if( probability_f ){
	  result.probabilities.insert( std::pair< label_type, real_type >( itr->first, tmpval ) );
	}

	if( tmpval >= maxval || itr==param_.begin() ){
	  maxval = tmpval;
	  ymax = itr->first;
	}
      }

      result.predicted_label = ymax;

      if( probability_f ){
	real_type denom = 0.;

	for( typename result_type::probabilities_type::iterator itr=result.probabilities.begin();
	     itr != result.probabilities.end(); ++itr ){

	  real_type val = exp( itr->second - maxval );
	  denom += val;
	  itr->second = val;
	}

	for( typename result_type::probabilities_type::iterator itr=result.probabilities.begin();
	     itr != result.probabilities.end(); ++itr ){

	  itr->second /= denom;
	}
      }

      return result;
    }
  };
}
