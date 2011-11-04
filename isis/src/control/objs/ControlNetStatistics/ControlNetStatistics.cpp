#include "ControlNetStatistics.h"

#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlCubeGraphNode.h"
#include "Cube.h"
#include "CubeManager.h"
#include "Filename.h"
#include "iString.h"
#include "Progress.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "Statistics.h" 


#include <geos_c.h>
#include <geos/algorithm/ConvexHull.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>

using namespace std;

namespace Isis {

  //! String names for Point Type
  string sPointType [] = { "Fixed", "Constrained", "Free" };

  //! String values for Boolean
  string sBoolean[]    = { "False", "True" };

  /**
   * ControlNetStatistics Constructor has ctor to it's base Control Network
   *
   * @author Sharmila Prasad (8/24/2010)
   *
   * @param pCNet - Input Control network
   * @param psSerialNumFile - Serial Number List file
   * @param pProgress - Check Progress if not Null
   */
  ControlNetStatistics::ControlNetStatistics(ControlNet *pCNet, const string &psSerialNumFile, Progress *pProgress) {
    mCNet = pCNet;
    mSerialNumList  = SerialNumberList(psSerialNumFile); 
    mProgress       = pProgress;
    
    GetPointIntStats();
    GetPointDoubleStats();
    GenerateImageStats();
  }

  /**
   * Constructor with  ControlNet
   *
   * @author Sharmila Prasad (11/5/2010)
   *
   * @param pCNet
   * @param pProgress
   */
  ControlNetStatistics::ControlNetStatistics(ControlNet *pCNet, Progress *pProgress) {
    mCNet      = pCNet;
    mProgress  = pProgress;
    
    GetPointIntStats();
    GetPointDoubleStats();
  }
  
  /**
   * Destructor
   *
   * @author Sharmila Prasad (9/17/2010)
   */
  ControlNetStatistics::~ControlNetStatistics() {

  }

  /**
   * Generates the summary stats for the entire control network.
   * Stats include Total images, Total, Valid, Ignored, Fixed Points,
   * Total, Valid, Ignored Measures and also Average, Min, Max Error,
   * Min, Max Line and Sample Errors
   *
   * @author Sharmila Prasad (8/25/2010)
   *
   * @param pStatsGrp - Output Control Net Stats in Pvl format
   */
  void ControlNetStatistics::GenerateControlNetStats(PvlGroup &pStatsGrp) {
    pStatsGrp = PvlGroup("ControlNetSummary");
    if (mSerialNumList.Size()) {
      pStatsGrp += PvlKeyword("TotalImages",     mSerialNumList.Size());
    }
    pStatsGrp += PvlKeyword("TotalPoints",       mCNet->GetNumPoints());
    pStatsGrp += PvlKeyword("ValidPoints",       NumValidPoints());
    pStatsGrp += PvlKeyword("IgnoredPoints",     mCNet->GetNumPoints() - NumValidPoints());
    pStatsGrp += PvlKeyword("FixedPoints",       NumFixedPoints());
    pStatsGrp += PvlKeyword("ConstrainedPoints", NumConstrainedPoints());
    pStatsGrp += PvlKeyword("FreePoints",        NumFreePoints());
    pStatsGrp += PvlKeyword("EditLockPoints",    mCNet->GetNumEditLockPoints());

    pStatsGrp += PvlKeyword("TotalMeasures",     NumMeasures());
    pStatsGrp += PvlKeyword("ValidMeasures",     NumValidMeasures());
    pStatsGrp += PvlKeyword("IgnoredMeasures",   NumIgnoredMeasures());
    pStatsGrp += PvlKeyword("EditLockMeasures",  mCNet->GetNumEditLockMeasures());

    double dValue = GetAverageResidual();
    pStatsGrp += PvlKeyword("AvgResidual",       (dValue == Isis::NULL8 ? "Null" : iString(dValue)));

    dValue = GetMinimumResidual();
    pStatsGrp += PvlKeyword("MinResidual",       (dValue == Isis::NULL8 ? "Null" : iString(dValue)));

    dValue = GetMaximumResidual();
    pStatsGrp += PvlKeyword("MaxResidual",       (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    dValue = GetMinLineResidual();
    pStatsGrp += PvlKeyword("MinLineResidual",   (dValue == Isis::NULL8 ? "Null" : iString(dValue)));

    dValue = GetMaxLineResidual();
    pStatsGrp += PvlKeyword("MaxLineResidual",   (dValue == Isis::NULL8 ? "Null" : iString(dValue)));

    dValue = GetMinSampleResidual();
    pStatsGrp += PvlKeyword("MinSampleResidual", (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    dValue = GetMaxSampleResidual();
    pStatsGrp += PvlKeyword("MaxSampleResidual", (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    // Shifts - Line, Sample, Pixel
    dValue = GetAvgPixelShift();
    pStatsGrp += PvlKeyword("AvgPixelShift",     (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    dValue = GetMinPixelShift();
    pStatsGrp += PvlKeyword("MinPixelShift",     (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    dValue = GetMaxPixelShift();
    pStatsGrp += PvlKeyword("MaxPixelShift",     (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    dValue = GetMinLineShift();
    pStatsGrp += PvlKeyword("MinLineShift",      (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    dValue = GetMaxLineShift();
    pStatsGrp += PvlKeyword("MaxLineShift",      (dValue == Isis::NULL8 ? "Null" : iString(dValue)));

    dValue = GetMinSampleShift();
    pStatsGrp += PvlKeyword("MinSampleShift",    (dValue == Isis::NULL8 ? "Null" : iString(dValue)));

    dValue = GetMaxSampleShift(); 
    pStatsGrp += PvlKeyword("MaxSampleShift",    (dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    
    // Convex Hull
    if (mSerialNumList.Size()) {
      dValue = mConvexHullRatioStats.Minimum();
      pStatsGrp += PvlKeyword("MinConvexHullRatio",(dValue == Isis::NULL8 ? "Null" : iString(dValue)));
      
      dValue = mConvexHullRatioStats.Maximum();
      pStatsGrp += PvlKeyword("MaxConvexHullRatio",(dValue == Isis::NULL8 ? "Null" : iString(dValue)));
      
      dValue = mConvexHullRatioStats.Average();
      pStatsGrp += PvlKeyword("AvgConvexHullRatio",(dValue == Isis::NULL8 ? "Null" : iString(dValue)));
    }
  }
  
  /**
   * Generate the Image stats
   * 
   * @author Sharmila Prasad (11/1/2011)
   */
  void ControlNetStatistics::GenerateImageStats(void){
    geos::geom::GeometryFactory geosFactory;
    
    CubeManager cubeMgr;
    cubeMgr.SetNumOpenCubes(50);
  
    mCubeGraphNodes = mCNet->GetCubeGraphNodes();
    
    if (mProgress != NULL) {
      mProgress->SetText("Generating Image Stats.....");
      mProgress->SetMaximumSteps(mCubeGraphNodes.size());
      mProgress->CheckStatus();
    }
    
    foreach (ControlCubeGraphNode * node, mCubeGraphNodes) {
      geos::geom::CoordinateSequence * ptCoordinates = new geos::geom::CoordinateArraySequence();
      
      // setup vector for number of image properties and init to 0
      vector<double> imgStats(numImageStats, 0);
      
      // Open the cube to get the dimensions
      iString sn = node->getSerialNumber();
      Cube *cube = cubeMgr.OpenCube(mSerialNumList.Filename(sn));

      imgStats[imgSamples] = cube->getSampleCount();
      imgStats[imgLines]   = cube->getLineCount();
      double cubeArea      = imgStats[imgSamples] * imgStats[imgLines];

      QList< ControlMeasure * > measures = node->getMeasures();

      // Populate pts with a list of control points
      // imgSamples, imgLines, imgTotalPoints, imgIgnoredPoints, imgFixedPoints, imgLockedPoints, imgLocked, imgConstrainedPoints, imgFreePoints, imgConvexHullArea, imgConvexHullRatio
      foreach (ControlMeasure * measure, measures) {
        ControlPoint *parentPoint = measure->Parent();
        imgStats[imgTotalPoints]++;
        if (parentPoint->IsIgnored()) {
          imgStats[imgIgnoredPoints]++;
        }
        if (parentPoint->GetType() == ControlPoint::Fixed) {
          imgStats[imgFixedPoints]++;
        }
        if (parentPoint->GetType() == ControlPoint::Constrained) {
          imgStats[imgConstrainedPoints]++;
        }
        if (parentPoint->GetType() == ControlPoint::Free) {
          imgStats[imgFreePoints]++;
        }
        if (parentPoint->IsEditLocked()) {
          imgStats[imgLockedPoints]++;
        }
        if (measure->IsEditLocked()) {
          imgStats[imgLocked]++;
        } 
        ptCoordinates->add(geos::geom::Coordinate(measure->GetSample(), measure->GetLine()));
      }
      ptCoordinates->add(geos::geom::Coordinate(measures[0]->GetSample(), measures[0]->GetLine()));
        
      if (ptCoordinates->size() >= 4) {
        
        // Calculate the convex hull
        geos::geom::Geometry * convexHull = geosFactory.createPolygon(
          geosFactory.createLinearRing(ptCoordinates), 0)->convexHull();
    
        // Calculate the area of the convex hull
        imgStats[imgConvexHullArea] = convexHull->getArea();
        
        imgStats[imgConvexHullRatio]= imgStats[imgConvexHullArea] / cubeArea;
      }
      
      // Add info to statistics to get min, max and avg convex hull
      mConvexHullStats.AddData(imgStats[imgConvexHullArea]);
      mConvexHullRatioStats.AddData(imgStats[imgConvexHullRatio]);
      
      mImageMap[sn] = imgStats;
      
      if (ptCoordinates) {
        delete ptCoordinates;
        ptCoordinates = NULL;
      }
      // Update Progress
      if (mProgress != NULL)
        mProgress->CheckStatus();
    }
  }

  /**
   * Print the Image Stats into specified output file
   *
   * @author Sharmila Prasad (9/1/2010)
   *  
   * Header: Filename, SerialNumber, TotalPoints, PointsIgnored, PointsLocked, Fixed, Constrained, Free 
   *  
   * @param psImageFile - Output Image Stats File
   */
  void ControlNetStatistics::PrintImageStats(const string &psImageFile) {
    // Check if the image list has been provided
    if (!mSerialNumList.Size()) {
      string msg = "Serial Number of Images has not been provided to get Image Stats";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    Filename outFile(psImageFile);
    ofstream ostm;
    string outName(outFile.Expanded());
    ostm.open(outName.c_str(), std::ios::out);
    
    map< string, vector<double> >::iterator it;

    // / imgSamples, imgLines, imgTotalPoints, imgIgnoredPoints, imgFixedPoints, imgLockedPoints, imgLocked, imgConstrainedPoints, imgFreePoints, imgConvexHullArea, imgConvexHullRatio
 
    // Log into the output file
    ostm << "Filename, SerialNumber, TotalPoints, PointsIgnored, PointsEditLocked, Fixed, Constrained, Free, ConvexHullRatio" <<  endl;
    for (it = mImageMap.begin(); it != mImageMap.end(); it++) {
      ostm << mSerialNumList.Filename((*it).first) << ", " << (*it).first << ", ";
      vector<double>imgStats = (*it).second ;
      ostm << imgStats[imgTotalPoints]<< ", " << imgStats[imgIgnoredPoints] << ", " ;
      ostm << imgStats[imgLockedPoints] << ", " << imgStats[imgFixedPoints] << ", " ;
      ostm << imgStats[imgConstrainedPoints] << ", " << imgStats[imgFreePoints] << ", ";
      ostm << imgStats[ imgConvexHullRatio] << endl;
    }
    ostm.close();
  }

  /**
   * Returns the Image Stats by Serial Number
   * 
   * @author Sharmila Prasad (11/3/2011)
   * 
   * @param psSerialNum - Image serialNum
   * 
   * @return const vector<double> 
   */
  const vector<double>  ControlNetStatistics::GetImageStatsBySerialNum(string psSerialNum) {
    return mImageMap[psSerialNum]; 
  }
  
  /**
   * Generate the statistics of a Control Network by Point
   * Stats include ID, Type of each Control Point and
   * Total, Ignored measures in each Control Point
   *
   * @author Sharmila Prasad (8/24/2010)
   *
   * @param psPointFile - Output Point Statisitics File
   */
  void ControlNetStatistics::GeneratePointStats(const string &psPointFile) {
    Isis::Filename outFile(psPointFile);

    ofstream ostm;
    string outName(outFile.Expanded());
    ostm.open(outName.c_str(), std::ios::out);
    ostm << " PointId, PointType, PointIgnore, PointEditLock, TotalMeasures, MeasuresValid, MeasuresIgnore, MeasuresEditLock," << endl;

    int iNumPoints = mCNet->GetNumPoints();

    // Initialise the Progress object
    if (mProgress != NULL && iNumPoints > 0) {
      mProgress->SetText("Point Stats: Loading Control Points...");
      mProgress->SetMaximumSteps(iNumPoints);
      mProgress->CheckStatus();
    }

    for (int i = 0; i < iNumPoints; i++) {
      const ControlPoint *cPoint = mCNet->GetPoint(i);
      int iNumMeasures     = cPoint->GetNumMeasures();
      int iValidMeasures   = cPoint->GetNumValidMeasures();
      int iIgnoredMeasures = iNumMeasures - iValidMeasures;
      
      // Log into the output file
      ostm << cPoint->GetId()   << ", " << sPointType[(int)cPoint->GetType()] << ", " << sBoolean[(int)cPoint->IsIgnored()] << ", " ;
      ostm << sBoolean[(int)cPoint->IsEditLocked()] << ", " << iNumMeasures << ", " << iValidMeasures << ", ";
      ostm << iIgnoredMeasures << ", " << cPoint->GetNumLockedMeasures() << endl;

      // Update Progress
      if (mProgress != NULL)
        mProgress->CheckStatus();
    }
    ostm.close();
  }
  
  /**
   * Get network statistics for total, valid, ignored, locked points and measures
   * 
   * @author sprasad (7/19/2011)
   */
  void ControlNetStatistics::GetPointIntStats() {
    // Init all the entries
    // totalPoints, validPoints, ignoredPoints, fixedPoints, constrainedPoints, editLockedPoints, 
    // totalMeasures, validMeasures, ignoredMeasures, editLockedMeasures
    for (int i=0; i<numPointIntStats; i++) {
      mPointIntStats[i] = 0;
    }
    
    int iNumPoints = mCNet->GetNumPoints();
      
    // totalPoints
    mPointIntStats[totalPoints] = iNumPoints;

    for (int i = 0; i < iNumPoints; i++) {
      if (!mCNet->GetPoint(i)->IsIgnored()) {
        // validPoints
        mPointIntStats[validPoints]++;
      } 
      else {
        // ignoredPoints
        mPointIntStats[ignoredPoints]++;
      }

      // fixedPoints
      if (mCNet->GetPoint(i)->GetType() == ControlPoint::Fixed)
        mPointIntStats[fixedPoints]++;

      // constrainedPoints
      if (mCNet->GetPoint(i)->GetType() == ControlPoint::Constrained)
        mPointIntStats[constrainedPoints]++;
      
      // free points
      if (mCNet->GetPoint(i)->GetType() == ControlPoint::Free)
        mPointIntStats[freePoints]++;
      
      // editLockedPoints
      if (mCNet->GetPoint(i)->IsEditLocked()) {
        mPointIntStats[editLockedPoints]++;
      } 

      // totalMeasures
      mPointIntStats[totalMeasures] += mCNet->GetPoint(i)->GetNumMeasures();
      
      // validMeasures
      mPointIntStats[validMeasures] += mCNet->GetPoint(i)->GetNumValidMeasures();
      
      // editLockedMeasures
      mPointIntStats[editLockedMeasures] +=  mCNet->GetPoint(i)->GetNumLockedMeasures();
    }
    // ignoredMeasures
    mPointIntStats[ignoredMeasures] = mPointIntStats[totalMeasures] -  mPointIntStats[validMeasures];
  }
  
  /**
   * Get the Network Statistics for Residuals (line, sample, magnitude) and 
   * Shifts (line, sample, pixel)
   * 
   * @author Sharmila Prasad (7/19/2011)
   */
  void ControlNetStatistics::GetPointDoubleStats(void) {
    // avgResidual, maxResidual, minResidual, minLineResidual, maxLineResidual, minSampleResidual, maxSampleResidual,
    // avgShift, maxShift, minShift, minLineShift, maxLineShift, minSampleShift, maxSampleShift
    for (int i=0; i<numPointDblStats; i++) {
      mPointDoubleStats[i] = 0;
    }

    int iNumPoints = mCNet->GetNumPoints();
    double dValue = 0;
    
    for (int i = 0; i < iNumPoints; i++) {
      Statistics resMagStats = mCNet->GetPoint(i)->GetStatistic(&ControlMeasure::GetResidualMagnitude);

      // avgResidual
      if (resMagStats.Average() != Isis::NULL8 ) {
        mPointDoubleStats[avgResidual] += abs(resMagStats.Average());
      }
      
      // maxResidual
      dValue = abs(resMagStats.Maximum());
      if (resMagStats.Maximum() != Isis::NULL8 && mPointDoubleStats[maxResidual] < dValue)
        mPointDoubleStats[maxResidual] = dValue;
    
      // minResidual
      dValue = abs(resMagStats.Minimum());
      if (resMagStats.Minimum() != Isis::NULL8 && mPointDoubleStats[minResidual] > dValue)
        mPointDoubleStats[minResidual] = dValue;
    
      // Line Residual
      Statistics resLineStats = mCNet->GetPoint(i)->GetStatistic(&ControlMeasure::GetLineResidual);
    
      // minLineResidual
      dValue = abs(resLineStats.Minimum());
      if (resLineStats.Minimum() != Isis::NULL8 && mPointDoubleStats[minLineResidual] > dValue)
        mPointDoubleStats[minLineResidual] = dValue;
    
      // maxLineResidual
      dValue = abs(resLineStats.Maximum());
      if (resLineStats.Maximum() != Isis::NULL8 && mPointDoubleStats[maxLineResidual] < dValue)
        mPointDoubleStats[maxLineResidual] = dValue;
    
      // Sample Residual
      Statistics resSampStats = mCNet->GetPoint(i)->GetStatistic(&ControlMeasure::GetSampleResidual);
    
      // minSampleResidual
      dValue = abs(resSampStats.Minimum());
      if (resSampStats.Minimum() != Isis::NULL8 && mPointDoubleStats[minSampleResidual] > dValue)
        mPointDoubleStats[minSampleResidual] = dValue;
    
      // maxSampleResidual
      dValue = abs(resSampStats.Maximum());
      if (resSampStats.Maximum() != Isis::NULL8 && mPointDoubleStats[maxSampleResidual] < dValue)
        mPointDoubleStats[maxSampleResidual] = dValue;
    
      // Pixel Shift
      Statistics pixShiftStats = mCNet->GetPoint(i)->GetStatistic(&ControlMeasure::GetPixelShift);
    
      // avgShift
      if (pixShiftStats.Average() !=  Isis::NULL8) {
        mPointDoubleStats[avgPixelShift] += abs(pixShiftStats.Average());
      }
    
      // maxShift
      dValue = abs(pixShiftStats.Maximum());
      if (pixShiftStats.Maximum() != Isis::NULL8 && mPointDoubleStats[maxPixelShift] < dValue)
        mPointDoubleStats[maxPixelShift] = dValue;
    
      // minShift
      dValue = abs(pixShiftStats.Minimum());
      if (pixShiftStats.Minimum() != Isis::NULL8 && mPointDoubleStats[minPixelShift] > dValue)  
        mPointDoubleStats[minPixelShift] = dValue;
    
      // Line Shift
      Statistics lineShiftStats = mCNet->GetPoint(i)->GetStatistic(&ControlMeasure::GetLineShift);
    
      // minLineShift
      dValue = abs(lineShiftStats.Minimum());
      if (lineShiftStats.Minimum() != Isis::NULL8 && mPointDoubleStats[minLineShift] > dValue)
          mPointDoubleStats[minLineShift] = dValue;
    
      // maxLineShift
      dValue = abs(lineShiftStats.Maximum());
      if (lineShiftStats.Maximum() != Isis::NULL8 && mPointDoubleStats[maxLineShift] < dValue)
        mPointDoubleStats[maxLineShift] = dValue;
    
      // Sample Shift
      Statistics sampShiftStats = mCNet->GetPoint(i)->GetStatistic(&ControlMeasure::GetSampleShift);
    
      // minSampleShift
      dValue = abs(sampShiftStats.Minimum());
      if (sampShiftStats.Minimum() != Isis::NULL8 && mPointDoubleStats[minSampleShift] > dValue)
        mPointDoubleStats[minSampleShift] = dValue;
    
      // maxSampleShift
      dValue = abs(sampShiftStats.Maximum());
      if (sampShiftStats.Maximum() != Isis::NULL8 && mPointDoubleStats[maxSampleShift] < dValue)
        mPointDoubleStats[maxSampleShift] = dValue;
    } 
    
    int iValidPoints = mPointIntStats[validPoints];
     
    // Residuals
    mPointDoubleStats[avgResidual] /= iValidPoints;
  
    // Shift
    mPointDoubleStats[avgPixelShift] /= iValidPoints;
  }
}
