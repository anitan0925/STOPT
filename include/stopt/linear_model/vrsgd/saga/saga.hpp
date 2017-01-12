/* -*- coding:utf-8 -*- */
#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include "math.h"
#include <algorithm>
#include <unordered_map>
#include <stopt/core/common.hpp>

namespace stopt
{
  template < class ParamType, class DataType  >
  class SAGA
  {
  public:
    typedef typename DataType::label_type label_type;
    typedef typename DataType::feature_type feature_type;
    typedef typename std::vector< std::pair<int64_t,DataType> > data_chunk_type;
    typedef double value_type;
    typedef std::unordered_map< label_type, value_type > probability_type;

    // hyper parameters
    value_type eta, lambda;
    int_type epoch;

    // parameters
    ParamType x, base_grad, saga_grad;
    std::unordered_map< int, std::unordered_map<int,value_type> > grad_store;

    SAGA( const value_type eta_, const value_type lambda_ ) : epoch(0)
    {
      eta = eta_;
      lambda = lambda_;
    }

    inline value_type probability( const feature_type &feature, ParamType &param, probability_type &probs, value_type &denom )
    {
      value_type val = 0.;
      value_type max_val = 0.;
      probs.delete_all();

      for( typename ParamType::iterator param_itr=param.begin(); param_itr!=param.end(); ++param_itr ){
	val = param_itr->second.value(feature);
	probs.insert( std::pair< label_type, real_type > ( param_itr->first,val ) );
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

    inline value_type loss( ParamType &param, typename data_chunk_type::const_iterator &sitr, typename data_chunk_type::const_iterator &eitr )
    {
      value_type loss_ave = 0;
      int64_t n = eitr - sitr;

      for( typename data_chunk_type::const_iterator itr=sitr; itr!=eitr; ++itr ){
	label_type label = itr->second.first;
	const feature_type &feature = itr->second.second;

	probability_type probs;
	double denom;

	typename ParamType::LinearParamType &target_param = param[label];
	double val = target_param.value( feature );
	double max_val = probability( feature, param, probs, denom );

	loss_ave -= ( (val-max_val) - log(denom) ) / n;
      }
      
      return loss_ave;
    }

    inline value_type regularized_loss( ParamType &p, data_chunk_type &data )
    {
      value_type val = 0;

      val  = loss( p, data.begin(), data.end() );
      if( lambda > 0 ){
      	val += regularization_value( p );
      }

      return val;
    }

    inline value_type regularization_value( ParamType &p )
    {
      return 0.5 * lambda * p.l2();
    }

    inline void monitoring( data_chunk_type &data, std::ostream &fout )
    {
      fout << epoch << "," << std::fixed << std::setprecision(16) << regularized_loss( x, data ) << std::endl;
    }

    inline void grad(  DataType &one_data, std::unordered_map<int_type,real_type> &coef )
    {
      label_type label = one_data.first;
      
      //value_store.reset();
      value_type max_value = 0;
      std::unordered_map< int_type, real_type > value_store;

      for( typename ParamType::iterator itr=x.begin(); itr!=x.end(); ++itr ){
	label_type itr_label = itr->first;
	value_type val = itr->second.value( one_data.second );
	value_store.insert( std::pair<int_type,real_type> ( itr_label, val ) );
	if( max_value < val ){ max_value = val; }
      }

      value_type sum=0;
      std::unordered_map<int_type,real_type> p;

      for( typename std::unordered_map< int_type,real_type >::iterator itr=value_store.begin(); itr!=value_store.end(); ++itr ){
	value_type val = exp( itr->second - max_value );
	p.insert( std::pair<int_type,real_type> ( itr->first, val ) );
	sum += val;
      }      

      // coef.reset();
      for( typename std::unordered_map<int_type,real_type>::iterator itr=p.begin(); itr!=p.end(); ++itr ){
	if( itr->first==label ){
	  coef.insert( std::pair<int_type,real_type>( itr->first, -( 1.0 - (itr->second)/sum ) ) );

	}else{
	  coef.insert( std::pair<int_type,real_type>( itr->first, (itr->second)/sum ) );
	}
      }
    }

    inline void init_params( ParamType param )
    {
      x = param;
    }

    inline void run( data_chunk_type &data, const int max_epoch, std::ostream &fout )
    {
      typedef std::unordered_map<int_type,real_type> grad_type;
      int64_t n = data.size();
      
      random_shuffle( data.begin(), data.end() );	    

      // init grad_store
      for( typename data_chunk_type::iterator itr=data.begin(); itr!=data.end(); ++itr ){
	std::unordered_map<int_type,real_type> coef; 
	//grad( itr->second.first, itr->second.second, coef );
	grad( itr->second, coef );
	grad_store.insert( std::pair< int_type, grad_type > ( itr->first, coef ) );
	
	for( typename std::unordered_map<int_type,real_type>::iterator itr2=coef.begin(); itr2!=coef.end(); ++itr2 ){
	  base_grad.insert_label( itr2->first );
	  base_grad[ itr2->first ].plus_weight( itr2->second/n, itr->second.second );
	}
      }
      ++epoch;

      while( true ){
	random_shuffle( data.begin(), data.end() );	    
	
	monitoring( data, fout );

	if( epoch >= max_epoch ){ break; } 

	for( typename data_chunk_type::iterator itr=data.begin(); itr!=data.end(); ++itr ){
	  std::unordered_map<int_type,real_type> new_coef, old_coef; 
	  grad( itr->second, new_coef );
	  old_coef = grad_store[ itr->first ];
	  
	  // saga grad
	  saga_grad.reset();
	  for( typename std::unordered_map<int_type,real_type>::iterator itr2=new_coef.begin(); itr2!=new_coef.end(); ++itr2 ){
	    saga_grad.insert_label( itr2->first );
	    saga_grad[ itr2->first ].plus_weight( itr2->second, itr->second.second );
	  }

	  for( typename std::unordered_map<int_type,real_type>::iterator itr2=old_coef.begin(); itr2!=old_coef.end(); ++itr2 ){
	    saga_grad.insert_label( itr2->first );
	    saga_grad[ itr2->first ].plus_weight( - itr2->second, itr->second.second );
	  }

	  saga_grad.plus( 1, base_grad );
	  
	  // update base_grad
	  for( typename std::unordered_map<int_type,real_type>::iterator itr2=new_coef.begin(); itr2!=new_coef.end(); ++itr2 ){
	    base_grad.insert_label( itr2->first );
	    if( old_coef.find(itr2->first) != old_coef.end() ){
	      base_grad[ itr2->first ].plus_weight( (itr2->second - old_coef[itr2->first])/n, itr->second.second );
	    }else{
	      base_grad[ itr2->first ].plus_weight( itr2->second/n, itr->second.second );
	    }
	  }
	  
	  // update grad_store
	  grad_store[ itr->first ]  = new_coef;
	  
	  // update x
	  if( lambda > 0 ){
	    x.scale( 1. - eta*lambda );
	  }
	  
	  x.plus( -eta, saga_grad );
	}
	++epoch;
      }

    }
  };
}
