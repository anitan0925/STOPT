/* -*- coding:utf-8 -*- */
#pragma once

#include <vector>

namespace stopt
{
  template < class ParamType, class FeatureType >
  class Averager
  {
  public:
    typedef FeatureType feature_type;
 
  private:
    struct ASGDInfo
    {
      double step_size;
      const feature_type *x;

      ASGDInfo() : step_size(0), x(NULL)
      {
      }

      ASGDInfo( double step_size0, const feature_type *x0 )
	: step_size(step_size0), x(x0)
      {
      }

      inline void clear()
      {
	step_size = 0;
	x = NULL;
      }
    };

    ASGDInfo asgd_info;

  public:
    int64_t average_start;

    Averager() : average_start(0)
    {
    }

    inline void show_param( std::ostream &fout )
    {
      fout << "average_start: " << average_start << std::endl;
    }

    inline void set( int64_t average_start0 )
    {
      average_start = average_start0;
    }

    inline void regist( double step_size, const feature_type *xp )
    {
      asgd_info.step_size = step_size;
      asgd_info.x = xp;
    }

    template < class param_type >
    inline void average( int64_t t, param_type &param )
    {
      double mu = (t <= average_start ) ? 1. : 1./( 1.+(t-average_start) );

      if( mu >= 1 ){
	param.average.delete_all();
	param.ascalar = param.scalar;
	param.wfrac   = 1.0;
	param.abias   = param.bias;

      }else{
	param.ascalar *= (1-mu);

	if( asgd_info.step_size != 0 ){
	  param.average.plus( -1 * param.wfrac * asgd_info.step_size, *(asgd_info.x) );
	}

	param.wfrac += mu * param.scalar/param.ascalar;
	param.abias = (1-mu)*param.abias + mu*param.bias;
      }

      asgd_info.clear();
    }
  };

  template < class ParamType, class FeatureType >
  class LabeledParamAverager
  {
  public:
    typedef typename ParamType::label_type label_type;
    typedef FeatureType feature_type;
    typedef typename ParamType::param_type param_type;
    typedef ParamType labeled_param_type;
    
  private:
    struct ASGDInfo
    {
      label_type label;
      double step_size;
      const feature_type *x;

      ASGDInfo( label_type label0, double step_size0, const feature_type *x0 )
	: label(label0), step_size(step_size0), x(x0)
      {
      }
    };

    std::vector<ASGDInfo> asgd_infos;

  public:
    int64_t average_start;

    LabeledParamAverager() : average_start(0)
    {
    }

    inline void set( int64_t average_start0 )
    {
      average_start = average_start0;
    }

    inline void show_param( std::ostream &fout )
    {
      fout << "average_start: " << average_start << std::endl;
    }

    inline void regist( label_type label, double step_size, const feature_type *xp )
    {
      asgd_infos.push_back( ASGDInfo(label,step_size,xp) );
    }

    inline void average( int64_t t, labeled_param_type &param )
    {
      double mu = (t <= average_start ) ? 1. : 1./( 1.+(t-average_start) );

      if( mu >= 1 ){
	// average_start  average  weight
	for( typename labeled_param_type::iterator itr=param.begin(); itr!=param.end(); ++itr ){
	  itr->second.average.delete_all();
	  itr->second.ascalar = itr->second.scalar;
	  itr->second.wfrac   = 1.0;
	  itr->second.abias   = itr->second.bias;
	}

      }else{
	// average_start
	for( typename labeled_param_type::iterator itr=param.begin(); itr!=param.end(); ++itr ){
	  itr->second.ascalar *= (1-mu);
	}

	for( typename std::vector<ASGDInfo>::iterator itr=asgd_infos.begin(); itr!=asgd_infos.end(); ++itr ){
	  param_type &p = param[itr->label];
	  if( itr->step_size != 0 ){
	    p.average.plus( -1 * p.wfrac * itr->step_size, *(itr->x) );
	  }
	}

	for( typename labeled_param_type::iterator itr=param.begin(); itr!=param.end(); ++itr ){
	  itr->second.wfrac += mu * itr->second.scalar/itr->second.ascalar;
	  itr->second.abias = (1-mu)*itr->second.abias + mu*itr->second.bias;
	}
      }

      asgd_infos.clear();
    }
  };
}
