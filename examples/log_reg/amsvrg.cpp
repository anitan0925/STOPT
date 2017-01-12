#include <stopt/core.hpp>
#include <stopt/linear_model.hpp>

int main( int argc, char **argv )
{
  // data type
  typedef int key_type;
  typedef double value_type;
  typedef int label_type;
  typedef stopt::SparseVector<key_type,value_type> feature_type;
  typedef stopt::DataType<label_type,feature_type> data_type;  
  typedef stopt::DataLoader<label_type,feature_type>::data_chunk_type data_chunk_type;

  // solver type
  typedef stopt::LabeledParam<label_type,key_type,value_type> param_type;
  typedef stopt::LogReg< param_type, data_type > oracle_type;
  typedef stopt::AMSVRG< oracle_type, param_type, data_type > solver_type;
  typedef stopt::AMSVRGOption opt_type;  

  opt_type opt;
  opt.set_args( argc, argv );

  data_chunk_type data;  
  try{
    data = stopt::DataLoader<label_type,feature_type>::get( opt.input_file );
  }catch(std::string ferr ){
    std::cout << ferr << std::endl;
    exit(-1);
  }
  solver_type solver( opt.biased_f, opt.eta, opt.lambda, opt.restart_method, opt.accuracy );

  // set initial param
  param_type init_param;
  init_param.init( data );
  solver.init_params(init_param);

  // solve
  std::ofstream fout_prog( opt.progress_file );
  std::cout << "# TRAINING" << std::endl;
  solver.run( data, opt.epoch, fout_prog );
  fout_prog.close();
  
  // result
  param_type param = solver.get_param();
  std::ofstream fout_weight( opt.weight_file.c_str() );
  param.dump( fout_weight, "#AMSVRG LogReg" );
  fout_weight.close();

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

