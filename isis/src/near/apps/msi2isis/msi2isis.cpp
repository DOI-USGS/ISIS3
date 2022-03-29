/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include <QString>

#include "Cube.h"
#include "CubeAttribute.h"
#include "Enlarge.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "ProcessBySample.h"
#include "ProcessByBrick.h"
#include "ProcessImportPds.h"
#include "ProcessRubberSheet.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "PvlToPvlTranslationManager.h"
#include "UserInterface.h"

using namespace Isis;
using namespace std;

void flipAndTrim(Buffer &in, Buffer &out);
void trimOnly(Buffer &in, Buffer &out);
void translateMsiLabels(Pvl inputLabelPvl, Pvl *isisLabelPvl);
int g_trim = 33;
int g_numSamples = 537;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // get the label for the input image
  FileName from = ui.GetFileName("FROM");
  if (from.extension().toUpper() != "LBL") {
    from = from.setExtension("lbl");
    if (!from.fileExists()) {
      from = from.setExtension("LBL");
    }
    if (!from.fileExists()) {
      throw IException(IException::Io,
                       "Unable to find PDS label file for ["
                       + ui.GetFileName("FROM") + "].",
                       _FILEINFO_);
    }
  }

  // These images are small (537 cols x 244 line) and will be run
  //  through 3 processes.
  //  (1) Import to Isis cube format
  //  (2) Enlarge to 537 cols x 412 lines
  //  (3) Flip over horizontal axis, if LINE_DISPLAY_DIRECTION=UP
  //      and null edges.

  //  The first process will import the image into Cube format as is.
  ProcessImportPds importPds;
  Pvl inputLabelPvl;

  importPds.SetPdsFile(from.expanded(), "", inputLabelPvl);

  // from the pds label, verfify that the image is valid before continuing
  if(inputLabelPvl["INSTRUMENT_ID"][0] != "MSI") {
    throw IException(IException::Io,
                     "The input label [" + from.expanded() + "] has an invalid "
                     "value for INSTRUMENT_ID = ["
                     + inputLabelPvl["INSTRUMENT_ID"][0]
                     + "]. The msi2isis program requires INSTRUMENT_ID = [MSI].",
                     _FILEINFO_);
  }
  int lines = inputLabelPvl.findObject("IMAGE")["LINES"][0].toInt();
  int samples = inputLabelPvl.findObject("IMAGE")["LINE_SAMPLES"][0].toInt();
  if ( lines != 244 || samples != 537) {
    QString msg = "The given file [" + from.expanded() + "] does not contain "
                  "a full MSI image. Full NEAR Shoemaker MSI images have "
                  "dimension 537 samples x 244 lines. The given image is ["
                  + QString(toString(samples)) + "] samples by ["
                  + QString(toString(lines)) + "] lines.";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  if (inputLabelPvl["SAMPLE_DISPLAY_DIRECTION"][0] != "RIGHT") {
    QString msg = "The input label [" + from.expanded() + "] has an invalid "
                  "value for SAMPLE_DISPLAY_DIRECTION = ["
                  + inputLabelPvl["SAMPLE_DISPLAY_DIRECTION"][0]
                  + "]. The msi2isis program requires "
                  "SAMPLE_DISPLAY_DIRECTION = [RIGHT].";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  if (inputLabelPvl["LINE_DISPLAY_DIRECTION"][0] != "UP") {
    QString msg = "The input label [" + from.expanded() + "] has an invalid "
                  "value for LINE_DISPLAY_DIRECTION = ["
                  + inputLabelPvl["LINE_DISPLAY_DIRECTION"][0]
                  + "]. The msi2isis program requires "
                  "LINE_DISPLAY_DIRECTION = [UP].";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  // Don't import projected image
  if(inputLabelPvl.hasObject("IMAGE_MAP_PROJECTION")) {
    QString msg = "Unable to import the NEAR Shoemaker MSI image from ["
                  + from.expanded() + "] using msi2isis.This program only "
                  "imports images that have not been projected. Use pds2isis. ";
    throw IException(IException::Io, msg, _FILEINFO_);
  }


  // the given input file appears to be valid, continue with the import process
  FileName importProcessOutCube("$TEMPORARY/" + from.baseName() + ".import.tmp.cub");
  CubeAttributeOutput outatt = CubeAttributeOutput("+Real");
  importPds.SetOutputCube(importProcessOutCube.expanded(), outatt);
  importPds.StartProcess();
  importPds.Finalize();

  CubeAttributeInput inatt;

  // The second process will enlarge the imported cube from 537x244 to 537x412
  FileName enlargeProcessOutCube("$TEMPORARY/" + from.baseName() + ".enlarge.tmp.cub");
  ProcessRubberSheet enlargeProcess;
  Cube *cube = enlargeProcess.SetInputCube(importProcessOutCube.expanded(), inatt);
  enlargeProcess.SetOutputCube(enlargeProcessOutCube.expanded(), outatt, 537, 412, 1);

  // Set up the interpolator
  Interpolator *interp;
  if(ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
    interp = new Interpolator(Interpolator::NearestNeighborType);
  }
  else if(ui.GetString("INTERP") == "BILINEAR") {
    interp = new Interpolator(Interpolator::BiLinearType);
  }
  else { //if(ui.GetString("INTERP") == "CUBICCONVOLUTION") {
    interp = new Interpolator(Interpolator::CubicConvolutionType);
  }

  double sampleScale = 1.0;
  double lineScale = 412.0/(double)lines;
  Enlarge *enlarge = new Enlarge(cube, sampleScale, lineScale);
  enlargeProcess.StartProcess(*enlarge, *interp);
  enlargeProcess.Finalize();

  // The third (last) process will flip the image lines and set the 33 pixels
  // along each border (top, bottom, left, and right) to null.
  ProcessBySample processSamps;
  processSamps.SetInputCube(enlargeProcessOutCube.expanded(), inatt);
  Cube *outputCube = processSamps.SetOutputCube("TO");

  // translate labels
  try {
    // translate the band bin and archive groups to this pvl
    Pvl bandBinAndArchivePvl;
    importPds.TranslatePdsLabels(bandBinAndArchivePvl);

    // add translated values from band bin and archive groups to the output cube
    PvlGroup outputBandBinGrp("BandBin");
    PvlGroup outputArchiveGrp("Archive");

    if(bandBinAndArchivePvl.findGroup("BandBin").keywords() > 0) {
      outputBandBinGrp = bandBinAndArchivePvl.findGroup("BandBin");
    }

    // This group will never be empty since we translate INSTRUMENT_ID and this
    // is required for the Instrument group
    outputArchiveGrp = bandBinAndArchivePvl.findGroup("Archive");

    outputCube->putGroup(PvlGroup("Instrument"));
    outputCube->putGroup(outputBandBinGrp);
    outputCube->putGroup(outputArchiveGrp);
    outputCube->putGroup(PvlGroup("Kernels"));

    Pvl *isisLabelPvl = outputCube->label();
    translateMsiLabels(inputLabelPvl, isisLabelPvl);
  }
  catch(IException &e) {
    delete enlarge;
    delete interp;
    remove(importProcessOutCube.expanded().toLatin1());
    remove(enlargeProcessOutCube.expanded().toLatin1());
    QString msg = "Unable to translate the labels from [" + from.expanded()
                  + "] to ISIS format using msi2isis.";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }
  // now, determine the number of samples, then flip and trim the output cube
  g_numSamples = outputCube->sampleCount();
  processSamps.ProcessCube(flipAndTrim);
  processSamps.Finalize();

  // clean up temp files and "new" pointers
  delete enlarge;
  delete interp;
  remove(importProcessOutCube.expanded().toLatin1());
  remove(enlargeProcessOutCube.expanded().toLatin1());
}

/**
 * @brief Flip the lines and trim the edges.
 *
 * This process will flip over the horizontal axis (i.e. flip lines) and trim
 * 33 pixels from the top, bottom, left and right edges of the input buffer.
 *
 * @param in Reference to the input buffer.
 * @param out Reference to the output buffer.
 */
void flipAndTrim(Buffer &in, Buffer &out) {
  // Trim the left and right sides of the image
  if(in.Sample() <= g_trim || in.Sample() > g_numSamples - g_trim) {
    for(int i = 0; i < in.size(); i++) {
      out[i] = NULL8;
    }
  }
  // Otherwise, if the current sample is between 33 and 504, trim the
  // first and last 33 lines and flip the rest.
  else {
    int lastLineIndex = in.size() - 1;
    for(int i = 0; i < in.size(); i++) {
      if (in.Line(i) <= g_trim || in.Line(i) > in.LineDimension() - g_trim) {
        out[i] = NULL8;
      }
      else {
        out[i] = in[lastLineIndex - i];
      }
    }
  }
}

/**
 * Translate the MSI labels into Isis.
 *
 * This method requires the Instrument, BandBin, and Kernels groups to already
 * exist in the labels.
 *
 * @param inputLabelPvl The PDS input file.
 * @param isisLabelPvl The output Isis cube label Pvl.
 */
void translateMsiLabels(Pvl inputLabelPvl, Pvl *isisLabelPvl) {
  PvlGroup &instGrp(isisLabelPvl->findGroup("Instrument", Pvl::Traverse));
  PvlGroup &bandBinGrp(isisLabelPvl->findGroup("BandBin", Pvl::Traverse));
  PvlGroup &kernelsGrp(isisLabelPvl->findGroup("Kernels", Pvl::Traverse));

  kernelsGrp += PvlKeyword("NaifFrameCode", "-93001");

  PvlToPvlTranslationManager labelXlater(inputLabelPvl,
                                    "$ISISROOT/appdata/translations/NearMsiImportPdsLabel.trn");
  labelXlater.Auto(*isisLabelPvl);

  // Add units to center wave length
  bandBinGrp.findKeyword("Center").setUnits("nm");

  // Read DPU deck temperature value from the labels (This value is also given
  // in celcius in the FITS header under the keyword NEAR-049)
  instGrp += PvlKeyword("DpuDeckTemperature",
                     inputLabelPvl["INSTRUMENT_TEMPERATURE"][1],
                     inputLabelPvl["INSTRUMENT_TEMPERATURE"].unit());

  // we might want to read values from the FITS header in the future. For
  // example...
  //
  // importPds.SaveFileHeader();
  // char *fitsHeader = importPds.FileHeader();
  //
  // or
  //
  // importPds.SaveDataHeader();
  // char *fitsHeader = importPds.DataHeader()

  // int startByte = 4251; //if this keyword value was found between 4251:4260
  // int numBytes = 10;
  // string summingMode(numBytes, '\0');
  // for (int byte = 0; byte < numBytes; byte++) {
  //   summingMode[byte] = fitsHeader[startByte + byte];
  // }
}
