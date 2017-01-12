/* -*- coding:utf-8 -*- */
#pragma once

#include <stopt/core/string_util.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace stopt
{

  template < class... >
  class DataType
  {
  };
  
  template < class LabelType, class FeatureType >
  class DataType<LabelType, FeatureType> : public std::pair<LabelType, FeatureType>
  {
  public:
    typedef std::pair<LabelType,FeatureType> base_type;
    typedef LabelType label_type;
    typedef FeatureType feature_type;
    
    DataType( const label_type &label, const feature_type &feature ) 
      : base_type(label,feature)
    {
    }
  };
  
  template < class FeatureType >
  class DataType<FeatureType> : public FeatureType
  {
  public:
    typedef FeatureType base_type;
    typedef FeatureType feature_type;
    
    DataType( const feature_type &feature ) 
      : base_type(feature)
    {
    }
  };

  template < class... >
  class DataLoader
  {
  };
  
  template < class LabelType, class FeatureType >
  class DataLoader< LabelType, FeatureType >
  {
  public:
    typedef LabelType label_type;
    typedef FeatureType feature_type;
    typedef DataType<label_type,feature_type> data_type;
    
    typedef typename feature_type::key_type key_type;
    typedef typename feature_type::value_type value_type;
    typedef std::vector< data_type > data_chunk_type;
    
    static data_chunk_type get( std::string input_file, std::string delim=" \t:" ) 
    {
      std::ifstream ifs(input_file);
      std::string s;
      data_chunk_type data;
    
      if( ifs.fail() ){
	throw "Error: fail to open file: " + input_file;
      }
      
      while( std::getline(ifs, s) ){
	label_type label;
	feature_type feature;
	std::stringstream ss;
	
	s = trim(s);
	std::vector<std::string> res = split(s, delim);
	auto itr=res.begin();
	
	while(true){
	  if( itr==res.begin() ){
	    ss << *itr; ss >> label;
	    reset_stringstream(ss);
	    ++itr;
	    continue;
	  }else{
	    key_type key;
	    value_type value;
	    // key
	    ss << *itr; ss >> key;
	    reset_stringstream(ss);
	    ++itr;	  
	    // value
	    ss << *itr; ss >> value;
	    reset_stringstream(ss);
	    ++itr;	  	  	  	  
	    
	    // set element
	    feature.insert(key,value);
	    
	    if(itr==res.end()){ break; }
	  }
	}
	
	data.push_back( data_type(label,feature) );
      }
      
      ifs.close();
      
      return data;
    }
  };
  
  template < class FeatureType >
  class DataLoader< FeatureType >
  {
  public:
    typedef FeatureType feature_type;
    typedef DataType<feature_type> data_type;
    
    typedef typename feature_type::key_type key_type;
    typedef typename feature_type::value_type value_type;
    typedef std::vector< data_type > data_chunk_type;
    
    static data_chunk_type get( std::string input_file, std::string delim=" \t:" ) 
    {
      std::ifstream ifs(input_file);
      std::string s;
      data_chunk_type data;
      
      if( ifs.fail() ){
	std::cerr << "Error: fail to open file: " << input_file << std::endl;
      }
      
      while( std::getline(ifs, s) ){
	feature_type feature;
	std::stringstream ss;
	
	s = trim(s);
	std::vector<std::string> res = split(s, delim);
	auto itr=res.begin();
	
	while(true){
	  key_type key;
	  value_type value;
	  // key
	  ss << *itr; ss >> key;
	  reset_stringstream(ss);
	  ++itr;	  
	  // value
	  ss << *itr; ss >> value;
	  reset_stringstream(ss);
	  ++itr;	  	  	  	  
	  
	  // set element
	  feature.insert(key,value);
	  
	if(itr==res.end()){ break; }
	}
	
	data.push_back( data_type(feature) );
      }
      
      ifs.close();
      
      return data;
    }
  };

}
