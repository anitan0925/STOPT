#pragma once

#include <stopt/linear_model/sgd/averaged_param.hpp>
#include <unordered_map>
#include <stopt/core/sparse_vector.hpp>

namespace stopt
{
  template < class LabelType, class KeyType, class ValueType >
  class LabeledAveragedParam : public SparseVector< LabelType,AveragedParam<KeyType,ValueType> >
  {
  public:
    typedef SparseVector< LabelType,AveragedParam<KeyType,ValueType> > base_type;
    typedef AveragedParam<KeyType,ValueType> param_type;
    typedef LabeledAveragedParam<LabelType,KeyType,ValueType> self_type;
    typedef LabelType label_type;
    // typedef typename base_type::iterator iterator;
    // typedef typename base_type::const_iterator const_iterator;    

    inline void insert_label( label_type label )
    {
      if( ! ( this->has_key(label) ) ){
	this->insert( label, param_type() );
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
    
    inline void scale( const double sc )
    {
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	itr->second.scale(sc,sc);
      }
    }
 
    inline void flush()
    {
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	itr->second.flush();
      }
    }

    inline self_type get_param()
    {
      flush();

      self_type original_param(*this);

      for( typename base_type::iterator itr=original_param.begin(); itr!=original_param.end(); ++itr ){
	itr->second.weight = itr->second.average;
	itr->second.bias = itr->second.abias;
	itr->second.average.delete_all();
	itr->second.abias = 0;
      }

      return original_param;
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

    inline void dump_average( std::ostream &fout, const std::string &s )
    {
      fout << s << std::endl;
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	std::stringstream ssufix;
	ssufix << itr->first << " ";
	itr->second.dump_average( fout, ssufix.str() );
      }
    }

    inline void dump_all( std::ostream &fout, const std::string &s )
    {
      fout << s << std::endl;
      for( typename base_type::iterator itr=base_type::begin(); itr!=base_type::end(); ++itr ){
	std::stringstream ssufix;
	ssufix << itr->first << " ";
	itr->second.dump( fout, ssufix.str() );
	itr->second.dump_average( fout, ssufix.str() );
      }
    }
  };
}
