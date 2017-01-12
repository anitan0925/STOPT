#include <stopt/core.hpp>
#include <stopt/linear_model.hpp>

typedef int key_type;
typedef double value_type;
typedef int label_type;
typedef stopt::SparseVector<key_type,value_type> feature_type;
typedef stopt::DataType<label_type,feature_type> data_type;  
typedef stopt::DataLoader<label_type,feature_type>::data_chunk_type data_chunk_type;
typedef stopt::LabeledParam<label_type,key_type,value_type> param_type;
typedef stopt::SGDOption opt_type;  

void load_param( const std::string &weight_file, param_type &param )
{
  std::ifstream fin( weight_file );  
  std::string line;

  if( fin.fail() ){
    std::cerr << "Error: fail to open weight file: " + weight_file << std::endl;
    exit(-1);
  }

  typedef param_type::param_type param_type;
  typedef param_type::vector_type vector_type;

  while( getline(fin, line) ){
    std::stringstream ss;
	
    if( line=="#weight" ){
      vector_type weight;

      getline(fin, line);
      std::vector<std::string> v = stopt::split( line, ": \t\n\r");
      std::vector<std::string>::iterator itr=v.begin();

      label_type label;
      ss << *itr; ss >> label;
      stopt::reset_stringstream(ss);
      ++itr;
      while( itr!=v.end() ){
	key_type key;
	ss << *itr; ss >> key;
	stopt::reset_stringstream(ss);	
	++itr;
	value_type value;
	ss << *itr; ss >> value;
	stopt::reset_stringstream(ss);		
	++itr;
	weight.insert(key,value);
      }

      if( !param.has_key(label) ){
	param.insert( label, param_type(weight,0) );
      }else{
	param[label].weight = weight;
      }

    }else if( line=="#bias" ){
      getline(fin, line);
      std::vector<std::string> v = stopt::split( line, ": \t\n\r");
      std::vector<std::string>::iterator itr=v.begin();

      label_type label;
      ss << *itr; ss >> label;
      stopt::reset_stringstream(ss);      
      ++itr;
      value_type bias; 
      ss << *itr; ss >> bias;
      stopt::reset_stringstream(ss);            

      if( !param.has_key(label) ){
	param.insert( label, param_type(vector_type(),bias));
      }else{
	param[label].bias = bias;
      }
    }
  }

  fin.close();
}

int main( int argc, char **argv )
{
  opt_type opt;
  opt.set_args( argc, argv );

  data_chunk_type data;  
  try{
    data = stopt::DataLoader<label_type,feature_type>::get( opt.test_file );
  }catch(std::string ferr ){
    std::cout << ferr << std::endl;
    exit(-1);
  }

  param_type param;
  load_param( opt.weight_file, param );

  // classifier
  if( true ){
    typedef stopt::LinearClassifier<param_type> classifier_type;
    typedef stopt::ClassifierEvaluator<label_type> evaluator_type;  
    
    std::cout << "# CLASSIFY" << std::endl;
    evaluator_type  evaluator;
    classifier_type classifier;
    classifier.set_param(param);

    for( typename data_chunk_type::iterator itr=data.begin(); itr!=data.end(); ++itr ){
      classifier_type::result_type result = classifier.predict(itr->second);
      evaluator.add( itr->first, result.predicted_label );
    }

    typedef evaluator_type::accuracy_type accuracy_type;
    typedef evaluator_type::precision_type precision_type;
    typedef evaluator_type::recall_type recall_type;
    typedef evaluator_type::p_iterator p_iterator;
    typedef evaluator_type::r_iterator r_iterator;

    accuracy_type accuracy = evaluator.accuracy();
    precision_type precision = evaluator.precision();
    recall_type recall = evaluator.recall();

    std::cout << "accuracy: " << accuracy.succ << "/" << accuracy.num
  	      << "(" << accuracy.accuracy << ")" << std::endl;

    std::cout << "precision: " << std::endl;
    for( p_iterator pitr=precision.begin(); pitr!=precision.end(); ++pitr ){
      std::cout << pitr->first << ": " << pitr->second.succ << "/" << pitr->second.num
  		<< "(" << pitr->second.accuracy << ")" << std::endl;
    }

    std::cout << "recall: " << std::endl;
    for( r_iterator ritr=recall.begin(); ritr!=recall.end(); ++ritr ){
      std::cout << ritr->first << ": " << ritr->second.succ << "/" << ritr->second.num
  		<< "(" << ritr->second.accuracy << ")" << std::endl;
    }
  }  
    
  return 0;
}

