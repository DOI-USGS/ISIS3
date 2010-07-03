#include "Isis.h"
#include "Preference.h"

#include <iostream>

using namespace std;
void IsisMain () {
  Isis::Preference::Preferences(true);
  cout << "That's all folks" << endl;

  vector<string> stackTrace;
  StackTrace::GetStackTrace(&stackTrace);

  if(stackTrace.size()) {
    cout << endl;
    cout << "----- Stack Trace -----" << endl;
    for(unsigned int i = 0; i < stackTrace.size(); i++) {
      cout << stackTrace[i] << endl;
    }
  
    cout << endl;
    cout << "----- ASSERTS -----" << endl; 
    ASSERT(0);
    ASSERT(1);

    cout << endl;
    cout << "----- POINTER ASSERTS -----" << endl; 
    int *test = new int[5];
    cout << "VALID:" << endl; 
    ASSERT_PTR(test);
    cout << "INVALID:" << endl; 
    ASSERT_PTR(test+1);
    cout << "NULL:" << endl; 
    ASSERT_PTR(0);

    test[0] = 5;

  }
  
}
