/* -*- coding:utf-8 -*- */
#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include "math.h"
#include <algorithm>
#include <stopt/core/common.hpp>

namespace stopt
{
  template < class OracleType, class ParamType, class DataType  >
  class SVRG
  {
  public:
    typedef typename std::vector<DataType> data_chunk_type;
    typedef typename OracleType::value_type value_type;
    typedef OracleType oracle_type;

    oracle_type oracle;

    // hyper parameters
    value_type eta, lambda;
    int_type max_inner_loop, m;
    int_type epoch;

    // parameters
    ParamType base_point, base_grad, base_sgrad, svrg;
    ParamType x;

    SVRG( const value_type eta_, const value_type lambda_, const int m_ ) : epoch(0)
    {
      eta = eta_;
      lambda = lambda_;
      max_inner_loop = -1;
      m = m_;
    }

    inline void set_m( const int m_ )
    {
      m = m_;
    }

    inline void init_params( ParamType param )
    {
      base_point = param;
      base_grad  = param;
      base_sgrad = param;
      svrg       = param;
      x = param;
    }

    inline ParamType get_param()
    {
      return x;
    }

    inline bool check_termination( const int max_epoch )
    {
      return epoch >= max_epoch;
    }

    inline void run( data_chunk_type &data, const int max_epoch, std::ostream &fout )
    {
      max_inner_loop = data.size() * m;

      fout << "epoch,value" << std::endl;
      while(true){
	random_shuffle( data.begin(), data.end() );	    
	if( inner_run( data, max_epoch, fout ) ){ break; }
      }
    }

    inline void monitoring( data_chunk_type &data, std::ostream &fout )
    {
      fout << epoch << "," << std::fixed << std::setprecision(16) << regularized_loss( x, data ) << std::endl;
    }

    inline bool inner_run( data_chunk_type &data, const int max_epoch, std::ostream &fout )
    {
      // init base_point and base_grad.
      base_point = x;
      oracle.gradient( base_grad, base_point, data.begin(), data.end() );	

      int k=0;
      int64_t n = data.size();

      typename data_chunk_type::iterator mb_begin, mb_end;
      mb_begin = data.begin();

      while( true ){

	if( true ){ // 1-epoch
	  int64_t eval_count = oracle.get_eval_count();
	  if( eval_count >= n ){
	    std::cout << "eval : " << eval_count  << ", increment epoch: " << eval_count / n << ", set_int_eval :"  << eval_count % n << std::endl;
	    for( int i=0; i<(eval_count / n); ++i ){
	      ++epoch;
	      monitoring( data, fout );
	    }
	    oracle.set_eval_count( eval_count % n );

	    if( check_termination( max_epoch ) ){ 
	      return true;
	    }
	  }
	}

        int64_t b = 1; // minibatch size 
	mb_end = mb_begin;
        bool end_f = ! safe_advance( mb_end, data.end(), b );

	// update svrg
	// stochastic gradient at x
	oracle.gradient( svrg, x, mb_begin, mb_end );

	// stochastic gradient at base_point
	oracle.gradient( base_sgrad, base_point, mb_begin, mb_end );
	svrg.plus( -1., base_sgrad );
	svrg.plus( 1., base_grad );

	// L2 regularization
	if( lambda > 0 ){
	  svrg.plus( lambda, x );
	}

	x.plus( -eta, svrg );	

	if( end_f ){
	  std::cout << "end" << std::endl;
	  mb_begin = data.begin();
	  mb_end   = mb_begin;
	}else{
	  safe_advance( mb_begin, data.end(), b );
	}

	k++;

	if( k >= max_inner_loop ){ break; }
      }

      return false;
    }

    inline value_type regularization_value( ParamType &p )
    {
      return 0.5 * lambda * p.l2();
    }

    inline value_type regularized_loss( ParamType &p, data_chunk_type &data )
    {
      value_type val = 0;

      val  = oracle.loss( p, data.begin(), data.end() );
      if( lambda > 0 ){
      	val += regularization_value( p );
      }

      return val;
    }

    template < class I >
    bool safe_advance( I &itr, const I &eitr, int64_t n )
    {
      int64_t counter = 0;
      for( ; (itr!=eitr) && (counter<n); ++itr, ++counter ){
      }

      return itr!=eitr;
    }
  };
}
