#include "Isis.h"

#include <cstdio>
#include <QString>

#include "pds.h"
#include "ProcessByLine.h"
#include "PvlToPvlTranslationManager.h"
#include "SpecialPixel.h"
#include "UserInterface.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "IString.h"
#include "OriginalLabel.h"

using namespace std;
using namespace Isis;

void WriteLine(Buffer &b);
void TranslateLabels(FileName in, Cube *ocube);
PDSINFO *pdsi;

void IsisMain() {
  // Grab the file to import
  UserInterface &ui = Application::GetUserInterface();
  FileName in = ui.GetFileName("FROM");
  FileName out = ui.GetFileName("TO");

  // Make sure it is a Clementine EDR
  bool projected;
  try {
    Pvl lab(in.expanded());
    projected = lab.hasObject("IMAGE_MAP_PROJECTION");
    QString id;
    id = (QString)lab["DATA_SET_ID"];
    id = id.simplified().trimmed();
    if(!id.contains("CLEM")) {
      QString msg = "Invalid DATA_SET_ID [" + id + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }
  catch(IException &e) {
    QString msg = "Input file [" + in.expanded() +
                  "] does not appear to be " +
                  "in Clementine EDR format";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if(projected) {
    QString msg = "[" + in.name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  //Decompress the file
  long int lines = 0;
  long int samps = 0;
  QString filename = in.expanded();
  pdsi = PDSR(filename.toLatin1().data(), &lines, &samps);

  ProcessByLine p;
  CubeAttributeOutput cubeAtt("+unsignedByte+1.0:254.0");
  Cube *ocube = p.SetOutputCube(ui.GetFileName("TO"), cubeAtt, pdsi->image_ncols, pdsi->image_nrows);
  p.StartProcess(WriteLine);
  TranslateLabels(in, ocube);
  p.EndProcess();
}

//Function to move uncompressed data to a cube
void WriteLine(Buffer &b) {
  for(int i = 0; i < pdsi->image_ncols; i++) {
    double d = pdsi->image[((b.Line()-1)*pdsi->image_ncols) + i];
    if(d <= 0.0) {
      b[i] = Isis::Lis;
    }
    else if(d >= 255.0) {
      b[i] = Isis::His;
    }
    else {
      b[i] = d;
    }
  }
}

/**
 *  Function to propagate the labels.
 *
 *  @internal
 *  @history  2009-02-17  Tracie Sucharski - Added BandBin keywords Center and
 *            Width to the translation table, Clementine.trn.  Do not alter this
 *            keywords for filter F,simply translate.
 *
 */

void TranslateLabels(FileName in, Cube *ocube) {
  // Get the directory where the Clementine translation tables are.
  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  QString transDir = (QString) dataDir["clementine1"];
  FileName transFile(transDir + "/translations/clementine.trn");

  Pvl pdsLab(in.expanded());
  PvlToPvlTranslationManager labelXlater(pdsLab, transFile.expanded());

  // Pvl outputLabels;
  Pvl *outputLabel = ocube->label();
  labelXlater.Auto(*(outputLabel));

  //Instrument group
  PvlGroup inst = outputLabel->findGroup("Instrument", Pvl::Traverse);

  PvlKeyword &startTime = inst.findKeyword("StartTime");
  startTime.setValue(startTime[0].mid(0, startTime[0].size() - 1));

  // Old PDS labels used keyword INSTRUMENT_COMPRESSION_TYPE & PDS Labels now use ENCODING_TYPE
  if(pdsLab.findObject("Image").hasKeyword("InstrumentCompressionType")) {
    inst += PvlKeyword("EncodingFormat", (QString) pdsLab.findObject("Image")["InstrumentCompressionType"]);
  }
  else {
    inst += PvlKeyword("EncodingFormat", (QString) pdsLab.findObject("Image")["EncodingType"]);
  }

  if(((QString)inst["InstrumentId"]) == "HIRES") {
    inst += PvlKeyword("MCPGainModeID", (QString)pdsLab["MCP_Gain_Mode_ID"], "");
  }

  ocube->putGroup(inst);

  PvlGroup bBin = outputLabel->findGroup("BandBin", Pvl::Traverse);
  QString filter = pdsLab["FilterName"];
  if(filter != "F") {
    //Band Bin group
    double center = pdsLab["CenterFilterWavelength"];
    center /= 1000.0;
    bBin.findKeyword("Center").setValue(toString(center), "micrometers");
  }
  double width = pdsLab["Bandwidth"];
  width /= 1000.0;
  bBin.findKeyword("Width").setValue(toString(width), "micrometers");
  ocube->putGroup(bBin);

  //Kernel group
  PvlGroup kern("Kernels");
  if(((QString)inst["InstrumentId"]) == "HIRES") {
    kern += PvlKeyword("NaifFrameCode", "-40001");
  }
  if(((QString)inst["InstrumentId"]) == "UVVIS") {
    kern += PvlKeyword("NaifFrameCode", "-40002");
  }
  if(((QString)inst["InstrumentId"]) == "NIR") {
    kern += PvlKeyword("NaifFrameCode", "-40003");
  }
  if(((QString)inst["InstrumentId"]) == "LWIR") {
    kern += PvlKeyword("NaifFrameCode", "-40004");
  }
  ocube->putGroup(kern);

  OriginalLabel org(pdsLab);
  ocube->write(org);
}
