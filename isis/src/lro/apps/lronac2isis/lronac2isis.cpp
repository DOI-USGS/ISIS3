#include "Isis.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Pvl.h"
#include "OriginalLabel.h"
#include "History.h"
#include "LineManager.h"
#include <vector>

#define MAX_INPUT_VALUE 4095

using namespace std;
using namespace Isis;

void ResetGlobals();
void Import(Buffer &buf);
void TranslateLrocNacLabels(FileName &labelFile, Cube *ocube);

// Global variables for processing functions
Cube *g_ocube;
std::vector< double> g_xterm, g_bterm, g_mterm;
bool g_flip = false;

void IsisMain() {
  // Initialize variables
  ResetGlobals();

  //Check that the file comes from the right camera
  UserInterface &ui = Application::GetUserInterface();
  FileName inFile = ui.GetFileName("FROM");
  QString id;
  try {
    Pvl lab(inFile.expanded());

    if(lab.HasKeyword("DATA_SET_ID"))
      id = (QString) lab.FindKeyword("DATA_SET_ID");
    else {
      QString msg = "Unable to read [DATA_SET_ID] from input file [" + inFile.expanded() + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    //Checks if in file is rdr
    bool projected = lab.HasObject("IMAGE_MAP_PROJECTION");
    if(projected) {
      QString msg = "[" + inFile.name() + "] appears to be an rdr file.";
      msg += " Use pds2isis.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Store the decompanding information
    PvlKeyword xtermKeyword = lab.FindKeyword("LRO:XTERM"),
               mtermKeyword = lab.FindKeyword("LRO:MTERM"),
               btermKeyword = lab.FindKeyword("LRO:BTERM");

    if(mtermKeyword.Size() != xtermKeyword.Size() || btermKeyword.Size() != xtermKeyword.Size()) {
      QString msg = "The decompanding terms do not have the same dimensions";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    for(int i = 0; i < xtermKeyword.Size(); i++) {
      g_xterm.push_back(toDouble(xtermKeyword[i]));
      g_mterm.push_back(toDouble(mtermKeyword[i]));
      g_bterm.push_back(toDouble(btermKeyword[i]));
    }

    double versionId = toDouble(lab.FindKeyword("PRODUCT_VERSION_ID")[0].remove(QRegExp("^v")));

    if(lab.FindKeyword("FRAME_ID")[0] == "RIGHT" && versionId < 1.30)
      g_flip = true;
    else
      g_flip = false;
  }
  catch(IException &e) {
    QString msg = "The PDS header is missing important keyword(s).";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  id = id.simplified().trimmed();
  if(id.mid(13, 3) != "EDR") {
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be "
                 + "in LROC-NAC EDR format. DATA_SET_ID is [" + id + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  //Process the file
  Pvl pdsLab;
  ProcessImportPds p;
  p.SetPdsFile(inFile.expanded(), "", pdsLab);

  // Set the output bit type to Real
  CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");

  g_ocube = new Cube();
  g_ocube->setByteOrder(outAtt.byteOrder());
  g_ocube->setFormat(outAtt.fileFormat());
  g_ocube->setMinMax((double) VALID_MIN2, (double) VALID_MAX2);
  g_ocube->setLabelsAttached(outAtt.labelAttachment() == AttachedLabel);
  g_ocube->setDimensions(p.Samples(), p.Lines(), p.Bands());
  g_ocube->setPixelType(Isis::Real);
  g_ocube->create(ui.GetFileName("TO"));

  // Do 8 bit to 12 bit conversion
  // And if NAC-R, flip the frame
  p.StartProcess(Import);

  // Then translate the labels
  TranslateLrocNacLabels(inFile, g_ocube);
  p.EndProcess();

  // Add History
  History history("IsisCube");
  history.AddEntry();
  g_ocube->write(history);

  // Add original label
  OriginalLabel origLabel(pdsLab);
  g_ocube->write(origLabel);

  g_ocube->close();
  delete g_ocube;
}

// The input buffer has a raw 16 bit buffer but the values are still 0 to 255.
// See "Appendix B - NAC and WAC Companding Schemes" of the LROC_SOC_SPEC
// document for reference.
void Import(Buffer &in) {
  LineManager outLines(*g_ocube);
  outLines.SetLine(in.Line(), in.Band());
  Buffer buf(in.SampleDimension(), in.LineDimension(), in.BandDimension(),
             g_ocube->getPixelType());

  // Do the decompanding
  for(int pixin = 0; pixin < in.size(); pixin++) {

    // if pixin < xtermo0, then it is in "segment 0"
    if(in[pixin] < g_xterm[0])
      buf[pixin] = (int) in[pixin];

    // otherwise, it is in segments 1 to 5
    else {
      unsigned int segment = 1;
      while(segment < g_xterm.size() && (in[pixin] - g_bterm[segment - 1]) / g_mterm[segment - 1] >= g_xterm[segment])
        segment++;

      // Compute the upper and lower bin values
      double upper = (in[pixin] + 1 - g_bterm[segment - 1]) / g_mterm[segment - 1] - 1;
      double lower = (in[pixin] - g_bterm[segment - 1]) / g_mterm[segment - 1];

      // Check if the bin is on the upper boundary of the last segment
      if(upper > MAX_INPUT_VALUE)
        upper = MAX_INPUT_VALUE;
      else if(segment < g_xterm.size() && upper >= g_xterm[segment]) {
        if((int)(g_bterm[segment] + g_mterm[segment]*upper) != in[pixin])
          upper = g_xterm[segment] - 1;
      }

      // Check if it is on the lower boundary of a segment
      if(lower < g_xterm[segment-1])
        lower = g_xterm[segment-1];

      // Output the middle bin value
      buf[pixin] = (upper + lower) / 2.0;

    }
  }

  // flip the NAC-R frame
  if(g_flip) {
    Buffer tmpbuf(buf);
    for(int i = 0; i < buf.size(); i++)
      buf[i] = tmpbuf[buf.size() - i - 1];
  }
  outLines.Copy(buf);
  g_ocube->write(outLines);
}

//Function to translate the labels
void TranslateLrocNacLabels(FileName &labelFile, Cube *ocube) {

  //Pvl to store the labels
  Pvl outLabel;
  //Set up the directory where the translations are
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Lro"] + "/translations/";
  Pvl labelPvl(labelFile.expanded());

  //Translate the Instrument group
  FileName transFile(transDir + "lronacInstrument.trn");
  PvlTranslationManager instrumentXlator(labelPvl, transFile.expanded());
  instrumentXlator.Auto(outLabel);

  //Translate the Archive group
  transFile = transDir + "lronacArchive.trn";
  PvlTranslationManager archiveXlater(labelPvl, transFile.expanded());
  archiveXlater.Auto(outLabel);

  //Translate the BandBin group
  transFile = transDir + "lronacBandBin.trn";
  PvlTranslationManager bandBinXlater(labelPvl, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  Pvl lab(labelFile.expanded());

  //Set up the Kernels group
  PvlGroup kern("Kernels");
  if(lab.FindKeyword("FRAME_ID")[0] == "LEFT")
    kern += PvlKeyword("NaifFrameCode", "-85600");
  else
    kern += PvlKeyword("NaifFrameCode", "-85610");

  PvlGroup inst = outLabel.FindGroup("Instrument", Pvl::Traverse);
  if(lab.FindKeyword("FRAME_ID")[0] == "LEFT") {
    inst.FindKeyword("InstrumentId") = "NACL";
    inst.FindKeyword("InstrumentName") = "LUNAR RECONNAISSANCE ORBITER NARROW ANGLE CAMERA LEFT";
  }
  else {
    inst.FindKeyword("InstrumentId") = "NACR";
    inst.FindKeyword("InstrumentName") = "LUNAR RECONNAISSANCE ORBITER NARROW ANGLE CAMERA RIGHT";
  }

  //Add all groups to the output cube
  ocube->putGroup(inst);
  ocube->putGroup(outLabel.FindGroup("Archive", Pvl::Traverse));
  ocube->putGroup(outLabel.FindGroup("BandBin", Pvl::Traverse));
  ocube->putGroup(kern);
}

void ResetGlobals() {
  g_ocube = NULL;
  g_xterm.clear();
  g_mterm.clear();
  g_bterm.clear();
  g_flip = false;
}
