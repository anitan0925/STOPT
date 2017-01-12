/* -*- coding:utf-8 -*- */
#pragma once

#include <vector>
#include <stopt/core/common.hpp>
#include <stopt/linear_model/sgd/sgd_option.hpp>
#include <stopt/linear_model/sgd/schedular.hpp>
#include <stopt/linear_model/sgd/averager.hpp>

namespace stopt
{
  template < class OracleType, class ParamType, class DataType >
  class SGD
  {
  public:
    typedef OracleType oracle_type;
    typedef ParamType param_type;
    typedef typename std::vector<DataType> data_chunk_type;    
    typedef BasicSchedular schedular_type;

  protected:
    oracle_type oracle;
    param_type x;
    int_type epoch;

    bool debug_f;

  public:
    SGD( const bool biased_f_, const real_type lambda_,
	 const real_type eta_, const real_type t0_, const int_type order_,
	 const int64_t astart_ )
    {
      oracle.biased_f = biased_f_;
      oracle.lambda = lambda_;
      oracle.average_f = astart_ >= 0 ? true : false;
      oracle.averager.set( astart_ );      
      oracle.schedular.set( eta_, t0_, order_ );
      epoch = 0;
      debug_f = false;
    }

    inline bool check_termination( const int_type max_epoch )
    {
      return epoch >= max_epoch;
    }    

    inline void show_param( std::ostream &fout )
    {
      fout << "[SGD_PARAMETER]" << std::endl;
      oracle.show_param(fout);
    }

    inline void init_params( ParamType param )
    {
      x = param;
    }    
    
    inline void run( data_chunk_type &data, const int_type max_epoch, std::ostream &fout )
    {
      show_param( std::cout );

      fout << "epoch,value" << std::endl;
      while(true){
	random_shuffle( data.begin(), data.end() );
	if( inner_run( data, max_epoch, fout ) ){ break; }
      }      
    }

    inline bool inner_run( data_chunk_type &data, const int_type max_epoch, std::ostream &fout )
    {
      typename data_chunk_type::iterator itr = data.begin();
      int64_t n = data.size();

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

	    if( check_termination( max_epoch ) ){ 
	      return true;
	    }
	  }
	}

	oracle.update( x, itr );
	
	++itr;
	if( itr == data.end() ){
	  itr = data.begin();
	}
      }
    }

    inline void monitoring( data_chunk_type &data, std::ostream &fout )
    {
      // To calculate the loss at the averaged param, call get_param().
      param_type tmp_x = get_param();
      real_type val = regularized_loss( tmp_x, data );
      fout << epoch << "," << std::fixed << std::setprecision(16) << val << std::endl;
      std::cout << "epoch: " << epoch << ", loss: " << std::fixed << std::setprecision(16) << val << std::endl;
    }    

    inline real_type regularized_loss( ParamType &p, data_chunk_type &data )
    {
      real_type val = 0;
      p.flush();
      
      val  = oracle.loss( p, data.begin(), data.end() );
      val += oracle.regularization_value( p );

      return val;
    }

    inline ParamType get_param()
    {
      return x.get_param();
    }    
  };
}

