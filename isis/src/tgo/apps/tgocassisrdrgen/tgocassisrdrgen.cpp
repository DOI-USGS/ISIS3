#include "Isis.h"

#include <QRegularExpression>
#include <QString>
#include <QDomDocument>

#include "Application.h"
#include "Cube.h"
#include "ExportDescription.h"
#include "FileName.h"
#include "Process.h"
#include "ProcessExportPds4.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlToXmlTranslationManager.h"

using namespace Isis;
using namespace std;

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

  PvlObject *label= icube->label();
  PvlKeyword &instrument = label->findObject("IsisCube").findKeyword("InstrumentId", Pvl::Traverse);

  // Check if the cube is able to be translated into a CaSSIS xml file
  // This could very well be unnecessary
  if (!instrument.isEquivalent("CaSSIS")) {
    QString msg = "Input file [" + ui.GetFileName("FROM") +
                "] does not appear to be a CaSSIS RDR product. The image" +
                "instrument is not the CaSSIS instrument";
    throw  IException(IException::User, msg, _FILEINFO_);
  }

  // std PDS4 label
  process.StandardPds4Label();
  // Add additional cassis label data here
  QDomDocument &pdsLabel = process.GetLabel();
  PvlToXmlTranslationManager cubeLab(*(icube->label()),
                                    "$tgo/translations/tgoCassisExport.trn");
  cubeLab.Auto(pdsLabel);

  ProcessExportPds4::translateUnits(pdsLabel);
  
  QString outFile = ui.GetFileName("TO");
  
  process.WritePds4(outFile);
}
