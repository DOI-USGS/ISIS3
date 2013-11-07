#include "IsisDebug.h"

#include "CnetEditorWindow.h"
#include "QIsisApplication.h"

using namespace Isis;

int main(int argc, char ** argv)
{
  QIsisApplication app(argc, argv);
  CnetEditorWindow window;
  window.show();

  app.exec();
}

