#include "Isis.h"
#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "EndianSwapper.h"
#include "Table.h"
#include "CubeAttribute.h"
#include "LineManager.h"
#include "IException.h"

using namespace std;
using namespace Isis;

void IgnoreData(Isis::Buffer &buf) {};
void WriteOutput(Isis::Buffer &buf);
void TranslateHrscLabels(Pvl &inLabels, Pvl &outLabel);

Cube *outCube = NULL;
long numLinesSkipped = 0;
std::vector< bool > lineInFile;

/**
 * This program imports Mars Express HRSC files
 *
 *  This works by first determining whether or not the input file
 *    has prefix data.
 *
 *  If there is prefix data, a StartProcess is called with the
 *    IgnoreData() function callback in order to get the import class to
 *    collect prefix data. Once the prefix data is populated, we go ahead
 *    and look for "gaps" - HRSC files can give us a time and exposure duration
 *    for each line, we look for where the time + exposure duration != next line's time.
 *    We populate a table (LineScanTimes) with the prefix data and lineInFile with whether or not
 *    a gap should be inserted.
 *
 *  For all files, we now process the data using the WriteOutput callback. If there were gaps,
 *    WriteOutput will put them in their proper places. Finally, we translate the labels and put
 *    the LineScanTimes (if necessary) in the output cube.
 *
 *   This is a two-pass system for files with prefix data, one-pass for files without.
 *
 *   The Isis2 equivalent to this program is mex2isis.pl. It is worth noting that regardless of the
 *     input file's byte order, the prefix data byte order is always LSB.
 */
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  try {
    Pvl temp(ui.GetFileName("FROM"));
    // Check for HRSC file
    if(temp["INSTRUMENT_ID"][0] != "HRSC") throw IException();
  }
  catch(IException &e) {
    IString msg = "File [" + ui.GetFileName("FROM") +
                  "] does not appear to be a Mars Express HRSC image.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  ProcessImportPds p;
  Pvl label;
  lineInFile.clear();
  numLinesSkipped = 0;

  p.SetPdsFile(ui.GetFileName("FROM"), "", label);

  CubeAttributeOutput outAtt(ui.GetFileName("TO"));
  outCube = new Cube();

  outCube->setByteOrder(outAtt.byteOrder());
  outCube->setFormat(outAtt.fileFormat());
  outCube->setLabelsAttached(outAtt.labelAttachment() == AttachedLabel);

  /**
    * Isis2 mex2isis.pl:
    *   if (index($detector_id,"MEX_HRSC_SRC") < 0 &&
    *   $processing_level_id < 3)
    */

  bool isSrcFile = (label["DETECTOR_ID"][0] != "MEX_HRSC_SRC");
  bool mapProjRdr = ((int)label["PROCESSING_LEVEL_ID"] >= 3);

  try {
    if (!isSrcFile) {
      QString msg = "File [" + ui.GetFileName("FROM");
      msg += "] is SRC data and cannot be read.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (mapProjRdr) {
      QString msg = "File [" + ui.GetFileName("FROM");
      msg += "] is map projected and cannot be read.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  catch (IException e) {
      QString msg = "File cannot be read by hrsc2isis, use pds2isis.";
      throw IException(e, IException::User, msg, _FILEINFO_);
  }

  TableField ephTimeField("EphemerisTime", TableField::Double);
  TableField expTimeField("ExposureTime", TableField::Double);
  TableField lineStartField("LineStart", TableField::Integer);

  TableRecord timesRecord;
  timesRecord += ephTimeField;
  timesRecord += expTimeField;
  timesRecord += lineStartField;

  Table timesTable("LineScanTimes", timesRecord);

  p.SetDataPrefixBytes((int)label.findObject("IMAGE")["LINE_PREFIX_BYTES"]);
  p.SaveDataPrefix();

  p.Progress()->SetText("Reading Prefix Data");
  p.StartProcess(IgnoreData);

  // The prefix data is always in LSB format, regardless of the overall file format
  EndianSwapper swapper("LSB");

  std::vector<double> ephemerisTimes;
  std::vector<double> exposureTimes;
  std::vector< std::vector<char *> > prefix = p.DataPrefix();

  for(int line = 0; line < p.Lines(); line++) {
    double ephTime = swapper.Double((double *)prefix[0][line]);
    double expTime = swapper.Float((float *)(prefix[0][line] + 8)) / 1000.0;

    if(line > 0) {
      /**
       * We know how many skipped lines with this equation. We take the
       *   difference in the current line and the last line's time, which will
       *   ideally be equal to the last line's exposure duration. We divide this by
       *   the last line's exposure duration, and the result is the 1-based count of
       *   how many exposures there were between the last line and the current line.
       *   We subtract one in order to remove the known exposure, and the remaining should
       *   be the 1-based count of how many lines were skipped. Add 0.5 to round up.
       */
      int skippedLines = (int)((ephTime - ephemerisTimes.back()) / exposureTimes.back() - 1.0 + 0.5);

      for(int i = 0; i < skippedLines; i++) {
        ephemerisTimes.push_back(ephemerisTimes.back() + exposureTimes.back());
        exposureTimes.push_back(exposureTimes.back());
        lineInFile.push_back(false);
      }
    }

    ephemerisTimes.push_back(ephTime);
    exposureTimes.push_back(expTime);
    lineInFile.push_back(true);
  }

  double lastExp = 0.0;
  for(unsigned int i = 0; i < ephemerisTimes.size(); i++) {
    if(lastExp != exposureTimes[i]) {
      lastExp = exposureTimes[i];
      timesRecord[0] = ephemerisTimes[i];
      timesRecord[1] = exposureTimes[i];
      timesRecord[2] = (int)i + 1;
      timesTable += timesRecord;
    }
  }

  outCube->setDimensions(p.Samples(), lineInFile.size(), p.Bands());

  p.Progress()->SetText("Importing");
  outCube->create(ui.GetFileName("TO"));
  p.StartProcess(WriteOutput);

  outCube->write(timesTable);

  // Get as many of the other labels as we can
  Pvl otherLabels;

  //p.TranslatePdsLabels (otherLabels);
  TranslateHrscLabels(label, otherLabels);

  if(otherLabels.hasGroup("Mapping") &&
      (otherLabels.findGroup("Mapping").keywords() > 0)) {
    outCube->putGroup(otherLabels.findGroup("Mapping"));
  }

  if(otherLabels.hasGroup("Instrument") &&
      (otherLabels.findGroup("Instrument").keywords() > 0)) {
    outCube->putGroup(otherLabels.findGroup("Instrument"));
  }

  if(otherLabels.hasGroup("BandBin") &&
      (otherLabels.findGroup("BandBin").keywords() > 0)) {
    outCube->putGroup(otherLabels.findGroup("BandBin"));
  }

  if(otherLabels.hasGroup("Archive") &&
      (otherLabels.findGroup("Archive").keywords() > 0)) {
    outCube->putGroup(otherLabels.findGroup("Archive"));
  }

  if(otherLabels.hasGroup("Kernels") &&
      (otherLabels.findGroup("Kernels").keywords() > 0)) {
    outCube->putGroup(otherLabels.findGroup("Kernels"));
  }

  p.EndProcess();

  outCube->close();
  delete outCube;
  outCube = NULL;
  lineInFile.clear();
}

void WriteOutput(Isis::Buffer &buf) {
  LineManager outLines(*outCube);

  if(lineInFile.size()) {
    for(int i = 0; i < outLines.size(); i++) {
      outLines[i] = Isis::Null;
    }

    while(!lineInFile[(buf.Line()+numLinesSkipped) % lineInFile.size()]) {
      outLines.SetLine(buf.Line() + numLinesSkipped, buf.Band());
      outCube->write(outLines);
      numLinesSkipped ++;
    }
  }

  outLines.SetLine(buf.Line() + numLinesSkipped, buf.Band());

  // outLines.Copy(buf); doesn't work because the raw buffers don't match
  for(int i = 0; i < outLines.size(); i++)
    outLines[i] = buf[i];

  outCube->write(outLines);
}

void TranslateHrscLabels(Pvl &inLabels, Pvl &outLabel) {
  // Get the directory where the MRO HiRISE translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Mex"] + "/translations/";

  // Translate the Instrument group
  FileName transFile(transDir + "hrscInstrument.trn");
  PvlToPvlTranslationManager instrumentXlater(inLabels, transFile.expanded());
  instrumentXlater.Auto(outLabel);

  if(inLabels.hasKeyword("MACROPIXEL_SIZE")) {
    outLabel.findGroup("Instrument", Pvl::Traverse) += PvlKeyword("Summing", inLabels["MACROPIXEL_SIZE"][0]);
  }
  else {
    outLabel.findGroup("Instrument", Pvl::Traverse) += PvlKeyword("Summing", "1");
  }

  // Remove 'Z' from times
  QString startTime = outLabel.findGroup("Instrument", Pvl::Traverse)["StartTime"][0];
  startTime = startTime.mid(0, startTime.size() - 1);
  outLabel.findGroup("Instrument", Pvl::Traverse)["StartTime"] = startTime;

  QString stopTime = outLabel.findGroup("Instrument", Pvl::Traverse)["StopTime"][0];
  stopTime = stopTime.mid(0, stopTime.size() - 1);
  outLabel.findGroup("Instrument", Pvl::Traverse)["StopTime"] = stopTime;

  // Translate the BandBin group
  transFile  = transDir + "hrscBandBin.trn";
  PvlToPvlTranslationManager bandBinXlater(inLabels, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Archive group
  transFile  = transDir + "hrscArchive.trn";
  PvlToPvlTranslationManager archiveXlater(inLabels, transFile.expanded());
  archiveXlater.Auto(outLabel);

  std::map<QString, int> naifIkCodes;
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_HEAD", -41210));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_S2",   -41211));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_RED",  -41212));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_P2",   -41213));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_BLUE", -41214));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_NADIR", -41215));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_GREEN", -41216));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_P1",   -41217));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_IR",   -41218));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_S1",   -41219));
  naifIkCodes.insert(std::pair<QString, int>("MEX_HRSC_SRC",  -41220));

  QString key = outLabel.findGroup("Archive", Pvl::Traverse)["DetectorId"];
  int ikCode = naifIkCodes[key];

  if(ikCode < -41220 || ikCode > -41210) {
    QString msg = "Unrecognized Detector ID [";
    msg += key;
    msg += "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  PvlGroup kerns("Kernels");
  kerns += PvlKeyword("NaifIkCode", toString(ikCode));
  outLabel.addGroup(kerns);
}
