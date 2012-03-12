#include "FileList.h"
#include "IException.h"
#include "Preference.h"

using namespace std;

int main(void) {
  Isis::Preference::Preferences(true);
  cout << "Testing on unitTest.list:" << endl;
  Isis::FileList fl2("unitTest.list");
  fl2.Write(cout);

  cout << "Testing on nonexistant file:" << endl;
  try {
    Isis::FileList fl2("NoWayThisFileExists");
  }
  catch(Isis::IException &e) {
//    e.print();
    cerr << "Unable to open the file" << endl;
  }
  return 0;
}
