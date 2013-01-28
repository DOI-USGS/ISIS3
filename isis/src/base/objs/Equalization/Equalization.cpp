#include "Equalization.h"

#include <iomanip>

#include "Buffer.h"
#include "Cube.h"
#include "FileList.h"
#include "IException.h"
#include "LeastSquares.h"
#include "LineManager.h"
#include "OverlapNormalization.h"
#include "OverlapStatistics.h"
#include "Process.h"
#include "ProcessByLine.h"
#include "Projection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Statistics.h"

using namespace std;

namespace Isis {


  Equalization::Equalization() {
    init();
  }


  Equalization::Equalization(QString fromListName) {
    init();
    loadInputs(fromListName);
  }


  Equalization::~Equalization() {
    clearAdjustments();
    m_holdIndices.clear();

    if (m_results != NULL) {
      delete m_results;
      m_results = NULL;
    }
  }


  void Equalization::addHolds(QString holdListName) {
    FileList holdList;
    holdList.read(FileName(holdListName));

    if (holdList.size() > m_imageList.size()) {
      QString msg = "The list of identifiers to be held must be less than or ";
      msg += "equal to the total number of identitifers.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure each file in the holdlist matches a file in the fromlist
    for (int i = 0; i < holdList.size(); i++) {
      bool matched = false;
      for (int j = 0; j < m_imageList.size(); j++) {
        if (holdList[i] == m_imageList[j]) {
          matched = true;
          m_holdIndices.push_back(j);
          break;
        }
      }
      if (!matched) {
        QString msg = "The hold list file [" + holdList[i].toString() +
                          "] does not match a file in the from list";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }


  /** 
   * @param percent Percentage of the lines to consider when gathering overall 
   *            cube statistics and overlap statistics 
   * @param mincnt Minimum number of points in overlapping area required to be 
   *            used in the solution 
   * @param wtopt Indicates whether overlaps should be weighted
   * @param sType An integer value corresponding to the enumerated value of the 
   *              OverlapNormalization::SolutionType to be used.
   * @param methodType An integer value corresponding to the enumerated value of 
   *            the desired LeastSquares::SolveMethod to be used.
   */
  void Equalization::calculateStatistics(double percent, int mincnt,
      bool wtopt, OverlapNormalization::SolutionType sType, 
      LeastSquares::SolveMethod methodType) {

    clearAdjustments();

    m_mincnt = mincnt;
    m_wtopt = wtopt;

    // Loop through all the input cubes, calculating statistics for each cube
    // to use later
    vector<OverlapNormalization *> oNormList;
    for (int band = 1; band <= m_maxBand; band++) {
      vector<Statistics *> statsList;
      for (int img = 0; img < (int) m_imageList.size(); img++) {
        ProcessByLine p;
        QString bandStr(toString(band));
        QString statMsg = "Calculating Statistics for Band " + bandStr +
            " of " + toString(m_maxBand) + " in Cube " + toString(img + 1) +
            " of " + toString(m_maxCube);
        p.Progress()->SetText(statMsg);
        CubeAttributeInput att("+" + bandStr);
        QString inp = m_imageList[img].toString();
        p.SetInputCube(inp, att);

        Statistics *stats = new Statistics();

        CalculateFunctor func(stats, percent);
        p.ProcessCubeInPlace(func, false);
        p.EndProcess();

        statsList.push_back(stats);
      }

      // Create a separate OverlapNormalization object for every band
      OverlapNormalization *oNorm = new OverlapNormalization(statsList);
      loadHolds(oNorm);
      oNormList.push_back(oNorm);
    }

    // A list for keeping track of which input cubes are known to overlap
    // another
    vector<bool> doesOverlapList;
    for (int i = 0; i < m_imageList.size(); i++) {
      doesOverlapList.push_back(false);
    }

    // Find overlapping areas and add them to the set of known overlaps for
    // each band shared amongst cubes
    vector<OverlapStatistics> overlapList;
    for (int i = 0; i < m_imageList.size(); i++) {
      addAdjustment(new ImageAdjustment());

      Cube cube1;
      cube1.open(m_imageList[i].toString());

      for (int j = (i + 1); j < m_imageList.size(); j++) {
        Cube cube2;
        cube2.open(m_imageList[j].toString());
        QString cubeStr1 = toString((int)(i + 1));
        QString cubeStr2 = toString((int)(j + 1));
        QString statMsg = "Gathering Overlap Statisitcs for Cube " +
                         cubeStr1 + " vs " + cubeStr2 + " of " +
                         toString(m_maxCube);

        // Get overlap statistics for cubes
        OverlapStatistics oStats(cube1, cube2, statMsg, percent);

        // Only push the stats onto the oList vector if there is an overlap in at
        // least one of the bands
        if (oStats.HasOverlap()) {
          oStats.SetMincount(mincnt);
          overlapList.push_back(oStats);
          for (int band = 1; band <= m_maxBand; band++) {
            // Fill wt vector with 1's if the overlaps are not to be weighted, or
            // fill the vector with the number of valid pixels in each overlap
            int weight = 1;
            if (wtopt) weight = oStats.GetMStats(band).ValidPixels();

            // Make sure overlap has at least MINCOUNT valid pixels and add
            if (oStats.GetMStats(band).ValidPixels() >= mincnt) {
              oNormList[band - 1]->AddOverlap(
                  oStats.GetMStats(band).X(), i,
                  oStats.GetMStats(band).Y(), j, weight);
              doesOverlapList[i] = true;
              doesOverlapList[j] = true;
            }
          }
        }
      }
    }

    // Print an error if one or more of the images does not overlap another
    {
      QString badFiles = "";
      for (int img = 0; img < m_imageList.size(); img++) {
        // Print the name of each input cube without an overlap
        if (!doesOverlapList[img]) {
          badFiles += "[" + m_imageList[img].toString() + "] ";
        }
      }
      if (badFiles != "") {
        QString msg = "File(s) " + badFiles;
        msg += " do(es) not overlap any other input images with enough valid pixels";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // Loop through each band making all necessary calculations
    try {
      for (int band = 0; band < m_maxBand; band++) {
        oNormList[band]->Solve(sType, methodType);

        for (unsigned int img = 0; img < m_adjustments.size(); img++) {
          m_adjustments[img]->addGain(oNormList[band]->Gain(img));
          if (oNormList[band] == 0.0) {
            throw IException(IException::Unknown, 
                             "Calculation for equalization statistics failed. Gain = 0."
                             _FILEINFO_);
          }
          m_adjustments[img]->addOffset(oNormList[band]->Offset(img));
          m_adjustments[img]->addAverage(oNormList[band]->Average(img));
        }
      }

    }
    catch (IException &e) {
      string msg = "Unable to calculate the equalization statistics. You may "
                   "want to try another LeastSquares::SolveMethod.";
      throw IException(e, IException::Unknown,  msg, _FILEINFO_);
    }
    // Compute the number valid and invalid overlaps
    for (unsigned int o = 0; o < overlapList.size(); o++) {
      for (int band = 1; band <= m_maxBand; band++) {
        (overlapList[o].IsValid(band)) ? m_validCnt++ : m_invalidCnt++;
      }
    }

    setResults(overlapList);
  }


  void Equalization::setResults(vector<OverlapStatistics> &overlapStats) {
    setResults();

    for (unsigned int i = 0; i < overlapStats.size(); i++) {
      m_results->AddObject(overlapStats[i].toPvl());
    }
  }


  void Equalization::setResults() {
    if (m_results != NULL) {
      delete m_results;
      m_results = NULL;
    }

    m_results = new Pvl();
    m_results->SetTerminator("");

    PvlObject equ("EqualizationInformation");
    PvlGroup gen("General");
    gen += PvlKeyword("TotalOverlaps", toString(m_validCnt + m_invalidCnt));
    gen += PvlKeyword("ValidOverlaps", toString(m_validCnt));
    gen += PvlKeyword("InvalidOverlaps", toString(m_invalidCnt));
    gen += PvlKeyword("Weighted", (m_wtopt) ? "true" : "false");
    gen += PvlKeyword("MinCount", toString(m_mincnt));
    equ.AddGroup(gen);
    for (int img = 0; img < m_imageList.size(); img++) {
      // Format and name information
      PvlGroup norm("Normalization");
      norm.AddComment("Formula: newDN = (oldDN - AVERAGE) * GAIN + AVERAGE + OFFSET");
      norm.AddComment("BandN = (GAIN, OFFSET, AVERAGE)");
      norm += PvlKeyword("FileName", m_imageList[img].original());

      // Band by band statistics
      for (int band = 1; band <= m_maxBand; band++) {
        QString mult = toString(m_adjustments[img]->getGain(band - 1));
        QString base = toString(m_adjustments[img]->getOffset(band - 1));
        QString avg = toString(m_adjustments[img]->getAverage(band - 1));
        QString bandNum = toString(band);
        QString bandStr = "Band" + bandNum;
        PvlKeyword bandStats(bandStr);
        bandStats += mult;
        bandStats += base;
        bandStats += avg;
        norm += bandStats;
      }
      equ.AddGroup(norm);
    }

    m_results->AddObject(equ);
  }


  void Equalization::importStatistics(QString instatsFileName) {
    // Check for errors with the input statistics
    vector<int> normIndices = validateInputStatistics(instatsFileName);

    clearAdjustments();
    for (int img = 0; img < (int) m_imageList.size(); img++) {
      // Apply correction based on pre-determined statistics information
      Pvl inStats(instatsFileName);
      PvlObject &equalInfo = inStats.FindObject("EqualizationInformation");
      PvlGroup &normalization = equalInfo.Group(normIndices[img]);

      // TODO should we also get the valid and invalid count?

      ImageAdjustment *adjustment = new ImageAdjustment();

      // Get and store the modifiers for each band
      for (int band = 1; band < normalization.Keywords(); band++) {
        adjustment->addGain(toDouble(normalization[band][0]));
        adjustment->addOffset(toDouble(normalization[band][1]));
        adjustment->addAverage(toDouble(normalization[band][2]));
      }

      addAdjustment(adjustment);
    }
  }


  void Equalization::applyCorrection(QString toListName="") {
    FileList outList;
    fillOutList(outList, toListName);

    QString maxCubeStr = toString((int) m_imageList.size());
    for (int img = 0; img < m_imageList.size(); img++) {
      // Set up for progress bar
      ProcessByLine p;
      p.Progress()->SetText("Equalizing Cube " + toString((int) img + 1) +
          " of " + maxCubeStr);

      // Open input cube
      CubeAttributeInput att;
      const QString inp = m_imageList[img].toString();
      Cube *icube = p.SetInputCube(inp, att);

      // Allocate output cube
      QString out = outList[img].toString();
      CubeAttributeOutput outAtt;
      p.SetOutputCube(out, outAtt, icube->sampleCount(),
          icube->lineCount(), icube->bandCount());

      // Apply gain/offset to the image
      ApplyFunctor func(m_adjustments[img]);
      p.ProcessCube(func, false);
      p.EndProcess();
    }
  }


  PvlGroup Equalization::getResults() {
    // TODO combine summary info into getSummary method and use with write
    PvlGroup results("Results");
    results += PvlKeyword("TotalOverlaps", toString(m_validCnt + m_invalidCnt));
    results += PvlKeyword("ValidOverlaps", toString(m_validCnt));
    results += PvlKeyword("InvalidOverlaps", toString(m_invalidCnt));
    results += PvlKeyword("Weighted", (m_wtopt) ? "true" : "false");
    results += PvlKeyword("MinCount", toString(m_mincnt));

    // Name and band modifiers for each image
    for (int img = 0; img < m_imageList.size(); img++) {
      results += PvlKeyword("FileName", m_imageList[img].toString());

      // Band by band statistics
      for (int band = 1; band <= m_maxBand; band++) {
        QString mult = toString(m_adjustments[img]->getGain(band - 1));
        QString base = toString(m_adjustments[img]->getOffset(band - 1));
        QString avg = toString(m_adjustments[img]->getAverage(band - 1));
        QString bandNum = toString(band);
        QString bandStr = "Band" + bandNum;
        PvlKeyword bandStats(bandStr);
        bandStats += mult;
        bandStats += base;
        bandStats += avg;
        results += bandStats;
      }
    }

    return results;
  }


  void Equalization::write(QString outstatsFileName) {
    // Write the equalization and overlap statistics to the file
    m_results->Write(outstatsFileName);
  }


  double Equalization::evaluate(double dn,
      int imageIndex, int bandIndex) const {
    return m_adjustments[imageIndex]->evaluate(dn, bandIndex);
  }


  void Equalization::loadInputs(QString fromListName) {
    // Get the list of cubes to mosaic
    m_imageList.read(fromListName);
    m_maxCube = m_imageList.size();

    if (m_imageList.size() < 2) {
      QString msg = "The input file [" + fromListName +
        "] must contain at least 2 file names";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    Cube tempCube;
    tempCube.open(m_imageList[0].toString());
    m_maxBand = tempCube.bandCount();

    errorCheck(fromListName);
  }


  void Equalization::setInput(int index, QString value) {
    m_imageList[index] = value;
  }


  const FileList & Equalization::getInputs() const {
    return m_imageList;
  }


  void Equalization::fillOutList(FileList &outList, QString toListName) {
    if (toListName.isEmpty()) {
      generateOutputs(outList);
    }
    else {
      loadOutputs(outList, toListName);
    }
  }


  void Equalization::errorCheck(QString fromListName) {
    for (int i = 0; i < m_imageList.size(); i++) {
      Cube cube1;
      cube1.open(m_imageList[i].toString());

      for (int j = (i + 1); j < m_imageList.size(); j++) {
        Cube cube2;
        cube2.open(m_imageList[j].toString());

        // Make sure number of bands match
        if (m_maxBand != cube2.bandCount()) {
          QString msg = "Number of bands do not match between cubes [" +
            m_imageList[i].toString() + "] and [" + m_imageList[j].toString() + "]";
          throw IException(IException::User, msg, _FILEINFO_);
        }

        //Create projection from each cube
        Projection *proj1 = cube1.projection();
        Projection *proj2 = cube2.projection();

        // Test to make sure projection parameters match
        if (*proj1 != *proj2) {
          QString msg = "Mapping groups do not match between cubes [" +
            m_imageList[i].toString() + "] and [" + m_imageList[j].toString() + "]";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }
    }
  }


  void Equalization::generateOutputs(FileList &outList) {
    for (int img = 0; img < m_imageList.size(); img++) {
      FileName file(m_imageList[img]);
      QString filename = file.path() + "/" + file.baseName() +
        ".equ." + file.extension();
      outList.push_back(FileName(filename));
    }
  }


  void Equalization::loadOutputs(FileList &outList, QString toListName) {
    outList.read(FileName(toListName));

    // Make sure each file in the tolist matches a file in the fromlist
    if (outList.size() != m_imageList.size()) {
      QString msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding output file in the TO LIST.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure that all output files do not have the same names as their
    // corresponding input files
    for (int i = 0; i < outList.size(); i++) {
      if (outList[i].toString().compare(m_imageList[i].toString()) == 0) {
        QString msg = "The to list file [" + outList[i].toString() +
          "] has the same name as its corresponding from list file.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }


  void Equalization::loadHolds(OverlapNormalization *oNorm) {
    for (unsigned int h = 0; h < m_holdIndices.size(); h++)
      oNorm->AddHold(m_holdIndices[h]);
  }


  void Equalization::clearAdjustments() {
    m_adjustments.clear();
  }


  void Equalization::addAdjustment(ImageAdjustment *adjustment) {
    m_adjustments.push_back(adjustment);
  }


  void Equalization::addValid(int count) {
    m_validCnt += count;
  }


  void Equalization::addInvalid(int count) {
    m_invalidCnt += count;
  }


  void Equalization::init() {
    m_validCnt = 0;
    m_invalidCnt = 0;

    m_mincnt = 1000;
    m_wtopt = false;

    m_maxCube = 0;
    m_maxBand = 0;

    m_results = NULL;
  }


  vector<int> Equalization::validateInputStatistics(QString instatsFileName) {
    Pvl inStats(instatsFileName);
    PvlObject &equalInfo = inStats.FindObject("EqualizationInformation");

    // Make sure each file in the instats matches a file in the fromlist
    if (m_imageList.size() > equalInfo.Groups() - 1) {
      QString msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding input file in the INPUT STATISTICS.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    vector<int> normIndices;

    // Check that each file in the FROM LIST is present in the INPUT STATISTICS
    for (int i = 0; i < m_imageList.size(); i++) {
      QString fromFile = m_imageList[i].original();
      bool foundFile = false;
      for (int j = 1; j < equalInfo.Groups(); j++) {
        PvlGroup &normalization = equalInfo.Group(j);
        QString normFile  = normalization["FileName"][0];
        if (fromFile == normFile) {

          // Store the index in INPUT STATISTICS file corresponding to the
          // current FROM LIST file
          normIndices.push_back(j);
          foundFile = true;
        }
      }
      if (!foundFile) {
        QString msg = "The from list file [" + fromFile +
                          "] does not have any corresponding file in the stats list.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    return normIndices;
  }


  void Equalization::CalculateFunctor::operator()(Buffer &in) const {
    // Make sure we consider the last line
    if ((in.Line() - 1) % m_linc == 0 || in.Line() == in.LineDimension()) {
      addStats(in);
    }
  }


  void Equalization::CalculateFunctor::addStats(Buffer &in) const {
    // Add data to Statistics object by line
    m_stats->AddData(&in[0], in.size());
  }


  void Equalization::ApplyFunctor::operator()(Buffer &in, Buffer &out) const {
    int index = in.Band() - 1;
    for (int i = 0; i < in.size(); i++) {
      out[i] = (IsSpecial(in[i])) ?
        in[i] : m_adjustment->evaluate(in[i], index);
    }
  }


}
