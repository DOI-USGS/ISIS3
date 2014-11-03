#include "Isis.h"
#include "SpecialPixel.h"
#include "CubeAttribute.h"
#include "Cube.h"
#include "LineManager.h"
#include "Progress.h"
#include "Camera.h"
#include "Constants.h"
#include "Preference.h"
#include "Statistics.h"
#include "TextFile.h"
#include "Stretch.h"
#include "iTime.h"

#include <QProcess>
#include <QDebug>

using namespace Isis;
using namespace std;

const QString knownFilters[] = {
  "NIR",
  "RED",
  "ORANGE",
  "GREEN",
  "BLUE",
  "LONG_UV",
  "SHORT_UV"
};

Stretch stretch;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
  Cube icube;

  if(inAtt.bands().size() != 0) {
    icube.setVirtualBands(inAtt.bands());
  }

  icube.open(FileName(ui.GetFileName("FROM")).expanded());

  // Make sure it is a Marci cube
  FileName inFileName = ui.GetFileName("FROM");
  try {
    if(icube.group("Instrument")["InstrumentID"][0] != "Marci") {
      throw IException();
    }

    if(!icube.group("Archive").hasKeyword("SampleBitModeId")) {
      throw IException();
    }
  }
  catch(IException &) {
    QString msg = "This program is intended for use on MARCI images only. [";
    msg += inFileName.expanded() + "] does not appear to be a MARCI image.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(icube.group("Archive")["SampleBitModeId"][0] != "SQROOT") {
    QString msg = "Sample bit mode [" + icube.group("Archive")["SampleBitModeId"][0] + "] is not supported.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Read in calibration coefficients
  FileName calFile =
      FileName("$mro/calibration/marci/marciCoefficients_v???.pvl").highestVersion();

  vector<double> decimation;

  // Decimation is described in the MRO MARCI Instrument and Calibration document pg. 63
  for(int i = 0; i < 6; i++) {
    // Decimation is 1.0 for bands 1-6
    decimation.push_back(1.0);
  }

  QString startTime = icube.label()->findGroup("Instrument", Pvl::Traverse)["StartTime"][0];
  iTime start(startTime);
  iTime changeTime("November 6, 2006 21:30:00 UTC");

  if(start < changeTime) {
    decimation.push_back(1.0);
  }
  else {
    decimation.push_back(0.25);
  }

  // Get the LUT data
  FileName temp = FileName("$mro/calibration/marcisqroot_???.lut").highestVersion();
  TextFile stretchPairs(temp.expanded());

  // Create the stretch pairs
  stretch.ClearPairs();
  for(int i = 0; i < stretchPairs.LineCount(); i++) {
    QString line;
    stretchPairs.GetLine(line, true);
    int temp1 = toInt(line.split(" ").first());
    int temp2 = toInt(line.split(" ").last());
    stretch.AddPair(temp1, temp2);
  }

  stretchPairs.Close();

  // This file stores radiance/spectral distance coefficients
  Pvl calibrationData(calFile.expanded());

  // This will store the radiance coefficient and solar spectral distance coefficients
  // for each band.
  //   calibrationCoeffs[band].first gives the radiance coefficient
  //   calibrationCoeffs[band].second gives the spectral distance
  vector< pair<double, double> > calibrationCoeffs;

  // Check our coefficient file
  if(calibrationData.objects() != 7) {
    QString msg = "Calibration file [" + calFile.expanded() + "] must contain data for 7 filters in ascending order;";
    msg += " only [" + QString(calibrationData.objects()) + "] objects were found";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  // Read it, make sure it's ordered
  for(int obj = 0; obj < calibrationData.objects(); obj ++) {
    PvlObject &calObj = calibrationData.object(obj);

    if((int)calObj["FilterNumber"] != obj + 1) {
      QString msg = "Calibration file [" + calFile.expanded() + "] must have the filters in ascending order";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    pair<double, double> calData(calObj["RadianceCoefficient"], calObj["SolarSpectralDistance"]);
    calibrationCoeffs.push_back(calData);
  }

  vector<Cube *> flatcubes;
  vector<LineManager *> fcubeMgrs;
  int summing = toInt(icube.group("Instrument")["SummingMode"][0]);
  double ifdelay = toDouble(icube.group("Instrument")["InterframeDelay"][0]) * 1000.0;
  int flipped = toInt(icube.group("Instrument")["DataFlipped"][0]);

  // Read in the flat files
  for(int band = 0; band < 7; band++) {
    QString filePattern = "$mro/calibration/marci/";

    if(band < 5) {
      filePattern += "vis";
    }
    else {
      filePattern += "uv";
    }

    // UV cubes are always summing mode = 8, we can assume this rule will never
    //   be broken
    if(band >= 5 && summing != 8) {
      continue;
    }

    filePattern += "flat_band" + toString(band + 1);
    filePattern += "_summing" + toString(summing) + "_v???.cub";

    FileName flatFile = FileName(filePattern).highestVersion();
    Cube *fcube = new Cube();
    fcube->open(flatFile.expanded());
    flatcubes.push_back(fcube);

    LineManager *fcubeMgr = new LineManager(*fcube);
    fcubeMgr->SetLine(1, 1);
    fcubeMgrs.push_back(fcubeMgr);
  }

  // Prepare the output cube
  Cube ocube;

  CubeAttributeOutput outAtt = ui.GetOutputAttribute("TO");
  ocube.setDimensions(icube.sampleCount(), icube.lineCount(), icube.bandCount());
  ocube.setByteOrder(outAtt.byteOrder());
  ocube.setFormat(outAtt.fileFormat());
  ocube.setLabelsAttached(outAtt.labelAttachment() == AttachedLabel);
  ocube.setPixelType(outAtt.pixelType());

  ocube.create(FileName(ui.GetFileName("TO")).expanded());

  LineManager icubeMgr(icube);

  // This will store a direct translation from band to filter index
  vector<int> filter;

  // Conversion from filter name to filter index
  map<QString, int> filterNameToFilterIndex;
  filterNameToFilterIndex.insert(pair<QString, int>("BLUE",     1));
  filterNameToFilterIndex.insert(pair<QString, int>("GREEN",    2));
  filterNameToFilterIndex.insert(pair<QString, int>("ORANGE",   3));
  filterNameToFilterIndex.insert(pair<QString, int>("RED",      4));
  filterNameToFilterIndex.insert(pair<QString, int>("NIR",      5));
  filterNameToFilterIndex.insert(pair<QString, int>("SHORT_UV", 6));
  filterNameToFilterIndex.insert(pair<QString, int>("LONG_UV",  7));

  PvlKeyword &filtNames = icube.label()->findGroup("BandBin", Pvl::Traverse)["FilterName"];
  int numFilters = filtNames.size();
  for(int i = 0; i < filtNames.size(); i++) {
    if(filterNameToFilterIndex.find(filtNames[i]) != filterNameToFilterIndex.end()) {
      filter.push_back(filterNameToFilterIndex.find(filtNames[i])->second);
    }
    else {
      QString msg = "Unrecognized filter name [" + QString(filtNames[i]) + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  PvlKeyword &sumMode = icube.label()->findGroup("Instrument", Pvl::Traverse)["SummingMode"];
  int summingMode = toInt(sumMode[0]);
  int filterHeight = 16 / summingMode;
  std::vector<int> padding;
  padding.resize(numFilters);
  PvlKeyword &colOff = icube.label()->findGroup("Instrument", Pvl::Traverse)["ColorOffset"];
  int colorOffset = toInt(colOff[0]);

  for(int filter = 0; filter < numFilters; filter++) {
    if(colorOffset > 0) {
      // find the filter num
      int filtNum = 0;
      int numKnownFilters = sizeof(knownFilters) / sizeof(QString);
      
      while(filtNum < numKnownFilters &&
            (QString)icube.label()->findGroup("BandBin", Pvl::Traverse)["FilterName"][filter] != knownFilters[filtNum]) {
        filtNum ++;
      }
      
      padding[filter] = (colorOffset * filterHeight) * filtNum;
    }
    else {
      padding[filter] = 0;
    }
  }

  int maxOffset = *max_element(padding.begin(),padding.end());

  QString prodId = icube.label()->findGroup("Archive", Pvl::Traverse)["ProductId"][0];
  prodId = prodId.toUpper();
  QString prodIdRight = prodId.right(7);
  QString prodIdLeft = prodId.left(15);

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString command = "grep " + prodIdLeft + " " + env.value("ISIS3DATA") + "/mro/calibration/marci/varexp.tab";
  QProcess filesearch;
  QByteArray result;
  filesearch.start(command);

  filesearch.waitForFinished();
  QTextStream rsyncStdoutStream(filesearch.readAllStandardOutput());
  vector<int> frameseq;
  vector<double> exptime;
  while (true) {
    QString line = rsyncStdoutStream.readLine();
    if (line.isNull()) {
      break;
    }
    else {
      if (line.contains(prodIdRight)) {
        QStringList results = line.split(",",QString::SkipEmptyParts);
        frameseq.push_back(results.at(1).toInt());
        exptime.push_back(results.at(2).toDouble());
      }
    }
  }

  if (flipped && exptime.size() > 0) {
    reverse(frameseq.begin(),frameseq.end());
    reverse(exptime.begin(),exptime.end());
  }

  if (exptime.size() == 0) {
    PvlGroup missing("NoExposureTimeDataFound");
    missing.addKeyword(PvlKeyword("FileNotFoundInVarexpFile", prodIdLeft), Pvl::Replace);
    Application::Log(missing);
  }

  bool iof = ui.GetBoolean("IOF");
  double exposure = ((double)icube.label()->findGroup("Instrument", Pvl::Traverse)["ExposureDuration"]) * 1000.0;
  Camera *cam = NULL;
  double solarDist = Isis::Null;

  if(iof) {
    cam = icube.camera();
    cam->SetImage(icubeMgr.size() / 2.0, 0.5 + (16 / 2) / summing);
    solarDist = cam->SolarDistance();
  }

  LineManager ocubeMgr(ocube);
  ocubeMgr.SetLine(1, 1);

  Progress prog;
  prog.SetText("Calibrating Image");
  prog.SetMaximumSteps(ocube.lineCount() * ocube.bandCount());
  prog.CheckStatus();

  Statistics stats;

  int band = 0;
  int frame = 0;
  int line = 0;
  int seqno = 0;
  do {
    icube.read(icubeMgr);
    ocube.read(ocubeMgr);

    int fcubeIndex = filter[ocubeMgr.Band()-1] - 1;
    if (band != ocubeMgr.Band()) {
      band = ocubeMgr.Band();
      line = 0;
      if (!flipped) {
        frame = 0;
        exposure = ((double)icube.label()->findGroup("Instrument", Pvl::Traverse)["ExposureDuration"]) * 1000.0;
      } 
      else {
        maxOffset = padding[band-1];
        frame = (icube.lineCount() - maxOffset) / filterHeight - 1;
        exposure = exptime[0];
      }
      seqno = 0;
    }

    flatcubes[fcubeIndex]->read((*fcubeMgrs[fcubeIndex]));

    line++;
    if (line > padding[band-1] || flipped) {
      if (!flipped) {
        frame = (int)((line-padding[band-1]-1)/filterHeight);
      }
      else {
        frame = (icube.lineCount() - maxOffset) / filterHeight - 1 - (int)((line-1)/filterHeight);
      }
      if (!flipped) {
        if (seqno < (int)frameseq.size()) {
          if (frame >= frameseq[seqno]) {
            exposure = exptime[seqno];
            if ((QString)icube.label()->findGroup("BandBin", Pvl::Traverse)["FilterName"][band-1] == "LONG_UV" ||
                 (QString)icube.label()->findGroup("BandBin", Pvl::Traverse)["FilterName"][band-1] == "SHORT_UV") {
              exposure = ifdelay - 57.763 - exposure;
            }
            seqno++;
          }
        }
      }
      else {
        if (seqno < (int)frameseq.size()) {
          if (frame < frameseq[seqno]) {
            seqno++;
            exposure = exptime[seqno];
            if ((QString)icube.label()->findGroup("BandBin", Pvl::Traverse)["FilterName"][band-1] == "LONG_UV" ||
                (QString)icube.label()->findGroup("BandBin", Pvl::Traverse)["FilterName"][band-1] == "SHORT_UV") {
              exposure = ifdelay - 57.763 - exposure;
            }
          }
        }
        else {
          exposure = ((double)icube.label()->findGroup("Instrument", Pvl::Traverse)["ExposureDuration"]) * 1000.0;
        }
      }
    }

    for(int i = 0; i < ocubeMgr.size(); i++) {
      if(IsSpecial((*fcubeMgrs[fcubeIndex])[i]) || (*fcubeMgrs[fcubeIndex])[i] == 0.0) {
        ocubeMgr[i] = Isis::Null;
      }
      else if(IsSpecial(icubeMgr[i])) {
        ocubeMgr[i] = icubeMgr[i];
      }
      else {
        ocubeMgr[i] = stretch.Map(icubeMgr[i]) / (*fcubeMgrs[fcubeIndex])[i];

        ocubeMgr[i] = ocubeMgr[i] / exposure / (summing * decimation[fcubeIndex]) / calibrationCoeffs[fcubeIndex].first;

        // Convert to I/F?
        if(iof) {
          ocubeMgr[i] /= calibrationCoeffs[fcubeIndex].second / Isis::PI / (solarDist * solarDist);
        }
      }
    }

    ocube.write(ocubeMgr);

    icubeMgr++;
    ocubeMgr++;

    bool newFramelet = false;

    for(int i = 0; i < (int)fcubeMgrs.size(); i++) {
      (*fcubeMgrs[i]) ++;

      if(fcubeMgrs[i]->end()) {
        fcubeMgrs[i]->SetLine(1, 1);
        newFramelet = true;
      }
    }

    if(newFramelet && cam != NULL) {
      // center the cameras position on the new framelet to keep the solar distance accurate
      cam->SetBand(icubeMgr.Band());
      cam->SetImage(icubeMgr.size() / 2.0 + 0.5, (icubeMgr.Line() - 0.5) + (16 / 2) / summing);
      solarDist = cam->SolarDistance();
    }

    prog.CheckStatus();
  }
  while(!ocubeMgr.end());

  // Propagate labels and objects (in case of spice data)
  PvlObject &inCubeObj = icube.label()->findObject("IsisCube");
  PvlObject &outCubeObj = ocube.label()->findObject("IsisCube");

  for(int g = 0; g < inCubeObj.groups(); g++) {
    outCubeObj.addGroup(inCubeObj.group(g));
  }

  for(int o = 0; o < icube.label()->objects(); o++) {
    if(icube.label()->object(o).isNamed("Table")) {
      Blob t(icube.label()->object(o)["Name"], icube.label()->object(o).name());
      icube.read(t);
      ocube.write(t);
    }
  }

  icube.close();
  ocube.close();

  // The cube still owns this
  cam = NULL;

  for(int i = 0; i < (int)flatcubes.size(); i++) {
    delete fcubeMgrs[i];
    delete flatcubes[i];
  }

  fcubeMgrs.clear();
  flatcubes.clear();
}

