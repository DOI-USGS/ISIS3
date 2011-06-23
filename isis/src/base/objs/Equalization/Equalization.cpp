#include "Equalization.h"

#include <iomanip>

#include "Buffer.h"
#include "Cube.h"
#include "iException.h"
#include "LineManager.h"
#include "OverlapNormalization.h"
#include "OverlapStatistics.h"
#include "Process.h"
#include "ProcessByLine.h"
#include "Projection.h"

using std::string;
using std::vector;


namespace Isis {


  Equalization::Equalization(string fromListName) {
    m_validCnt = 0;
    m_invalidCnt = 0;

    m_mincnt = 0;
    m_wtopt = false;

    m_currentImage = 0;
    m_maxCube = 0;
    m_maxBand = 0;

    // Get the list of cubes to mosaic
    m_imageList.Read(fromListName);
    if (m_imageList.size() < 1) {
      string msg = "The list file [" + fromListName +
        "] does not contain any data";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Make sure number of bands and projection parameters match for all cubes
    for (unsigned int i = 0; i < m_imageList.size(); i++) {
      Cube cube1;
      cube1.open(m_imageList[i]);
      m_maxBand = cube1.getBandCount();

      for (unsigned int j = (i + 1); j < m_imageList.size(); j++) {
        Cube cube2;
        cube2.open(m_imageList[j]);

        // Make sure number of bands match
        if (m_maxBand != cube2.getBandCount()) {
          string msg = "Number of bands do not match between cubes [" +
            m_imageList[i] + "] and [" + m_imageList[j] + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }

        //Create projection from each cube
        Projection *proj1 = cube1.getProjection();
        Projection *proj2 = cube2.getProjection();

        // Test to make sure projection parameters match
        if (*proj1 != *proj2) {
          string msg = "Mapping groups do not match between cubes [" +
            m_imageList[i] + "] and [" + m_imageList[j] + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }
      }
    }
  }


  Equalization::~Equalization() {
    adjustments.clear();
    hold.clear();
  }


  void Equalization::addHolds(string holdListName) {
    FileList holdList;
    holdList.Read(holdListName);

    // Make sure each file in the holdlist matches a file in the fromlist
    for (int i = 0; i < (int) holdList.size(); i++) {
      bool matched = false;
      for (int j = 0; j < (int) m_imageList.size(); j++) {
        if (holdList[i] == m_imageList[j]) {
          matched = true;
          hold.push_back(j);
          break;
        }
      }
      if (!matched) {
        string msg = "The hold list file [" + holdList[i] +
                          "] does not match a file in the from list";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }


  vector<OverlapStatistics> Equalization::calculateStatistics(
      double sampPercent, int mincnt, bool wtopt, int sType) {

    adjustments.clear();

    m_mincnt = mincnt;
    m_wtopt = wtopt;

    // Loop through all the input cubes, calculating statistics for each cube to use later
    iString maxCubeStr((int) m_imageList.size());
    vector<OverlapNormalization *> oNormList;
    for (int band = 1; band <= m_maxBand; band++) {
      vector<Statistics> statsList;
      for (int img = 0; img < (int) m_imageList.size(); img++) {
        Process p;
        const CubeAttributeInput att;
        const string inp = m_imageList[img];
        Cube *icube = p.SetInputCube(inp, att);

        // Add a Statistics object to the list for every band of every input cube
        m_currentImage = img;
        Statistics stats = getBandStatistics(*icube, band, sampPercent, maxCubeStr);
        statsList.push_back(stats);
        p.EndProcess();
      }

      // Create a separate OverlapNormalization object for every band
      OverlapNormalization *oNorm = new OverlapNormalization(statsList);
      for (int h = 0; h < (int)hold.size(); h++) oNorm->AddHold(hold[h]);
      oNormList.push_back(oNorm);
    }

    // A list for keeping track of which input cubes are known to overlap another
    vector<bool> doesOverlapList;
    for (unsigned int i = 0; i < m_imageList.size(); i++) doesOverlapList.push_back(false);

    // Find overlapping areas and add them to the set of known overlaps for each
    // band shared amongst cubes
    vector<OverlapStatistics> overlapList;
    for (unsigned int i = 0; i < m_imageList.size(); i++) {
      adjustments.push_back(new ImageAdjustment());

      Cube cube1;
      cube1.open(m_imageList[i]);

      for (unsigned int j = (i + 1); j < m_imageList.size(); j++) {
        Cube cube2;
        cube2.open(m_imageList[j]);
        iString cubeStr1((int)(i + 1));
        iString cubeStr2((int)(j + 1));
        string statMsg = "Gathering Overlap Statisitcs for Cube " +
                         cubeStr1 + " vs " + cubeStr2 + " of " + maxCubeStr;

        // Get overlap statistics for cubes
        OverlapStatistics oStats(cube1, cube2, statMsg, sampPercent);

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

            // Make sure overlap has at least MINCOUNT pixels and add
            if (oStats.GetMStats(band).ValidPixels() >= mincnt) {
              oNormList[band-1]->AddOverlap(
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
      string badFiles = "";
      for (unsigned int img = 0; img < m_imageList.size(); img++) {
        // Print the name of each input cube without an overlap
        if (!doesOverlapList[img]) {
          badFiles += "[" + m_imageList[img] + "] ";
        }
      }
      if (badFiles != "") {
        string msg = "File(s) " + badFiles;
        msg += " do(es) not overlap any other input images with enough valid pixels";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }

    // Loop through each band making all necessary calculations
    for (int band = 0; band < m_maxBand; band++) {
      oNormList[band]->Solve((OverlapNormalization::SolutionType) sType);

      for (unsigned int img = 0; img < adjustments.size(); img++) {
        adjustments[img]->addGain(oNormList[band]->Gain(img));
        adjustments[img]->addOffset(oNormList[band]->Offset(img));
        adjustments[img]->addAverage(oNormList[band]->Average(img));
      }
    }

    // Compute the number valid and invalid overlaps
    for (unsigned int o = 0; o < overlapList.size(); o++) {
      for (int band = 1; band <= m_maxBand; band++) {
        if (overlapList[o].IsValid(band)) m_validCnt++;
        else m_invalidCnt++;
      }
    }

    return overlapList;
  }


  void Equalization::importStatistics(string instatsFileName) {
    // Check for errors with the input statistics
    vector<int> normIndices = validateInputStatistics(instatsFileName);

    adjustments.clear();
    for (int img = 0; img < (int) m_imageList.size(); img++) {
      // Apply correction based on pre-determined statistics information
      Pvl inStats(instatsFileName);
      PvlObject &equalInfo = inStats.FindObject("EqualizationInformation");
      PvlGroup &normalization = equalInfo.Group(normIndices[img]);

      // TODO should we also get the valid and invalid count?

      ImageAdjustment *adjustment = new ImageAdjustment();

      // Get and store the modifiers for each band
      for (int band = 1; band < normalization.Keywords(); band++) {
        adjustment->addGain(normalization[band][0]);
        adjustment->addOffset(normalization[band][1]);
        adjustment->addAverage(normalization[band][2]);
      }

      adjustments.push_back(adjustment);
    }
  }


  void Equalization::applyCorrection(string toListName="") {
    FileList outList;
    if (!toListName.empty()) {
      loadOutputs(outList, toListName);
    }

    iString maxCubeStr((int) m_imageList.size());
    for (int img = 0; img < (int) m_imageList.size(); img++) {
      // Set up for progress bar
      ProcessByLine p;
      iString curCubeStr(img + 1);
      p.Progress()->SetText("Equalizing Cube " + curCubeStr + " of " + maxCubeStr);

      // Open input cube
      CubeAttributeInput att;
      const string inp = m_imageList[img];
      Cube *icube = p.SetInputCube(inp, att);

      // Establish the output file depending upon whether or not a to list
      // was entered
      // TODO move this out of the loop, don't use file list
      string out;
      if (outList.size() > 0) {
        out = outList[img];
      }
      else {
        Filename file = m_imageList[img];
        out = file.Path() + "/" + file.Basename() + ".equ." + file.Extension();
      }

      // Allocate output cube
      CubeAttributeOutput outAtt;
      p.SetOutputCube(out, outAtt, icube->getSampleCount(),
          icube->getLineCount(), icube->getBandCount());

      // Apply gain/offset to the image
      m_currentImage = img;
      EqualizationFunctor func(adjustments[m_currentImage]);
      p.StartProcessIO(func);
      p.EndProcess();
    }
  }


  PvlGroup Equalization::getResults() {
    // TODO combine summary info into getSummary method and use with write
    PvlGroup results("Results");
    results += PvlKeyword("TotalOverlaps", m_validCnt + m_invalidCnt);
    results += PvlKeyword("ValidOverlaps", m_validCnt);
    results += PvlKeyword("InvalidOverlaps", m_invalidCnt);
    string weightStr = "false";
    if (m_wtopt) weightStr = "true";
    results += PvlKeyword("Weighted", weightStr);
    results += PvlKeyword("MinCount", m_mincnt);

    // Name and band modifiers for each image
    for (unsigned int img = 0; img < m_imageList.size(); img++) {
      results += PvlKeyword("FileName", m_imageList[img]);

      // Band by band statistics
      for (int band = 1; band <= m_maxBand; band++) {
        iString mult(adjustments[img]->getGain(band - 1));
        iString base(adjustments[img]->getOffset(band - 1));
        iString avg(adjustments[img]->getAverage(band - 1));
        iString bandNum(band);
        string bandStr = "Band" + bandNum;
        PvlKeyword bandStats(bandStr);
        bandStats += mult;
        bandStats += base;
        bandStats += avg;
        results += bandStats;
      }
    }

    return results;
  }


  void Equalization::write(string outstatsFileName,
      vector<OverlapStatistics> *overlapStats) {

    PvlObject equ("EqualizationInformation");
    PvlGroup gen("General");
    gen += PvlKeyword("TotalOverlaps", m_validCnt + m_invalidCnt);
    gen += PvlKeyword("ValidOverlaps", m_validCnt);
    gen += PvlKeyword("InvalidOverlaps", m_invalidCnt);
    string weightStr = "false";
    if (m_wtopt) weightStr = "true";
    gen += PvlKeyword("Weighted", weightStr);
    gen += PvlKeyword("MinCount", m_mincnt);
    equ.AddGroup(gen);
    for (unsigned int img = 0; img < m_imageList.size(); img++) {
      // Format and name information
      PvlGroup norm("Normalization");
      norm.AddComment("Formula: newDN = (oldDN - AVERAGE) * GAIN + AVERAGE + OFFSET");
      norm.AddComment("BandN = (GAIN, OFFSET, AVERAGE)");
      norm += PvlKeyword("FileName", m_imageList[img]);

      // Band by band statistics
      for (int band = 1; band <= m_maxBand; band++) {
        iString mult(adjustments[img]->getGain(band - 1));
        iString base(adjustments[img]->getOffset(band - 1));
        iString avg(adjustments[img]->getAverage(band - 1));
        iString bandNum(band);
        string bandStr = "Band" + bandNum;
        PvlKeyword bandStats(bandStr);
        bandStats += mult;
        bandStats += base;
        bandStats += avg;
        norm += bandStats;
      }
      equ.AddGroup(norm);
    }

    // Write the equalization and overlap statistics to the file
    string out = Filename(outstatsFileName).Expanded();
    ofstream os;
    os.open(out.c_str(), ios::app);
    Pvl p;
    p.SetTerminator("");
    p.AddObject(equ);
    os << p << endl;

    if (overlapStats != NULL) {
      for (unsigned int i = 0; i < overlapStats->size(); i++) {
        os << (*overlapStats)[i];
        if (i != overlapStats->size() - 1) os << endl;
      }
    }

    os << "End";
  }


  double Equalization::evaluate(double dn,
      int imageIndex, int bandIndex) const {
    return adjustments[imageIndex]->evaluate(dn, bandIndex);
  }


  void Equalization::loadOutputs(FileList &outList, string toListName) {
    outList.Read(toListName);

    // Make sure each file in the tolist matches a file in the fromlist
    if (outList.size() != m_imageList.size()) {
      string msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding output file in the TO LIST.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Make sure that all output files do not have the same names as their
    // corresponding input files
    for (unsigned i = 0; i < outList.size(); i++) {
      if (outList[i].compare(m_imageList[i]) == 0) {
        string msg = "The to list file [" + outList[i] +
                          "] has the same name as its corresponding from list file.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }


  vector<int> Equalization::validateInputStatistics(
      string instatsFileName) {

    Pvl inStats(instatsFileName);
    PvlObject &equalInfo = inStats.FindObject("EqualizationInformation");

    // Make sure each file in the instats matches a file in the fromlist
    if (m_imageList.size() > (unsigned)equalInfo.Groups() - 1) {
      string msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding input file in the INPUT STATISTICS.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    vector<int> normIndices;

    // Check that each file in the FROM LIST is present in the INPUT STATISTICS
    for (unsigned i = 0; i < m_imageList.size(); i++) {
      string fromFile = m_imageList[i];
      bool foundFile = false;
      for (int j = 1; j < equalInfo.Groups(); j++) {
        PvlGroup &normalization = equalInfo.Group(j);
        string normFile  = normalization["Filename"][0];
        if (fromFile == normFile) {

          // Store the index in INPUT STATISTICS file corresponding to the
          // current FROM LIST file
          normIndices.push_back(j);
          foundFile = true;
        }
      }
      if (!foundFile) {
        string msg = "The from list file [" + fromFile +
                          "] does not have any corresponding file in the stats list.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }

    return normIndices;
  }


  // Gather general statistics on a particular band of a cube
  Statistics Equalization::getBandStatistics(Cube &icube, const int band,
      double sampPercent, string maxCubeStr) {
    // Create our progress message
    iString curCubeStr(m_currentImage + 1);
    string statMsg = "";
    if (icube.getBandCount() == 1) {
      statMsg = "Calculating Statistics for Band 1 in Cube " + curCubeStr +
        " of " + maxCubeStr;
    }
    else {
      iString curBandStr(band);
      iString maxBandStr(icube.getBandCount());
      statMsg = "Calculating Statistics for Band " + curBandStr + " of " +
        maxBandStr + " in Cube " + curCubeStr + " of " + maxCubeStr;
    }

    // Calculate our line incrementer
    int linc = (int) (100.0 / sampPercent + 0.5);

    // Make sure band is valid
    if ((band <= 0) || (band > icube.getBandCount())) {
      string msg = "Invalid band in method [getBandStatistics]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Construct a line buffer manager and a statistics object
    LineManager line(icube);


    Progress progress;
    progress.SetText(statMsg);

    // Calculate the number of steps for the Progress object, and add an extra
    // step if the total lines and incrementer do not divide evenly
    int maxSteps = icube.getLineCount() / linc;
    if (icube.getLineCount() % linc != 0) maxSteps += 1;
    progress.SetMaximumSteps(maxSteps);
    progress.CheckStatus();

    // Add data to Statistics object by line
    Statistics stats;
    int i = 1;
    while (i <= icube.getLineCount()) {
      line.SetLine(i, band);
      icube.read(line);
      stats.AddData(line.DoubleBuffer(), line.size());

      // Make sure we consider the last line
      if (i + linc > icube.getLineCount() && i != icube.getLineCount()) {
        i = icube.getLineCount();
        progress.AddSteps(1);
      }
      else i += linc; // Increment the current line by our incrementer

      progress.CheckStatus();
    }

    return stats;
  }


  void Equalization::EqualizationFunctor::operator()(Buffer &in, Buffer &out) {
    int index = in.Band() - 1;
    for (int i = 0; i < in.size(); i++) {
      out[i] = (IsSpecial(in[i])) ?
        in[i] : m_adjustment->evaluate(in[i], index);
    }
  }


}
