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

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"


#include "ApolloPanTile.h"

using namespace cv;

namespace Isis {

  /**
   * Default constructor
   */
  ApolloPanTile::ApolloPanTile() {
    m_imageNumber = "";
    m_tileNumber = -1;
    m_timingMarks.clear();
    m_fiducialMarks.clear();
    m_transX.clear();
    m_transY.clear();
    m_rows = 0;
    m_columns = 0;
    m_topTrim = 0;
    m_bottomTrim = 0;
    m_leftTrim = 0;
    m_rightTrim = 0;
    m_timingOffset = 0;
    m_sampleOffset = 0;
    m_startTime = 0;
    m_stopTime = 0;
  }


  /**
   * Construct an empty tile.
   * 
   * @param imageNumber The four-digit image number.
   * @param tileNumber The tile number, 1-8 numbered left to right.
   */
  ApolloPanTile::ApolloPanTile(QString imageNumber, int tileNumber) {
    m_imageNumber = imageNumber;
    m_tileNumber = tileNumber;
    m_timingMarks.clear();
    m_fiducialMarks.clear();
    m_transX.clear();
    m_transY.clear();
    m_rows = 0;
    m_columns = 0;
    m_topTrim = 0;
    m_bottomTrim = 0;
    m_leftTrim = 0;
    m_rightTrim = 0;
    m_timingOffset = 0;
    m_sampleOffset = 0;
    m_startTime = 0;
    m_stopTime = 0;
  }


  /**
   * Destroy a tile.
   */
  ApolloPanTile::~ApolloPanTile() {
  }


  /**
   * Detect the edges of the image, timing marks, and fiducial marks.
   */
  void ApolloPanTile::detectTile(){
    QString filename = "/work/projects/as15pan/AS15-P-" + m_imageNumber +
                       "/AS15-P-" + m_imageNumber + "_000" + QString::number(m_tileNumber) + ".tif";
    Mat tileData = imread(filename.toLatin1().data(), 1);
    m_rows = tileData.rows;
    m_columns = tileData.cols;
    detectTimingMarks(tileData);
    detectFiducialMarks(tileData);
    detectEdges(tileData);
  }


  /**
   * Detect the tile using a custom input file.
   * 
   * @param imageFileName The filename of the tif file to detect.
   */
  void ApolloPanTile::detectTile(FileName imageFileName) {
    Mat tileData = imread(imageFileName.toString().toLatin1().data(), 1);
    if (tileData.empty()) {
      QString msg = "Could not read image file [" + imageFileName.toString() + "] for tile ["
                     + QString::number(m_tileNumber) + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    m_rows = tileData.rows;
    m_columns = tileData.cols;
    detectTimingMarks(tileData);
    detectFiducialMarks(tileData);
    detectEdges(tileData);
  }


  /**
   * @brief Detect the timing marks.
   * 
   * Detect the timing marks by cropping the section of the tile that contains them.  Then,
   * convert to gray, erode, and apply a threshold.  Finally, use canny edge detection to
   * extract the timing marks' contours.
   * 
   * @param tileData A reference to the tileData matrix.
   * 
   * @see detectTile
   */
  void ApolloPanTile::detectTimingMarks(Mat &tileData) {

    // Crop and convert to gray
    Rect timingROI;
    if (m_tileNumber == 1) {
      // Remove the gauge area on tile 1
      timingROI = Rect(0, 24900, m_columns-5000, 300);
    }
    else {
      timingROI = Rect(0, 24900, m_columns, 300);
    }
    Mat timingData = tileData(timingROI);
    Mat timingGray;
    cvtColor(timingData, timingGray, COLOR_BGR2GRAY);

    // Erode the tile to remove noise
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(11,11));
    erode(timingGray, timingGray, element);

    // Threshold the tile
    threshold(timingGray, timingGray, 180, 255, THRESH_BINARY);

    // Detect with canny
    Mat cannyOutput;
    vector< vector<Point> > rawContours;
    vector<Vec4i> hierarchy;
    Canny(timingGray, cannyOutput, 90, 180, 3, true);
    element = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
    dilate(cannyOutput, cannyOutput, element);
    findContours(cannyOutput, rawContours, hierarchy,
                 RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Find the center of mass of each contour
    vector<Moments> timingMoments(rawContours.size());
    vector<Point2f> timingMassCenters(rawContours.size());
    for( size_t i = 0; i < rawContours.size(); i++ ) {
      timingMoments[i] = moments(rawContours[i], false);
      timingMassCenters[i] = Point2f(static_cast<float>(timingMoments[i].m10/timingMoments[i].m00),
                                     static_cast<float>(timingMoments[i].m01/timingMoments[i].m00));
    }

    // Find the bounding rectangle for each contour
    vector<vector<Point> > contoursPoly(rawContours.size());
    vector<Rect> boundingRectangles(rawContours.size());
    for( size_t i = 0; i < rawContours.size(); i++ ) {
      approxPolyDP(Mat(rawContours[i]), contoursPoly[i], 3, true);
      boundingRectangles[i] = boundingRect(Mat(contoursPoly[i]));
    }

    // Store contours in containers
    Statistics lineStats;
    for (size_t i = 0; i < rawContours.size(); i++) {
      TimingMark mark(timingMassCenters[i], boundingRectangles[i]);
      if ((mark.leftSample() < 20) ||
          (m_columns - mark.rightSample() < 20)) {
        mark.setValid(false);
      }
      if ((fabs(mark.height() - 75) < 50) &&
          (mark.length() > 40) ) {
        m_timingMarks.push_back(mark);
      lineStats.AddData(mark.line());
      }
      else {
        m_rejectedTimingMarks.push_back(mark);
      }
    }

    // Do a second pass on the timing marks
    for (size_t i = 0; i < m_timingMarks.size(); i++) {
      if (fabs(lineStats.Average() - m_timingMarks[i].line()) > 40) {
        m_rejectedTimingMarks.push_back(m_timingMarks[i]);
        m_timingMarks.erase(m_timingMarks.begin() + i);
        i--;
      }
    }
    
    // Sort the timing marks
    sort(m_timingMarks.begin(), m_timingMarks.end());

    // Check for anything that was missed
    checkMissingTimingMarks();
  }


  /**
   * @brief Detect the fiducial marks.
   * 
   * Detect the fiducial marks by cropping the section of the tile that contains them.  Then,
   * convert to gray, erode, and apply a threshold.  Finally, use canny edge detection to
   * extract the fiducial marks' contours.
   * 
   * @param tileData A reference to the tileData matrix.
   * 
   * @see detectTile
   */
  void ApolloPanTile::detectFiducialMarks(Mat &tileData) {

    // Crop
    Rect topROI;
    Rect bottomROI;
    if (m_tileNumber == 1) {
      // Remove the gauge area on tile 1
      topROI = Rect(0, 1000, m_columns-5000, 200);
      bottomROI = Rect(0, 24500, m_columns-5000, 200);
    }
    else {
      topROI = Rect(0, 1000, m_columns, 200);
      bottomROI = Rect(0, 24500, m_columns, 200);
    }
    Mat topData = tileData(topROI);
    Mat bottomData = tileData(bottomROI);
    Mat fiducialData;

    // Mask out the numbers on tile 4
    if (m_tileNumber == 5) {
      Mat numberRegion = topData(Rect(21000,0,5000,200));
      numberRegion.setTo(Scalar(0, 0, 0));
    }

    // Combine both fiducial sections
    vconcat(topData, bottomData, fiducialData);

    // Convert to gray
    Mat fiducialGray;
    cvtColor(fiducialData, fiducialGray, COLOR_BGR2GRAY);

    // Erode the tile to remove noise
    Mat element = getStructuringElement( MORPH_ELLIPSE, Size(11,11));
    erode(fiducialGray, fiducialGray, element);

    // Threshold the tile
    threshold(fiducialGray, fiducialGray, 180, 255, THRESH_BINARY);

    // Detect with canny
    Mat cannyOutput;
    vector< vector<Point> > rawContours;
    vector<Vec4i> hierarchy;
    Canny(fiducialGray, cannyOutput, 90, 180, 3, true);
    element = getStructuringElement( MORPH_ELLIPSE, Size(3,3));
    dilate(cannyOutput, cannyOutput, element);
    findContours(cannyOutput, rawContours, hierarchy,
                 RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Find the center of mass of each contour
    vector<Moments> fiducialMoments(rawContours.size());
    vector<Point2f> fiducialMassCenters(rawContours.size());
    for( size_t i = 0; i < rawContours.size(); i++ ) {
      fiducialMoments[i] = moments(rawContours[i], false);
      fiducialMassCenters[i] = Point2f(static_cast<float>(fiducialMoments[i].m10/fiducialMoments[i].m00),
                                       static_cast<float>(fiducialMoments[i].m01/fiducialMoments[i].m00));
    }

    // Find the bounding rectangle for each contour
    vector<vector<Point> > contoursPoly( rawContours.size() );
    vector<Rect> boundingRectangles( rawContours.size() );
    for( size_t i = 0; i < rawContours.size(); i++ ) {
      approxPolyDP(Mat(rawContours[i]), contoursPoly[i], 3, true);
      boundingRectangles[i] = boundingRect(Mat(contoursPoly[i]));
    }

    // Store contours in containers
    for (size_t i = 0; i < rawContours.size(); i++) {
      FiducialMark mark(fiducialMassCenters[i], boundingRectangles[i]);
      if ((mark.leftSample() < 20) ||
          (m_columns - mark.rightSample() < 20)) {
        mark.setValid(false);
      }
      if ((fabs(mark.length() - 115) < 85) &&
          (fabs(mark.height() - 75) < 50) &&
          (mark.topLine() > 1020)) {
        m_fiducialMarks.push_back(mark);
      }
      else {
        m_rejectedFiducialMarks.push_back(mark);
      }
    }

    // Sort the fiducial marks
    sort(m_fiducialMarks.begin(), m_fiducialMarks.end());

    // Check for anything that was missed
    checkMissingFiducialMarks();
  }


  /**
   * Detect the top and bottom edges of the tile.
   * 
   * @param tileData A reference to the tileData matrix.
   * 
   * @see detectTile
   */
  void ApolloPanTile::detectEdges(Mat &tileData) {

    // Crop
    Rect topROI(0, 1250, m_columns, 200);
    Rect bottomROI(0,24200, m_columns, 200);
    Mat topData = tileData(topROI);
    Mat bottomData = tileData(bottomROI);

    // Convert to gray
    Mat topGray;
    Mat bottomGray;
    cvtColor(topData, topGray, COLOR_BGR2GRAY);
    cvtColor(bottomData, bottomGray, COLOR_BGR2GRAY);

    // Threshold
    threshold(topGray, topGray, 100, 255, THRESH_BINARY);
    threshold(bottomGray, bottomGray, 100, 255, THRESH_BINARY);

    // Detect contours with canny
    Mat topCannyOutput;
    Mat bottomCannyOutput;
    vector<vector<Point> > topContours;
    vector<vector<Point> > bottomContours;
    vector<Vec4i> topHierarchy;
    vector<Vec4i> bottomHierarchy;
    Canny(topGray, topCannyOutput, 90, 180, 3, true);
    Canny(bottomGray, bottomCannyOutput, 90, 180, 3, true);
    findContours(topCannyOutput, topContours, topHierarchy,
                 RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
    findContours(bottomCannyOutput, bottomContours, bottomHierarchy,
                 RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Find the bounding rectangle for each contour
    vector< vector<Point> > topPoly(topContours.size());
    vector< vector<Point> > bottomPoly(bottomContours.size());
    vector<Rect> topBoundingRectangles(topContours.size());
    vector<Rect> bottomBoundingRectangles(bottomContours.size());
    for( size_t i = 0; i < topBoundingRectangles.size(); i++ ) {
      approxPolyDP(Mat(topContours[i]), topPoly[i], 3, true);
      topBoundingRectangles[i] = boundingRect(Mat(topPoly[i]));
    }
    for( size_t i = 0; i < bottomBoundingRectangles.size(); i++ ) {
      approxPolyDP(Mat(bottomContours[i]), bottomPoly[i], 3, true);
      bottomBoundingRectangles[i] = boundingRect(Mat(bottomPoly[i]));
    }
    for (size_t i = 0; i < topBoundingRectangles.size(); i++) {
      if (topBoundingRectangles[i].width > 1000) {
        if ((topBoundingRectangles[i].tl().y + topBoundingRectangles[i].height + 1250) > m_topTrim) {
          m_topTrim = topBoundingRectangles[i].tl().y + topBoundingRectangles[i].height + 1250;
        }
      }
    }
    for (size_t i = 0; i < bottomBoundingRectangles.size(); i++) {
      if (bottomBoundingRectangles[i].width > 1000) {
        if ( (m_rows - bottomBoundingRectangles[i].tl().y - 24200) > m_bottomTrim ) {
          m_bottomTrim = m_rows - bottomBoundingRectangles[i].tl().y - 24200;
        }
      }
    }
  }


  /**
   * Serialize the tile as a PvlObject.
   * 
   * @return @b PvlObject A PvlObject containing the tile's information.
   * 
   * @TODO save rejected marks.
   */
  PvlObject ApolloPanTile::toPvl() {

    // Create Fiducial Mark group
    PvlKeyword fiducialNumber("Number");
    PvlKeyword fiducialLine("Line");
    PvlKeyword fiducialSample("Sample");
    PvlKeyword fiducialValid("Valid");
    PvlKeyword fiducialLength("Length");
    PvlKeyword fiducialHeight("Height");
    PvlKeyword fiducialCalibratedX("Calibrated_X");
    PvlKeyword fiducialCalibratedY("Calibrated_Y");
    PvlKeyword fiducialResidualX("Residual_X");
    PvlKeyword fiducialResidualY("Residual_Y");
    PvlKeyword fiducialResidualMagnitude("Residual_Magnitude");
    for (size_t i = 0; i < m_fiducialMarks.size(); i++) {
      fiducialNumber            += QString::number(m_fiducialMarks[i].number());
      fiducialLine              += QString::number(m_fiducialMarks[i].line());
      fiducialSample            += QString::number(m_fiducialMarks[i].sample());
      fiducialValid             += QString::number(m_fiducialMarks[i].valid());
      fiducialLength            += QString::number(m_fiducialMarks[i].length());
      fiducialHeight            += QString::number(m_fiducialMarks[i].height());
      fiducialCalibratedX       += QString::number(m_fiducialMarks[i].calibratedX());
      fiducialCalibratedY       += QString::number(m_fiducialMarks[i].calibratedY());
      fiducialResidualX         += QString::number(m_fiducialMarks[i].residualX());
      fiducialResidualY         += QString::number(m_fiducialMarks[i].residualY());
      fiducialResidualMagnitude += QString::number(m_fiducialMarks[i].residualMagnitude());
    }
    PvlGroup fiducialGroup("Fiducial Marks");
    fiducialGroup += fiducialNumber;
    fiducialGroup += fiducialLine;
    fiducialGroup += fiducialSample;
    fiducialGroup += fiducialValid;
    fiducialGroup += fiducialLength;
    fiducialGroup += fiducialHeight;
    fiducialGroup += fiducialCalibratedX;
    fiducialGroup += fiducialCalibratedY;
    fiducialGroup += fiducialResidualX;
    fiducialGroup += fiducialResidualY;
    fiducialGroup += fiducialResidualMagnitude;

    // Create Timing Mark group
    PvlKeyword timingNumber("Number");
    PvlKeyword timingLine("Line");
    PvlKeyword timingSample("Sample");
    PvlKeyword timingLength("Length");
    PvlKeyword timingHeight("Height");
    PvlKeyword timingValid("Valid");
    PvlKeyword timingValue("Value");
    for (size_t i = 0; i < m_timingMarks.size(); i++) {
      timingNumber += QString::number(m_timingMarks[i].number());;
      timingLine   += QString::number((m_timingMarks[i].topLine()
                                       + m_timingMarks[i].bottomLine())/2);
      timingSample += QString::number(m_timingMarks[i].leftSample());;
      timingLength += QString::number(m_timingMarks[i].length());;
      timingHeight += QString::number(m_timingMarks[i].height());;
      timingValid  += QString::number(m_timingMarks[i].valid());;
      timingValue  += QString::number(m_timingMarks[i].value());;
    }
    PvlGroup timingGroup("Timing Marks");
    timingGroup += timingNumber;
    timingGroup += timingLine;
    timingGroup += timingSample;
    timingGroup += timingLength;
    timingGroup += timingHeight;
    timingGroup += timingValid;
    timingGroup += timingValue;

    PvlGroup generalGroup("General");
    generalGroup += PvlKeyword("Rows", QString::number(m_rows));
    generalGroup += PvlKeyword("Columns", QString::number(m_columns));
    generalGroup += PvlKeyword("Top_Trim", QString::number(m_topTrim));
    generalGroup += PvlKeyword("Bottom_Trim", QString::number(m_bottomTrim));
    generalGroup += PvlKeyword("Left_Trim", QString::number(m_leftTrim));
    generalGroup += PvlKeyword("Right_Trim", QString::number(m_rightTrim));
    generalGroup += PvlKeyword("Timing_Offset", QString::number(m_timingOffset));
    generalGroup += PvlKeyword("Sample_Offset", QString::number(m_sampleOffset));
    PvlKeyword affineX("Affine_X");
    for (size_t i = 0; i < m_transX.size(); i++) {
      affineX += QString::number(m_transX[i]);
    }
    generalGroup += affineX;
    PvlKeyword affineY("Affine_Y");
    for (size_t i = 0; i < m_transY.size(); i++) {
      affineY += QString::number(m_transY[i]);
    }
    generalGroup += affineY;
    generalGroup += PvlKeyword("Left_Clock_Count",
                          toString(m_stopTime - iTime("1971/7/26 13:34:00.795").EtString().toDouble()));
    generalGroup += PvlKeyword("Right_Clock_Count",
                          toString(m_startTime - iTime("1971/7/26 13:34:00.795").EtString().toDouble()));
    generalGroup += PvlKeyword("Left_Time", iTime(m_stopTime).UTC());
    generalGroup += PvlKeyword("Right_Time", iTime(m_startTime).UTC());
    PvlKeyword ephemerisTime("Ephemeris_Time", toString(m_stopTime));
    PvlKeyword exposureTime("Exposure_Time", toString(m_timingMarks[0].exposureTime()));
    PvlKeyword exposureSample("Exposure_Sample", "1");
    for (int i = 0; i < (int)m_timingMarks.size() - 1; i++) {
      ephemerisTime += toString(m_timingMarks[i].time());
      exposureTime += toString(m_timingMarks[i+1].exposureTime());
      exposureSample += QString::number(m_timingMarks[i].rightSample());
    }
    
    generalGroup += ephemerisTime;
    generalGroup += exposureTime;
    generalGroup += exposureSample;
    
    // Create trimming group
    PvlGroup trimmingGroup("Trimming");
    trimmingGroup += PvlKeyword("Top_Trim", QString::number(m_topTrim));
    trimmingGroup += PvlKeyword("Bottom_Trim", QString::number(m_bottomTrim));
    trimmingGroup += PvlKeyword("Left_Trim", QString::number(m_leftTrim));
    trimmingGroup += PvlKeyword("Right_Trim", QString::number(m_rightTrim));
    
    // Create output object
    PvlObject results("AS15-P-" + m_imageNumber + "_000" + QString::number(m_tileNumber));
    results += generalGroup;
    results += trimmingGroup;
    results += fiducialGroup;
    results += timingGroup;

    return results;
  }
  
  /**
   * Serialize the tile as a PvlObject.
   *
   * @return @b PvlObject A PvlObject containing the tile's information.
   * 
   * K. Edmundson 2017-08-17
   *
   * @TODO save rejected marks.
   */
  PvlObject ApolloPanTile::toPvlNew() {

    // Create Fiducial Mark group
    PvlKeyword fiducialNumber("Number");
    PvlKeyword fiducialLine("Line");
    PvlKeyword fiducialSample("Sample");
    PvlKeyword fiducialValid("Valid");
    PvlKeyword fiducialLength("Length");
    PvlKeyword fiducialHeight("Height");
    PvlKeyword fiducialCalibratedX("Calibrated_X");
    PvlKeyword fiducialCalibratedY("Calibrated_Y");
    PvlKeyword fiducialResidualX("Residual_X");
    PvlKeyword fiducialResidualY("Residual_Y");
    PvlKeyword fiducialResidualMagnitude("Residual_Magnitude");
    for (size_t i = 0; i < m_fiducialMarks.size(); i++) {
      fiducialNumber            += QString::number(m_fiducialMarks[i].number());
      fiducialLine              += QString::number(m_fiducialMarks[i].line());
      fiducialSample            += QString::number(m_fiducialMarks[i].sample());
      fiducialValid             += QString::number(m_fiducialMarks[i].valid());
      fiducialLength            += QString::number(m_fiducialMarks[i].length());
      fiducialHeight            += QString::number(m_fiducialMarks[i].height());
      fiducialCalibratedX       += QString::number(m_fiducialMarks[i].calibratedX());
      fiducialCalibratedY       += QString::number(m_fiducialMarks[i].calibratedY());
      fiducialResidualX         += QString::number(m_fiducialMarks[i].residualX());
      fiducialResidualY         += QString::number(m_fiducialMarks[i].residualY());
      fiducialResidualMagnitude += QString::number(m_fiducialMarks[i].residualMagnitude());
    }
    QString str = "Fiducial_Marks";
    PvlGroup fiducialGroup(str);
    fiducialGroup += fiducialNumber;
    fiducialGroup += fiducialLine;
    fiducialGroup += fiducialSample;
    fiducialGroup += fiducialValid;
    fiducialGroup += fiducialLength;
    fiducialGroup += fiducialHeight;
    fiducialGroup += fiducialCalibratedX;
    fiducialGroup += fiducialCalibratedY;
    fiducialGroup += fiducialResidualX;
    fiducialGroup += fiducialResidualY;
    fiducialGroup += fiducialResidualMagnitude;

    // Create Timing Mark group
    PvlKeyword timingNumber("Number");
    PvlKeyword timingLine("Line");
    PvlKeyword timingSample("Sample");
    PvlKeyword timingLength("Length");
    PvlKeyword timingHeight("Height");
    PvlKeyword timingValid("Valid");
    PvlKeyword timingValue("Value");
    for (size_t i = 0; i < m_timingMarks.size(); i++) {
      TimingMark tm = m_timingMarks[i];
      timingNumber += QString::number(m_timingMarks[i].number());
      timingLine   += QString::number((m_timingMarks[i].topLine()
                                       + m_timingMarks[i].bottomLine())/2);
      timingSample += QString::number(m_timingMarks[i].leftSample());
      timingLength += QString::number(m_timingMarks[i].length());
      timingHeight += QString::number(m_timingMarks[i].height());
      timingValid  += QString::number(m_timingMarks[i].valid());
      timingValue  += QString::number(m_timingMarks[i].value());
    }
    str = "Timing_Marks";
    PvlGroup timingGroup(str);
    timingGroup += timingNumber;
    timingGroup += timingLine;
    timingGroup += timingSample;
    timingGroup += timingLength;
    timingGroup += timingHeight;
    timingGroup += timingValid;
    timingGroup += timingValue;

    // Create trimming group
    str = "Trimming";
    PvlGroup trimmingGroup(str);
    trimmingGroup += PvlKeyword("Top_Trim", QString::number(m_topTrim));
    trimmingGroup += PvlKeyword("Bottom_Trim", QString::number(m_bottomTrim));
    trimmingGroup += PvlKeyword("Left_Trim", QString::number(m_leftTrim));
    trimmingGroup += PvlKeyword("Right_Trim", QString::number(m_rightTrim));

    // Create results object that wraps everything
    PvlObject results("AS15-P-" + m_imageNumber + "_000" + QString::number(m_tileNumber));

    str = "General";
    PvlGroup generalGroup(str);
    generalGroup += PvlKeyword("Rows", QString::number(m_rows));
    generalGroup += PvlKeyword("Columns", QString::number(m_columns));
    generalGroup += PvlKeyword("Timing_Offset", QString::number(m_timingOffset));
    generalGroup += PvlKeyword("Sample_Offset", QString::number(m_sampleOffset));
    PvlKeyword affineX("Affine_X");
    for (size_t i = 0; i < m_transX.size(); i++) {
      affineX += QString::number(m_transX[i]);
    }
    generalGroup += affineX;
    PvlKeyword affineY("Affine_Y");
    for (size_t i = 0; i < m_transY.size(); i++) {
      affineY += QString::number(m_transY[i]);
    }
    generalGroup += affineY;

    generalGroup += m_leftClockCount;
    generalGroup += m_rightClockCount;
    generalGroup += m_leftTime;
    generalGroup += m_rightTime;

    PvlKeyword etimes("Ephemeris_Time");
    for (size_t i = 0; i < m_etimes.size(); i++) {
      etimes += QString::number(m_etimes[i],'f',5);
    }
    generalGroup += etimes;

    PvlKeyword exptimes("Exposure_Time");
    for (size_t i = 0; i < m_exptimes.size(); i++) {
      exptimes += QString::number(m_exptimes[i],'g',16);
    }
    generalGroup += exptimes;

    PvlKeyword expSampleTimes("Exposure_Sample");
    for (size_t i = 0; i < m_expSampleTimes.size(); i++) {
      expSampleTimes += QString::number(m_expSampleTimes[i]);
    }
    generalGroup += expSampleTimes;

    // set values for first sample in image in fiducial space
    PvlKeyword ephemerisTime("Ephemeris_Time", toString(m_stopTime));
    PvlKeyword exposureTime("Exposure_Time", toString(m_timingMarks[0].exposureTime()));


    // TODO: describe better - to handle tile 8
//    double expSamp1 = 0.5 * m_transY[0] + 0.5 * m_transY[1] + m_transY[2];
//    if (expSamp1 <= 0.0)
//      expSamp1 = 1.0;
//    PvlKeyword exposureSample("Exposure_Sample", toString((int)(expSamp1)));

//  PvlKeyword exposureSample("Exposure_Sample", "1");

//    for (int i = 0; i < (int)m_timingMarks.size() - 1; i++) {
//      ephemerisTime += toString(m_timingMarks[i].time());
//      exposureTime += toString(m_timingMarks[i+1].exposureTime());
//    exposureSample += QString::number(m_timingMarks[i].rightSample());
//      exposureSample += QString::number((int)(m_timingMarks[i].rightSampleFiducialSpace()));
//    }

//    generalGroup += ephemerisTime;
//    generalGroup += exposureTime;
//    generalGroup += exposureSample;

    results += generalGroup;
    results += trimmingGroup;
    results += fiducialGroup;
    results += timingGroup;

    return results;
  }
  

  /**
   * Load the tile information in from a Pvl file.
   * 
   * @param filename The Pvl file's filename.
   * 
   * @TODO Load rejected marks.
   */
  void ApolloPanTile::fromPvl(QString filename) {

    // Load the file and get the tile object
    Pvl pvlFile(filename);
    PvlObject tilePvl = pvlFile.object(0);

    QString tileName = tilePvl.name();
    m_imageNumber = tileName.mid(7,4);
    m_tileNumber = tileName.mid(15,1).toInt();

    PvlGroup generalGroup = tilePvl.findGroup("General");
    m_rows = generalGroup.findKeyword("Rows")[0].toInt();
    m_columns = generalGroup.findKeyword("Columns")[0].toInt();
    m_timingOffset = generalGroup.findKeyword("Timing_Offset")[0].toInt();
    m_sampleOffset = generalGroup.findKeyword("Sample_Offset")[0].toInt();

    m_transX.clear();
    m_transY.clear();
    if (generalGroup.findKeyword("Affine_X")[0] != "Null") {
      for (int i = 0; i < generalGroup.findKeyword("Affine_X").size(); i++) {
        m_transX.push_back(generalGroup.findKeyword("Affine_X")[i].toDouble());
      }
    }
    if (generalGroup.findKeyword("Affine_Y")[0] != "Null") {
      for (int i = 0; i < generalGroup.findKeyword("Affine_Y").size(); i++) {
        m_transY.push_back(generalGroup.findKeyword("Affine_Y")[i].toDouble());
      }
    }
    
    PvlGroup trimmingGroup = tilePvl.findGroup("Trimming");
    m_topTrim = trimmingGroup.findKeyword("Top_Trim")[0].toInt();
    m_bottomTrim = trimmingGroup.findKeyword("Bottom_Trim")[0].toInt();
    m_leftTrim = trimmingGroup.findKeyword("Left_Trim")[0].toInt();
    m_rightTrim = trimmingGroup.findKeyword("Right_Trim")[0].toInt();

    m_fiducialMarks.clear();
    PvlGroup fiducialGroup = tilePvl.findGroup("Fiducial Marks");
    PvlKeyword fiducialNumber = fiducialGroup.findKeyword("Number");
    PvlKeyword fiducialLine = fiducialGroup.findKeyword("Line");
    PvlKeyword fiducialSample = fiducialGroup.findKeyword("Sample");
    PvlKeyword fiducialValid = fiducialGroup.findKeyword("Valid");
    PvlKeyword fiducialLength = fiducialGroup.findKeyword("Length");
    PvlKeyword fiducialHeight = fiducialGroup.findKeyword("Height");
    
    // This is a hacky answer to old detection pvls not having these.
    PvlKeyword fiducialCalibratedX;
    PvlKeyword fiducialCalibratedY;
    PvlKeyword fiducialResidualX;
    PvlKeyword fiducialResidualY;
    if (fiducialGroup.hasKeyword("Calibrated_X")) {
      fiducialCalibratedX = fiducialGroup.findKeyword("Calibrated_X");
      fiducialCalibratedY = fiducialGroup.findKeyword("Calibrated_Y");
      fiducialResidualX = fiducialGroup.findKeyword("Residual_X");
      fiducialResidualY = fiducialGroup.findKeyword("Residual_Y");
    }
    for (int i = 0; i < fiducialNumber.size(); i++) {
      float matLine = fiducialLine[i].toFloat();
      int offset = 24300;
      if (matLine < 20000) {
        matLine -= 1000;
        offset = 1000;
      }
      else {
        matLine -= 24300;
      }
      Point2f massCenter(fiducialSample[i].toFloat(), matLine);
      Rect boundingRect((int)fiducialSample[i].toDouble() - fiducialLength[i].toInt()/2,
                        (int)matLine - fiducialHeight[i].toInt()/2,
                        fiducialLength[i].toInt(), fiducialHeight[i].toInt());
      FiducialMark mark(massCenter, boundingRect);
      mark.setLineOffset(offset);
      mark.setValid((bool)fiducialValid[i].toInt());
      mark.setNumber(fiducialNumber[i].toInt());
      if (fiducialGroup.hasKeyword("Calibrated_X")) {
        mark.setCalibratedX(fiducialCalibratedX[i].toDouble());
        mark.setCalibratedY(fiducialCalibratedY[i].toDouble());
        mark.setResidualX(fiducialResidualX[i].toDouble());
        mark.setResidualY(fiducialResidualY[i].toDouble());
        mark.computeResidualMagnitude();
      }
      m_fiducialMarks.push_back(mark);
    }

    m_timingMarks.clear();
    PvlGroup timingGroup = tilePvl.findGroup("Timing Marks");
    PvlKeyword timingNumber = timingGroup.findKeyword("Number");
    PvlKeyword timingLine = timingGroup.findKeyword("Line");
    PvlKeyword timingSample = timingGroup.findKeyword("Sample");
    PvlKeyword timingLength = timingGroup.findKeyword("Length");
    PvlKeyword timingHeight = timingGroup.findKeyword("Height");
    PvlKeyword timingValid = timingGroup.findKeyword("Valid");
    PvlKeyword timingValue = timingGroup.findKeyword("Value");
    for (int i = 0; i < timingNumber.size(); i++) {
      float matLine = timingLine[i].toFloat() - 24900;
      Point2f massCenter(timingSample[i].toFloat() + timingLength[i].toFloat()/2, matLine);
      Rect boundingRect(timingSample[i].toInt(), (int)matLine - timingHeight[i].toInt()/2,
                        timingLength[i].toInt(), timingHeight[i].toInt());
      TimingMark mark(massCenter, boundingRect);
      mark.setValid((bool)timingValid[i].toInt());
      mark.setNumber(timingNumber[i].toInt());
      mark.setValue(timingValue[i].toInt());
      m_timingMarks.push_back(mark);
    }
  }
  
  /**
   * Load the tile information in from a Pvl file.
   *
   * @param filename The Pvl file's filename.
   * 
   * K. Edmundson 2017-08-17
   *
   * @TODO Load rejected marks.
   */
  void ApolloPanTile::fromPvlNew(QString filename) {

    fromPvl(filename);
    
    Pvl pvlFile(filename);
    PvlObject tilePvl = pvlFile.object(0);

    PvlGroup generalGroup = tilePvl.findGroup("General");
    
    PvlKeyword ephemerisTime = generalGroup.findKeyword("Ephemeris_Time");
    for (int i = 0; i < ephemerisTime.size(); i++) {
      double et = ephemerisTime[i].toDouble();
      m_etimes.push_back(et);
    }

    PvlKeyword exposureTime = generalGroup.findKeyword("Exposure_Time");
    for (int i = 0; i < exposureTime.size(); i++) {
      double et = exposureTime[i].toDouble();
      m_exptimes.push_back(et);
    }

    PvlKeyword expSamples = generalGroup.findKeyword("Exposure_Sample");
    for (int i = 0; i < expSamples.size(); i++) {
      double es = expSamples[i].toInt();
      m_expSampleTimes.push_back(es);
    }

    m_leftClockCount = generalGroup.findKeyword("Left_Clock_Count");
    m_rightClockCount = generalGroup.findKeyword("Right_Clock_Count");
    m_leftTime = generalGroup.findKeyword("Left_Time");
    m_rightTime = generalGroup.findKeyword("Right_Time");
  }  


  /**
   * Return the relative size of every timing mark.  This is used for determining
   * what the value of every timing mark is.
   * 
   * @return @b vector<double> The relative sizes of timing marks.
   */
  vector<double> ApolloPanTile::jenksData() {
    vector<double> data;
    for (size_t i = 1; i < m_timingMarks.size(); i++) {
      data.push_back( (double)m_timingMarks[i].length()
                     / (double)(m_timingMarks[i].rightSample()
                                - m_timingMarks[i-1].rightSample()) );
    }
    return data;
  }


  /**
   * Classify the timing marks in the tile based on the input breaks.
   * The input vector should contain lowest and highest values for each type of mark.
   * 
   * @param breaks A vector containing start, stop cutoffs for small, medium, and large marks.
   */
  void ApolloPanTile::classifyTimingMarks(vector<double> breaks) {
    for (size_t i = 1; i < m_timingMarks.size(); i++) {
      if (m_timingMarks[i].valid()) {
        double value = (double)m_timingMarks[i].length()
                      / (double)(m_timingMarks[i].rightSample() - m_timingMarks[i-1].rightSample());
        if (value < breaks[2]) {
          m_timingMarks[i].setValue(0);
        }
        else if  (value < breaks[4]) {
          m_timingMarks[i].setValue(1);
        }
        else {
          m_timingMarks[i].setValue(2);
        }
      }
    }
  }


  /**
   * Check if any timing marks are missing.
   * If a mark is missing check rejections.
   */
  void ApolloPanTile::checkMissingTimingMarks() {

    for (size_t i = 1; i < m_timingMarks.size() - 1; i++) {
      if (m_timingMarks[i-1].valid() && m_timingMarks[i].valid() && m_timingMarks[i+1].valid()) {
        continue;
      }
      if ( (double)(m_timingMarks[i].rightSample() - m_timingMarks[i-1].rightSample()) /
           (double)(m_timingMarks[i+1].rightSample() - m_timingMarks[i].rightSample()) > 1.75 ) {
        double expectedSample = (double)(m_timingMarks[i].rightSample()
                                         + m_timingMarks[i-1].rightSample())/2;
        vector<TimingMark> prospectMarks;
        vector<int> prospectIndex;
        for (size_t j = 0; j < m_rejectedTimingMarks.size(); j++) {
          if ((m_rejectedTimingMarks[j].leftSample() > m_timingMarks[i-1].rightSample())
              && (m_rejectedTimingMarks[j].rightSample() < m_timingMarks[i].leftSample())) {
            prospectMarks.push_back(m_rejectedTimingMarks[j]);
            prospectIndex.push_back(j);
          }
        }
        if (prospectMarks.size() > 0) {
          TimingMark bestContour = prospectMarks.front();
          int bestIndex = prospectIndex.front();
          for (size_t j = 1; j < prospectMarks.size(); j++) {
            if (abs(prospectMarks[j].rightSample() - expectedSample)
                < abs(bestContour.rightSample() - expectedSample)) {
              bestContour = prospectMarks[j];
              bestIndex = prospectIndex[j];
            }
          }
          // Insert match into timing data
          m_timingMarks.insert(m_timingMarks.begin() + i, bestContour);
          m_rejectedTimingMarks.erase(m_rejectedTimingMarks.begin() + bestIndex);
        }
      }
    }
  }


  /**
   * Check if any fiducial marks are missing.
   * If a mark is missing check rejections.
   */
  void ApolloPanTile::checkMissingFiducialMarks() {
    for (size_t i = 1; i < m_fiducialMarks.size()-1; i++) {
      if ( (fabs(m_fiducialMarks[i].sample() - m_fiducialMarks[i+1].sample()) > 100) &&
          (fabs(m_fiducialMarks[i].line() - m_fiducialMarks[i+1].line()) < 100) ) {
        double expectedSample = 0;
        double expectedLine = m_fiducialMarks[i-1].line();
        // if a bottom mark is missing
        if (m_fiducialMarks[i].line() < 10000) {
          expectedSample = m_fiducialMarks[i].sample();
        }
        // if a top mark is missing
        else {
          expectedSample = m_fiducialMarks[i+1].sample();
        }
        vector<FiducialMark> candidateMarks;
        vector<int> candidateIndex;
        for (size_t k = 0; k < m_rejectedFiducialMarks.size(); k++) {
          if ( (m_rejectedFiducialMarks[k].length() > 40) &&
              (fabs(m_rejectedFiducialMarks[k].sample() + m_rejectedFiducialMarks[k].line() - expectedLine - expectedSample) < 30) ){
            candidateMarks.push_back(m_rejectedFiducialMarks[k]);
            candidateIndex.push_back(k);
          }
        }
        if (candidateMarks.size() > 0) {
          FiducialMark bestContour = candidateMarks.front();
          int bestIndex = candidateIndex.front();
          for (size_t k = 1; k < candidateMarks.size(); k++) {
            if ( fabs(candidateMarks[k].sample() + candidateMarks[k].line() - expectedLine - expectedSample) <
                fabs(bestContour.sample() + bestContour.line() - expectedLine - expectedSample) ) {
              bestContour = candidateMarks[k];
              bestIndex = candidateIndex[k];
            }
          }
          m_fiducialMarks.insert(m_fiducialMarks.begin() + i, bestContour);
          m_rejectedFiducialMarks.erase(m_rejectedFiducialMarks.begin() + bestIndex);
        }
      }
    }
  }


  /**
   * Number the timing marks based on the timing mark offset.
   */
  void ApolloPanTile::numberTimingMarks() {
    int startIndex = 0;
    if (!m_timingMarks[0].valid()) {
      startIndex = 1;
    }
    for (size_t i = 0; i < m_timingMarks.size(); i++) {
      m_timingMarks[i].setNumber(m_timingOffset + i - startIndex);
    }
  }


  /**
   * Number the fiducial marks based on given start.
   * 
   * @param firstNumber The first fiducial mark's number.
   */
  void ApolloPanTile::numberFiducialMarks(int firstNumber) {
    int currentNumber = firstNumber;
    for (size_t i = 0; i < m_fiducialMarks.size(); i++) {
      m_fiducialMarks[i].setNumber(currentNumber);
      currentNumber++; 
      
      if ( (fabs(m_fiducialMarks[i].sample() - m_fiducialMarks[i+1].sample()) > 100) &&
          (fabs(m_fiducialMarks[i].line() - m_fiducialMarks[i+1].line()) < 100) ) {
        if (i < m_fiducialMarks.size() - 1 && currentNumber != 40) {
          cout << "Missed fiducial [" << currentNumber << "] on tile ["
               << QString::number(m_tileNumber) << "], index ["
               << i << "]." << endl;
        }
        currentNumber++;
      }
    }
  }


  /**
   * Load the calibrated fiducial mark locations based on an input CSV.
   * 
   * @param csvFilename The filename of the CSV file that contains the calibrated
   *                    fiducial mark coordinates in image space.
   */
  void ApolloPanTile::loadCalibratedFiducials(QString csvFilename) {

    // Open the csv file with the calibrated fiducial mark locations
    CSVReader calibratedData(csvFilename);
    calibratedData.setHeader(true);

    // Get the y values (in mm)
    CSVReader::CSVAxis yCol = calibratedData.getColumn("y (mm)");
    CSVReader::CSVDblVector calY = calibratedData.convert<double>(yCol);

    // Get the x values (in mm)
    CSVReader::CSVAxis xCol = calibratedData.getColumn("x (mm)");
    CSVReader::CSVDblVector calX = calibratedData.convert<double>(xCol);

    // Store the values in the fiducial marks.
    for (size_t i = 0 ; i < m_fiducialMarks.size(); i++) {
      if ((int)i >= calX.dim() || (int)i >= calY.dim()) {
      QString msg = "Insufficient data in calibrated csv for mark ["
                    + QString::number(m_fiducialMarks[i].number()) +"].";
      throw IException(IException::User, msg, _FILEINFO_);
      }
      m_fiducialMarks[i].setCalibratedX(calX[m_fiducialMarks[i].number()]);
      m_fiducialMarks[i].setCalibratedY(calY[m_fiducialMarks[i].number()]);
    }
  }


  /**
   * Compute and store an affine transformation for the fiducial marks in the tile.
   * 
   * @throws IException::Programmer "Cannot compute affine transformation because
   *                                 there are no fiducial marks"
   */
  void ApolloPanTile::computeAffine() {
    
    // Make sure there are fiducial marks.
    int numberOfFiducials = m_fiducialMarks.size();
    if (numberOfFiducials < 1) {
      QString msg = "Cannot compute affine transformation because there are no fiducial marks";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Build the affine object and the data arrays.
    Affine tileTrans;
    double samples [numberOfFiducials];
    double lines [numberOfFiducials];
    double calibratedXs [numberOfFiducials];
    double calibratedYs [numberOfFiducials];
    for (int i = 0; i < numberOfFiducials; i++) {
      samples[i] = m_fiducialMarks[i].sample();
      lines[i] = m_fiducialMarks[i].line();
      calibratedXs[i] = m_fiducialMarks[i].calibratedX();
      calibratedYs[i] = m_fiducialMarks[i].calibratedY();
    }

    // Solve the transformation
    tileTrans.Solve(lines, samples, calibratedXs, calibratedYs, numberOfFiducials);

    // Save the transformation coefficients
    m_transX = tileTrans.Coefficients(1);
    m_transY = tileTrans.Coefficients(2);
  }


  /**
   * Compute the residuals for the fiducial marks based on the stored affine transformation.
   * 
   * @throws IException::Programmer "Cannot compute residuals if an affine
   *                                 has not been computed first."
   */
  void ApolloPanTile::computeResiduals() {
    if (m_transX.size() < 3 || m_transY.size() < 3) {
      QString msg = "Cannot compute residuals if an affine has not been computed first.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    for (size_t i = 0; i < m_fiducialMarks.size(); i++) {
      double transformedX = m_fiducialMarks[i].line() * m_transX[0] +
                            m_fiducialMarks[i].sample() * m_transX[1] +
                            m_transX[2];
      double transformedY = m_fiducialMarks[i].line() * m_transY[0] +
                            m_fiducialMarks[i].sample() * m_transY[1] +
                            m_transY[2];
      m_fiducialMarks[i].setResidualX(m_fiducialMarks[i].calibratedX() - transformedX);
      m_fiducialMarks[i].setResidualY(m_fiducialMarks[i].calibratedY() - transformedY);
      m_fiducialMarks[i].computeResidualMagnitude();
    }
  }


  /**
   * Flag all fiducial marks that have a residual magnitude greater than the threshold.
   * Flagged fiducial marks are reported and set as invalid.
   * 
   * @param threshold The threshold for flagging.
   */
  void ApolloPanTile::flagOutliers(double threshold) {
    for (size_t i = 0; i < m_fiducialMarks.size(); i++) {
      if (m_fiducialMarks[i].residualMagnitude() > threshold) {
        m_fiducialMarks[i].setValid(false);
        cout << "Fiducial mark [" << QString::number(i) << "] on tile ["
             << QString::number(m_tileNumber) << "] is an outlier." << endl;
      }
    }
  }


  /**
   * Compute the start time and exposure time for every timing mark in the tile.
   * 
   * @param flippedSecondStart The index of the start of the first second in the image
   *                           relative to the left edge of the image.
   * @param firstSecond The time of the first second in the image in seconds since launch.
   */
  void ApolloPanTile::computeTiming(int flippedSecondStart, double firstSecond) {
    for (int i = 1; i < (int)m_timingMarks.size() - 1; i++) {
      m_timingMarks[i].setExposureTime(1/((double)(m_timingMarks[i].rightSample()
                                                   - m_timingMarks[i-1].rightSample()) * 100));
      m_timingMarks[i].setTime(firstSecond
                               + (double)(flippedSecondStart - m_timingMarks[i].number()) * 0.01);
    }
  }


  /**
   * Compute the start and stop time for the tile.
   * Also computes right and left trim for tiles 1 and 8 respectively.
   */
  void ApolloPanTile::computeStartStop() {
    TimingMark startMark;
    if (m_timingMarks.back().valid()) {
      startMark = m_timingMarks.back();
    }
    else {
      startMark = m_timingMarks[m_timingMarks.size() - 2];
    }
    if (m_tileNumber != 1) {
      m_startTime = startMark.time() - startMark.exposureTime() * (m_columns - startMark.rightSample());
    }
    else {
      m_startTime = m_timingMarks[m_timingMarks.size() - 2].time();
      m_rightTrim = m_columns - m_timingMarks[m_timingMarks.size() - 2].rightSample();;
    }

    TimingMark stopMark;
    if (m_timingMarks.front().valid()) {
      stopMark = m_timingMarks.front();
    }
    else {
      stopMark = m_timingMarks[1];
    }
    if (m_tileNumber != 8) {
      m_stopTime = stopMark.time() + stopMark.exposureTime() * stopMark.rightSample();
    }
    else {
      m_stopTime = m_timingMarks.front().time();
      m_leftTrim = m_timingMarks.front().rightSample();
    }
  }


  /**
   * Return the timing code segment on this tile.
   * 
   * @return @b vector<int> The segment of the timing code that is on this tile.
   */
  vector<int> ApolloPanTile::codeSegment() {
    vector<int> segment;
    for (size_t i = 0; i < m_timingMarks.size(); i++) {
      segment.push_back(m_timingMarks[i].value());
    }
    return segment;
  }


  /**
   * Return the number of timing marks.
   * 
   * @return @b int The number of timing marks.
   */
  int ApolloPanTile::numberOfTimingMarks() {
    return m_timingMarks.size();
  }


  /**
   * Return the number of fiducial marks.
   * 
   * @return @b int The number of fiducial marks.
   */
  int ApolloPanTile::numberOfFiducialMarks() {
    return m_fiducialMarks.size();
  }


  /**
   * Return the number of the image that the tile is a part of.
   * 
   * @return @b QString The four digit image number
   */
  QString ApolloPanTile::imageNumber() {
    return m_imageNumber;
  }


  /**
   * Return the timing mark at the specified index.
   * 
   * @param index The index of the desired mark.
   * 
   * @throws IException::Programmer "Attempted to access timing mark which is out of bounds"
   */
  TimingMark &ApolloPanTile::timingMark(int index) {
    if (index < (int)m_timingMarks.size() && index >=0) {
      return m_timingMarks[index];
    }
    else {
      QString msg = "Attempted to access timing mark [" + QString::number(index)
                    + "] in tile [" + QString::number(m_tileNumber) + "] which is out of bounds";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Return the fiducial mark at the specified index.
   * 
   * @param index The index of the desired mark.
   * 
   * @throws IException::Programmer "Attempted to access fiducial mark which is out of bounds"
   */
  FiducialMark &ApolloPanTile::fiducialMark(int index) {
    if (index < (int)m_fiducialMarks.size() && index >=0) {
      return m_fiducialMarks[index];
    }
    else {
      QString msg = "Attempted to access fiducial mark [" + QString::number(index)
                    + "] in tile [" + QString::number(m_tileNumber) + "] which is out of bounds";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Return the timing mark with the specified number.
   * 
   * @param number The number of the desired timing mark
   * 
   * @throws IException::Programmer "Timing mark is not in tile."
   */
  TimingMark &ApolloPanTile::timingMarkByNumber(int number) {
    for (int i = 0; i < (int)m_timingMarks.size(); i++) {
      if (m_timingMarks[i].number() == number) {
        return m_timingMarks[i];
      }
    }
    QString msg = "Timing mark [" + QString::number(number) + "] is not in tile ["
                  + QString::number(m_tileNumber) + "].";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Return the timing mark offset.
   * 
   * @return @b int The timing mark offset.
   */
  int ApolloPanTile::timingOffset() {
    return m_timingOffset;
  }


  /**
   * Return the sample offset for the match to the tile to the left.
   * 
   * @return @b int The sample offset for this tile.
   */
  int ApolloPanTile::sampleOffset() {
    return m_sampleOffset;
  }


  /**
   * Returns the number of samples in the tile.
   * 
   * @return @b int The number of samples.
   */
  int ApolloPanTile::samples() {
    return m_columns;
  }


  /**
   * Returns the number of lines in the tile.
   * 
   * @return @b int The number of lines.
   */
  int ApolloPanTile::lines() {
    return m_rows;
  }


  /** 
   * Set the timing mark offset.
   * 
   * @param offset The new timing mark offset.
   */
  void ApolloPanTile::setTimingOffset(int offset) {
    m_timingOffset = offset;
  }


  /**
   * Set the sample offset.
   * 
   * @param offset The new sample offset.
   */
  void ApolloPanTile::setSampleOffset(int offset) {
    m_sampleOffset = offset;
  }


  /**
   * Clears the stored fiducial marks.
   */
  void ApolloPanTile::clearFiducialMarks() {
    m_fiducialMarks.clear();
  }


  /**
   * Clears the stored timing marks.
   */
  void ApolloPanTile::clearTimingMarks() {
    m_timingMarks.clear();
  }


  /**
   * Adds a fiducial mark to the tile and then sorts the fiducial marks so they are in order.
   * 
   * @param mark The fiducial mark to add.
   */
  void ApolloPanTile::addFiducialMark(FiducialMark mark) {
    m_fiducialMarks.push_back(mark);
    sort(m_fiducialMarks.begin(), m_fiducialMarks.end());
  }


  /**
   * Adds a timing mark to the tile and then sorts the timing marks so they are in order.
   * 
   * @param mark The timing mark to add.
   */
  void ApolloPanTile::addTimingMark(TimingMark mark) {
    m_timingMarks.push_back(mark);
    sort(m_timingMarks.begin(), m_timingMarks.end());
  }
};
