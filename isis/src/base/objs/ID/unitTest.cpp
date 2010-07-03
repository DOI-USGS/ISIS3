#include <iostream>
#include "iException.h"
#include "ID.h"
#include "Preference.h"

int main() {
  Isis::Preference::Preferences(true);
  std::cout << "Test One: core test and limit test" << std::endl;
  try{
    Isis::ID pid("ABCD??EFG");
    for (int i=0; i<100; i++) {
      std::string test = pid.Next();
      if (i%10 == 0) {
        std::cout << test << std::endl;
      }
    }
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  std::cout << std::endl << "Test 2: No '?' test" << std::endl;
  try{
    Isis::ID pid2("Serial");
    for (int i=0; i<5; i++ ) {
      std::cout << pid2.Next() << std::endl;
    }
  }
  catch (Isis::iException &e){
    e.Report(false);
  }

  std::cout << std::endl << "Test 3: Broken replacement string" << std::endl;
  try{
    Isis::ID pid3("Serial??Number??");
    for (int i=0; i<5; i++ ) {
      std::cout << pid3.Next() << std::endl;
    }
  }
  catch (Isis::iException &e){
    e.Report(false);
  }

  std::cout << std::endl << "Test 4: differing start numbers" << std::endl;
  try{
    Isis::ID pid4("Test??",0);
    for (int i=0; i<5; i++) {
      std::cout << pid4.Next() << std::endl;
    }
  }
  catch (Isis::iException &e){
    e.Report(false);
  }
  return 0;
}
