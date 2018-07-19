#include "IsisDebug.h"

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

