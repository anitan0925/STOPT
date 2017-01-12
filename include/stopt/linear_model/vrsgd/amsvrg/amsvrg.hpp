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
  class AMSVRG
  {
  public:
    typedef typename std::vector<DataType> data_chunk_type;
    typedef typename OracleType::value_type value_type;
    typedef OracleType oracle_type;

    oracle_type oracle;

    // hyper parameters
    real_type eta, lambda, p;
    int_type restart_method;

    // parameters
    ParamType base_point, base_grad, base_sgrad, svrg;
    ParamType x, y, z, prev_y;

    // tmp parameters
    int_type m;
    int_type epoch;

    bool debug_f;

    AMSVRG( const bool biased_f_, const value_type eta_, const value_type lambda_, const int restart_method_, const value_type p_ ) : m(0), epoch(0)
    {
      oracle.biased_f = biased_f_;
      eta = eta_;
      lambda = lambda_;
      restart_method = restart_method_;
      p = p_;
      debug_f = false;
    }

    inline void init_params( ParamType param )
    {
      base_point = param;
      base_grad  = param;
      base_sgrad = param;
      svrg       = param;
      x          = param;
      y          = param;
      z          = param;
      prev_y     = param;
    }

    inline ParamType get_param()
    {
      return y.get_param();
    }

    inline void set_param( ParamType w )
    {
      y = w; 
      z = w;
    }

    inline bool check_termination( const int_type max_epoch )
    {
      return epoch >= max_epoch;
    }

    inline void show_param()
    {
      std::cout << "[AMSVRG_PARAMETER]" << std::endl;
      std::cout << "eta: " << eta << std::endl;
      std::cout << "lambda: " << lambda << std::endl;
      std::cout << "accuracy: " << p << std::endl;
      std::cout << "restart method: " << restart_method << std::endl;
      std::cout << "biased_f: " << oracle.biased_f << std::endl;
    }

    inline void run( data_chunk_type &data, const int_type max_epoch, std::ostream &fout )
    {
      show_param();

      fout << "epoch,value" << std::endl;
      while(true){
	set_param( y ); // option 1
	random_shuffle( data.begin(), data.end() );
	if( inner_run( data, max_epoch, fout ) ){ break; }

      }
    }

    // tau_k
    inline value_type get_tau( int_type k )
    {
      return 4./(k+4.);
    }

    // alpha_{k+1}
    inline value_type get_alpha( int_type k )
    {
      return ( (k+2.)*eta )/4.;
    }

    // b_{k+1}
    inline int64_t get_minibatch_size( int64_t n, int k )
    {
      // value_type p=0.1;
      // value_type p=10.0;
      return ceil( n*(k+2.) )/( p*(n-1) + (k+2.) );
    }

    inline void monitoring( data_chunk_type &data, std::ostream &fout )
    {
      real_type val = regularized_loss( y, data );
      fout << epoch << "," << std::fixed << std::setprecision(16) << val << std::endl;
      std::cout << "epoch: " << epoch << ", loss: " << std::fixed << std::setprecision(16) << val << std::endl;      
    }

    inline bool inner_run( data_chunk_type &data, const int_type max_epoch, std::ostream &fout )
    {
      // init base_point and base_grad.
      base_point = y;
      oracle.gradient( base_grad, base_point, data.begin(), data.end() );

      int_type k=0;
      int64_t n = data.size();

      typename data_chunk_type::iterator mb_begin, mb_end;
      mb_begin = data.begin();

      while( true ){

	if( true ){
	  int64_t eval_count = oracle.get_eval_count();
	  if( eval_count >= n ){
	    if( debug_f ){
	      std::cout << "eval : " << eval_count  << ", increment epoch: " << eval_count / n << ", set_int_eval :"  << eval_count % n << std::endl;
	    }
	    for( int i=0; i<(eval_count / n); ++i ){
	      ++epoch;
	      monitoring( data, fout );
	    }
	    oracle.set_eval_count( eval_count % n );

	    // random_shuffle( data.begin(), data.end() );	    

	    if( check_termination( max_epoch ) ){ 
	      return true;
	    }
	  }
	}

	// tau_k, alpha_{k+1}
	value_type tau   = get_tau( k );
	value_type alpha = get_alpha( k );

	// std::cout << tau << " " << alpha << std::endl;

	// minibatch b_{k+1}
        int64_t b = get_minibatch_size( n, k );
	mb_end = mb_begin;
        bool end_f = ! safe_advance( mb_end, data.end(), b );
        // std::cout << "minibatch_size: " << b;
        // std::cout << ", real minibatch_size: " << mb_end-mb_begin << std::endl;

	// update x
	// x = (1-tau)*y + tau*z
	x = y;
	x.scale( 1.-tau );
	x.plus( tau, z );

	// update svrg
	// stochastic gradient at x
	oracle.gradient( svrg, x, mb_begin, mb_end );
	// svrg.dump( std::cout, "sg=");

	// stochastic gradient at base_point
	oracle.gradient( base_sgrad, base_point, mb_begin, mb_end );
	svrg.plus( -1., base_sgrad );
	svrg.plus( 1., base_grad );
	// svrg.dump( std::cout, "svrg=");

	// L2 regularization
	if( lambda > 0 ){
	  svrg.plus( lambda, x );
	}

	// update y
	if( restart_method==2 || restart_method==3 || restart_method==4 ){
	  prev_y = y;
	}

	y = x;
	y.plus( -eta, svrg );

	// update z
	z.plus( -alpha, svrg );

	if( end_f ){
	  if( debug_f ){ std::cout << "end" << std::endl; }
	  mb_begin = data.begin();
	  mb_end   = mb_begin;
	}else{
	  safe_advance( mb_begin, data.end(), b );
	  // mb_begin = mb_end;
	}

	if( restart_method==1 ){
	  if( restart_test1( n, b ) ){
	    if( debug_f ){ std::cout << "restart1" << std::endl; }
	    break;
	  }

	}else if (restart_method==2 ){
	  if( restart_test2() ){
	    if( debug_f ){ std::cout << "restart2" << std::endl; }
	    break;
	  }	 

	}else if ( restart_method==3 ){ // hybrid test bad

	  if( restart_test1( n, b ) ){
	    if( debug_f ){ std::cout << "restart3_1" << std::endl; }
	    break;
	  }

	  if( restart_test2() ){
	    if( debug_f ){ std::cout << "restart3_2" << std::endl; }
	    break;
	  }	 

	}
	else if ( restart_method==4 ) {

	  if( restart_test4( n, b ) ){
	    if( debug_f ){ std::cout << "restart4" << std::endl; }
	    break;
	  }	  

	}

	k++;
      }

      return false;
    }

    inline bool restart_test4( int64_t n, int64_t b )
    {
      m += b;
      if( n <= m ){

	if( 10*n <= m ){
	  m = 0;
	  if( debug_f ){ std::cout << "restart4_1" << std::endl; }
	  return true;

	}else{

	  if( restart_test2() ){
	    m = 0;
	    if( debug_f ){ std::cout << "restart4_2" << std::endl; }
	    return true;

	  }else{
	    return false;
	  }
	}

      }else{
	return false;
      }
    }

    inline bool restart_test1( int64_t n, int64_t b )
    {
      m += b;
      if( m >= n ){
	m = 0;
	return true;

      }else{
	return false;
      }
    }

    inline bool restart_test2() 
    {
      ParamType y_diff;
      y_diff = y;
      y_diff.plus( -1., prev_y );
      double val = y_diff.dot( svrg );

      if( val > 0 ){
	y = prev_y;
	return true;
      }else{
	return false;
      }
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

    inline value_type accuracy( ParamType &p, data_chunk_type &data )
    {
      return oracle.accuracy( p, data.begin(), data.end() );
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
