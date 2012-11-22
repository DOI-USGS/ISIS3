#include "Isis.h"

#include <QString>

#include "Cube.h"
#include "IString.h"
#include "FileName.h"
#include "ProcessImportPds.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "UserInterface.h"

using namespace Isis;
using namespace std;

void addTableKeywords(Pvl *isisLabel, Pvl pdsLabelPvl);

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();
  // Copy the input image to specified output cube in BSQ format
  FileName from = ui.GetFileName("FROM");
  IString pdsLabelFile = from.expanded();
  Pvl pdsLabelPvl;
  ProcessImportPds p;
  p.SetPdsFile(pdsLabelFile, "", pdsLabelPvl);

  IString instId = pdsLabelPvl["INSTRUMENT_ID"][0];
  if(instId != "HIRISE_IDEAL_CAMERA") {
    IString msg = "Invalid PDS label [" + from.expanded() + "]. The PDS product"
                  " must be from an Ideal camera model derived from a HiRISE"
                  " image. The INSTRUMENT_ID = [" + instId + "] is unsupported"
                  " by pds2hideal.";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  Cube *outputCube = p.SetOutputCube("TO");
  Pvl otherGroups;
  // translate the band bin and archive groups to this pvl  
  p.TranslatePdsLabels(otherGroups);

  p.ImportTable("INSTRUMENT_POINTING_TABLE");
  p.ImportTable("INSTRUMENT_POSITION_TABLE");
  p.ImportTable("SUN_POSITION_TABLE");
  p.ImportTable("BODY_ROTATION_TABLE");
  p.StartProcess();

  // add translated values from band bin and archive groups to the output cube
  outputCube->putGroup(otherGroups.FindGroup("BandBin"));
  outputCube->putGroup(otherGroups.FindGroup("Archive"));

  PvlGroup kernelGroup("Kernels");
  kernelGroup += PvlKeyword("NaifIkCode", IString(-74699));
  kernelGroup += PvlKeyword("TargetPosition", "Table");
  kernelGroup += PvlKeyword("InstrumentPointing", "Table");
  kernelGroup += PvlKeyword("InstrumentPosition", "Table");
  QString shapeModelPath = IString(ui.GetString("SHAPEMODELPATH")).ToQt();
  if (!shapeModelPath.endsWith('/')) {
    shapeModelPath += "/";
  }
  QString shapeModelValue = shapeModelPath + QString::fromStdString(pdsLabelPvl["SHAPE_MODEL"]);
  kernelGroup += PvlKeyword("ShapeModel", shapeModelValue.toStdString());
  outputCube->putGroup(kernelGroup);


  Pvl *isisLabel = outputCube->getLabel();
  PvlTranslationManager labelXlater(pdsLabelPvl, 
                                    "$mro/translations/hiriseIdealPdsImportLabel.trn");
  labelXlater.Auto(*isisLabel);
  
  PvlObject &naifKeywords = isisLabel->FindObject("NaifKeywords");
  PvlKeyword bodyRadii("BODY499_RADII");
  bodyRadii.AddValue(double(pdsLabelPvl["A_AXIS_RADIUS"]));
  bodyRadii.AddValue(double(pdsLabelPvl["B_AXIS_RADIUS"]));
  bodyRadii.AddValue(double(pdsLabelPvl["C_AXIS_RADIUS"]));
  naifKeywords += bodyRadii;

  PvlObject &isisCubeObject = isisLabel->FindObject("IsisCube");
  // Compute and add SOFTWARE_NAME to the Archive Group
  IString sfname = "Isis " + Application::Version() + " " +
            Application::GetUserInterface().ProgramName();
  PvlGroup &archiveGroup = isisCubeObject.FindGroup("Archive");
  archiveGroup += PvlKeyword("SOFTWARE_NAME", sfname);
  
  PvlObject &pdsImageObj = pdsLabelPvl.FindObject("IMAGE");
  double samples = double(pdsImageObj["LINE_SAMPLES"]);
  double lines = double(pdsImageObj["LINES"]);
  double firstSamp = double(pdsImageObj["FIRST_LINE_SAMPLE"]);
  double firstLine = double(pdsImageObj["FIRST_LINE"]);
  double sourceLines = double(pdsImageObj["SOURCE_LINES"]);
  double sourceSamps = double(pdsImageObj["SOURCE_LINE_SAMPLES"]);
  if (sourceLines != lines) {
    // this image is cropped, create an AlphaCube group
    PvlGroup alphaCube("AlphaCube");
    alphaCube += PvlKeyword("AlphaSamples", sourceSamps);
    alphaCube += PvlKeyword("AlphaLines", sourceLines);
    alphaCube += PvlKeyword("AlphaStartingSample", firstSamp);
    alphaCube += PvlKeyword("AlphaEndingSample", (double) firstSamp + samples);
    alphaCube += PvlKeyword("AlphaStartingLine", firstLine);
    alphaCube += PvlKeyword("AlphaEndingLine", (double) firstLine + lines);
    alphaCube += PvlKeyword("BetaSamples", samples);
    alphaCube += PvlKeyword("BetaLines", lines);
    isisCubeObject += alphaCube;
  }

  addTableKeywords(isisLabel, pdsLabelPvl);
  p.EndProcess();
}

/**
 * This method will add the appropriate keywords from the TABLE objects of the
 * input labels to the Table objects in output Isis labels.
 *
 * @param isisLabel Pointer to the output file's label
 * @param pdsLabelPvl A Pvl containing the input pds file's label.
 *
 */
void addTableKeywords(Pvl *isisLabel, Pvl pdsLabelPvl) {
  // add keywords to appropriate tables
   for (int i = 0; i < isisLabel->Objects(); i++) {
     if (isisLabel->Object(i).Name() == "Table") {
       PvlKeyword keyword;
       if (QString(isisLabel->Object(i)["Name"]) == "InstrumentPointing") {
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POINTING_TABLE")["TIME_DEPENDENT_FRAMES"];
         keyword.SetName("TimeDependentFrames");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POINTING_TABLE")["CONSTANT_FRAMES"];
         keyword.SetName("ConstantFrames");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POINTING_TABLE")["CONSTANT_ROTATION"];
         keyword.SetName("ConstantRotation");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POINTING_TABLE")["CK_TABLE_START_TIME"];
         keyword.SetName("CkTableStartTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POINTING_TABLE")["CK_TABLE_END_TIME"];
         keyword.SetName("CkTableEndTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POINTING_TABLE")["CK_TABLE_ORIGINAL_SIZE"];
         keyword.SetName("CkTableOriginalSize");
         isisLabel->Object(i) += keyword;
       }
       if (QString(isisLabel->Object(i)["Name"]) == "InstrumentPosition") {
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POSITION_TABLE")["CACHE_TYPE"];
         keyword.SetName("CacheType");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POSITION_TABLE")["SPK_TABLE_START_TIME"];
         keyword.SetName("SpkTableStartTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POSITION_TABLE")["SPK_TABLE_END_TIME"];
         keyword.SetName("SpkTableEndTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("INSTRUMENT_POSITION_TABLE")["SPK_TABLE_ORIGINAL_SIZE"];
         keyword.SetName("SpkTableOriginalSize");
         isisLabel->Object(i) += keyword;
       }
       if (QString(isisLabel->Object(i)["Name"]) == "BodyRotation") {
         keyword = pdsLabelPvl.FindObject("BODY_ROTATION_TABLE")["TIME_DEPENDENT_FRAMES"];
         keyword.SetName("TimeDependentFrames");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("BODY_ROTATION_TABLE")["CK_TABLE_START_TIME"];
         keyword.SetName("CkTableStartTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("BODY_ROTATION_TABLE")["CK_TABLE_END_TIME"];
         keyword.SetName("CkTableEndTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("BODY_ROTATION_TABLE")["CK_TABLE_ORIGINAL_SIZE"];
         keyword.SetName("CkTableOriginalSize");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("BODY_ROTATION_TABLE")["SOLAR_LONGITUDE"];
         keyword.SetName("SolarLongitude");
         isisLabel->Object(i) += keyword;
       }
       if (QString(isisLabel->Object(i)["Name"]) == "SunPosition") {
         keyword = pdsLabelPvl.FindObject("SUN_POSITION_TABLE")["CACHE_TYPE"];
         keyword.SetName("CacheType");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("SUN_POSITION_TABLE")["SPK_TABLE_START_TIME"];
         keyword.SetName("SpkTableStartTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("SUN_POSITION_TABLE")["SPK_TABLE_END_TIME"];
         keyword.SetName("SpkTableEndTime");
         isisLabel->Object(i) += keyword;
         keyword = pdsLabelPvl.FindObject("SUN_POSITION_TABLE")["SPK_TABLE_ORIGINAL_SIZE"];
         keyword.SetName("SpkTableOriginalSize");
         isisLabel->Object(i) += keyword;
       }
     }
   }
}

