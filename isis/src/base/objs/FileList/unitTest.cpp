#include "FileList.h"
#include "IException.h"
#include "Preference.h"
#include "FileName.h"

using namespace std;
using namespace Isis;

int main(void) {
  Isis::Preference::Preferences(true);
  cerr << "Testing on unitTest.list:" << endl;
  FileList fl2(FileName("unitTest.list"));
  fl2.write(cout);

  cerr << "Testing on nonexistant file:" << endl;
  try {
    FileList fl2(FileName("NoWayThisFileExists"));
  }
  catch(Isis::IException &e) {
//    e.print();
    cerr << "Unable to open the file" << endl;
  }
  return 0;
}
