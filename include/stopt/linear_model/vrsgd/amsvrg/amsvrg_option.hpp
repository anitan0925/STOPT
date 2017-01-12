/* -*- coding:utf-8 -*- */
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <stopt/core/common.hpp>
#include <stopt/external.hpp>

namespace stopt
{
  class AMSVRGOption
  {
  public:
    real_type lambda;
    real_type eta;
    int_type epoch;
    std::string input_file, output_file, test_file;
    std::string weight_file, progress_file;
    bool biased_f;
    int_type restart_method;
    real_type accuracy;
    
    AMSVRGOption() : 
      lambda(0),
      eta(1.0),
      epoch(10),
      restart_method(1),
      accuracy(10.),
      weight_file("weight.txt"),
      progress_file("progress.txt"),
      biased_f(true)
    {
    }
    
    inline bool set_args( int_type argc, char **argv )
    {
      cxxopts::Options opt("AMSVRG","An Option for AMSVRG");
      opt.add_options()
	("r,lambda", "lambda", cxxopts::value<real_type>() )
	("l,eta", "eta", cxxopts::value<real_type>() )
	("e,epoch", "epoch", cxxopts::value<int_type>() )
	("i,input", "input", cxxopts::value<std::string>() )
	("o,output", "output", cxxopts::value<std::string>() )
	("w,weight", "weight", cxxopts::value<std::string>() )
	("t,test", "test", cxxopts::value<std::string>() )
	("p,progress", "progress", cxxopts::value<std::string>() )
	("a,biased_flag", "biased_flag", cxxopts::value<int_type>() )
	("s,restart_method", "restart_method", cxxopts::value<int_type>() )
	("c,accuracy", "accuracy", cxxopts::value<real_type>() )
	("f,config", "config", cxxopts::value<std::string>() );

      opt.parse( argc, argv );

      std::ifstream fin;
      fin.open( opt["config"].as<std::string>() );
      if( fin ){
	nlohmann::json j;
	fin >> j;
	if( j.find("lambda")          != j.end() ){ lambda          = j["lambda"]; }
	if( j.find("eta")             != j.end() ){ eta             = j["eta"]; }
	if( j.find("epoch")           != j.end() ){ epoch           = j["epoch"]; }
	if( j.find("input")           != j.end() ){ input_file      = j["input"]; }
	if( j.find("test")            != j.end() ){ test_file       = j["test"]; }
	if( j.find("output")          != j.end() ){ output_file     = j["output"]; }
	if( j.find("weight")          != j.end() ){ weight_file     = j["weight"]; }
	if( j.find("progress")        != j.end() ){ progress_file   = j["progress"]; }
	if( j.find("biased_flag")     != j.end() ){ biased_f        = j["biased_flag"]; }
	if( j.find("restart_method")  != j.end() ){ restart_method  = j["restart_method"]; }
	if( j.find("accuracy")        != j.end() ){ accuracy        = j["accuracy"]; }
      }
      fin.close();

      if( opt.count("lambda")         ){ lambda         = opt["lambda"].as<real_type>(); }
      if( opt.count("eta")            ){ eta            = opt["eta"].as<real_type>(); }
      if( opt.count("epoch")          ){ epoch          = opt["epoch"].as<int_type>(); }
      if( opt.count("input")          ){ input_file     = opt["input"].as<std::string>(); }
      if( opt.count("test")           ){ test_file      = opt["test"].as<std::string>(); }
      if( opt.count("output")         ){ output_file    = opt["output"].as<std::string>(); }
      if( opt.count("weight")         ){ weight_file    = opt["weight"].as<std::string>(); }
      if( opt.count("progress")       ){ progress_file  = opt["progress"].as<std::string>(); }
      if( opt.count("biased_flag")    ){ biased_f       = opt["biased_flag"].as<int_type>(); }
      if( opt.count("restart_method") ){ restart_method = opt["restart_method"].as<int_type>(); }
      if( opt.count("accuracy")       ){ accuracy       = opt["accuracy"].as<real_type>(); }

      return true;
    }
  };
}
