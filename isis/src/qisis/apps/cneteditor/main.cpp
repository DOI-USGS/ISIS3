#include <QApplication>

#include "CnetEditorWindow.h"


using namespace Isis;


int main(int argc, char ** argv)
{
  QApplication app(argc, argv);
  CnetEditorWindow * window = new CnetEditorWindow;
  window->show();

  app.exec();
}
