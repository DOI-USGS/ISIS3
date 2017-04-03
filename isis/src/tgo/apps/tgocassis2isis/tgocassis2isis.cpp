#include "Isis.h"

#include <QString>

#include "Cube.h"
#include "FileName.h"
#include "LineManager.h"
#include "OriginalXmlLabel.h"
#include "Preference.h"
#include "ProcessImport.h"
#include "UserInterface.h"
#include "XmlTranslationManager.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void translateCoreInfo(FileName &inputLabel, ProcessImport &importer);
void translateLabels(FileName &inputLabel, Cube *outputCube);

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  FileName xmlFileName = ui.GetFileName("FROM");

  ProcessImport importer;
  translateCoreInfo(xmlFileName, importer);

  importer.SetInputFile(xmlFileName.removeExtension().addExtension("dat").expanded());
  Cube *outputCube = importer.SetOutputCube("TO");

  translateLabels(xmlFileName, outputCube);
  
  FileName outputCubeFileName(ui.GetFileName("TO"));

  OriginalXmlLabel xmlLabel;
  xmlLabel.readFromXmlFile(xmlFileName);

  importer.StartProcess();

  // Write out original label before closing the cube
  outputCube->write(xmlLabel);

  importer.EndProcess();

  return;
}


/**
 * Translate core info from labels and set ProcessImport object with 
 * these values.
 *
 * @param inputLabel Reference to the xml label file name from the input image.
 * @param importer Reference to the ProcessImport object to which core info will
 *                 be set.
 *  
 * @internal
 *   @history 2017-01-20 Jeannie Backer - Original Version
 *   @history 2017-01-21 Krisitn Berry - Flipped ns & nl. They're flipped in the CaSSIS header.
 */
void translateCoreInfo(FileName &inputLabel, ProcessImport &importer) {
  // Get the directory where the Tgo translation tables are
  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
  QString missionDir = (QString) dataDir["Tgo"];
  FileName transFile(missionDir + "/translations/tgoCassis.trn");

  // Get the translation manager ready
  XmlTranslationManager labelXlater(inputLabel, transFile.expanded());

  QString str;
  // Set up the ProcessImport
  str = labelXlater.Translate("CoreSamples");
  int ns = toInt(str);
  str = labelXlater.Translate("CoreLines");
  int nl = toInt(str);
  str = labelXlater.Translate("CoreBands");
  int nb = toInt(str);
  importer.SetDimensions(ns, nl, nb);

  str = labelXlater.Translate("CoreType");    
  importer.SetPixelType(PixelTypeEnumeration(str));

  str = labelXlater.Translate("CoreByteOrder");    
  importer.SetByteOrder(ByteOrderEnumeration(str));

  importer.SetFileHeaderBytes(0);

  str = labelXlater.Translate("CoreBase");
  importer.SetBase(toDouble(str));
  str = labelXlater.Translate("CoreMultiplier");
  importer.SetMultiplier(toDouble(str));
}


/**
 * Translate instrument, bandbin, and archive info from xml label into ISIS3 
 * label and add kernels group. 
 *
 * @param inputLabel Reference to the xml label file name for the input image.
 * @param outputCube Pointer to output cube where ISIS3 labels will be added and 
 *                   updated.
 *  
 * @internal
 *   @history 2017-01-20 Jeannie Backer - Original Version
 *   @history 2017-01-23 Kristin Berry - Added support for bandBin group and archive group
 */
void translateLabels(FileName &inputLabel, Cube *outputCube) {
  // Get the directory where the Tgo translation tables are
  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
  QString missionDir = (QString) dataDir["Tgo"];
  FileName transFile(missionDir + "/translations/tgoCassisInstrument.trn");

  // Get the translation manager ready for translating the instrument label
  XmlTranslationManager labelXlater(inputLabel, transFile.expanded());

  // Pvl output label
  Pvl *outputLabel = outputCube->label();
  labelXlater.Auto(*(outputLabel));

  // Add needed keywords that are not in the translation table
  PvlGroup &inst = outputLabel->findGroup("Instrument", Pvl::Traverse);

  // Add units of measurement to keywords from translation table
  inst.findKeyword("ExposureDuration").setUnits("seconds");

  // Translate BandBin group
  FileName bandBinTransFile(missionDir + "/translations/tgoCassisBandBin.trn");
  XmlTranslationManager bandBinXlater(inputLabel, bandBinTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  bandBinXlater.Auto(*(outputLabel));

  PvlGroup &bandBin = outputLabel->findGroup("BandBin", Pvl::Traverse);
  bandBin.findKeyword("Center").setUnits("nm");
  bandBin.findKeyword("Width").setUnits("nm");

  // Create the Archive Group
  FileName archiveTransFile(missionDir + "/translations/tgoCassisArchive.trn");
  XmlTranslationManager archiveXlater(inputLabel, archiveTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  archiveXlater.Auto(*(outputLabel));

  // Create YearDoy keyword in Archive group
  iTime stime(outputLabel->findGroup("Instrument", Pvl::Traverse)["StartTime"][0]);

  PvlGroup &archive = outputLabel->findGroup("Archive", Pvl::Traverse);
                                                  
  PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
  archive.addKeyword(yeardoy);
  archive.findKeyword("PredictMaximumExposureTime").setUnits("ms");
  archive.findKeyword("CassisOffNadirAngle").setUnits("deg");
  archive.findKeyword("PredictedRepetitionFrequency").setUnits("ms");
  archive.findKeyword("GroundTrackVelocity").setUnits("km/s");
  archive.findKeyword("ForwardRotationAngle").setUnits("deg");
  archive.findKeyword("SpiceMisalignment").setUnits("deg");
  archive.findKeyword("FocalLength").setUnits("m");
  archive.findKeyword("ImageFrequency").setUnits("ms");
  archive.findKeyword("ExposureTimePEHK").setUnits("ms");

  // Setup the kernel group
  PvlGroup kern("Kernels");
  QString spacecraftNumber;
  QString instId  = (QString) inst.findKeyword("InstrumentId");
  QString spcName = (QString) inst.findKeyword("SpacecraftId");
  QString filter  = (QString) bandBin.findKeyword("FilterName");

  if(spcName.compare("TGO", Qt::CaseInsensitive) == 0
     && instId.compare("CaSSIS", Qt::CaseInsensitive) == 0) {

    int spacecraftCode = -143400;

    kern += PvlKeyword("NaifFrameCode", toString(spacecraftCode));

    if (filter.compare("PAN", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143421;
    }
    if (filter.compare("RED", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143422;
    }
    if (filter.compare("NIR", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143423;
    }
    if (filter.compare("BLU", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143424;
    }

    // Add Kernel to BandBin Group
    bandBin.addKeyword(PvlKeyword("NaifIkCode", toString(spacecraftCode)));
  }
  else {
    QString msg = "Unrecognized Spacecraft name [" 
      + spcName
      + "] and instrument ID ["
      + instId
      + "]";
      throw IException(IException::User, msg, _FILEINFO_);
  }
  outputCube->putGroup(kern);
}

