#include <iomanip>

#include "Filename.h"
#include "IException.h"
#include "Preference.h"
#include "QtImporter.h"

using namespace std;
using namespace Isis;


int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << setprecision(9);

  try {
    cout << "Testing QtImporter..." << endl << endl;
    Filename inputName("test.png");

    cout << "Creating Instance" << endl;
    QtImporter *importer = new QtImporter(inputName);

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

