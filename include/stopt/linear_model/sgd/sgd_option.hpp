/* -*- coding:utf-8 -*- */
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <stopt/core/common.hpp>
#include <stopt/external.hpp>

namespace stopt
{
  class SGDOption
  {
  public:
    real_type lambda;
    real_type eta;
    real_type t0;
    int_type order;
    int64_t astart;
    int_type epoch;
    std::string input_file, output_file, test_file;
    std::string weight_file, progress_file;
    bool biased_f;
    
    SGDOption() : 
      lambda(0),
      eta(1.0),
      t0(1.0e+5),
      order(1),      
      astart(-1),
      epoch(10),
      weight_file("weight.txt"),
      progress_file("progress.txt"),
      biased_f(true)
    {
    }
    
    inline bool set_args( int_type argc, char **argv )
    {
      cxxopts::Options opt("SGD","An Option for SGD");
      opt.add_options()
	("r,lambda", "lambda", cxxopts::value<real_type>() )
	("l,eta", "eta", cxxopts::value<real_type>() )
	("z,t0", "t0", cxxopts::value<real_type>() )
	("d,order", "order", cxxopts::value<int_type>() )	
	("s,astart", "astart", cxxopts::value<int64_t>() )	
	("e,epoch", "epoch", cxxopts::value<int_type>() )
	("i,input", "input", cxxopts::value<std::string>() )
	("o,output", "output", cxxopts::value<std::string>() )
	("w,weight", "weight", cxxopts::value<std::string>() )
	("t,test", "test", cxxopts::value<std::string>() )
	("p,progress", "progress", cxxopts::value<std::string>() )
	("a,biased_flag", "biased_flag", cxxopts::value<int_type>() )
	("f,config", "config", cxxopts::value<std::string>() );

      opt.parse( argc, argv );

      std::ifstream fin;
      fin.open( opt["config"].as<std::string>() );
      if( fin ){
	nlohmann::json j;
	fin >> j;
	if( j.find("lambda")          != j.end() ){ lambda          = j["lambda"]; }
	if( j.find("eta")             != j.end() ){ eta             = j["eta"]; }
	if( j.find("t0")              != j.end() ){ t0              = j["t0"]; }
	if( j.find("order")           != j.end() ){ order           = j["order"]; }	
	if( j.find("astart")          != j.end() ){ astart          = j["astart"]; }		
	if( j.find("epoch")           != j.end() ){ epoch           = j["epoch"]; }
	if( j.find("input")           != j.end() ){ input_file      = j["input"]; }
	if( j.find("test")            != j.end() ){ test_file       = j["test"]; }
	if( j.find("output")          != j.end() ){ output_file     = j["output"]; }
	if( j.find("weight")          != j.end() ){ weight_file     = j["weight"]; }
	if( j.find("progress")        != j.end() ){ progress_file   = j["progress"]; }
	if( j.find("biased_flag")     != j.end() ){ biased_f        = j["biased_flag"]; }
      }
      fin.close();

      if( opt.count("lambda")         ){ lambda         = opt["lambda"].as<real_type>(); }
      if( opt.count("eta")            ){ eta            = opt["eta"].as<real_type>(); }
      if( opt.count("t0")             ){ t0             = opt["t0"].as<real_type>(); }
      if( opt.count("order")          ){ order          = opt["order"].as<int_type>(); }      
      if( opt.count("astart")         ){ astart         = opt["astart"].as<int64_t>(); }      
      if( opt.count("epoch")          ){ epoch          = opt["epoch"].as<int_type>(); }
      if( opt.count("input")          ){ input_file     = opt["input"].as<std::string>(); }
      if( opt.count("test")           ){ test_file      = opt["test"].as<std::string>(); }
      if( opt.count("output")         ){ output_file    = opt["output"].as<std::string>(); }
      if( opt.count("weight")         ){ weight_file    = opt["weight"].as<std::string>(); }
      if( opt.count("progress")       ){ progress_file  = opt["progress"].as<std::string>(); }
      if( opt.count("biased_flag")    ){ biased_f       = opt["biased_flag"].as<int_type>(); }

      return true;
    }
  };
}

