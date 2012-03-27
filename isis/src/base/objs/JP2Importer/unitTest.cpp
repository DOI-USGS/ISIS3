#include <iomanip>

#include "Filename.h"
#include "IException.h"
#include "JP2Importer.h"
#include "Preference.h"

using namespace std;
using namespace Isis;


int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << setprecision(9);

  try {
    cout << "Testing JP2Importer..." << endl << endl;
    Filename inputName("test.jp2");

    cout << "Creating Instance" << endl;
    JP2Importer *importer = new JP2Importer(inputName);

    cout << "Importing" << endl;
    Filename outputName("test.cub");
    importer->import(outputName);

    cout << "Clean-up" << endl;
    delete importer;
    remove(outputName.Expanded().c_str());

    cout << endl << "Done" << endl;
  }
  catch (IException &e) {
    e.print();
  }
}

