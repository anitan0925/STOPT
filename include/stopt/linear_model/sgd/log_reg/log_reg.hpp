/* -*- coding:utf-8 -*- */
#pragma once

#include <unordered_map>
#include <iostream>
#include <stopt/core/common.hpp>
#include <stopt/linear_model/sgd/gradient.hpp>
#include <stopt/linear_model/sgd/schedular.hpp>
#include <stopt/linear_model/sgd/averager.hpp>

namespace stopt
{
  template < class ParamType, class DataType >
  class SLLogReg : public SLGradient< ParamType, DataType >
  {
  public:
    typedef SLGradient< ParamType, DataType > gradient_type;
    typedef typename gradient_type::label_type label_type;
    typedef typename gradient_type::value_type value_type;
    typedef typename gradient_type::param_type param_type;
    typedef DataType data_type;
    typedef typename gradient_type::data_chunk_type data_chunk_type;    
    typedef typename gradient_type::feature_type feature_type;
    typedef std::unordered_map<label_type,value_type> probability_type;
    typedef BasicSchedular schedular_type;
    typedef LabeledParamAverager<param_type, feature_type> averager_type;
    
    schedular_type schedular;
    averager_type averager;
    bool average_f;
    real_type lambda;
    int64_t sgdcounter_;

    SLLogReg() : average_f(false), lambda(0), sgdcounter_(0), gradient_type::SLGradient()
    {
    }

    inline void show_param( std::ostream &fout )
    {
      schedular.show_param( fout );
      averager.show_param( fout );
      fout << "average_f: " << average_f << std::endl;
      fout << "lambda: " << lambda << std::endl;
      fout << "biased_f: " << gradient_type::biased_f << std::endl;
    }

    inline void increment_counter()
    {
      ++sgdcounter_;
    }

    inline int64_t get_counter()
    {
      return sgdcounter_;
    }

    inline void update( param_type &param, const typename data_chunk_type::const_iterator &data_itr )
    {
      const label_type label = data_itr->first;
      const feature_type &feature = data_itr->second;
      int64_t t = get_counter();
      real_type lr = schedular.learning_rate(t);
      
      real_type denom;
      probability_type probs;

      gradient_type::eval_count += 1;

      probability( feature, param, probs, denom );

      if( lambda > 0 ){
	param.scale(1-lambda*lr);
      }

      for( typename param_type::iterator param_itr=param.begin(); param_itr!=param.end(); ++param_itr ){
	value_type p = probs[param_itr->first]/denom;

	label_type loop_label = param_itr->first;
	typename param_type::param_type &loop_param = param_itr->second;

	if( loop_label==label ){
	  double inner_step_size = loop_param.plus_weight( lr*(1-p), feature );
	  if( average_f ){
	    averager.regist( loop_label, inner_step_size, &feature );
	  }
	  if( gradient_type::biased_f ){
	    loop_param.plus_bias( lr*(1-p) );
	  }
	}else{
	  double inner_step_size = loop_param.plus_weight( -1.0*lr*p, feature );
	  if( average_f ){
	    averager.regist( loop_label, inner_step_size, &feature );
	  }
	  if( gradient_type::biased_f ){
	    loop_param.plus_bias( -1.0*lr*p );
	  }
	}
      }

      averager.average(t,param);

      increment_counter();
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

    inline value_type regularization_value( ParamType &p )
    {
      return 0.5 * lambda * p.l2();
    }    
  };
}

