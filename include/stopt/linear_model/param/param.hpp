/* -*- coding:utf-8 -*- */
#pragma once

#include <iostream>
#include <fstream>
#include <stopt/core/sparse_vector.hpp>

namespace stopt
{
  template < class KeyType, class ValueType >
  class Param
  {
  public:
    typedef Param < KeyType, ValueType > self_type;
    typedef KeyType key_type;
    typedef ValueType value_type;
    typedef SparseVector<KeyType,ValueType> vector_type;

    ValueType scalar;
    vector_type weight;
    ValueType bias;

  public:

    Param() : scalar(1), bias(0)
    {
    }

    Param( const vector_type &weight0, const ValueType bias0 ) 
      : scalar(1), weight(weight0), bias(bias0)
    {
    }

    ~Param()
    {
      weight.delete_all();
    }

    inline Param &operator=( const Param &rhds )
    {
      scalar        = rhds.scalar;
      weight        = rhds.weight;
      bias          = rhds.bias;
      return *this;
    }

    template < class DataType >
    inline ValueType value( const DataType &x, double dum=1.0 )
    {
      return scalar * weight.dot(x) + bias;
    }

    inline void scale_weight( const ValueType sc )
    {
      scalar *= sc;
      if( fabs(scalar) <= 1e-5 || fabs(scalar) >= 1e+5 ){
	flush();
      }
    }

    inline void scale_bias( const ValueType bsc )
    {
      bias *= bsc;
    }

    inline void scale( const ValueType sc, const ValueType bsc )
    {
      scale_weight(sc);
      scale_bias(bsc);
    }

    inline void scale( const ValueType sc )
    {
      scale_weight(sc);
      scale_bias(sc);
    }

    // w = w + step_size*x
    // Returning inner step size.
    template < class DataType >
    inline ValueType plus_weight( const ValueType c, const DataType &x )
    {
      ValueType inner_step_size = c / scalar;
      weight.plus( inner_step_size, x );
      return inner_step_size;
    }

    inline void plus_bias( const ValueType c )
    {
      bias += c;
    }

    inline void plus( ValueType c, self_type &v )
    {
      v.flush();
      plus_weight( c, v.weight );
      plus_bias( c*v.bias );
    }

    inline void flush()
    {
      weight.scale(scalar);
      scalar = 1.0;
    }

    inline void dump( std::ostream &fout, std::string sufix )
    {
      flush();

      fout << "#weight" << std::endl;
      fout << sufix;
      for( typename vector_type::iterator itr = weight.begin(); itr != weight.end(); ++itr ){
	if( itr!=weight.begin() ){  fout << " ";	}
	fout << itr->first << ":" << itr->second;
      }
      fout << std::endl;
      fout << "#bias" << std::endl;
      fout << sufix << bias << std::endl;
    }

    inline void reset()
    {
      scalar = 1;
      weight.delete_all();
      bias = 0;
    }

    // Return copy.
    inline self_type get_param()
    {
      flush();
      self_type return_param(*this);
      return return_param;
    }

    inline ValueType l2()
    {
      flush();
      return weight.dot(weight) + bias*bias;
    }

    inline ValueType l1()
    {
      flush();
      ValueType val = 0;
      for( typename vector_type::iterator itr=weight.begin(); itr!=weight.end(); ++itr ){
	val += fabs( itr->second );
      }
      val += fabs(bias);

      return val;
    }

    inline ValueType dot( self_type &rhds )
    {
      return ( scalar * rhds.scalar * weight.dot( rhds.weight ) + bias * rhds.bias );

    }
  };
}
