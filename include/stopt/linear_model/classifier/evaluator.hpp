/* -*- coding:utf-8 -*- */
#pragma once

#include <unordered_map>

namespace stopt
{
  template < class LabelType >
  class ClassifierEvaluator
  {
  private:
    typedef LabelType label_type;

    class Summary
    {
    public:
      size_t num, succ, fail;
      double accuracy;

      Summary() : num(0), succ(0), fail(0), accuracy(0.)
      {
      }

      inline void reset()
      {
	num = succ = fail = 0;
	accuracy = 0.;
      }
    };

  public:
    typedef Summary summary_type;
    typedef summary_type accuracy_type;
    typedef std::unordered_map< label_type, summary_type > precision_type;
    typedef std::unordered_map< label_type, summary_type > recall_type;
    typedef typename precision_type::iterator p_iterator;
    typedef typename recall_type::iterator r_iterator;

  private:
    accuracy_type accuracy_;
    precision_type precision_;
    recall_type recall_;
    
  public:

    inline void reset()
    {
      accuracy_.reset();
      precision_.clear();
      recall_.clear();
    }

    inline void add( const label_type &yorg, const label_type &ypred )
    {
      if( precision_.find(ypred) == precision_.end() ){
	precision_.insert( std::pair<label_type,summary_type>( ypred, summary_type() ) );
      }

      if( recall_.find(yorg) == recall_.end() ){
	recall_.insert( std::pair<label_type,summary_type>( yorg, summary_type() ) );
      }

      ++(accuracy_.num);
      ++(precision_[ypred].num);
      ++(recall_[yorg].num);

      if( yorg==ypred ){
	++(accuracy_.succ);
	++(precision_[ypred].succ);
	++(recall_[yorg].succ);

      }else{
	++(accuracy_.fail);
	++(precision_[ypred].fail);
	++(recall_[yorg].fail);
      }
    }

    inline accuracy_type accuracy()
    {
      if( accuracy_.num ){
	accuracy_.accuracy = ((double)accuracy_.succ)/((double)accuracy_.num);
      }else{
	accuracy_.accuracy = 0.;
      }

      return accuracy_;
    }

    inline precision_type precision()
    {
      for( p_iterator itr=precision_.begin(); itr!=precision_.end(); ++itr ){
	if( itr->second.num ){
	  itr->second.accuracy = ((double)itr->second.succ)/((double)itr->second.num);
	}else{
	  itr->second.accuracy = 0.;
	}
      }

      return precision_;
    }

    inline recall_type recall()
    {
      for( r_iterator itr=recall_.begin(); itr!=recall_.end(); ++itr ){
	if( itr->second.num ){
	  itr->second.accuracy = ((double)itr->second.succ)/((double)itr->second.num);
	}else{
	  itr->second.accuracy = 0.;
	}
      }

      return recall_;
    }
  };
}
