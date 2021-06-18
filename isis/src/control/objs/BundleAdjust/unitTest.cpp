/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>

#include "BundleAdjust.h"
#include "IException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  qDebug() << "This class is currently tested by the jigsaw application";
  try {
    Preference::Preferences(true);
  }
  catch (IException &e) {
    e.print();
  }
}
