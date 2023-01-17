/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>

#include "CnetEditorWindow.h"
#include "QIsisApplication.h"

using namespace Isis;

int main(int argc, char ** argv)
{
  if (getenv("ISISROOT") == NULL || QString(getenv("ISISROOT")) == "") {
    std::cerr << "Please set ISISROOT before running any Isis applications" << std::endl;
    exit(1);
  }
  QIsisApplication app(argc, argv);
  CnetEditorWindow window;
  window.show();

  app.exec();
}
