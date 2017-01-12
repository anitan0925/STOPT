#pragma once

#include <stopt/linear_model/param/param.hpp>

namespace stopt
{
  template < class KeyType, class ValueType >
  class AveragedParam : public Param< KeyType, ValueType >
  {
  public:
    typedef Param < KeyType, ValueType > base_type;
    typedef AveragedParam < KeyType, ValueType > self_type;

    typedef typename base_type::vector_type vector_type;
    typedef typename base_type::value_type value_type;
    vector_type average;
    value_type wfrac;
    value_type ascalar;
    value_type abias;

    AveragedParam() 
      : wfrac(0), ascalar(1), abias(0), base_type()
    {
    }

    AveragedParam( const self_type &rhds ) : base_type(rhds)
    {
      average = rhds.average;
      wfrac   = rhds.wfrac;
      ascalar = rhds.ascalar;
      abias   = rhds.abias;
    }

    ~AveragedParam() 
    {
      average.delete_all();
    }

    inline void flush()
    {
      average.plus( wfrac, base_type::weight );
      average.scale( ascalar );
      ascalar = 1;
      wfrac = 0;

      base_type::flush();
    }

    inline void dump( std::ostream &fout, std::string sufix )
    {
      flush();

      fout << "#weight" << std::endl;
      fout << sufix;
      for( typename vector_type::iterator itr = base_type::weight.begin(); itr != base_type::weight.end(); ++itr ){
	if( itr!=base_type::weight.begin() ){  fout << " ";	}
	fout << itr->first << ":" << itr->second;
      }
      fout << std::endl;
      fout << "#bias" << std::endl;
      fout << sufix << base_type::bias << std::endl;
    }

    inline void dump_average( std::ostream &fout, std::string sufix )
    {
      flush();

      fout << "#average" << std::endl;
      fout << sufix;
      for( typename vector_type::iterator itr = average.begin(); itr != average.end(); ++itr ){
	if( itr!=average.begin() ){  fout << " "; }
	fout << itr->first << ":" << itr->second;
      }
      fout << std::endl;
      fout << "#abias" << std::endl;
      fout << sufix << abias << std::endl;
    }

    inline void delete_all()
    {
      average.delete_all();
      wfrac = 0;
      ascalar = 1;
      abias = 0;
      base_type::reset();
    }

    inline self_type get_param()
    {
      flush();

      self_type original_param(*this);
      original_param.weight = original_param.average;
      original_param.bias = original_param.abias;
      original_param.average.delete_all();
      original_param.abias = 0;

      return original_param;
    }
  };  
}
