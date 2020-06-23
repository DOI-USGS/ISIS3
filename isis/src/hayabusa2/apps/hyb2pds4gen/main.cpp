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

void generateCSVOutput(Pvl &inputCubeLabel);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Check if input file is indeed, a cube
  if (ui.GetFileName("FROM").right(3) != "cub") {
    QString msg = "Input file [" + ui.GetFileName("FROM") +
                  "] does not appear to be a cube";
    throw  IException(IException::User, msg, _FILEINFO_);
  }

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


/**
 * Write extra values to an output CSV formatted file.
 */
void generateCSVOutput(Pvl &inputCubeLabel) {
  if (inputCubeLabel.findObject("IsisCube").hasGroup("Instrument")) {
    UserInterface &ui = Application::GetUserInterface();
    // Create the vars for holding the info
    QString keys;
    QString values;
    const QString delimeter = ",";

    // Output the result
    fstream outFile;//QFile???
    QString outputImage = ui.GetAsString("TO");
    FileName imgOutput(outputImage);
    FileName csvOutput = imgOutput.removeExtension().setExtension("csv");

    PvlGroup instGroup = inputCubeLabel.findObject("IsisCube").findGroup("Instrument");
    if (instGroup["InstrumentId"][0].compare("amica", Qt::CaseInsensitive) == 0) {
      if (inputCubeLabel.findObject("IsisCube").hasGroup("RadiometricCalibration")) {
        outFile.open(csvOutput.expanded().toLatin1().data(), std::ios::out);
        PvlGroup radiometricGroup = inputCubeLabel.findObject("IsisCube").findGroup("RadiometricCalibration");
        outFile << "RadiometricCalibrationUnits"
                << delimeter
                << "RadianceStandard"
                << delimeter
                << "RadianceScaleFactor"
                << endl;
        outFile << radiometricGroup["Units"][0]
                << delimeter
                << radiometricGroup["RadianceStandard"][0]
                << delimeter
                << radiometricGroup["RadianceScaleFactor"][0]
                << endl;
        outFile.close();
      }
    }
    else { // NIRS
      outFile.open(csvOutput.expanded().toLatin1().data(), std::ios::out);
      outFile << "IntegrationTime" << endl;
      outFile << instGroup["IntegrationTime"][0] << endl;
      outFile.close();
    }
  }
}
