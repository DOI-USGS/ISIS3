#include "Isis.h"

#include <QDir>
#include <QString>

#include "Cube.h"
#include "FileName.h"
#include "IString.h"
#include "ProcessExportPds.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "PvlFormatPds.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "Statistics.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

enum Pixtype { NONE, NEG, BOTH };

void setRangeAndPixels(UserInterface &ui, ProcessExportPds &p,
                       double &min, double &max, Pixtype ptype);

// Finds the scale letter, documented below
char findScaleLetter(double scale);

// Main
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ProcessExportPds p;
  Cube *inCube = p.SetInputCube("DTM");

  // Verify HiRISE
  if (inCube->bandCount() > 1 ||
      inCube->label()->hasObject("Instrument")) {
    QString msg = "Input cube [" + ui.GetFileName("FROM") + "] does not appear"
                + "to be a DTM";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  double min = -DBL_MAX;
  double max = DBL_MAX;

  // Setup output type for DTM, default 32 bit
  if (ui.GetString("DTMBITTYPE") == "8BIT") {
    p.SetOutputType(Isis::UnsignedByte);
    min = 0.0;
    max = 255.0;
    setRangeAndPixels(ui, p, min, max, BOTH);
  }
  else if (ui.GetString("DTMBITTYPE") == "S16BIT") {
    p.SetOutputType(Isis::SignedWord);
    min = -32768.0;
    max = 32767.0;
    setRangeAndPixels(ui, p, min, max, NEG);
  }
  else if (ui.GetString("DTMBITTYPE") == "U16BIT") {
    p.SetOutputType(Isis::UnsignedWord);
    min = 0.0;
    max = 65535.0;
    setRangeAndPixels(ui, p, min, max, BOTH);
  }
  else {
    p.SetOutputType(Isis::Real);
    p.SetOutputNull(Isis::NULL4);
    p.SetOutputLrs(Isis::LOW_REPR_SAT4);
    p.SetOutputLis(Isis::LOW_INSTR_SAT4);
    p.SetOutputHrs(Isis::HIGH_REPR_SAT4);
    p.SetOutputHis(Isis::HIGH_INSTR_SAT4);
    setRangeAndPixels(ui, p, min, max, NONE);
  }

  // Set Endian for all output
  if (ui.GetString("ENDIAN") == "MSB") {
    p.SetOutputEndian(Isis::Msb);
  }
  else if (ui.GetString("ENDIAN") == "LSB") {
    p.SetOutputEndian(Isis::Lsb);
  }

  p.SetExportType(ProcessExportPds::Fixed);

  // These are our output labels, will be modifying heavily
  Pvl &pdsLabel = p.StandardPdsLabel(ProcessExportPds::Image);

  double northAzimuth = 0.0;
  double newRadius = 0.0;
  bool isEquirectangular = false;

  // Do not use this after calling AddObject on pdsLabel, there is a note
  // at that location.
  PvlObject &mapping = pdsLabel.findObject("IMAGE_MAP_PROJECTION");

  // Set data for VIEWING_PARAMETERS keyword
  // Also set radius values
  if (mapping["MAP_PROJECTION_TYPE"][0] == "EQUIRECTANGULAR") {
    mapping["MAP_PROJECTION_TYPE"].setValue("\"EQUIRECTANGULAR\"");
    northAzimuth = 270;
    isEquirectangular = true;

    // Get projection and use radius for new radius
    Projection *proj = ProjectionFactory::CreateFromCube(*inCube);
    newRadius = proj->LocalRadius((double)mapping["CENTER_LATITUDE"]);

    // Convert radius to KM
    newRadius /= 1000;
    mapping.findKeyword("A_AXIS_RADIUS").setValue(toString(newRadius));
    mapping.findKeyword("A_AXIS_RADIUS").setUnits("KM");
    mapping.findKeyword("B_AXIS_RADIUS").setValue(toString(newRadius));
    mapping.findKeyword("B_AXIS_RADIUS").setUnits("KM");
    mapping.findKeyword("C_AXIS_RADIUS").setValue(toString(newRadius));
    mapping.findKeyword("C_AXIS_RADIUS").setUnits("KM");
  }
  else if (mapping["MAP_PROJECTION_TYPE"][0] == "POLAR STEREOGRAPHIC") {
    double clat = ((toDouble(mapping["MAXIMUM_LATITUDE"][0]) -
                  toDouble(mapping["MINIMUM_LATITUDE"][0]) ) / 2) +
                  toDouble(mapping["MINIMUM_LATITUDE"][0]) ;
    double clon = ((toDouble(mapping["EASTERNMOST_LONGITUDE"][0])  -
                  toDouble(mapping["WESTERNMOST_LONGITUDE"][0]) ) / 2) +
                  toDouble(mapping["WESTERNMOST_LONGITUDE"][0]) ;
    isEquirectangular = false;

    // North polar case
    if (clat > 0.0 && clon < 270.0) {
      northAzimuth = 270.00 - clon;
    }
    else if (clat > 0.0 && clon >= 270.0) {
      northAzimuth = 360.00 + (270.00 - clon);
    }

    // South polar case
    if (clat < 0.0 && clon < 90.0) {
      northAzimuth = 270.00 + clon;
    }
    else if (clat < 0.0 && clon >= 90.0) {
      northAzimuth = -(360.00 - (270.00 + clon));
    }
  }
  // Unsupported projection
  else {
    QString msg = "The projection type [" +
        mapping["MAP_PROJECTION_TYPE"][0]
        + "] is not supported";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // A lot of label hacking goes on below, for the most part, it is in order,
  // to see the general organization of the labels look at the template.

  // Create and then add object, this will be used later in orthos as well.
  PvlObject viewingParameters("VIEWING_PARAMETERS");
  viewingParameters.addKeyword(PvlKeyword("NORTH_AZIMUTH", toString(northAzimuth), "DEG"));

  pdsLabel.addObject(viewingParameters);

  // This is our replacement for mapping
  PvlObject &projection = pdsLabel.findObject("IMAGE_MAP_PROJECTION");
  projection.addKeyword(
      PvlKeyword("^DATA_SET_MAP_PROJECTION", "DSMAP.CAT"));

  // Delete unneeded keywords in object
  projection.deleteKeyword("FIRST_STANDARD_PARALLEL");
  projection.deleteKeyword("SECOND_STANDARD_PARALLEL");

  // Set format and template, Template is the order of keywords, format is
  // well, format, like integer, real, (hex, 4)
  if (isEquirectangular) {
    pdsLabel.setFormatTemplate("$mro/templates/labels/hirisePdsDTMEqui.pft");
  }
  else {
    pdsLabel.setFormatTemplate("$mro/templates/labels/hirisePdsDTMPolar.pft");
  }

  Pvl format;

  // Have to do things this way to get an array of values
  PvlKeyword validMin("VALID_MINIMUM", "REAL");
  validMin += "2";
  format.addKeyword(validMin);
  PvlKeyword validMax("VALID_MAXIMUM", "REAL");
  validMax += "2";
  format.addKeyword(validMax);

  format.addKeyword(PvlKeyword("SAMPLE_BIT_MASK", "BINARY"));
  // Aiming for MISSING_CONSTANT = (HEX, 4); Two separate items.
  PvlKeyword mc("MISSING_CONSTANT", "HEX");
  mc += "4";
  format.addKeyword(mc);
  PvlFormat *form = pdsLabel.format();
  form->add(format);
  pdsLabel.setFormat(form);

  QString note = "Pixel values in this file represent elevations in meters "
      "above the martian equipotential surface (Mars 2000 Datum) defined by "
      "Smith, et al. (2001). Conversion from pixel units to geophysical "
      "units is given by the keyvalues for SCALING_FACTOR and OFFSET. This "
      "DTM was produced using ISIS and SOCET Set (copyright BAE Systems) "
      "software as described in Kirk et al. (2008).";
  pdsLabel.findObject("IMAGE").addKeyword(PvlKeyword("NOTE", note));

  // Four labels that are pretty set in stone
  pdsLabel.addKeyword(PvlKeyword("INSTRUMENT_HOST_NAME", "MARS RECONNAISSANCE ORBITER"));
  pdsLabel.addKeyword(PvlKeyword("INSTRUMENT_HOST_ID", "MRO"));
  pdsLabel.addKeyword(PvlKeyword("INSTRUMENT_NAME", "HIGH RESOLUTION IMAGING SCIENCE EXPERIMENT"));
  pdsLabel.addKeyword(PvlKeyword("INSTRUMENT_ID", "HIRISE"));

  Pvl inputParameters(ui.GetFileName("PARAMSPVL"));

  // These come from the user, mostly, and are in the order they appear in the output labels.
  pdsLabel.addKeyword(inputParameters["DATA_SET_ID"]);
  pdsLabel.addKeyword(inputParameters["DATA_SET_NAME"]);
  pdsLabel.addKeyword(inputParameters["PRODUCER_INSTITUTION_NAME"]);
  pdsLabel.addKeyword(inputParameters["PRODUCER_ID"]);

  if (ui.WasEntered("PRODUCER_FULL_NAME")) {
    pdsLabel.addKeyword(PvlKeyword("PRODUCER_FULL_NAME", ui.GetString("PRODUCER_FULL_NAME")));
  }
  else {
    pdsLabel.addKeyword(inputParameters["PRODUCER_FULL_NAME"]);
  }

  // Will we be using the default names?
  bool defaultNames = ui.GetBoolean("DEFAULTNAMES");
  FileName ortho1(ui.GetFileName("ORTHO1"));
  FileName ortho3(ui.GetFileName("ORTHO3"));

  QString vers; // Version string, will be used in orthos
  if (!defaultNames) {
    QString pId = ui.GetFileName("DTMTO");
    pId.left(pId.indexOf( QChar('.')) + 1 );
    pdsLabel.addKeyword(PvlKeyword("PRODUCT_ID", ui.GetString("DTM_PRODUCT_ID")));
    pdsLabel.addKeyword(PvlKeyword("SOURCE_PRODUCT_ID", ui.GetString("SOURCE_PRODUCT_ID")));
  }
  else {
    // Set projection letter
    QString pId = "DTE";
    pId += isEquirectangular ? "E" : "P";

    // Find scale letter
    pId += findScaleLetter(projection.findKeyword("MAP_SCALE"));

    // Product ids of orthos
    pId += ortho1.baseName().mid(3, 12);
    pId += ortho3.baseName().mid(3, 12);
    QString producing = inputParameters["PRODUCING_INSTITUTION"][0];
    if (producing.size() > 1) {
      QString msg = "PRODUCING_INSTITUTION value [" + producing + "] must be a single character";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    pId += "_" + producing;

    double version = 0.0;
    if (ui.WasEntered("PRODUCT_VERSION_ID")) {
      version = ui.GetDouble("PRODUCT_VERSION_ID");
    }
    else {
      version = toDouble(inputParameters["PRODUCT_VERSION_ID"]);
    }

    // Format the version for the output name:
    // Only important thing to note is that a #.0 number is converted to 0#
    // for the name, otherwise, it is predictable, always 2 greatest numbers:
    // ##.
    // >10, takes first two digits
    // The number found here is used in ortho images as well.
    if (version >= 10.0) {
      vers = toString(version).left(2);
    }
    else if (version >= 1.0) {
      vers = toString(version);
      bool wasInt = false;
      // Checking for integer values, if so, make #.0 into 0#
      // necessary because in DTMgen version 1.0 corresponded to a 01 in names.
      if (vers.size() == 3) {
        if (vers.at(2) == '0') {
          vers = "0" + vers.left(1);
          wasInt = true;
        }
      }
      // Wasn't int, make #.# into ##
      if (!wasInt) {
        vers = toString(version).remove(QChar('.'));
        if (vers.size() > 2) {
          vers = vers.left(2);
        }
      }
    }
    // 0 - <1, if 0.#, is 0#, is 0.#####, is first two ##
    else if (version >= 0.001) { // Any less and we get E... not dealing with that.
      vers = toString(version).remove(QChar('.'));
      int nonZero = vers.lastIndexOf('0');
      if (vers.size() > 2) vers = vers.mid(nonZero-1, 2);
    }
    // It was negative, or something else crazy?
    else {
      QString msg = "Version number [" + toString(version) + "] is invalid";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Finish off name string
    pId += vers;
    pdsLabel.addKeyword(PvlKeyword("PRODUCT_ID", pId));

    // Source product ids
    QString orthoS1 = ortho1.baseName().left(15);
    QString orthoS3 = ortho3.baseName().left(15);
    PvlKeyword sourceProductId("SOURCE_PRODUCT_ID");
    sourceProductId.addValue(orthoS1);
    sourceProductId.addValue(orthoS3);
    pdsLabel.addKeyword(sourceProductId);
  } // End scope of defaultNames true

  if (ui.WasEntered("PRODUCT_VERSION_ID")) {
    pdsLabel.addKeyword(PvlKeyword("PRODUCT_VERSION_ID", ui.GetAsString("PRODUCT_VERSION_ID")));
  }
  else {
    pdsLabel.addKeyword(inputParameters["PRODUCT_VERSION_ID"]);
  }

  pdsLabel.addKeyword(PvlKeyword("RATIONALE_DESC", ui.GetString("RATIONALE_DESC")));
  pdsLabel.addKeyword(inputParameters["SOFTWARE_NAME"]);

  // Label records should always be 1, my example didn't included it, so we won't.
  pdsLabel.deleteKeyword("LABEL_RECORDS");

  // Delete or change unneeded keywords in image group
  PvlObject &image = pdsLabel.findObject("IMAGE");
  image.findKeyword("CORE_NULL").setName("MISSING_CONSTANT");
  image.deleteKeyword("BAND_STORAGE_TYPE");
  image.deleteKeyword("CORE_LOW_REPR_SATURATION");
  image.deleteKeyword("CORE_LOW_INSTR_SATURATION");
  image.deleteKeyword("CORE_HIGH_REPR_SATURATION");
  image.deleteKeyword("CORE_HIGH_INSTR_SATURATION");

  // Create statistics and add to image group
  Statistics *stat = inCube->statistics();
  image.addKeyword(PvlKeyword("VALID_MINIMUM", toString(stat->Minimum())));
  image.addKeyword(PvlKeyword("VALID_MAXIMUM", toString(stat->Maximum())));

  // The output directory, will be used for ortho images as well
  FileName outDir = FileName(ui.GetString("OUTPUTDIR"));
  if (!outDir.fileExists()) {
    outDir.dir().mkpath(".");
  }
  QString outDirString = outDir.expanded();
  if (outDirString.mid(outDirString.size()-1, 1) != "/") {
    outDirString += "/";
    outDir = FileName(outDirString);
  }

  // Output the DTM, it should be ready
  FileName outFile;
  if (defaultNames) {
    QString dir = outDir.expanded();
    outFile = FileName(dir + pdsLabel.findKeyword("PRODUCT_ID")[0] + ".IMG");
  }
  else {
    outFile = FileName(ui.GetFileName("DTMTO"));
  }
  ofstream oCube(outFile.expanded().toAscii().data());

  p.OutputLabel(oCube);

  p.StartProcess(oCube);
  oCube.close();
  p.EndProcess();

  /*
   *
   * End of DTM work. Labels are complete, names complete, it's written out.
   *
   */

  p.ClearInputCubes();

  if (ui.GetString("ORTHOBITTYPE") == "8BIT") {
    p.SetOutputType(Isis::UnsignedByte);
    min = 0.0;
    max = 255.0;
    setRangeAndPixels(ui, p, min, max, BOTH);
  }
  else if (ui.GetString("ORTHOBITTYPE") == "S16BIT") {
    p.SetOutputType(Isis::SignedWord);
    min = -32768.0;
    max = 32767.0;
    setRangeAndPixels(ui, p, min, max, NEG);
  }
  else if (ui.GetString("ORTHOBITTYPE") == "U16BIT") {
    p.SetOutputType(Isis::UnsignedWord);
    min = 0.0;
    max = 65535.0;
    setRangeAndPixels(ui, p, min, max, BOTH);
  }
  else {
    p.SetOutputType(Isis::Real);
    p.SetOutputNull(Isis::NULL4);
    p.SetOutputLrs(Isis::LOW_REPR_SAT4);
    p.SetOutputLis(Isis::LOW_INSTR_SAT4);
    p.SetOutputHrs(Isis::HIGH_REPR_SAT4);
    p.SetOutputHis(Isis::HIGH_INSTR_SAT4);
    setRangeAndPixels(ui, p, min, max, NONE);
  }

  // Loop through 4 ortho images
  for (int i = 0; i < 4; /*4 = Number of ortho images*/ i++) {
    QString ortho("ORTHO" + toString(i + 1));
    Cube *orthoInCube = p.SetInputCube(ortho);

    Pvl &orthoLabel = p.StandardPdsLabel(ProcessExportPds::Image);
    PvlObject &orthoMap = orthoLabel.findObject("IMAGE_MAP_PROJECTION");

    QString orthoOut;
    if (!defaultNames) {
      orthoOut = ui.GetFileName(ortho);
    }
    else {
      // Will always be DT, orthos are 1 and 2, can only be Equirectangular
      // or Polar Stereographic, hence E or P
      // XML has detailed description of name format.
      orthoOut += "DT";
      orthoOut += (i < 2) ? "1" : "2";
      orthoOut += isEquirectangular ? "E" : "P";

      // Find scale letter
      orthoOut += findScaleLetter(orthoMap.findKeyword("MAP_SCALE"));

      // continue building name
      orthoOut += ortho1.baseName().mid(3, 12);
      orthoOut += ortho3.baseName().mid(3, 12);
      QString producing = inputParameters["PRODUCING_INSTITUTION"][0];
      if (producing.size() > 1) {
        QString msg = "PRODUCING_INSTITUTION value [" + producing + "] must be a single character";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      orthoOut += "_" + producing;

      // Found the version above
      orthoOut += vers;
      orthoLabel.addKeyword(PvlKeyword("PRODUCT_ID", orthoOut));
    }

    orthoMap.addKeyword(
        PvlKeyword("^DATA_SET_MAP_PROJECTION", "DSMAP.CAT"));

    if (isEquirectangular) {
      // Get projection and use radius for new radius
      Projection *proj = ProjectionFactory::CreateFromCube(*orthoInCube);
      newRadius = proj->LocalRadius((double)orthoMap["CENTER_LATITUDE"]);

      // Convert radius to KM
      newRadius /= 1000;
      orthoMap.findKeyword("A_AXIS_RADIUS").setValue(toString(newRadius));
      orthoMap.findKeyword("A_AXIS_RADIUS").setUnits("KM");
      orthoMap.findKeyword("B_AXIS_RADIUS").setValue(toString(newRadius));
      orthoMap.findKeyword("B_AXIS_RADIUS").setUnits("KM");
      orthoMap.findKeyword("C_AXIS_RADIUS").setValue(toString(newRadius));
      orthoMap.findKeyword("C_AXIS_RADIUS").setUnits("KM");
    }
    // Else case for Polar Stereographic is unnecessary as all it does is
    // calculate view parameters which we already have.


    // Set format and template
    if (isEquirectangular) {
      orthoLabel.setFormatTemplate("$mro/templates/labels/hirisePdsDTMEqui.pft");
    }
    else {
      orthoLabel.setFormatTemplate("$mro/templates/labels/hirisePdsDTMPolar.pft");
    }

    if (!defaultNames) {
      orthoLabel.addKeyword(PvlKeyword("PRODUCT_ID", ui.GetString(ortho+"_PRODUCT_ID")));
      orthoLabel.addKeyword(PvlKeyword("SOURCE_PRODUCT_ID", ui.GetString("SOURCE_PRODUCT_ID")));
    }
    else {
      QString orthoS1 = ortho1.baseName().left(15);
      QString orthoS3 = ortho3.baseName().left(15);
      PvlKeyword sourceProductId("SOURCE_PRODUCT_ID");
      sourceProductId.addValue(orthoS1);
      sourceProductId.addValue(orthoS3);
      orthoLabel.addKeyword(sourceProductId);
    }

    // Four labels that are pretty set in stone
    orthoLabel.addKeyword(PvlKeyword("INSTRUMENT_HOST_NAME", "MARS RECONNAISSANCE ORBITER"));
    orthoLabel.addKeyword(PvlKeyword("INSTRUMENT_HOST_ID", "MRO"));
    orthoLabel.addKeyword(PvlKeyword("INSTRUMENT_NAME", "HIGH RESOLUTION IMAGING SCIENCE EXPERIMENT"));
    orthoLabel.addKeyword(PvlKeyword("INSTRUMENT_ID", "HIRISE"));

    // These come from the user, mostly, and are in the order they appear in the output
    // labels.
    orthoLabel.addKeyword(inputParameters["DATA_SET_ID"]);
    orthoLabel.addKeyword(inputParameters["DATA_SET_NAME"]);
    orthoLabel.addKeyword(inputParameters["PRODUCER_INSTITUTION_NAME"]);
    orthoLabel.addKeyword(inputParameters["PRODUCER_ID"]);

    if (ui.WasEntered("PRODUCER_FULL_NAME")) {
      orthoLabel.addKeyword(PvlKeyword("PRODUCER_FULL_NAME", ui.GetString("PRODUCER_FULL_NAME")));
    }
    else {
      orthoLabel.addKeyword(inputParameters["PRODUCER_FULL_NAME"]);
    }

    if (ui.WasEntered("PRODUCT_VERSION_ID")) {
      orthoLabel.addKeyword(PvlKeyword("PRODUCT_VERSION_ID", ui.GetAsString("PRODUCT_VERSION_ID")));
    }
    else {
      orthoLabel.addKeyword(inputParameters["PRODUCT_VERSION_ID"]);
    }

    orthoLabel.addKeyword(PvlKeyword("RATIONALE_DESC", ui.GetString("RATIONALE_DESC")));
    orthoLabel.addKeyword(inputParameters["SOFTWARE_NAME"]);

    // Long ago we made a VIEWING_PARAMETERS object, it still is correct
    // Need to be careful to not use any object references from the Pvl
    // after this call, known problem with Pvl and vector reallocation.
    orthoLabel.addObject(viewingParameters);

    FileName orthoFile;
    if (defaultNames) {
      QString dir = outDir.expanded();
      orthoFile = FileName(dir + orthoLabel.findKeyword("PRODUCT_ID")[0] + ".IMG");
    }
    else {
      orthoFile = FileName(ui.GetFileName(ortho + "TO"));
    }

    ofstream orthoPdsOut(orthoFile.expanded().toAscii().data());
    p.OutputLabel(orthoPdsOut);
    p.StartProcess(orthoPdsOut);
    orthoPdsOut.close();
    p.EndProcess();

  } // End scope of for loop for 4 ortho images

}

/*
 * Scale letter of the image, A = 0.25, B = 0.5, C = 1.0, and so on.
 * We are using a 10% fudge range.
 */
char findScaleLetter(double scale) {
  int steps = 0;
  double matchNum = 0.25;
  double epsilon = matchNum * 0.1; // = 10% of matchNum

  bool bounded = false;
  while (!bounded) {
    if ((scale + epsilon) > matchNum && (scale - epsilon) < matchNum) {
      bounded = true;
    }
    else {
      steps++;
    }

    // Increase to next possible scale and increase epsilon
    matchNum *= 2;
    epsilon *= 2;

    if (matchNum > 129) { // Max allowed is J, 128, before skipping to Z
      bounded = true;
      steps = 25; // Get us to 'Z'
    }
  }
  char scaleLetter = 'A';
  // For however many steps we took, increase the letter.
  scaleLetter += steps;
  // 0.25 = A, 0.5 = B, 1.0 = C...
  return scaleLetter;
}

//Sets up special pixels and valid pixel ranges
void setRangeAndPixels(UserInterface &ui, ProcessExportPds &p, double &min, double &max, Pixtype ptype) {
  if (ptype == NEG) {
    if (ui.GetBoolean("NULL")) {
      p.SetOutputNull(min++);
    }
    if (ui.GetBoolean("LRS")) {
      p.SetOutputLrs(min++);
    }
    if (ui.GetBoolean("LIS")) {
      p.SetOutputLis(min++);
    }
    if (ui.GetBoolean("HIS")) {
      p.SetOutputHis(min++);
    }
    if (ui.GetBoolean("HRS")) {
      p.SetOutputHrs(min++);
    }
  }
  else if (ptype == BOTH) {
    if (ui.GetBoolean("NULL")) {
      p.SetOutputNull(min++);
    }
    if (ui.GetBoolean("LRS")) {
      p.SetOutputLrs(min++);
    }
    if (ui.GetBoolean("LIS")) {
      p.SetOutputLis(min++);
    }
    if (ui.GetBoolean("HRS")) {
      p.SetOutputHrs(max--);
    }
    if (ui.GetBoolean("HIS")) {
      p.SetOutputHis(max--);
    }
  }
  p.SetOutputRange(min, max);
}
