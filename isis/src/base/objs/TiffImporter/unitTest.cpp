#include <iomanip>

#include <QFile>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "TiffImporter.h"

using namespace std;
using namespace Isis;


int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << setprecision(9);

  try {
    cout << "Testing TiffImporter..." << endl << endl;
    FileName inputName("test.tif");

    cout << "Creating Instance" << endl;
    TiffImporter *importer = new TiffImporter(inputName);

    cout << "Importing" << endl;
    FileName outputName("test.cub");
    importer->import(outputName);

    cout << "Clean-up" << endl;
    delete importer;
    QFile::remove(outputName.expanded());

    cout << endl << "Done" << endl;
  }
  catch (IException &e) {
    e.print();
  }
}

