#include "hyb2onc2isis.h"

#include "AlphaCube.h"
#include "Application.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "iTime.h"
#include "OriginalLabel.h"
#include "ProcessImportFits.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "PvlToPvlTranslationManager.h"

#include "UserInterface.h"

#include <QDebug>
#include <QString>


using namespace std;
using namespace Isis;
namespace Isis {


void hyb2onc2isis(UserInterface &ui) {



  QString fitsFileName = FileName(ui.GetFileName("FROM")).expanded();  

  QString outputCubeFileName = FileName(ui.GetFileName("TO")).expanded();


  QString target("");
  if (ui.WasEntered("TARGET")) {
    target = ui.GetString("TARGET");
  }

  CubeAttributeOutput &att =
    ui.GetOutputAttribute("TO");



  // Create a PVL to store the translated labels in
  hyb2onc2isis(fitsFileName,outputCubeFileName,att,target);

  return;

  }


Pvl hyb2onc2isis(QString fitsFileName, QString outputCubeFileName, CubeAttributeOutput att, QString target) {

  Pvl finalPvl;
  ProcessImportFits importFits;
  importFits.setFitsFile(FileName(fitsFileName));
  importFits.setProcessFileStructure(0);
  bool updatedKeywords = true;
  bool distortionCorrection = true;

  Cube *outputCube = importFits.SetOutputCube(outputCubeFileName,att);

  // Get the directory where the Hayabusa translation tables are.
  PvlGroup dataDir (Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Hayabusa2"] + "/translations/";

  // Create a PVL to store the translated labels in
  Pvl outputLabel;

  //Uncropped # of samples/lines
  int N = 1024;

  // Get the FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsImageLabel(0));
  try {
    fitsLabel.addGroup(importFits.extraFitsLabel(0));
  }
  catch (IException &e) {
    QString msg = "Input file [" + fitsFileName +
                  "] does not appear to be a Hayabusa2/ONC label file.";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  QString instid;
  QString missid;
  QString naifid;
  QString formatType;

  try {
    instid = fitsLabel.findGroup("FitsLabels").findKeyword("INSTRUME")[0];
    missid = fitsLabel.findGroup("FitsLabels").findKeyword ("SPCECRFT")[0];
  }
  catch (IException &e) {
    QString msg = "Unable to read instrument ID, [INSTRUME], or spacecraft ID, [SPCECRFT], "
                  "from input file [" + fitsFileName + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }
  try {
    naifid = fitsLabel.findGroup("FitsLabels").findKeyword("NAIFID")[0];
  }
  catch(IException &e) {
    updatedKeywords=false;
  }

  try {
    formatType = fitsLabel.findGroup("FitsLabels").findKeyword("EXTNAME")[0];
  }
  catch(IException &e) {
    QString msg = "Unable to read EXTNAME from input file ["
        +fitsFileName + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }

  try {
    naifid = fitsLabel.findGroup("FitsLabels").findKeyword("NAIFID")[0];
  }
  catch(IException &e) {
    updatedKeywords=false;
  }

  if (formatType.contains("2a") || formatType.contains("2b"))
    distortionCorrection = false;

  missid = missid.simplified().trimmed();
  if (QString::compare(missid, "HAYABUSA-2", Qt::CaseInsensitive) != 0) {
    QString msg = "Input file [" + fitsFileName +
                  "] does not appear to be a Hayabusa2 label file.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  instid = instid.simplified().trimmed();
  if (QString::compare(instid, "Optical Navigation Camera", Qt::CaseInsensitive) != 0) {
    QString msg = "Input file [" + fitsFileName +
                  "] does not appear to be a Hayabusa2/ONC label file.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  // Translate the Instrument group

  FileName transFile(transDir + "hyb2oncInstrument.trn");

  if (updatedKeywords) {
    transFile = transDir+"hyb2oncInstrumentUpdated.trn";
  }
  PvlToPvlTranslationManager instrumentXlater (fitsLabel, transFile.expanded());
  instrumentXlater.Auto(outputLabel);


  //  Update target if user specifies it
  PvlGroup &instGrp = outputLabel.findGroup("Instrument",Pvl::Traverse);

  //Check for cropped image
  int ss = instGrp["SelectedImageAreaX1"];
  int sl = instGrp["SelectedImageAreaY1"];
  int es = instGrp["SelectedImageAreaX2"];
  int el = instGrp["SelectedImageAreaY2"];

  if (ss > 1 || sl >1 || es < N || el < N) {

    AlphaCube aCube(N, N, outputCube->sampleCount(), outputCube->lineCount(),
                    ss-0.5, sl - 0.5, es + 0.5, el + 0.5);
    aCube.UpdateGroup(*outputCube);

    instGrp["SelectedImageAreaX1"] = "1";
    instGrp["SelectedImageAreaY1"] = "1";
    instGrp["SelectedImageAreaX2"] = QString("%1").arg(N);
    instGrp["SelectedImageAreaY2"] = QString("%1").arg(N);


  }

  QString labelTarget;
  instGrp.addKeyword(PvlKeyword("DistortionCorrection"));
  try {
    labelTarget = fitsLabel.findGroup("FitsLabels").findKeyword("TARGET")[0];
  }
  catch(IException &e) {
    QString msg = "Unable to read TARGET from input file ["
        +fitsFileName + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }
  if (labelTarget=="SKY") {
    instGrp["TargetName"] = "RYUGU";
  }

  if (distortionCorrection) {
    instGrp["DistortionCorrection"] = "yes";
  }
  else {
     instGrp["DistortionCorrection"] = "no";
  }


  //If the user wants to specify a different target, overwrite this value
  if (target !="") {
    instGrp["TargetName"] = target;
  }
  instGrp["ExposureDuration"].setUnits("seconds");
  outputCube->putGroup(instGrp);

  // Translate the BandBin group

  transFile = transDir + "hyb2oncBandBin.trn";
  if (updatedKeywords) {
    transFile = transDir + "hyb2oncBandBinUpdated.trn";
  }


  PvlToPvlTranslationManager bandBinXlater (fitsLabel, transFile.expanded());
  bandBinXlater.Auto(outputLabel);
  PvlGroup &bandGrp = outputLabel.findGroup("BandBin",Pvl::Traverse);
  if (bandGrp.hasKeyword("Width")) { // if width exists, then so must center
    bandGrp["Width"].setUnits("nanometers");
    bandGrp["Center"].setUnits("nanometers");
  }
  outputCube->putGroup(outputLabel.findGroup("BandBin",Pvl::Traverse));

  // Translate the Archive group

  transFile = transDir + "hyb2oncArchive.trn";
  if (updatedKeywords) {
    transFile = transDir + "hyb2oncArchiveUpdated.trn";
  }

  PvlToPvlTranslationManager archiveXlater (fitsLabel, transFile.expanded());
  archiveXlater.Auto(outputLabel);
  PvlGroup &archGrp = outputLabel.findGroup("Archive", Pvl::Traverse);
  QString source = archGrp.findKeyword("SourceProductId")[0];
  archGrp["SourceProductId"].setValue(FileName(source).baseName());

  //  Create YearDoy keyword in Archive group
  iTime stime(outputLabel.findGroup("Instrument", Pvl::Traverse)["StartTime"][0]);
  PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
  archGrp.addKeyword(yeardoy);
  outputCube->putGroup(archGrp);


  // Create a Kernels group
  if (updatedKeywords) {
    transFile = transDir + "hyb2oncKernelsUpdated.trn";
  }
  else {
    transFile = transDir + "hyb2oncKernels.trn";
  }

  PvlToPvlTranslationManager kernelsXlater(fitsLabel, transFile.expanded());
  kernelsXlater.Auto(outputLabel);
  outputCube->putGroup(outputLabel.findGroup("Kernels", Pvl::Traverse));

  // Now write the FITS augmented label as the original label
  // Save the input FITS label in the Cube original labels
  OriginalLabel originalLabel(fitsLabel);
  outputCube->write(originalLabel);

  // Convert the image data
  importFits.Progress()->SetText("Importing Hayabusa2 image");

  importFits.StartProcess();
  importFits.Finalize();


  return outputLabel;


  }

}




