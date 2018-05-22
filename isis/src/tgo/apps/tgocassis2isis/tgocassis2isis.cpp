#include "Isis.h"

#include <QString>

#include "AlphaCube.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "LineManager.h"
#include "OriginalXmlLabel.h"
#include "Preference.h"
#include "ProcessImport.h"
#include "UserInterface.h"
#include "XmlToPvlTranslationManager.h"

using namespace std;
using namespace Isis;

void translateCoreInfo(FileName &inputLabel, ProcessImport &importer);
void translateCoreInfo(XmlToPvlTranslationManager labelXlater, ProcessImport &importer);
void translateLabels(FileName &inputLabel, Cube *outputCube, QString transFile);

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  FileName xmlFileName = ui.GetFileName("FROM");

  try {
    ProcessImport importer;
    translateCoreInfo(xmlFileName, importer);
    
    if(xmlFileName.removeExtension().addExtension("dat").fileExists()){
      importer.SetInputFile(xmlFileName.removeExtension().addExtension("dat").expanded());
    } 
    else if (xmlFileName.removeExtension().addExtension("img").fileExists()) {
      importer.SetInputFile(xmlFileName.removeExtension().addExtension("img").expanded());
    }
    else {
      QString msg = "Cannot find image file for [" + xmlFileName.name() + "]. Confirm that the "
        ".dat or .img file for this XML exists and is located in the same directory.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    
    Cube *outputCube = importer.SetOutputCube("TO");

    QString transRawFile = "/translations/tgoCassisInstrument.trn";
    QString transExportFile = "/translations/tgoCassisExportedInstrument.trn";

    try {
      translateLabels(xmlFileName, outputCube, transRawFile); 
    } 
    catch (IException &e) {
      translateLabels(xmlFileName, outputCube, transExportFile);
      
      // Try to translate a mapping group
      try {
        PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory"); 
        QString missionDir = (QString) dataDir["Tgo"];
        FileName mapTransFile(missionDir + "/translations/tgoCassisMapping.trn");

        // Get the translation manager ready for translating the mapping label

        XmlToPvlTranslationManager labelXlater(xmlFileName, mapTransFile.expanded());

        // Pvl output label
        Pvl *outputLabel = outputCube->label();
        labelXlater.Auto(*(outputLabel));
      } 
      catch (IException &e) {
        Pvl *outputLabel = outputCube->label();
        if(outputLabel->hasGroup("Mapping")) {
          outputLabel->deleteGroup("Mapping"); 
        }
      }
    }

    FileName outputCubeFileName(ui.GetFileName("TO"));

    OriginalXmlLabel xmlLabel;
    xmlLabel.readFromXmlFile(xmlFileName);

    importer.StartProcess();

    // Write out original label before closing the cube
    outputCube->write(xmlLabel);

    importer.EndProcess();
  }
  catch (IException &e) {
    QString msg = "Given file [" + xmlFileName.expanded() + "] does not appear to be a valid TGO CaSSIS label.";
      throw IException(e, IException::User, msg, _FILEINFO_);
  }

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

  // Get the translation manager ready
  FileName transFile; 
  try {
    transFile = FileName(missionDir + "/translations/tgoCassis.trn"); 
    XmlToPvlTranslationManager labelXlater(inputLabel, transFile.expanded());
    translateCoreInfo(labelXlater, importer);
  } 
  catch (IException &e) {
   // if exported, use this!
   transFile = FileName(missionDir + "/translations/tgoCassisExported.trn"); 
   XmlToPvlTranslationManager labelXlater(inputLabel, transFile.expanded());
   translateCoreInfo(labelXlater, importer);
  }
}

/**
 * Translate core info from labels and set ProcessImport object with 
 * these values.
 *
 * @param labelXlater Reference to the XmlToPvlTranslationManager objcet to use for the translation.
 * @param importer Reference to the ProcessImport object to which core info will
 *                 be set.
 */
void translateCoreInfo(XmlToPvlTranslationManager labelXlater, ProcessImport &importer) {
  // Set up the ProcessImport
  QString str;
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
void translateLabels(FileName &inputLabel, Cube *outputCube, QString instTransFile) {
  // Get the directory where the Tgo translation tables are
  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
  QString missionDir = (QString) dataDir["Tgo"];
  FileName transFile(missionDir + instTransFile);

  // Get the translation manager ready for translating the instrument label
  XmlToPvlTranslationManager labelXlater(inputLabel, transFile.expanded());

  // Pvl output label
  Pvl *outputLabel = outputCube->label();
  labelXlater.Auto(*(outputLabel));

  // Add needed keywords that are not in the translation table
  PvlGroup &inst = outputLabel->findGroup("Instrument", Pvl::Traverse);

  // Add units of measurement to keywords from translation table
  inst.findKeyword("ExposureDuration").setUnits("seconds");

  // Translate BandBin group
  FileName bandBinTransFile(missionDir + "/translations/tgoCassisBandBin.trn");
  XmlToPvlTranslationManager bandBinXlater(inputLabel, bandBinTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  bandBinXlater.Auto(*(outputLabel));

  PvlGroup &bandBin = outputLabel->findGroup("BandBin", Pvl::Traverse);
  bandBin.findKeyword("Center").setUnits("nm");
  bandBin.findKeyword("Width").setUnits("nm");

  // Create the Archive Group
  FileName archiveTransFile(missionDir + "/translations/tgoCassisArchive.trn");
  FileName subTransFile(missionDir + "/translations/tgoCassisSubWindow.trn");
  XmlToPvlTranslationManager archiveXlater(inputLabel, archiveTransFile.expanded());
  XmlToPvlTranslationManager subXlater(inputLabel, subTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  archiveXlater.Auto(*(outputLabel));
  subXlater.Auto(*(outputLabel));

  // Remove trailing "Z" from PDS4 .xml (on re-ingestion) and create YearDoy keyword in Archive group
  PvlKeyword *startTime = &outputLabel->findGroup("Instrument", Pvl::Traverse)["StartTime"];
  QString startTimeString = startTime[0];
  if (QString::compare(startTimeString.at(startTimeString.size() - 1), "Z", Qt::CaseInsensitive) == 0){
    startTimeString = startTimeString.left(startTimeString.length() - 1);
    startTime->setValue(startTimeString);
  }
  iTime stime(startTimeString);
  
  PvlGroup &archive = outputLabel->findGroup("Archive", Pvl::Traverse);
                                                  
  PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
  archive.addKeyword(yeardoy);

  // Set units on optional archived keywords
  if (archive.hasKeyword("PredictMaximumExposureTime")) {
    archive.findKeyword("PredictMaximumExposureTime").setUnits("ms");
  }
  if (archive.hasKeyword("CassisOffNadirAngle")) {
    archive.findKeyword("CassisOffNadirAngle").setUnits("deg");
  }
  if (archive.hasKeyword("PredictedRepetitionFrequency")) {
    archive.findKeyword("PredictedRepetitionFrequency").setUnits("ms");
  }
  if (archive.hasKeyword("GroundTrackVelocity")) {
    archive.findKeyword("GroundTrackVelocity").setUnits("km/s");
  }
  if (archive.hasKeyword("ForwardRotationAngle")) {
    archive.findKeyword("ForwardRotationAngle").setUnits("deg");
  }
  if (archive.hasKeyword("SpiceMisalignment")) {
    archive.findKeyword("SpiceMisalignment").setUnits("deg");
  }
  if (archive.hasKeyword("FocalLength")) {
    archive.findKeyword("FocalLength").setUnits("m");
  }
  if (archive.hasKeyword("ImageFrequency")) {
    archive.findKeyword("ImageFrequency").setUnits("ms");
  }
  if (archive.hasKeyword("ExposureTimePEHK")) {
    archive.findKeyword("ExposureTimePEHK").setUnits("ms");
  }

  // Setup the kernel group
  PvlGroup kern("Kernels");
  QString spacecraftNumber;
  QString instId  = (QString) inst.findKeyword("InstrumentId");
  QString spcName = (QString) inst.findKeyword("SpacecraftName");
  QString filter  = (QString) bandBin.findKeyword("FilterName");

  if(spcName.compare("TRACE GAS ORBITER", Qt::CaseInsensitive) == 0
     && instId.compare("CaSSIS", Qt::CaseInsensitive) == 0) {

    int spacecraftCode = -143400;

    kern += PvlKeyword("NaifFrameCode", toString(spacecraftCode));

    if (filter.compare("PAN", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143421;
    }
    else if (filter.compare("RED", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143422;
    }
    else if (filter.compare("NIR", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143423;
    }
    else if (filter.compare("BLU", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143424;
    }
    else {
      QString msg = "Unrecognized filter name [" 
        + filter
        + "].";
        throw IException(IException::User, msg, _FILEINFO_);

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

  // Add an alpha cube group based on the subwindowing
  int windowNumber = (int) archive["Window_Count"] + 1;
  QString windowString = "Window_" + toString(windowNumber);
  int frameletStartSample = (int) archive[windowString + "_Start_Sample"] + 1;
  int frameletEndSample   = (int) archive[windowString + "_End_Sample"] + 1;
  int frameletStartLine   = (int) archive[windowString + "_Start_Line"] + 1;
  int frameletEndLine     = (int) archive[windowString + "_End_Line"] + 1;
  AlphaCube frameletArea(2048, 2048,
                         frameletEndSample - frameletStartSample + 1,
                         frameletEndLine - frameletStartLine + 1,
                         frameletStartSample - 0.5, frameletStartLine - 0.5,
                         frameletEndSample + 0.5, frameletEndLine + 0.5);
  frameletArea.UpdateGroup(*outputCube);
}

