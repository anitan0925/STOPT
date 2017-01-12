/* -*- coding:utf-8 -*- */
#pragma once

#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace stopt
{
  std::string trim(const std::string& string, std::string trimc = " \t\v\r\n")
  {
    std::string result;
    std::string::size_type left = string.find_first_not_of(trimc);
 
    if (left != std::string::npos){
      std::string::size_type right = string.find_last_not_of(trimc);
      result = string.substr(left, right - left + 1);
    }
    return result;
  }
  
  std::vector<std::string> split( const std::string &s, std::string delim=" \t:" )
  {
    std::vector<std::string> elms;
    size_t offset = 0;
    while (true) {
      size_t next = s.find_first_of(delim, offset);
      if (next == std::string::npos) {
	elms.push_back(s.substr(offset));
	return elms;
      }
      elms.push_back(s.substr(offset, next - offset));
      offset = next + 1;
    }
  }
  
  std::vector<std::string> rsplit( const std::string &s, std::string delim_ )
  {
    std::regex delim_rgx(delim_);
    std::vector<std::string> res;
    
    std::sregex_token_iterator itr( s.begin(), s.end(), delim_rgx, -1 );
    std::sregex_token_iterator end;
    for ( ; itr != end; ++itr ){
      res.push_back(*itr);
    }
    return res;
  }
  
  void reset_stringstream( std::stringstream& ss )
  {
    static const std::string empty_string;
    ss.str(empty_string);
    ss.clear();
  }
}
