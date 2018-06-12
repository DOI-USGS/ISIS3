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
  Cube *inputCube = process.SetInputCube("FROM");
  Pvl *inputLabel = inputCube->label();

  QString logicalId = ui.GetString("PDS4LOGICALIDENTIFIER");
  process.setLogicalId(logicalId);

  QDomDocument &pdsLabel = process.StandardPds4Label();
  /*
   * Add additional pds label data here
   */
//  QDomDocument &pdsLabel = process.GetLabel();
  PvlToXmlTranslationManager xlator(*(inputLabel),
                                    "$hayabusa/translations/hyb1Pds4Export.trn");
  xlator.Auto(pdsLabel);

  PvlGroup instGroup = inputLabel->findObject("IsisCube").findGroup("Instrument");
  QStringList xmlPath;
  xmlPath << "Product_Observational" 
          << "Observation_Area" 
          << "Discipline_Area" 
          << "img:Imaging" 
          << "img:Subframe_Parameters";
  if (instGroup.hasKeyword("FirstLine") && instGroup.hasKeyword("LastLine")) {

    int lines = (int) instGroup["LastLine"] - (int) instGroup["FirstLine"];
    QDomElement baseElement = pdsLabel.documentElement();
    QDomElement subframeParametersElement = process.getElement(xmlPath, baseElement);

    QDomElement linesElement = pdsLabel.createElement("img:lines");
    PvlToXmlTranslationManager::setElementValue(linesElement, toString(lines));
    subframeParametersElement.appendChild(linesElement);

  }

  if (instGroup.hasKeyword("FirstSample") && instGroup.hasKeyword("LastSample")) {
    int samples = (int) instGroup["LastSample"] - (int) instGroup["FirstSample"];
    QDomElement baseElement = pdsLabel.documentElement();
    QDomElement subframeParametersElement = process.getElement(xmlPath, baseElement);

    QDomElement samplesElement = pdsLabel.createElement("img:samples");
    PvlToXmlTranslationManager::setElementValue(samplesElement, toString(samples));
    subframeParametersElement.appendChild(samplesElement);
  }

  double radianceScalingFactor = 1.0;
  if (instGroup.hasKeyword("RadianceScaleFactor")) {
    radianceScalingFactor *= (double)instGroup["RadianceScaleFactor"];
  }
  if (instGroup.hasKeyword("RadianceStandard")) {
    radianceScalingFactor *= (double)instGroup["RadianceStandard"];
  }
  xmlPath[4] = "img:Radiometric_Correction_Parameters";
  QDomElement baseElement = pdsLabel.documentElement();
  QDomElement radiometricParametersElement = process.getElement(xmlPath, baseElement);
  QDomElement radianceFactorElement = pdsLabel.createElement("img:radiance_scaling_factor_WO_units");
  PvlToXmlTranslationManager::setElementValue(radianceFactorElement, toString(radianceScalingFactor));
  radiometricParametersElement.appendChild(radianceFactorElement);

  ProcessExportPds4::translateUnits(pdsLabel);

  QString outFile = ui.GetFileName("TO");
  
  process.WritePds4(outFile);

  return;
}
