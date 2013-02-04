#include "Isis.h"

#include <fstream>
#include <string>

#include "Angle.h"
#include "Cube.h"
#include "Camera.h"
#include "CameraFocalPlaneMap.h"
#include "Distance.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "FileName.h"
#include "Histogram.h"
#include "LineManager.h"
#include "Longitude.h"
#include "OriginalLabel.h"
#include "ProcessExportPds.h"
#include "Pvl.h"
#include "PvlFormat.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "PvlTranslationManager.h"
#include "Table.h"
#include "UserInterface.h"

using namespace Isis;
using namespace std;

pair<double, double> inputRange(Cube *inputCube);
void updatePdsLabelTimeParametersGroup(Pvl &pdsLabel);
void updatePdsLabelImageObject(PvlObject *isisCubeLab, Pvl &pdsLabel);
void updatePdsLabelRootObject(PvlObject *isisCubeLab, Pvl &pdsLabel,
                              UserInterface &ui, Camera *cam);
void IsisMain() {
  // Get user interface and create a ProcessExportPds object
  UserInterface &ui = Application::GetUserInterface();  
  ProcessExportPds p;
  Cube *inputCube = p.SetInputCube("FROM");
  PvlObject *isisCubeLab = inputCube->getLabel();

  // Error check to make sure this is a valid cube for this program
  QString origInstrument = isisCubeLab->FindObject("IsisCube")
                           .FindGroup("OriginalInstrument")["InstrumentId"][0];
  if (origInstrument != "HIRISE") {
    QString msg = "Input cube must from a HiRISE image. The original "
                  "InstrumentId = [" + origInstrument 
                  + "] is unsupported by hideal2pds.";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  QString instrumentId = isisCubeLab->FindObject("IsisCube")
                                    .FindGroup("Instrument")["InstrumentId"][0];
  if (instrumentId != "IdealCamera") {
    QString msg = "Input cube must be IdealCamera. InstrumentId = [" 
                  + instrumentId + "] is unsupported by hideal2pds.";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  QString target = isisCubeLab->FindObject("IsisCube")
                              .FindGroup("Instrument")["TargetName"][0];
  if (target.toUpper() != "MARS") {
    QString msg = "Input cube must from a HiRise image. The target = [" 
                  + target + "] is unsupported by hideal2pds.";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  // use histogram to calculate min/max for input range
  pair<double, double> inRange;
  inRange = inputRange(inputCube);
  p.SetInputRange(inRange.first, inRange.second);
  // output bit type will be 16bit unsigned word
  p.SetOutputType(Isis::UnsignedWord); 
  p.SetOutputNull(Isis::NULLU2);         
  p.SetOutputLrs(Isis::LOW_REPR_SATU2);  
  p.SetOutputLis(Isis::LOW_INSTR_SATU2); 
  p.SetOutputHrs(Isis::HIGH_REPR_SATU2); 
  p.SetOutputHis(Isis::HIGH_INSTR_SATU2);
  p.SetOutputRange(VALID_MINU2, VALID_MAXU2); 
  // output byte order will be MSB
  p.SetOutputEndian(Isis::Msb);
  p.SetFormat(ProcessExport::BSQ);
  // multiple table files should be Fixed according to PDS documentation
  p.SetExportType(ProcessExportPds::Fixed);
  p.SetPdsResolution(ProcessExportPds::Meter);

  // output PDS file with detached labels and tables for this application
  FileName outPdsFile(ui.GetFileName("TO", "img"));
  QString pdsLabelFile = outPdsFile.path() + "/" + outPdsFile.baseName() + ".lbl";
  p.SetDetached(pdsLabelFile);
  // create generic pds label - this will be finalized with proper line/byte counts later
  Pvl &pdsLabel = p.StandardPdsLabel(ProcessExportPds::Image);

  QString isisLabelFile = ui.GetFileName("FROM");

  // Translate the keywords from the input cube label that go in the PDS label
  PvlTranslationManager cubeLab(*(inputCube->getLabel()), 
                                "$mro/translations/hiriseIdealPdsExportCubeLabel.trn");
  cubeLab.Auto(pdsLabel);

  // get original label information
  OriginalLabel origBlob;
  inputCube->read(origBlob);
  Pvl origLabel;
  PvlObject origLabelObj = origBlob.ReturnLabels();
  origLabelObj.SetName("OriginalLabelObject");
  origLabel.AddObject(origLabelObj);
  PvlTranslationManager orig(origLabel, "$mro/translations/hirisePdsRdrOriginalLabel.trn");
  orig.Auto(pdsLabel);

  updatePdsLabelTimeParametersGroup(pdsLabel);
  updatePdsLabelImageObject(isisCubeLab, pdsLabel);
  Camera *cam = inputCube->getCamera();
  updatePdsLabelRootObject(isisCubeLab, pdsLabel, ui, cam);

  // Export each of the spice tables and update table keywords in PDS file
  // 
  // *** NOTE ***
  //    This could change the start byte/line values for the tables that have
  //    already been set in the labels by the ExportTable call. This is not
  //    a problem since our tables are detached.  However, it could be a
  //    problem if we decide to allow attached PDS products in the future.
  QString pdsTableFile = "";
  pdsTableFile = outPdsFile.baseName() + "_INSTRUMENT_POINTING_TABLE.dat"; 
  Table instRotationTable = cam->instrumentRotation()->Cache("InstrumentPointing");
  p.ExportTable(instRotationTable, pdsTableFile);
  PvlObject isisTableLab = instRotationTable.Label();
  PvlObject &instPtTabLab = pdsLabel.FindObject("INSTRUMENT_POINTING_TABLE");
  PvlKeyword tableKeyword = isisTableLab.FindKeyword("TimeDependentFrames");
  tableKeyword.SetName("TIME_DEPENDENT_FRAMES");
  instPtTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("ConstantFrames");
  tableKeyword.SetName("CONSTANT_FRAMES");
  instPtTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("ConstantRotation");
  tableKeyword.SetName("CONSTANT_ROTATION");
  instPtTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("CkTableStartTime");
  tableKeyword.SetName("CK_TABLE_START_TIME");
  instPtTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("CkTableEndTime");
  tableKeyword.SetName("CK_TABLE_END_TIME");
  instPtTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("CkTableOriginalSize");
  tableKeyword.SetName("CK_TABLE_ORIGINAL_SIZE");
  instPtTabLab += tableKeyword;

  pdsTableFile = outPdsFile.baseName() + "_INSTRUMENT_POSITION_TABLE.dat"; 
  Table instPositionTable = cam->instrumentPosition()->Cache("InstrumentPosition");
  p.ExportTable(instPositionTable, pdsTableFile); 
  isisTableLab = instPositionTable.Label();
  PvlObject &instPosTabLab = pdsLabel.FindObject("INSTRUMENT_POSITION_TABLE");
  tableKeyword = isisTableLab.FindKeyword("CacheType");
  tableKeyword.SetName("CACHE_TYPE");
  instPosTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("SpkTableStartTime");
  tableKeyword.SetName("SPK_TABLE_START_TIME");
  instPosTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("SpkTableEndTime");
  tableKeyword.SetName("SPK_TABLE_END_TIME");
  instPosTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("SpkTableOriginalSize");
  tableKeyword.SetName("SPK_TABLE_ORIGINAL_SIZE");
  instPosTabLab += tableKeyword;

  pdsTableFile = outPdsFile.baseName() + "_BODY_ROTATION_TABLE.dat"; 
  Table bodyRotationTable = cam->bodyRotation()->Cache("BodyRotation");
  p.ExportTable(bodyRotationTable, pdsTableFile); 
  isisTableLab = bodyRotationTable.Label();
  PvlObject &bodyRotTabLab = pdsLabel.FindObject("BODY_ROTATION_TABLE");
  tableKeyword = isisTableLab.FindKeyword("TimeDependentFrames");
  tableKeyword.SetName("TIME_DEPENDENT_FRAMES");
  bodyRotTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("CkTableStartTime");
  tableKeyword.SetName("CK_TABLE_START_TIME");
  bodyRotTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("CkTableEndTime");
  tableKeyword.SetName("CK_TABLE_END_TIME");
  bodyRotTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("CkTableOriginalSize");
  tableKeyword.SetName("CK_TABLE_ORIGINAL_SIZE");
  bodyRotTabLab += tableKeyword;
  if (isisTableLab.HasKeyword("SolarLongitude")) {
    tableKeyword = isisTableLab.FindKeyword("SolarLongitude");
    tableKeyword.SetName("SOLAR_LONGITUDE");
  }
  else {
    tableKeyword = PvlKeyword("SOLAR_LONGITUDE", 
                              toString(cam->solarLongitude().force360Domain()
                                   .positiveEast(Angle::Degrees)), 
                              "DEGREES");
  }
  bodyRotTabLab += tableKeyword;

  pdsTableFile = outPdsFile.baseName() + "_SUN_POSITION_TABLE.dat"; 
  Table sunPositionTable  = cam->sunPosition()->Cache("SunPosition");
  p.ExportTable(sunPositionTable, pdsTableFile); 
  isisTableLab = sunPositionTable.Label();
  PvlObject &sunPosTabLab = pdsLabel.FindObject("SUN_POSITION_TABLE");
  tableKeyword = isisTableLab.FindKeyword("CacheType");
  tableKeyword.SetName("CACHE_TYPE");
  sunPosTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("SpkTableStartTime");
  tableKeyword.SetName("SPK_TABLE_START_TIME");
  sunPosTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("SpkTableEndTime");
  tableKeyword.SetName("SPK_TABLE_END_TIME");
  sunPosTabLab += tableKeyword;
  tableKeyword = isisTableLab.FindKeyword("SpkTableOriginalSize");
  tableKeyword.SetName("SPK_TABLE_ORIGINAL_SIZE");
  sunPosTabLab += tableKeyword;

  // Read in the proper keyword types (Real, Enum, String, Integer, etc) for 
  // each PvlKeyword so that the PDS labels have proper format
  PvlFormat *formatter = pdsLabel.GetFormat();
  formatter->Add("$mro/templates/labels/hiriseIdealPds.typ");

  // Format ordering of keywords/objects/groups/comments in the PDS labels
  pdsLabel.SetFormatTemplate("$mro/templates/labels/hiriseIdealPds.pft");

  // image line/byte offsets are calculated and values are updated in the labels
  // now that all translations/additions/modifications to the labels have been
  // completed
  p.OutputDetachedLabel();
  QString outFileName(outPdsFile.expanded());
  ofstream outputStream(outFileName.toAscii().data());
  p.StartProcess(outputStream);
  p.EndProcess();
  outputStream.close();
}

/**
 * This method uses a Histogram object to find the minimum and maximum DN 
 * values of the input cube.  These values are used by the Process object 
 * to set the input range. 
 *  
 * @param inputCube Pointer to the inputCube 
 *  
 * @return A pair whose first value is the minimum DN of the input cube and
 *         second value is the maximum DN of the input cube.
 */
pair<double, double> inputRange(Cube *inputCube) {
  Process histProcess;
  int band = 1;
  Histogram hist(*inputCube, band, histProcess.Progress());

  // Loop and accumulate histogram
  histProcess.Progress()->SetText("Gathering Histogram to Find Input Range");
  histProcess.Progress()->SetMaximumSteps(inputCube->getLineCount());
  histProcess.Progress()->CheckStatus();
  LineManager line(*inputCube);
  for(int i = 1; i <= inputCube->getLineCount(); i++) {
    line.SetLine(i, band);
    inputCube->read(line);
    hist.AddData(line.DoubleBuffer(), line.size());
    histProcess.Progress()->CheckStatus();
  }

  // get the requested cumulative percentages
  pair<double, double> inRange(hist.Minimum(), hist.Maximum());
  histProcess.EndProcess();
  return inRange;
}

/**
 * This method updates the values of the keywords in the IMAGE object of 
 * the pds label file. 
 *  
 * The DESCRIPTION keyword is added. 
 *  
 * If the input cube has an AlphaCube group that indicates a crop has 
 * been performed, SOURCE_LINE_SAMPLES, SOURCE_LINES, FIRST_LINE_SAMPLE, 
 * and FIRST_LINE keywords are added. 
 *  
 * The values for CENTER_FILTER_WAVELENGTH and BAND_WIDTH are updated. 
 *  
 * @param inputCubeLab PvlObject pointer to the input cube labels 
 * @param pdsLabel Pvl of the output PDS labels 
 */
void updatePdsLabelImageObject(PvlObject *isisCubeLab, Pvl &pdsLabel) {
  // Add the image description to the IMAGE object in the label of the PDS product
  PvlObject &image = pdsLabel.FindObject("IMAGE");
  image += PvlKeyword("DESCRIPTION", 
                      "HiRISE mosaicked product, not map projected");

  // Add AlphaCube values to the IMAGE object
  // if AlphaCube doesn't exist in the Isis cube, add default values
  double sourceSamples = double(image["LINE_SAMPLES"]);
  double sourceLines = double(image["LINES"]);
  double firstSample = 0.5;
  double firstLine = 0.5;
  if (isisCubeLab->FindObject("IsisCube").HasGroup("AlphaCube")) {
    PvlGroup alphaCubeGroup = isisCubeLab->FindObject("IsisCube")
                                         .FindGroup("AlphaCube");

    int alphaSamples = int(alphaCubeGroup["AlphaSamples"]);
    int alphaLines = int(alphaCubeGroup["AlphaLines"]);

    double alphaStartingSample = double(alphaCubeGroup["AlphaStartingSample"]);
    double alphaEndingSample = double(alphaCubeGroup["AlphaEndingSample"]);

    double alphaStartingLine = double(alphaCubeGroup["AlphaStartingLine"]);
    double alphaEndingLine = double(alphaCubeGroup["AlphaEndingLine"]);

    double betaSamples = double(alphaCubeGroup["BetaSamples"]);
    double betaLines = double(alphaCubeGroup["BetaLines"]);

    if ( betaSamples != double(image["LINE_SAMPLES"])
         || betaLines != double(image["LINES"])
         || (double) alphaSamples < betaSamples
         || (double) alphaLines < betaLines
         || betaSamples != (alphaEndingSample - alphaStartingSample)
         || betaLines != (alphaEndingLine - alphaStartingLine) ) {
      // The given input cube is not supported for export to PDS. The AlphaCube
      // group values indicate that this cube has been not only cropped, but
      // also scaled (reduced or enlarged)
      QString msg = "The AlphaCube group values of the input Isis cube indicate "
                    "that this cube has been scaled. Unable to export scaled "
                    "cubes to PDS using hideal2pds.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    sourceSamples = alphaSamples;
    sourceLines = alphaLines;
    firstSample = alphaStartingSample;
    firstLine = alphaStartingLine;
  }
  image += PvlKeyword("SOURCE_LINE_SAMPLES", toString(sourceSamples));
  image += PvlKeyword("SOURCE_LINES", toString(sourceLines));
  image += PvlKeyword("FIRST_LINE_SAMPLE", toString(firstSample));
  image += PvlKeyword("FIRST_LINE", toString(firstLine));


  // Add center wavelength and bandwidth with correct units to the IMAGE object
  PvlKeyword &oldCenter = image["CENTER_FILTER_WAVELENGTH"];
  PvlKeyword newCenter("CENTER_FILTER_WAVELENGTH");
  for(int val = 0; val < oldCenter.Size(); ++val) {
    if(((QString)(oldCenter.Unit(val))).toUpper() == "NANOMETERS") {
      newCenter.AddValue(oldCenter[val], "NM");
    }
    else {
      newCenter.AddValue(oldCenter[val], oldCenter.Unit(val));
    }
  }
  image.AddKeyword(newCenter, Pvl::Replace);

  PvlKeyword &oldBandWidth = image["BAND_WIDTH"];
  PvlKeyword newBandWidth("BAND_WIDTH");
  for(int val = 0; val < oldBandWidth.Size(); ++val) {
    if(((QString)(oldBandWidth.Unit(val))).toUpper() == "NANOMETERS") {
      newBandWidth.AddValue(oldBandWidth[val], "NM");
    }
    else {
      newBandWidth.AddValue(oldBandWidth[val], oldBandWidth.Unit(val));
    }
  }
  image.AddKeyword(newBandWidth, Pvl::Replace);
}


/**
 * This method updates the values of the keywords in the ROOT object of 
 * the pds label file. 
 *  
 * The RATIONALE_DESC keyword is updated if the user entered this 
 * parameter. 
 *  
 * The PRODUCT_VERSION_ID is added based on the user entered parameter. 
 *  
 * The NOT_APPLICABLE_CONSTANT keyword is added. 
 *  
 * The SOFTWARE_NAME keyword is determined and added. 
 *  
 * The SHAPE_MODEL keyword from the Kernels group of the input cube is 
 * added with the path removed. 
 *  
 * The NaifKeywords values are added if the Object exists in the input 
 * cube.  Otherwise, the corresponding values are calculated and added 
 * to the pds labels. These values are added: BODY_FRAME_CODE, 
 * IDEAL_FOCAL_LENGTH, IDEAL_PIXEL_PITCH, IDEAL_TRANSX, IDEAL_TRANSY, 
 * IDEAL_TRANSS, and IDEAL_TRANSL. The BODY_RADII keyword is split into
 * A_AXIS_RADIUS, B_AXIS_RADIUS, and C_AXIS_RADIUS 
 *  
 * @param inputCubeLab PvlObject pointer to the input cube labels 
 * @param pdsLabel Pvl of the output PDS labels 
 * @param ui UserInterface reference, so that the user entered 
 *           RATIONALE_DESC and PRODUCT_VERSION_ID parameters can be
 *           read in.
 * @param cam Pointer to the Camera object created from the input cube.
 */
void updatePdsLabelRootObject(PvlObject *isisCubeLab, Pvl &pdsLabel,
                              UserInterface &ui, Camera *cam) {
  // Replace INSTRUMENT_ID value in the output labels
  PvlKeyword instId("INSTRUMENT_ID", "HIRISE_IDEAL_CAMERA");
  pdsLabel.AddKeyword(instId, PvlContainer::Replace);
  
  // Add user-entered keywords to ROOT object in the label of the PDS product
  if(ui.WasEntered("RATIONALE_DESC")) {
    PvlKeyword rationale("RATIONALE_DESC", ui.GetAsString("RATIONALE_DESC"));
    pdsLabel.AddKeyword(rationale, PvlContainer::Replace);
  }
  else if ( !pdsLabel.HasKeyword("RATIONALE_DESC") 
            || QString(pdsLabel["RATIONALE_DESC"]) == "NULL" ){
    QString msg = "Unable to export HiRise product to PDS without "
                  "RationaleDescription value. The input cube value for this "
                  "keyword is Null, the user is required to enter a value.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  pdsLabel += PvlKeyword("PRODUCT_VERSION_ID", ui.GetString("VERSION"));

  // Add the N/A constant keyword to the ROOT object
  pdsLabel += PvlKeyword("NOT_APPLICABLE_CONSTANT", toString(-9998));

  // Compute and add SOFTWARE_NAME to the ROOT object
  QString sfname;
  sfname.clear();
  sfname += "Isis " + Application::Version() + " " +
            Application::GetUserInterface().ProgramName();
  pdsLabel += PvlKeyword("SOFTWARE_NAME", sfname);
  QString matchedCube = isisCubeLab->FindObject("IsisCube").FindGroup("Instrument")
                                  .FindKeyword("MatchedCube")[0];
  FileName matchedCubeFileNoPath(matchedCube);
  pdsLabel += PvlKeyword("MATCHED_CUBE", matchedCubeFileNoPath.name());

  // Add jitter correction flag value to the ROOT object
  bool jitter = false;
  if (isisCubeLab->FindObject("IsisCube").FindGroup("Instrument")
                 .HasKeyword("ImageJitterCorrected")) {
    jitter = toInt(isisCubeLab->FindObject("IsisCube")
                            .FindGroup("Instrument")["ImageJitterCorrected"][0]);
    pdsLabel += PvlKeyword("IMAGE_JITTER_CORRECTED", toString((int)jitter));          
  }
  else {
    pdsLabel += PvlKeyword("IMAGE_JITTER_CORRECTED", "UNK");
  }                                                                  

  // Add Isis Kernels group keywords to the ROOT object
  QString shapeModel = isisCubeLab->FindObject("IsisCube").FindGroup("Kernels")
                                  .FindKeyword("ShapeModel")[0];
  FileName shapeModelFileNoPath(shapeModel);
  pdsLabel += PvlKeyword("SHAPE_MODEL", shapeModelFileNoPath.name());

  // PRODUCT_ID and SOURCE_PRODUCT_ID should be keywords added when creating the 
  // mosaic input cube.
  
  // Add NaifKeywords Object values to the ROOT object
  QString radiiName = "BODY" + QString(cam->naifBodyCode()) + "_RADII";
  PvlObject naifKeywordGroup = cam->getStoredNaifKeywords();

  if (naifKeywordGroup.HasKeyword(radiiName)) {
    PvlKeyword naifBodyRadii = naifKeywordGroup.FindKeyword(radiiName);
    pdsLabel += PvlKeyword("A_AXIS_RADIUS", naifBodyRadii[0], "KILOMETERS");
    pdsLabel += PvlKeyword("B_AXIS_RADIUS", naifBodyRadii[1], "KILOMETERS");
    pdsLabel += PvlKeyword("C_AXIS_RADIUS", naifBodyRadii[2], "KILOMETERS");
  }
  else {
    Distance naifBodyRadii[3];
    cam->radii(naifBodyRadii);
    pdsLabel += PvlKeyword("A_AXIS_RADIUS", toString(naifBodyRadii[0].kilometers()), "KILOMETERS");
    pdsLabel += PvlKeyword("B_AXIS_RADIUS", toString(naifBodyRadii[1].kilometers()), "KILOMETERS");
    pdsLabel += PvlKeyword("C_AXIS_RADIUS", toString(naifBodyRadii[2].kilometers()), "KILOMETERS");
  }

  if (naifKeywordGroup.HasKeyword("BODY_FRAME_CODE")) {
    pdsLabel += naifKeywordGroup.FindKeyword("BODY_FRAME_CODE");
  }
  else {
    pdsLabel += PvlKeyword("BODY_FRAME_CODE", toString(cam->naifBodyFrameCode()));
  }

  if (naifKeywordGroup.HasKeyword("IDEAL_FOCAL_LENGTH")) {
    pdsLabel += naifKeywordGroup.FindKeyword("IDEAL_FOCAL_LENGTH");
  }
  else {
    pdsLabel += PvlKeyword("IDEAL_FOCAL_LENGTH", toString(cam->FocalLength()));
  }

  if (naifKeywordGroup.HasKeyword("IDEAL_PIXEL_PITCH")) {
    pdsLabel += naifKeywordGroup.FindKeyword("IDEAL_PIXEL_PITCH");
  }
  else {
    pdsLabel += PvlKeyword("IDEAL_PIXEL_PITCH", toString(cam->PixelPitch()));
  }

  if (naifKeywordGroup.HasKeyword("IDEAL_TRANSX")) {
    pdsLabel += naifKeywordGroup.FindKeyword("IDEAL_TRANSX");
  }
  else {
    const double *transXValues = cam->FocalPlaneMap()->TransX();
    PvlKeyword transX("IDEAL_TRANSX");
    for (int i = 0; i < 3; i++) {
      transX += toString(transXValues[i]);
    }
    pdsLabel += transX;
  }

  if (naifKeywordGroup.HasKeyword("IDEAL_TRANSY")) {
    pdsLabel += naifKeywordGroup.FindKeyword("IDEAL_TRANSY");
  }
  else {
    const double *transYValues = cam->FocalPlaneMap()->TransY();
    PvlKeyword transY("IDEAL_TRANSY");
    for (int i = 0; i < 3; i++) {
      transY += toString(transYValues[i]);
    }
    pdsLabel += transY;
  }

  if (naifKeywordGroup.HasKeyword("IDEAL_TRANSS")) {
    pdsLabel += naifKeywordGroup.FindKeyword("IDEAL_TRANSS");
  }
  else {
    const double *transSValues = cam->FocalPlaneMap()->TransS();
    PvlKeyword transS("IDEAL_TRANSS");
    for (int i = 0; i < 3; i++) {
      transS += toString(transSValues[i]);
    }
    pdsLabel += transS;
  }

  if (naifKeywordGroup.HasKeyword("IDEAL_TRANSL")) {
    pdsLabel += naifKeywordGroup.FindKeyword("IDEAL_TRANSL");
  }
  else {
    const double *transLValues = cam->FocalPlaneMap()->TransL();
    PvlKeyword transL("IDEAL_TRANSL");
    for (int i = 0; i < 3; i++) {
      transL += toString(transLValues[i]);
    }
    pdsLabel += transL;
  }
}


/**
 * This method updates the values of the keywords in the Time Parameters
 * Group of the pds label file. 
 *  
 * The PRODUCT_CREATION_TIME keyword is determined and added to the PDS 
 * labels. 
 *  
 * @param pdsLabel Pvl of the output PDS labels 
 */
void updatePdsLabelTimeParametersGroup(Pvl &pdsLabel) {
  // Calculate and add PRODUCT_CREATION_TIME to the TIME_PARAMETERS group
  time_t startTime = time(NULL);
  struct tm *tmbuf = gmtime(&startTime);
  char timestr[80];
  strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
  QString dateTime = (QString) timestr;
  iTime tmpDateTime(dateTime);
  PvlGroup &timeParam = pdsLabel.FindGroup("TIME_PARAMETERS");
  timeParam += PvlKeyword("PRODUCT_CREATION_TIME", tmpDateTime.UTC());
}


