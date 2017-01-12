/* -*- coding:utf-8 -*- */
#pragma once

#include <unordered_map>
#include <stopt/core/common.hpp>

namespace stopt
{
  template < class KeyType, class ValueType >
  class SparseVector : public std::unordered_map< KeyType, ValueType >
  {
  public:
    typedef KeyType key_type;
    typedef ValueType value_type;
    typedef std::unordered_map< key_type, value_type > base_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    
    inline void insert(const key_type& key, const value_type &val )
    {
      base_type::insert( std::pair<key_type,value_type>(key,val) );
    }

    inline void delete_all()
    {
      base_type::clear();
    }
    
    inline void accumulate(const key_type& key, const value_type &val )
    {
      iterator itr = base_type::find(key);
      if( itr==base_type::end() ){
	base_type::insert( std::pair<key_type,value_type>(key,val) );
      }else{
	itr->second += val;
      }
    }
    
    template < class VectType >  
    inline void plus( const VectType &other )
    {
      plus(1,other);
    }
    
    template < class VectType >
    inline void plus(const value_type& coeff, const VectType& other)
    {
      for( typename VectType::const_iterator itr=other.begin();
	   itr!=other.end(); ++itr ){
	accumulate( itr->first, coeff*itr->second );
      }
    }
    
    inline void scale( const value_type& coeff)
    {
      for( iterator itr=base_type::begin();
	   itr!=base_type::end(); ++itr ){
	itr->second *= coeff;
      }
    }
    
    inline bool has_key( const key_type& key ) const
    {
      return base_type::find(key) != base_type::end();
    }
    
    inline void delete_key( const key_type& key )
    {
      base_type::erase(key);
    }
    
    inline value_type get( const key_type& key,
			   const value_type &default_value ) const
    {
      const_iterator itr = base_type::find(key);
      if( itr!=base_type::end() ){
	return itr->second;
      }else{
	return default_value;
      }
    }
    
    template < class Vect >
    inline typename TypePromote
    <
      value_type, 
      typename Vect::value_type
      >
    ::result_type dot( const Vect &other ) const
    {    
      typename TypePromote< value_type, typename Vect::value_type >::result_type
	result=0;
      
      for( typename Vect::const_iterator itr=other.begin();
	   itr!=other.end(); ++itr ){
	result += this->get(itr->first,0) * itr->second;
      }
      return result;
    }  
    
    inline void show( std::ostream &fout, const std::string &s ) const
    {
      fout << s << std::endl;
      fout << "size: " << base_type::size() << std::endl;
      
      for( const_iterator itr=base_type::begin();
	   itr!=base_type::end(); ++itr ){
	fout << "[" << itr->first << "] " << itr->second << std::endl;
      }
      fout << ";" << std::endl;
    }  
  };
  
}
