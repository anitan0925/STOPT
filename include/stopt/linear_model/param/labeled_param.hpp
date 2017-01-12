/* -*- coding:utf-8 -*- */
#pragma once

#include <vector>
#include <unordered_map>
#include <stopt/core/sparse_vector.hpp>

namespace stopt
{
  template < class LabelType, class KeyType, class _ValueType >
  class LabeledParam : public SparseVector< LabelType,Param<KeyType,_ValueType> >
  {
  public:
    typedef LabelType label_type;
    typedef SparseVector< LabelType,Param<KeyType,_ValueType > > base_type;
    typedef _ValueType ValueType;
    typedef Param<KeyType,ValueType> param_type;
    typedef LabeledParam<LabelType,KeyType,ValueType> self_type;

    inline LabeledParam &operator=( LabeledParam &rhds )
    {
      for( typename base_type::iterator itr=rhds.begin(); itr!=rhds.end(); ++itr ){
	insert_label( itr->first );
	(*this)[ itr->first ] = itr->second;
      }
       return *this;
    }

    inline void init_label( std::vector<label_type> &labels )
    {
      for( typename std::vector<label_type>::iterator itr=labels.begin(); itr!=labels.end(); ++itr ){
	insert_label( *itr );
      }
    }

    template < class D >
    inline void init( std::vector< D > &data )
    {
      for( typename std::vector< D >::iterator itr=data.begin(); itr!=data.end(); ++itr ){
	(*this).insert_label( itr->first );
	(*this)[ itr->first ].plus_weight( 0., itr->second );
      }
    }

    inline void flush()
    {
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	itr->second.flush();
      }
    }

    inline void reset()
    {
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	itr->second.reset();
      }
    }

    inline self_type get_param()
    {
      flush();
      return (*this);
    }

    inline void dump( std::ostream &fout, const std::string &s )
    {
      fout << s << std::endl;
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	std::stringstream ssufix;
	ssufix << itr->first << " ";
	itr->second.dump( fout, ssufix.str() );
      }
    }

    inline void plus( ValueType c, self_type &v )
    {
      for( typename base_type::iterator itr=v.begin(); itr!=v.end(); ++itr ){
	insert_label( itr->first );
	(*this)[itr->first].plus( c, itr->second );
      }
    }

    inline void insert_label( label_type label )
    {
      if( ! ( this->has_key(label) ) ){
	this->insert( label, param_type() );
      }
    }

    inline ValueType l2()
    {
      ValueType val=0;
      flush();
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	val += itr->second.l2();
      }
      return val;
    }

    inline ValueType l1()
    {
      ValueType val=0;
      flush();
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	val += itr->second.l1();
      }
      return val;
    }

    inline void scale( const ValueType sc )
    {
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	itr->second.scale(sc,sc);
      }
    }

    inline ValueType dot( self_type &rhds )
    {
      ValueType val = 0.;

      for( typename base_type::iterator itr=rhds.begin(); itr!=rhds.end(); ++itr ){
	if( this->has_key( itr->first ) ){
	  val += ( (*this)[ itr->first ] ).dot( itr->second );
	}
      }

      return val;
    }

    inline void check_scalar()
    {
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	std::cout << "label: " << itr->first << ", scalar: " << itr->second.scalar << std::endl;
      }
    }
  }; 
}
