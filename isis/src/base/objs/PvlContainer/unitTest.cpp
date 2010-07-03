#include <iostream>
#include "PvlContainer.h"
#include "Preference.h"
using namespace std;
int main () {
  Isis::Preference::Preferences(true);

  Isis::PvlKeyword dog("DOG",5.2,"meters");
  Isis::PvlKeyword cat("CATTLE");
  cat = "Meow";
  cat.AddComment("Cats shed");

  Isis::PvlContainer ani("Animals");
  ani += dog;
  ani += cat;
  ani.AddComment("/* Pets are cool */");

//  cout << "1 ..." << endl;
  cout << ani << endl; 

//  cout << "2 ..." << endl;
  cout << (double) ani["dog"] << endl;

  ani -= "dog";
//  cout << "3 ..." << endl;
  cout << ani << endl;

//  cout << "4 ..." << endl;
  ani -= ani[0];
  cout << ani << endl;

  cout << "Test inserter ..." << endl;
  Isis::PvlKeyword monkey("Orangutan", "gross");
  ani.AddKeyword(dog, ani.Begin());
  ani.AddKeyword(monkey, ani.Begin());
  cout << ani << endl;
}
