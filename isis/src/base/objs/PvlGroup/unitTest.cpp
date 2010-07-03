#include <sstream>
#include <iostream>
#include "PvlGroup.h"
#include "PvlTokenizer.h"
#include "Preference.h"
#include "iException.h"

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  Isis::PvlKeyword dog("DOG", 5.2, "meters");
  Isis::PvlKeyword cat("CATTLE");
  cat = "Meow";
  cat.AddComment("Cats shed");

  Isis::PvlGroup ani("Animals");
  ani += dog;
  ani += cat;
  ani.AddComment("/* Pets are cool");

  cout << ani << endl;

  cout << (double) ani["dog"] << endl << endl;

  ani -= "dog";
  cout << ani << endl << endl;

  ani -= ani[0];
  cout << ani << endl << endl;

  stringstream os;
  os << "# Testing" << endl
     << "/* 123 */" << endl
     << "Group=POODLE " << endl
     << "CAT=\"TABBY\" " << endl
     << "BIRD=(PARROT) \0" << endl
     << "REPTILE={SNAKE,LIZARD} \t" << endl
     << "-VEGGIE \n"
     << " "
     << "    BOVINE    =   (   COW  ,  CAMEL  ) \n  "
     << "TREE = {   \"MAPLE\"   ,\n \"ELM\" \n, \"PINE\"   }" << endl
     << "FLOWER = \"DAISY & \nTULIP \""
     << "# This is a comment\n"
     << "/* This is another comment */\n"
     << "BIG = (\"  NOT  \",\"REALLY LARGE\")" << endl
     << "EndGroup" << endl;

  PvlGroup g;
  os >> g;
  cout << g << endl;

  try {
    stringstream os2;
    os2 << "# Testing" << endl
        << "/* 123 */" << endl
        << "Group=POODLE " << endl
        << "CAT=\"TABBY\" " << endl
        << "BIRD=(PARROT) \0" << endl
        << "REPTILE={SNAKE,LIZARD} \t" << endl
        << "-VEGGIE \n"
        << " "
        << "    BOVINE    =   (   COW  ,  CAMEL  ) \n  "
        << "TREE = {   \"MAPLE\"   ,\n \"ELM\" \n, \"PINE\"   }" << endl
        << "FLOWER = \"DAISY & \nTULIP \""
        << "# This is a comment\n"
        << "/* This is another comment */\n"
        << "BIG = (\"  NOT  \",\"REALLY LARGE\")" << endl;

    PvlGroup g2;
    os2 >> g2;
    cout << g2 << endl;
  }
  catch(iException &e) {
    cout.flush();
    e.Report(false);
  }
}
