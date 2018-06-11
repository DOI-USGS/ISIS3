#include "Isis.h"

#include <QRegularExpression>
#include <QString>

#include "Application.h"
#include "Cube.h"
#include "ExportDescription.h"
#include "FileName.h"
#include "Process.h"
#include "ProcessExportPds.h"
#include "ProcessExportPds4.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlToXmlTranslationManager.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Check if input file is indeed, a cube
  if (ui.GetFileName("FROM").right(3) != "cub") {
    QString msg = "Input file [" + ui.GetFileName("FROM") +
                  "] does not appear to be a cube";
    throw  IException(IException::User, msg, _FILEINFO_);
  }

  // Setup the process and set the input cube
  ProcessExportPds4 process;
  Cube *icube = process.SetInputCube("FROM");

  process.StandardPds4Label();
  /*
   * Add additional pds label data here
   */
  QDomDocument &pdsLabel = process.GetLabel();
  PvlToXmlTranslationManager cubeLab(*(icube->label()),
                                    "$hayabusa/translations/hyb1Pds4Export.trn");
  cubeLab.Auto(pdsLabel);

  ProcessExportPds4::translateUnits(pdsLabel);

  QString outFile = ui.GetFileName("TO");
  
  process.WritePds4(outFile);

  return;
}
