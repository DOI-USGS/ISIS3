/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/05/09 18:49:25 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ApolloPanImage.h"

namespace Isis {


  /**
   * Construct an empty image.
   */
  ApolloPanImage::ApolloPanImage() {
    m_imageNumber = "";
  }


  /**
   * Construct an image.
   * 
   * @param imageNumber The four-digit image number.
   * @param lastTile The last tile in the image. Tiles 1-lastTile will be created. Defaul of 8.
   */
  ApolloPanImage::ApolloPanImage(QString imageNumber, int lastTile) {
    m_imageNumber = imageNumber;
    m_tiles.clear();
    for (int i = 1; i <= lastTile + 1; i++) {
      m_tiles.push_back(ApolloPanTile(imageNumber, i));
    }
  }


  /**
   * Destroy an image.
   */
  ApolloPanImage::~ApolloPanImage() {
  }


  /**
   * Detects the edges, timing marks, and fiducial marks of each tile in the images.
   */
  void ApolloPanImage::detectTiles() {
    for (int i = 0 ; i < 8; i++) {
      m_tiles[i].detectTile();
    }
  }


  /**
   * Detect the tiles using custom input files.
   * 
   * @param listFilename The filename of the list of images to use.
   * 
   * @throws IException::User "Can't open or invalid file list."
   */
  void ApolloPanImage::detectTiles(QString listFilename) {
    try {
      FileList imageList(listFilename);
      for (size_t i = 0; i < 8; i++) {
        m_tiles[i].detectTile(imageList[i]);
      }
    }
    catch (IException &e) {
      QString msg = "Can't open or invalid file list [" + listFilename + "].";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Decodes what value each timing mark represents.
   */
  void ApolloPanImage::decodeTimingMarks() {

    // Compute breaks
    vector<double> imageData;
    for (size_t i = 0; i < m_tiles.size(); i++) {
      vector<double> tileData = m_tiles[i].jenksData();
      imageData.insert(imageData.end(), tileData.begin(), tileData.end());
    }
    vector<double> jenksBreaks = computeJenksBreaks(imageData, 3);

    // classify the timing marks
    for (size_t i = 0; i < m_tiles.size(); i++) {
      m_tiles[i].classifyTimingMarks(jenksBreaks);
    }
  }


  /**
   * Compute the cutoffs for classifying timing marks using Jenk's natural breaks.
   * 
   * @param inData A vector containing the relative timing mark sizes.
   * @param classCount The number of classes to sort into.
   * 
   * @return @b vector<double> A vector containing the cutoffs stored as start, stop of
   *                           each class.  The final entry in the vector contains the
   *                           goodness of the classification.
   */
  vector<double> ApolloPanImage::computeJenksBreaks(vector<double> inData, size_t classCount) {
    
    vector<double> breaks (2*classCount+1, 0);
    size_t dataCount = inData.size();
    sort(inData.begin(),inData.end());
    
    double mat1 [dataCount + 1][classCount + 1];
    double mat2 [dataCount + 1][classCount + 1];
    
    for (size_t i = 0; i < dataCount + 1; i++) {
      for (size_t j = 0; j < classCount + 1; j++) {
        mat1[i][j] = 0;
        mat2[i][j] = 0;
      }
    }
    
    for (size_t i = 1; i <= classCount; i++) {
      mat1[0][i] = 1;
      mat2[0][i] = 0;
      for (size_t j = 2; j <= dataCount; j++) {
        mat2[j][i] = numeric_limits<double>::infinity();
      }
    }
    
    double ssd = 0;
    for (size_t rangeEnd = 2; rangeEnd <= dataCount; rangeEnd++) {
      double sumX = 0;
      double sumX2 = 0;
      double w = 0;
      for (size_t m = 1; m <= rangeEnd; m++) {
        int dataId = rangeEnd - m + 1;
        double val = inData[dataId - 1];
        sumX2 += val * val;
        sumX += val;
        w++;
        ssd = sumX2 - (sumX * sumX) / w;
        for (size_t j = 2; j <= classCount; j++) {
          if (!(mat2[rangeEnd][j] < (ssd + mat2[dataId - 1][j - 1]))) {
            mat1[rangeEnd][j] = dataId;
            mat2[rangeEnd][j] = ssd + mat2[dataId - 1][j - 1];
          }
        }
      }
      mat1[rangeEnd][1] = 1;
      mat2[rangeEnd][1] = ssd;
    }
    
    breaks[2*classCount - 1] = inData.back();
    
    int k = dataCount;
    for (size_t j = classCount; j >= 2; j--) {
      int id = (int)(mat1[k][j]) - 2;
      breaks[2*j - 2] = inData[id+1];
      breaks[2*j - 3] = inData[id];
      k = (int)mat1[k][j] - 1;
    }
    
    breaks[0] = inData.front();
    
    breaks.push_back((mat2[dataCount][1]-mat2[dataCount][classCount - 1])/mat2[dataCount][1]);
    
    return breaks;
  }


  /**
   * Write the image information out to Pvl files.
   * 
   * @param filePrefix The directory where the pvls will be written to
   *                   Files we be output as filePrefix + AS15-P-####_000#.pvl.
   */
  void ApolloPanImage::writeToPvl(QString filePrefix) {
    for (size_t i = 0; i < m_tiles.size(); i++) {
      QString filename = filePrefix + "/AS15-P-" + m_imageNumber
                         + "_000" + QString::number(i + 1) + ".pvl";
      Pvl tilePvl;
      tilePvl += m_tiles[i].toPvl();
      tilePvl.write(filename.toLatin1().data());
    }
  }


  /**
   * Read the tile information from a Pvl file.
   * 
   * @param filePrefix The file prefix "/archive/missions/apollo_pan/AS15/REVS###/REV##/AS15-P-####"
   * @param lastTile The last tile in the image. Defaults to 8.
   */
  void ApolloPanImage::readFromPvl(QString filePrefix, int lastTile) {
    m_imageNumber = filePrefix.right(4);
    m_tiles.clear();
    for (int i = 1; i <= lastTile; i++) {
      QString tileFile = filePrefix + "/AS15-P-" + m_imageNumber 
                         + "_000" + QString::number(i) +".pvl";
      ApolloPanTile tile(m_imageNumber, i);
      m_tiles.push_back(tile);
      m_tiles.back().fromPvl(tileFile);
    }
  }


  /**
   * Read the tile information from Pvl files in a file list.
   * 
   * @param inputList The file list containing Pvl files in order from tile 1-8.
   * @param lastTile The last tile in the image. Defaults to 8.
   */
  void ApolloPanImage::readFromPvl(FileList inputList, int lastTile) {
    m_tiles.clear();
    for (int i = 0; i < lastTile; i++) {
      ApolloPanTile tile(m_imageNumber, i);
      tile.fromPvl(inputList[i].original());
      m_tiles.push_back(tile);
    }
    m_imageNumber = m_tiles.back().imageNumber();
  }


  /**
   * Match the tiles together in order to construct data for the whole image.
   * 
   * @throws IException::Unknown "Failed matching tile"
   */
  void ApolloPanImage::matchTiles() {
    for (int i = m_tiles.size() - 2; i >= 0; i--) {
      vector<int> match = matchByFiducials(m_tiles[i+1], m_tiles[i], 20);
      if (checkMatch(m_tiles[i+1], m_tiles[i], match[0])) {
        // If good match
        if (m_tiles[i+1].timingMark(0).valid()) {
          m_tiles[i].setTimingOffset(m_tiles[i+1].timingOffset() + match[0]);
        }
        else {
          m_tiles[i].setTimingOffset(m_tiles[i+1].timingOffset() + match[0] -1);
        }
        m_tiles[i].setSampleOffset(match[1]);
      }
      else {
        match = matchByFiducials(m_tiles[i+1], m_tiles[i], 40);
        if (checkMatch(m_tiles[i+1], m_tiles[i], match[0])) {
          // If good match
          if (m_tiles[i+1].timingMark(0).valid()) {
            m_tiles[i].setTimingOffset(m_tiles[i+1].timingOffset() + match[0]);
          }
          else {
            m_tiles[i].setTimingOffset(m_tiles[i+1].timingOffset() + match[0] -1);
          }
          m_tiles[i].setSampleOffset(match[1]);
        }
        else {
          match =  matchByTiming(m_tiles[i+1], m_tiles[i]);
          if (checkMatch(m_tiles[i+1], m_tiles[i], match[0])) {
          if (m_tiles[i+1].timingMark(0).valid()) {
              m_tiles[i].setTimingOffset(m_tiles[i+1].timingOffset() + match[0]);
            }
            else {
              m_tiles[i].setTimingOffset(m_tiles[i+1].timingOffset() + match[0] -1);
            }
            m_tiles[i].setSampleOffset(match[1]);
          }
          else {
            QString msg = "Failed matching tile [" + QString::number(i+1) + "] to ["
                          + QString::number(i+2) + "].";
            throw IException(IException::Unknown, msg, _FILEINFO_);
          }
        }
      }
    }
  }


  /**
   * Match two tiles based on their fiducial marks.
   * 
   * @param tileA The tile being matched to.
   * @param tileB The tile being matched.
   * @param threshold The threshold for declaring a match.
   * 
   * @return @b vector<int> The timing mark offset followed by the sample offset.
   *                        Both will be 0 if no match is made.
   */
  vector<int> ApolloPanImage::matchByFiducials(ApolloPanTile tileA, ApolloPanTile tileB, int threshold) {
    // Output is markoffset, sampleoffset
    vector<int> results(2,0);
    int tileATimingSize = tileA.numberOfTimingMarks();
    int tileAFiducialSize = tileA.numberOfFiducialMarks();
    int tileBFiducialSize = tileB.numberOfFiducialMarks();

    // Walk the two images across each other until they match
    int startIndex = 0;
    if (!tileB.timingMark(0).valid()) {
      startIndex = 1;
    }
    for (int j = tileATimingSize/3; j < tileATimingSize; j++) {
      int sampleOffset = tileA.timingMark(j).leftSample() - tileB.timingMark(startIndex).leftSample();
      int maxDiff = 0;

      // Check for fiducial mark matches
      int fidIndex = 0;
      while ((tileA.fiducialMark(fidIndex).leftSample() < sampleOffset) &&
            (fidIndex < tileAFiducialSize - 1)) {
        fidIndex++;
      }
      for (int k = fidIndex; k < min(tileAFiducialSize, (int)fidIndex + tileBFiducialSize); k++) {
        if (tileA.fiducialMark(k).valid() && tileB.fiducialMark(k-fidIndex).valid()) {
          if (abs(tileA.fiducialMark(k).sample() - tileB.fiducialMark(k-fidIndex).sample() - sampleOffset ) > maxDiff) {
            maxDiff = abs(tileA.fiducialMark(k).sample() - tileB.fiducialMark(k-fidIndex).sample() - sampleOffset);
          }
        }
      }
      if (maxDiff < threshold) {
        results[0] = j;
        results[1] = sampleOffset;
        break;
      }
    }
    return results;
  }


  /**
   * Match two tiles based on their timing marks.
   * 
   * @param tileA The tile being matched to.
   * @param tileB The tile being matched.
   * 
   * @return @b vector<int> The timing mark offset followed by the sample offset.
   *                        Both will be 0 if no match is made.
   */
  vector<int> ApolloPanImage::matchByTiming(ApolloPanTile tileA, ApolloPanTile tileB) {
    // Output is markoffset, sampleoffset
    vector<int> results(2,0);
    
    // Find the timing mark closest to the typical offset of 28000
    int numberOfMarksA = tileA.numberOfTimingMarks();
    int offset = 0;
    while ((tileA.timingMark(offset).leftSample() < 28000) && (offset < numberOfMarksA - 1)) {
      offset++;
    }
    bool matched = checkMatch(tileA, tileB, offset);
    while (!matched && (offset < numberOfMarksA - 1)) {
      offset++;
      matched = checkMatch(tileA, tileB, offset);
    }
    results[0] = offset;
    if (tileB.timingMark(0).valid()) {
      results[1] = tileA.timingMark(offset).leftSample() - tileB.timingMark(0).leftSample();
    }
    else {
      results[1] = tileA.timingMark(offset).leftSample() - tileB.timingMark(1).leftSample();
    }
    return results;
  }


  /**
   * Check if a match was successful based on creating a valid time code.
   * 
   * @param tileA The tile being matched to.
   * @param tileB The tile being matched.
   * @param offset The timing mark offset of tileB
   * 
   * @return @b bool If the match was successful
   */
  bool ApolloPanImage::checkMatch(ApolloPanTile tileA, ApolloPanTile tileB, int offset) {
    bool goodMatch = false;
    
    // Check if the tiles where matched end to end
    if (!(offset < tileA.numberOfTimingMarks() + 1)) {
      return goodMatch;
    }
    
    if (offset < 5) {
      return goodMatch;
    }
    
    // Make code segment
    vector<int> codeSegment;
    vector<int> tileACode = tileA.codeSegment();
    vector<int> tileBCode = tileB.codeSegment();;

    int startIndex = 1;
    if (!tileB.timingMark(0).valid()) {
      startIndex = 2;
    }
    codeSegment.insert(codeSegment.begin(),tileACode.begin()+1,tileACode.begin()+offset + 1);
    codeSegment.insert(codeSegment.end(),tileBCode.begin()+startIndex,tileBCode.end()-1);
    
    // Check the code segment
    int index = offset - 1;
    // Iterate backwards to the start of a word
    while (!((codeSegment[index] == 2) && (codeSegment[index + 1] != 2)) && 
          (index >= 0)) {
      index--;
    }
    // Check the overlapping words
    int readIndex = index + 1;
    while (!((codeSegment[readIndex] == 2) && (codeSegment[readIndex + 1] != 2)) &&
          (readIndex < (int)codeSegment.size() - 1)) {
      readIndex++;
    }
    if ( (readIndex - index != 10) &&
        (readIndex < (int)codeSegment.size() - 1) ) {
      return goodMatch;
    }
    index = readIndex;
    readIndex++;
    while (!((codeSegment[readIndex] == 2) && (codeSegment[readIndex + 1] != 2)) &&
          (readIndex < (int)codeSegment.size() - 1)) {
      readIndex++;
    }
    if ( (readIndex - index != 10) &&
        (readIndex < (int)codeSegment.size() - 1) ) {
      return goodMatch;
    }
    goodMatch = true;
    return goodMatch;
  }


  /**
   * Number the timing marks on each tiles.
   */
  void ApolloPanImage::numberTimingMarks() {
    for (int i = 0; i < (int)m_tiles.size(); i++) {
      m_tiles[i].numberTimingMarks();
    }
  }


  /**
   * Number the fiducial marks on each tile.
   * 
   * @param firstFiducialIndex The index of the first fiducial mark in the image. Defaults to 0.
   */
  void ApolloPanImage::numberFiducialMarks(int firstFiducialIndex) {
    m_tiles.back().numberFiducialMarks(firstFiducialIndex);
    for (int i = m_tiles.size() - 2; i >= 0; i--) {
      int startNumber = 0;
      int fidIndex = 0;
      while ( (m_tiles[i+1].fiducialMark(fidIndex).sample() < m_tiles[i].sampleOffset()) &&
              (fidIndex < m_tiles[i+1].numberOfFiducialMarks() - 1) ){
        fidIndex++;
      }
      startNumber = m_tiles[i+1].fiducialMark(fidIndex).number();
      m_tiles[i].numberFiducialMarks(startNumber);
    }
  }


  /**
   * Compute the affine transformation for each tile.
   * 
   * @param csvFilename The filename of the csv containing the
   *                    calibrated fiducial mark coordinates.
   */
  void ApolloPanImage::computeAffines(QString csvFilename) {
    for (int i = 0; i < (int)m_tiles.size(); i++) {
      m_tiles[i].loadCalibratedFiducials(csvFilename);
      m_tiles[i].computeAffine();
    }
  }


  /**
   * Flag fiducial marks that have residuals beyond a threshold.
   * 
   * @param threshold The threshold for flagging.
   */
  void ApolloPanImage::flagOutliers(double threshold) {
    for (int i = 0; i < (int)m_tiles.size(); i++) {
      m_tiles[i].computeResiduals();
      m_tiles[i].flagOutliers(threshold);
    }
  }


  /**
   * Check for any errors in the time code.
   */
  void ApolloPanImage::checkTimeCode() {
    vector<int> code = createTimeCode();

    /// Find the start of a word
    int index = 0;
    while (code[index] != 2 && index < (int)code.size()) {
      index++;
    }
    while (index + 11 < (int)code.size()) {
      // iterate to the start of the next word
      int readIndex = index + 1;
      while((code[readIndex] != 2) && (readIndex < (int)code.size() - 1)) {
        readIndex++;
      }
      if ((readIndex - index != 10) &&
	   (readIndex != (int)code.size() - 2) &&
          !((readIndex - index == 9) && (code[readIndex+1] == 2)) &&
          !((readIndex - index == 1) && (code[index] == 2))) {
        cout << "Error in timing code between numbers: [" << index << "] and [" << readIndex << "]." << endl;
      }
      index = readIndex;
    }
  }


  /**
   * Read the time code and add timing information to each timing mark
   */
  void ApolloPanImage::readTimeCode() {
    vector<int> code = createTimeCode();
    reverse(code.begin(), code.end());
    int secondStart = 0;

    // Find the start of a second
    while ( (code[secondStart] != 2 || code[secondStart + 1] != 2) &&
            secondStart < (int)code.size()-38 ) {
      secondStart++;
    }
    int flippedSecondStart = code.size() - secondStart - 2;

    iTime launchTime("1971/7/26 13:34:00.795");
    double firstSecond = launchTime.EtString().toDouble();
    // Seconds
    firstSecond += code[secondStart + 2] +
                   code[secondStart + 3] * 2 +
                   code[secondStart + 4] * 4 +
                   code[secondStart + 5] * 8 +
                   code[secondStart + 7] * 10 +
                   code[secondStart + 8] * 20 +
                   code[secondStart + 9] * 40;
    // Minutes
    firstSecond += (code[secondStart + 11] +
                    code[secondStart + 12] * 2 +
                    code[secondStart + 13] * 4 +
                    code[secondStart + 14] * 8 +
                    code[secondStart + 16] * 10 +
                    code[secondStart + 17] * 20 +
                    code[secondStart + 18] * 40) * 60;
    // Hours
    firstSecond += (code[secondStart + 21] +
                    code[secondStart + 22] * 2 +
                    code[secondStart + 23] * 4 +
                    code[secondStart + 24] * 8 +
                    code[secondStart + 26] * 10 +
                    code[secondStart + 27] * 20) * 3600;
    // Days
    firstSecond += (code[secondStart + 31] +
                    code[secondStart + 32] * 2 +
                    code[secondStart + 33] * 4 +
                    code[secondStart + 34] * 8 +
                    code[secondStart + 36] * 10) * 86400;

    // Add the time data for interior timing marks
    for (int i = 0; i < (int)m_tiles.size(); i++) {
      m_tiles[i].computeTiming(flippedSecondStart, firstSecond);
    }
  }


  /**
   * Add the information to exterior timing marks on each tile.
   */
  void ApolloPanImage::fillExteriorTimingMarks() {
    for (int i = 0; i < (int)m_tiles.size(); i++) {

      // The first timing mark
      TimingMark &firstMark = m_tiles[i].timingMark(0);
      if (i != (int)m_tiles.size() - 1) {
        firstMark.setValue(m_tiles[i+1].timingMarkByNumber(firstMark.number()).value());
        firstMark.setTime(m_tiles[i+1].timingMarkByNumber(firstMark.number()).time());
        firstMark.setExposureTime(m_tiles[i+1].timingMarkByNumber(firstMark.number()).exposureTime());
      }
      else {
        firstMark.setTime(m_tiles[i].timingMark(1).time() + .01);
        firstMark.setExposureTime(m_tiles[i].timingMark(1).exposureTime());
      }
      
      // The last timing mark
      TimingMark &lastMark = m_tiles[i].timingMark(m_tiles[i].numberOfTimingMarks() - 1);
      if (i != 0) {
        lastMark.setValue(m_tiles[i-1].timingMarkByNumber(lastMark.number()).value());
        lastMark.setTime(m_tiles[i-1].timingMarkByNumber(lastMark.number()).time());
        lastMark.setExposureTime(m_tiles[i-1].timingMarkByNumber(lastMark.number()).exposureTime());
      }
      else {
        TimingMark &secondLastMark = m_tiles[i].timingMark(m_tiles[i].numberOfTimingMarks() - 2);
        lastMark.setTime(secondLastMark.time() - 1);
        lastMark.setExposureTime(secondLastMark.exposureTime());
      }
    }
  }


  /**
   * Calculate the stop and start timing information for each tile.
   */
  void ApolloPanImage::computeStartStop() {
    for (int i = 0; i < (int)m_tiles.size(); i++) {
      m_tiles[i].computeStartStop();
    }
  }


  /**
   * Stitch together the time code from each tile.
   * 
   * @return @b vector<int> The complte time code for the image.
   */
  vector<int> ApolloPanImage::createTimeCode() {
    vector<int> imageCode;
    // The first mark will always be unknown, -1, because there is no
    // previous mark to use for determining relative size.
    imageCode.push_back(-1);
    for (int i = m_tiles.size() - 1; i >= 0; i--) {
      int index = 1;
      if (!m_tiles[i].timingMark(0).valid()) {
        index = 2;
      }
      if (i != 0) {
        
        // For tiles 8-2 we want to stop before the next tile starts
        while (m_tiles[i].timingMark(index).number() < m_tiles[i-1].timingOffset()+1 &&
               index < m_tiles[i].numberOfTimingMarks()) {
          imageCode.push_back(m_tiles[i].timingMark(index).value());
          index++;
        }
      }
      else {
        
        // For tile 1 we want to add everything
        for (int j = index; j < m_tiles[i].numberOfTimingMarks(); j++) {
          imageCode.push_back(m_tiles[i].timingMark(j).value());
        }
      }
    }
    return imageCode;
  }
};
