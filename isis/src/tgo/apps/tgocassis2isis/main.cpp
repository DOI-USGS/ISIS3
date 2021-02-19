/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include <QString>
#include <QtMath>

#include "AlphaCube.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "LineManager.h"
#include "OriginalXmlLabel.h"
#include "Preference.h"
#include "ProcessImport.h"
#include "PvlTranslationTable.h"
#include "UserInterface.h"
#include "XmlToPvlTranslationManager.h"

using namespace std;
using namespace Isis;

void translateCoreInfo(FileName &inputLabel, ProcessImport &importer);
void translateCoreInfo(XmlToPvlTranslationManager labelXlater, ProcessImport &importer);
bool translateMappingLabel(FileName inputLabel, Cube *outputCube);
bool translateMosaicLabel(FileName inputLabel, Cube *outputCube);
void translateLabels(FileName &inputLabel, Cube *outputCube, QString transFile);
QString convertUniqueIdToObservationId(Pvl &outputLabel);

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

    QString transRawFile = "TgoCassisInstrument.trn";
    QString transExportFile = "TgoCassisExportedInstrument.trn";

    // first assume lev1b image
    Pvl *outputLabel = outputCube->label();
    QString target = "";
    try {
      translateLabels(xmlFileName, outputCube, transRawFile);
    }
    catch (IException &e) {
      if (translateMappingLabel(xmlFileName, outputCube)) {
        if (!translateMosaicLabel(xmlFileName, outputCube)) {
          translateLabels(xmlFileName, outputCube, transExportFile);
        }
        else {
          if(outputLabel->findObject("IsisCube").hasGroup("Instrument")) {
            outputLabel->findObject("IsisCube").deleteGroup("Instrument");
          }
        }
      }
      else {
        if(outputLabel->findObject("IsisCube").hasGroup("Mapping")) {
          outputLabel->findObject("IsisCube").deleteGroup("Mapping");
        }
        translateLabels(xmlFileName, outputCube, transExportFile);
      }
    }

    if (!outputCube->group("Archive").hasKeyword("ObservationId")){
      convertUniqueIdToObservationId(*outputLabel);
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
 */
void translateCoreInfo(FileName &inputLabel, ProcessImport &importer) {
  // Get the directory where the Tgo translation tables are
  QString missionDir = "$ISISROOT/appdata/translations/";

  // Get the translation manager ready
  FileName transFile;
  try {
    transFile = FileName(missionDir + "TgoCassis.trn");
    XmlToPvlTranslationManager labelXlater(inputLabel, transFile.expanded());
    translateCoreInfo(labelXlater, importer);
  }
  catch (IException &e) {
    // if exported, use this!
    transFile = FileName(missionDir + "TgoCassisRdr.trn");
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
 *
 * @internal
 *   @history 2017-01-20 Jeannie Backer - Original Version
 *   @history 2017-01-21 Krisitn Berry - Flipped ns & nl. They're flipped in the CaSSIS header.
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
 * Translate the cartographic info from the xml.
 *
 * @param xmlFileName The xml label file name for the input image.
 * @param outputCube Pointer to output cube where ISIS labels will be added and
 *                   updated.
 */
bool translateMappingLabel(FileName xmlFileName, Cube *outputCube) {
  //Translate the Mapping Group
  try {
    QString missionDir = "$ISISROOT/appdata/translations/";
    FileName mapTransFile(missionDir + "TgoCassisMapping.trn");

    // Get the translation manager ready for translating the mapping label

    XmlToPvlTranslationManager labelXMappinglater(xmlFileName, mapTransFile.expanded());

    // Pvl output label
    Pvl *outputLabel = outputCube->label();
    labelXMappinglater.Auto(*(outputLabel));
  }
  catch (IException &e) {
    Pvl *outputLabel = outputCube->label();
    if(outputLabel->hasGroup("Mapping")) {
      outputLabel->deleteGroup("Mapping");
    }
    return false;
  }
  return true;
}


/**
 * Translate the Mosaic group info from the xml.
 *
 * @param xmlFileName The xml label file name for the input image.
 * @param outputCube Pointer to output cube where ISIS labels will be added and
 *                   updated.
 */
bool translateMosaicLabel(FileName xmlFileName, Cube *outputCube) {
  QDomDocument xmlDoc;

  QFile xmlFile(xmlFileName.expanded());
  if ( !xmlFile.open(QIODevice::ReadOnly) ) {
    QString msg = "Could not open label file [" + xmlFileName.expanded() +
                  "].";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  QString errmsg;
  int errline, errcol;
  if ( !xmlDoc.setContent(&xmlFile, false, &errmsg, &errline, &errcol) ) {
    xmlFile.close();
    QString msg = "XML read/parse error in file [" + xmlFileName.expanded()
        + "] at line [" + toString(errline) + "], column [" + toString(errcol)
        + "], message: " + errmsg;
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  xmlFile.close();

  QDomElement inputParentElement = xmlDoc.documentElement();
  if (!inputParentElement.isNull()) {
    inputParentElement = inputParentElement.firstChildElement("Identification_Area");
    if (!inputParentElement.isNull()) {
      QDomElement logicalId = inputParentElement.firstChildElement("logical_identifier");
      if (!logicalId.isNull()) {
        QString logicalIdText = logicalId.text();
        QStringList logicalIdStringList = logicalIdText.split(":");
        if (logicalIdStringList.contains("data_mosaic")) {
          try {
            QString missionDir = "$ISISROOT/appdata/translations";
            FileName bandBinTransFile(missionDir + "/translations/TgoCassisMosaicBandBin.trn");
            // Get the translation manager ready for translating the band bin label
            XmlToPvlTranslationManager labelXBandBinlater(xmlFileName, bandBinTransFile.expanded());
            // Pvl output label
            Pvl *outputLabel = outputCube->label();
            labelXBandBinlater.Auto(*(outputLabel));
            FileName mosaicTransFile(missionDir + "/translations/TgoCassisMosaic.trn");

            // Get the translation manager ready for translating the mosaic label
            XmlToPvlTranslationManager labelXMosaiclater(xmlFileName, mosaicTransFile.expanded());
            labelXMosaiclater.Auto(*(outputLabel));
            return true;
          }
          catch (IException &e) {
            Pvl *outputLabel = outputCube->label();
            if(outputLabel->hasGroup("Mosaic")) {
              outputLabel->deleteGroup("Mosaic");
            }
            if(outputLabel->hasGroup("BandBin")) {
              outputLabel->deleteGroup("BandBin");
            }
            return false;
          }
        }
      }
    }
  }
  return false;
}


/**
 * Translate instrument, bandbin, and archive info from xml label into ISIS
 * label and add kernels group.
 *
 * @param inputLabel Reference to the xml label file name for the input image.
 * @param outputCube Pointer to output cube where ISIS labels will be added and
 *                   updated.
 *
 * @internal
 *   @history 2017-01-20 Jeannie Backer - Original Version
 *   @history 2017-01-23 Kristin Berry - Added support for bandBin group and archive group
 */
void translateLabels(FileName &inputLabel, Cube *outputCube, QString instTransFile) {
  // Get the directory where the Tgo translation tables are
  QString missionDir = "$ISISROOT/appdata/translations/";
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
  FileName bandBinTransFile(missionDir + "TgoCassisBandBin.trn");
  XmlToPvlTranslationManager bandBinXlater(inputLabel, bandBinTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  bandBinXlater.Auto(*(outputLabel));

  PvlGroup &bandBin = outputLabel->findGroup("BandBin", Pvl::Traverse);
  bandBin.findKeyword("Center").setUnits("nm");
  bandBin.findKeyword("Width").setUnits("nm");

  // Create the Archive Group
  FileName archiveTransFile(missionDir + "TgoCassisArchive.trn");
  XmlToPvlTranslationManager archiveXlater(inputLabel, archiveTransFile.expanded());

  FileName subTransFile(missionDir + "TgoCassisSubWindow.trn");
  XmlToPvlTranslationManager subXlater(inputLabel, subTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  archiveXlater.Auto(*(outputLabel));
  subXlater.Auto(*(outputLabel));

  // Remove trailing "Z" from PDS4 .xml (on re-ingestion) and create YearDoy keyword in Archive group
  PvlKeyword *startTime = &outputLabel->findGroup("Instrument", Pvl::Traverse)["StartTime"];
  QString startTimeString = startTime[0];
  if (startTimeString.endsWith("Z", Qt::CaseInsensitive)) {
    startTimeString.chop(1);
    startTime->setValue(startTimeString);
  }

  if (outputLabel->hasGroup("StopTime")) {
    PvlKeyword *stopTime = &outputLabel->findGroup("Instrument", Pvl::Traverse)["StopTime"];
    QString stopTimeString = stopTime[0];
    if (stopTimeString.endsWith("Z", Qt::CaseInsensitive)){
      stopTimeString.chop(1);
      stopTime->setValue(stopTimeString);
    }
  }

  iTime stime(startTimeString);

  PvlGroup &archive = outputLabel->findGroup("Archive", Pvl::Traverse);

  // Calculate SummingMode keyword and add to label
  QString sumMode;
  if (inst.hasKeyword("Expanded") && (int)inst.findKeyword("Expanded") == 1) {
    sumMode = "0";
  }
  else {
    sumMode = (QString)archive["Window" + (QString)archive["WindowCount"] + "Binning"];
  }
  PvlKeyword summingMode("SummingMode", sumMode);
  outputLabel->findGroup("Instrument", Pvl::Traverse).addKeyword(summingMode);

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
  if (archive.hasKeyword("Window_Count")) {
    int windowNumber = (int)archive["Window_Count"] + 1;
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
}


QString convertUniqueIdToObservationId(Pvl &outputLabel) {
  if (outputLabel.findObject("IsisCube").hasGroup("Mosaic")) {
    return ""; // translation file should auto translate this case to Mosaic group.
               // For any other product, this ID goes in the Archive group.
  }

  QString target = "";
  if (outputLabel.findObject("IsisCube").hasGroup("Instrument")) {
    target = outputLabel.findGroup("Instrument", Pvl::Traverse)
                        .findKeyword("TargetName")[0];
  }
  else {
    target = outputLabel.findGroup("Mapping", Pvl::Traverse)
                        .findKeyword("TargetName")[0];
  }

  PvlGroup &archiveGroup = outputLabel.findGroup("Archive", Pvl::Traverse);
  QString uniqueId = archiveGroup.findKeyword("UniqueIdentifier")[0];

  QString observationId = "";
  BigInt uniqueIdDecimalValue = uniqueId.toLongLong();
  BigInt operationPeriod = (uniqueIdDecimalValue & 1879048192);
  operationPeriod /= qPow(2,28);
  FileName transFile("$ISISROOT/appdata/translations/TgoCassisOperationPeriod.trn");
  PvlTranslationTable transTable(transFile);
  observationId = transTable.Translate("OperationPeriod", toString(operationPeriod));
  BigInt orbitNumber = (uniqueIdDecimalValue & 268433408);
  orbitNumber /= qPow(2,11);
  observationId += "_";
  observationId += QString("%1").arg(orbitNumber, 6, 10, QChar('0'));

  int orbitPhase = (uniqueIdDecimalValue & 2044);
  if (target.compare("mars", Qt::CaseInsensitive) == 0) {
    orbitPhase /= qPow(2,2);
  }
  else {
    orbitPhase = 900;
  }
  observationId += "_";
  observationId += QString("%1").arg(orbitPhase, 3, 10, QChar('0'));

  int imageType = (uniqueIdDecimalValue & 3);
  observationId += "_";
  observationId += toString(imageType);

  archiveGroup += PvlKeyword("ObservationId", observationId);

  return observationId;

}
