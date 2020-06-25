#include "Isis.h"

#include <QDomDocument>
#include <QDomElement>
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

void generateXmlTables(Pvl &inputCubeLabel);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  QString translationFile = "$ISISROOT/appdata/translations/Hayabusa2OncPds4Export.trn";

  // Setup the process and set the input cube
  ProcessExportPds4 process;
  Cube *inputCube = process.SetInputCube("FROM");
  Pvl *inputLabel = inputCube->label();
  
  process.setImageType(ProcessExportPds4::BinSetSpectrum);

  QDomDocument &pdsLabel = process.StandardPds4Label();
  ProcessExportPds4::translateUnits(pdsLabel);
  
  QString logicalId = ui.GetString("PDS4LOGICALIDENTIFIER");
  process.setLogicalId(logicalId);

  PvlToXmlTranslationManager xlator(*(inputLabel), translationFile);
  xlator.Auto(pdsLabel);

  PvlGroup instGroup = inputLabel->findObject("IsisCube").findGroup("Instrument");

  QString outFile = ui.GetFileName("TO");
  process.WritePds4(outFile);

  return;
}


void generateXmlTables(Pvl &inputCubeLabel) {

}
