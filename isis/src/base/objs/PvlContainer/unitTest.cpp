#include <iostream>
#include "PvlContainer.h"
#include "Preference.h"
using namespace std;
int main() {
  Isis::Preference::Preferences(true);

  Isis::PvlKeyword dog("DOG", 5.2, "meters");
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

  // This is testing reallocation caused by adds,
  // was a problem when using std::vector
  cout << endl << "Test reallocation ..." << endl;
  Isis::PvlKeyword * ptr1 = &ani["DOG"];
  for (int i = 0; i < 250; i++) 
    ani += Isis::PvlKeyword("Test_keyword", i);

  Isis::PvlKeyword * ptr2 = &ani["DOG"];
  if (ptr1 == ptr2) 
    cout << "Pointer to DOG is equivalent" << endl;
  else
    cout << "FAILURE: Pointer to DOG changed after multiple adds" << endl; 

}
