#include <iostream>
#include <new>
#include "MaximumLikelihoodWFunctions.h"
#include "Statistics.h"
#include "IException.h"
#include "Preference.h"


using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout.precision(6);
  cout.setf(ios::fixed,ios::floatfield);

  double temp;
  Isis::MaximumLikelihoodWFunctions wFunc(Isis::MaximumLikelihoodWFunctions::Huber);

  cout << "Test Huber" << endl;
  wFunc = Isis::MaximumLikelihoodWFunctions(Isis::MaximumLikelihoodWFunctions::Huber);
  cout << "Model set in Constructor with defualt TC: wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstant(2.0);
  cout << "TC constant re-set to 2.0:              : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstantDefault();
  cout << "TC constant re-est to default:          : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;

  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::Huber);
  cout << "Model Manually set with default TC:     : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;
  
  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::Huber,2.0);
  cout << "Model Manually set with TC =2.0:        : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl << endl;



  cout << "Test HuberModified" << endl;
  wFunc = Isis::MaximumLikelihoodWFunctions(Isis::MaximumLikelihoodWFunctions::HuberModified);
  cout << "Model set in Constructor with defualt TC: wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstant(2.0);
  cout << "TC constant re-set to 2.0:              : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstantDefault();
  cout << "TC constant re-est to default:          : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;

  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::HuberModified);
  cout << "Model Manually set with default TC:     : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;
  
  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::HuberModified,2.0);
  cout << "Model Manually set with TC =2.0:        : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl << endl;



  cout << "Test Welsch" << endl;
  wFunc = Isis::MaximumLikelihoodWFunctions(Isis::MaximumLikelihoodWFunctions::Welsch);
  cout << "Model set in Constructor with defualt TC: wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstant(2.0);
  cout << "TC constant re-set to 2.0:              : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstantDefault();
  cout << "TC constant re-est to default:          : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;

  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::Welsch);
  cout << "Model Manually set with default TC:     : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;
  
  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::Welsch,2.0);
  cout << "Model Manually set with TC =2.0:        : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl << endl;




  cout << "Test Chen" << endl;
  wFunc = Isis::MaximumLikelihoodWFunctions(Isis::MaximumLikelihoodWFunctions::Chen);
  cout << "Model set in Constructor with defualt TC: wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstant(2.0);
  cout << "TC constant re-set to 2.0:              : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl; 

  wFunc.setTweakingConstantDefault();
  cout << "TC constant re-est to default:          : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;

  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::Chen);
  cout << "Model Manually set with default TC:     : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl;
  
  wFunc.setModel(Isis::MaximumLikelihoodWFunctions::Chen,2.0);
  cout << "Model Manually set with TC =2.0:        : wFunc->sqrtWeightScaler(-0.5): ";
  temp = wFunc.sqrtWeightScaler(-0.5);
  cout << temp << " wFunc->sqrtWeightScaler(0.75): ";
  temp = wFunc.sqrtWeightScaler(0.75);
  cout << temp << " wFunc->sqrtWeightScaler(-2): ";
  temp = wFunc.sqrtWeightScaler(-2.0);
  cout << temp << " wFunc->sqrtWeightScaler(2.5): ";
  temp = wFunc.sqrtWeightScaler(2.5);
  cout << temp << endl << endl;





  





}
