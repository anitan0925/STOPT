/* -*- coding:utf-8 -*- */
#pragma once

#include <cmath>

namespace stopt
{
  class BasicSchedular
  {
  public:
    double eta;
    double t0;
    double e;

    BasicSchedular() : eta(1.0), t0(1.0e+5), e(1)
    {
    }

    BasicSchedular(double eta0, double t00, double e0)
      : eta(), t0(), e()
    { set(eta0, t00, e0); }

    inline void show_param( std::ostream &fout )
    {
      fout << "eta: " << eta << std::endl;
      fout << "t0: " << t0  << std::endl;
      fout << "order: " << e   << std::endl;
    }

    inline void set( double eta0=1.0, double t00=1.0e+5, double e0=1 )
    {
      eta = eta0;
      t0  = t00;
      e   = e0;
    }

    inline double learning_rate( size_t t )
    { 
      return eta/pow(t0+t, e);
    }
  };
}
