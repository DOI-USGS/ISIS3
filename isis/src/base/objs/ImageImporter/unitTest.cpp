#include <iomanip>

#include <QFile>

#include "FileName.h"
#include "IException.h"
#include "ImageImporter.h"
#include "Preference.h"

using namespace std;
using namespace Isis;


int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << setprecision(9);

  try {
    cout << "Testing ImageImporter..." << endl << endl;
    FileName inputName("test.png");

    cout << "Creating Instance" << endl;
    ImageImporter *importer = ImageImporter::fromFileName(inputName);

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

