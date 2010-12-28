#include "Chip.h"
#include "Pvl.h"
#include "InterestOperator.h"
#include "Plugin.h"
#include "iException.h"
#include "Filename.h"
#include "Statistics.h"
#include "PolygonTools.h"
#include "SpecialPixel.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ImageOverlapSet.h"
#include "ImagePolygon.h"
#include "MeasureValidationResults.h"
#include "PolygonTools.h"

//#define _DEBUG_

namespace Isis {

#ifdef _DEBUG_
  //debugging
  fstream ostm;

  void StartDebug() {
    ostm.open("Debug.log", std::ios::out | std::ios::app);
    ostm << "\n*************************\n";
  }

  void CloseDebug() {
    ostm << "\n*************************\n";
    ostm.close();
  }
#endif

  /**
   * Create InterestOperator object. Because this is a pure virtual class you can
   * not create an InterestOperator class directly. Instead, see the InterestOperatorFactory
   * class.
   *
   * @param pvl  A pvl object containing a valid InterestOperator specification
   *
   */
  InterestOperator::InterestOperator(Pvl &pPvl): ControlNetValidMeasure(pPvl) {
    p_interestAmount = 0.0;
    p_worstInterest = 0.0;
    p_lines = 1;
    p_samples = 1;
    p_deltaSamp = 0;
    p_deltaLine = 0;
    p_clipPolygon = NULL;
    mbOverlaps = false;

    mOperatorGrp = PvlGroup("InterestOptions");
    Parse(pPvl);

#ifdef _DEBUG_
    StartDebug();
#endif
  }


  //! Destroy InterestOperator object
  InterestOperator::~InterestOperator() {
    if(p_clipPolygon != NULL) delete p_clipPolygon;
#ifdef _DEBUG_
    CloseDebug();
#endif
  }


 /**
  * Create an InterestOperator object using a PVL specification.
  * An example of the PVL required for this is:
  *
  * @code
  *   Group = Operator
  *     Name      = StandardDeviation
  *     Samples   = 21
  *     Lines     = 21
  *     DeltaLine = 50
  *     DeltaSamp = 25
  *   EndGroup
  * @endcode
  *
  * There are many other options that can be set via the pvl and are
  * described in other documentation (see below).
  *
  * @param pvl The pvl object containing the specification
  *
  * @history 2010-04-09 Sharmila Prasad Check for validity of new keyword "MaxEmissionAngle"
  * @history 2010-06-10 Sharmila Prasad Parse only Interest specific keywords and store 
  *                                     in Operator group
  */
  void InterestOperator::Parse(Pvl &pPvl) {
    try {
      // Get info from the operator group
      // Required Parameters
      PvlGroup &op = pPvl.FindGroup("Operator", Pvl::Traverse);

      mOperatorGrp += Isis::PvlKeyword(op["Name"]);

      p_samples   = op["Samples"];
      mOperatorGrp += Isis::PvlKeyword("Samples", p_samples);

      p_lines     = op["Lines"];
      mOperatorGrp += Isis::PvlKeyword("Lines", p_lines);

      p_deltaLine = op["DeltaLine"];
      mOperatorGrp += Isis::PvlKeyword("DeltaLine", p_deltaLine);

      p_deltaSamp = op["DeltaSamp"];
      mOperatorGrp += Isis::PvlKeyword("DeltaSamp", p_deltaSamp);

      p_minimumInterest = op["MinimumInterest"];
      mOperatorGrp += Isis::PvlKeyword("MinimumInterest", p_minimumInterest);

    }
    catch(iException &e) {
      std::string msg = "Improper format for InterestOperator PVL [" + pPvl.Filename() + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  /**
   * Initialise the InterestResults structure given the index
   *
   * @author Sharmila Prasad (6/8/2010)
   *
   * @param piIndex - Index for the Interest Results structure
   */
  void InterestOperator::InitInterestResults(int piIndex) {
    mtInterestResults[piIndex].msSerialNum  = "";
    mtInterestResults[piIndex].mdInterest   = Isis::Null;
    mtInterestResults[piIndex].mdBestSample = Isis::Null;
    mtInterestResults[piIndex].mdBestLine   = Isis::Null;
    mtInterestResults[piIndex].mdOrigSample = Isis::Null;
    mtInterestResults[piIndex].mdOrigLine   = Isis::Null;
    mtInterestResults[piIndex].mdEmission   = 135;
    mtInterestResults[piIndex].mdIncidence  = 135;
    mtInterestResults[piIndex].mdDn         = Isis::ValidMinimum;
    mtInterestResults[piIndex].mdResolution = DBL_MAX ;
    mtInterestResults[piIndex].miDeltaSample= 0;
    mtInterestResults[piIndex].miDeltaLine  = 0;
    mtInterestResults[piIndex].mbValid      = false;
  }

  /**
   * Walk the pattern chip through the search chip to find the best interest
   *
   * @param cube [in] The Isis::Cube to look for an interesting area in
   * @param piSample [in] The sample postion in the cube where the chip is located
   * @param piLine [in] The line postion in the cube where the chip is located
   * @param pUnivGrndMap Reference to the Universal Ground map of this image
   * @return  Returns the status of the operation.  The following conditions can
   *          occur true=Success, false=Failed
   *
   * @history 2010-03-30 Sharmila Prasad - Check for valid DN Value and Emission Angle in
   *          the user defined ValidMin-ValidMax range when selecting point of interest
   *          in a Control Measure
   * @history 2010-06-23 Sharmila Prasad - Validate for Resolution Range and Pixels/Meters 
   *                                       from edge options
   */
  bool InterestOperator::Operate(Cube &pCube, UniversalGroundMap &pUnivGrndMap, 
                                 int piSample, int piLine) {

    if(!pUnivGrndMap.HasCamera()) {  // Level 3 images/mosaic or bad image
      std::string msg = "Cannot run interest on images with no camera. Image " +
         pCube.Filename() + " has no Camera";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    int pad = Padding();
    Chip chip(2 * p_deltaSamp + p_samples + pad, 2 * p_deltaLine + p_lines + pad);
    chip.TackCube(piSample, piLine);
    if(p_clipPolygon != NULL) chip.SetClipPolygon(*p_clipPolygon);
    chip.Load(pCube);

    // Walk the search chip and find the best interest
    int iBestSamp = 0;
    int iBestLine = 0;
    double dSmallestDist = DBL_MAX;
    double dBestInterest = Isis::Null;
    int iLines   =  2 * p_deltaLine + p_lines / 2 + 1;
    int iSamples =  2 * p_deltaSamp + p_samples / 2 + 1;
    bool bCalculateInterest = false;

    for(int lin = p_lines / 2 + 1; lin <= iLines; lin++) {
      for(int samp = p_samples / 2 + 1; samp <= iSamples; samp++) {
        // Cannot take dnValues from the chip as it contains the interpolated dnValue
        // hence get the dn values directly from the cube
        chip.SetChipPosition((double)samp, (double)lin);

        bCalculateInterest = false;
        MeasureValidationResults results = 
          ValidStandardOptions(chip.CubeSample(), chip.CubeLine(), &pCube);
        if(results.isValid()) {
          bCalculateInterest = true;
        }

        if(bCalculateInterest) {
          Chip subChip = chip.Extract(p_samples + pad, p_lines + pad, samp, lin);
          double interest = Interest(subChip);
          if(interest != Isis::Null) {
            if((dBestInterest == Isis::Null) || CompareInterests(interest, dBestInterest)) {
              double dist = std::sqrt(std::pow(piSample - samp, 2.0) + std::pow(piLine - lin, 2.0));
              if(interest == dBestInterest && dist > dSmallestDist) {
                continue;
              }
              else {
                dBestInterest = interest;
                iBestSamp = samp;
                iBestLine = lin;
                dSmallestDist = dist;
              }
            }
          }
        }
      }
    }

    // Check to see if we went through the interest chip and never got a interest at
    // any location.
    if(dBestInterest == Isis::Null || dBestInterest < p_minimumInterest) {
      if(pUnivGrndMap.SetImage(piSample, piLine)) {
        p_interestAmount = dBestInterest;
      }
      return false;
    }

    p_interestAmount = dBestInterest;
    chip.SetChipPosition(iBestSamp, iBestLine);
    p_cubeSample = chip.CubeSample();
    p_cubeLine   = chip.CubeLine();

    return true;
  }

  /**
   * Read the Serial#'s and overlaplist if any and call API to find the reference
   * for all the points in the network
   *
   * @author Sharmila Prasad (6/9/2010)
   *
   * @param pNewNet - Input Control Net
   * @param psSerialNumFile - Serial Number File
   * @param psOverlapListFile - Overlaplist File containing overlap data
   * 
   * @history 10/15/2010 Sharmila Prasad - Use a single copy of Control Net 
   *
   */
  void InterestOperator::Operate(ControlNet &pNewNet, std::string psSerialNumFile, 
                                 std::string psOverlapListFile) {
    ReadSerialNumbers(psSerialNumFile);

    // Find all the overlaps between the images in the FROMLIST
    // The overlap polygon coordinates are in Lon/Lat order
    if(psOverlapListFile != "") {
      mOverlaps.ReadImageOverlaps(psOverlapListFile);
      mbOverlaps = true;
    }

    // Process the entire control net by calculating interest and moving the
    // point to a more interesting area
    FindCnetRef(pNewNet);
  }

  /**
   * Process a Control Point which is Locked or has the Reference Measure locked
   * 
   * @author Sharmila Prasad (10/5/2010)
   * 
   * @param pCPoint - Control Point wtih the Locks(s)
   * @param pPvlObj - Output log Pvl
   */
  void InterestOperator::ProcessLocked_Point_Reference(
      ControlPoint & pCPoint, PvlObject & pPvlObj,
      int & piMeasuresModified)
  {
    int iNumMeasures  = pCPoint.Size();
    bool bPntEditLock = pCPoint.IsEditLocked();
    int iMsrIgnored   = 0;
    
    for (int measure=0; measure<iNumMeasures; measure++) {
      ControlMeasure newMeasure(pCPoint[measure]);
      newMeasure.SetDateTime();
      newMeasure.SetChooserName("Application cnetref(interest)");
      bool bMeasureLocked = newMeasure.IsEditLocked();

      std::string sn = newMeasure.GetCubeSerialNumber();
      double dSample = newMeasure.GetSample();
      double dLine   = newMeasure.GetLine();

      // Log
      PvlGroup pvlMeasureGrp("MeasureDetails");
      pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
      pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation",
          LocationString(newMeasure.GetSample(), newMeasure.GetLine()));
  
      if (!newMeasure.IsIgnored()) {
        Cube *measureCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));
        
        MeasureValidationResults results = 
          ValidStandardOptions(dSample, dLine, measureCube);
        if (!results.isValid()) {
          if(bPntEditLock) {
            pvlMeasureGrp += Isis::PvlKeyword("UnIgnored", "Failed Validation Test but not "
                                                           "Ignored as Point EditLock is True");
          }
          else if (bMeasureLocked == measure){ 
            pvlMeasureGrp += Isis::PvlKeyword("Error","Reference failed the Validation Test "
                                                      "but is Locked");
          }
          else {
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Failed Emission, Incidence, Resolution "
                                                         "and/or Dn Value Test");
            newMeasure.SetIgnore(true);
            iMsrIgnored++;
            piMeasuresModified++;
          }
        }
      }
      else {
        pvlMeasureGrp += Isis::PvlKeyword("Ignored", "Originally Ignored");
        iMsrIgnored++;
      }

      pPvlObj += pvlMeasureGrp;
      pCPoint.UpdateMeasure(newMeasure);
    }

    if ((iNumMeasures - iMsrIgnored) < 2) {
      if (bPntEditLock) {
        pPvlObj += Isis::PvlKeyword("UnIgnored", "Good Measures less than 2 "
                                                 "but Point EditLock is True");
      }
      else {
        pCPoint.SetIgnore(true);
        pPvlObj += Isis::PvlKeyword("Ignored", "Good Measures less than 2");
      }
    }
  }
  
  /**
   * This traverses all the control points and measures in the network and
   * checks for valid Measure which passes the Emission Incidence Angle, DN value 
   * tests and  picks the Measure with the best Interest as the Reference
   *
   * @author Sharmila Prasad (5/14/2010)
   *
   * @param pNewNet - Input Control Net  
   *
   * @history 2010-07-13 Tracie Sucharski, Changes  for binary control networks, 
   *                        Measure type of Estimated is now Candidate and
   *                        instead of a separate keyword indicating whether
   *                        a meausre is the reference, the MeasureType is set
   *                        to Reference.
   * @history 2010-10-04 Sharmila Prasad - Modified for binary Control Net ex Edit Lock 
   * @history 2010-10-15 Sharmila Prasad - Use only a single copy of Control Net  
   *
   * @return none
   */
  void InterestOperator::FindCnetRef(ControlNet &pNewNet) {
    int iPointsModified = 0;
    int iMeasuresModified = 0;
    int iRefChanged = 0;

    // Status Report
    mStatus.SetText("Choosing Reference by Interest...");
    mStatus.SetMaximumSteps(pNewNet.Size());
    mStatus.CheckStatus();

    // Process each existing control point in the network
    for(int point = 0; point < pNewNet.Size(); ++point) {
      ControlPoint newPnt = ((ControlNet &) pNewNet)[point];

      // Create a copy of original control point
      const ControlPoint origPnt(newPnt);
        
      // Logging
      PvlObject pvlPointObj("PointDetails");
      pvlPointObj += Isis::PvlKeyword("PointId", newPnt.GetId());
      
      // Get number of measures locked and check if Reference
      // Measure is locked
      int iNumMeasuresLocked = newPnt.GetNumLockedMeasures();
      bool bRefLocked = newPnt.IsReferenceLocked();

      int iOrigRefIndex = newPnt.GetReferenceIndexNoException();

      // Only perform the interest operation on points of type "Tie" and
      // Points having atleast 1 measure and Point is not Ignored
      if(!newPnt.IsIgnored() && newPnt.GetType() == ControlPoint::Tie &&
         iOrigRefIndex >= 0 &&
         (iNumMeasuresLocked == 0 || (iNumMeasuresLocked > 0 && bRefLocked))) {

        // Check only the validity of the Point / Measures only if Point and/or 
        // Reference Measure is locked. 
        if(newPnt.IsEditLocked() || iNumMeasuresLocked > 0) {
          ProcessLocked_Point_Reference(newPnt, pvlPointObj, iMeasuresModified);
          ((ControlNet &) pNewNet).UpdatePoint(newPnt);

          mPvlLog += pvlPointObj;
          mStatus.CheckStatus();
          
          if(newPnt != origPnt) {
            iPointsModified ++;
          }
          continue;
        }
        
        int iBestMeasureIndex = InterestByPoint(newPnt);

        // Process for point with good interest and a best index
        double dReferenceLat = 0, dReferenceLon = 0;
        if(iBestMeasureIndex >= 0) {
          std::string sn = mtInterestResults[iBestMeasureIndex].msSerialNum;
          Cube *bestCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

          // Get the Camera for the reference image and get the lat/lon from that measurment
          Camera *bestCamera;
          try {
            bestCamera = bestCube->Camera();
          }
          catch(Isis::iException &e) {
            std::string msg = "Cannot Create Camera for Image:" + mSerialNumbers.Filename(sn);
            throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
          }

          double dBestSample   = mtInterestResults[iBestMeasureIndex].mdBestSample;
          double dBestLine     = mtInterestResults[iBestMeasureIndex].mdBestLine;

          bestCamera->SetImage(dBestSample, dBestLine);
          dReferenceLat = bestCamera->UniversalLatitude();
          dReferenceLon = bestCamera->UniversalLongitude();
        }

        // Create a measurment for each image in this point using
        // the reference lat/lon.
        int iNumIgnore = 0;
        for(int measure = 0; measure < newPnt.Size(); ++measure) {
          ControlMeasure newMeasure = newPnt[measure];
          newMeasure.SetDateTime();
          newMeasure.SetChooserName("Application cnetref(interest)");
          std::string sn = newMeasure.GetCubeSerialNumber();

          // Log
          PvlGroup pvlMeasureGrp("MeasureDetails");
          pvlMeasureGrp += Isis::PvlKeyword("SerialNum", sn);
          pvlMeasureGrp += Isis::PvlKeyword("OriginalLocation", LocationString(newMeasure.GetSample(), 
                                                                               newMeasure.GetLine()));

          // Initialize the UGM of this cube with the reference lat/lon
          if(!newMeasure.IsIgnored() && iBestMeasureIndex >= 0 && 
             mtInterestResults[iBestMeasureIndex].mdInterest != WorstInterest()) {
            Cube *measureCube =  mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

            // default setting
            newMeasure.SetIgnore(false);
            newMeasure.SetType(ControlMeasure::Candidate);

            // Get the Camera
            Camera *measureCamera;
            try {
              measureCamera = measureCube->Camera();
            }
            catch(Isis::iException &e) {
              std::string msg = "Cannot Create Camera for Image:" + mSerialNumbers.Filename(sn);
              throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
            }

            if(measureCamera->SetUniversalGround(dReferenceLat, dReferenceLon) && 
               measureCamera->InCube()) {
              // Check for reference, Put the corresponding line/samp into a newMeasure
              if(measure == iBestMeasureIndex) {
                newMeasure.SetCoordinate(mtInterestResults[measure].mdBestSample, 
                                         mtInterestResults[measure].mdBestLine, ControlMeasure::Candidate);
                newMeasure.SetType(ControlMeasure::Reference);

                pvlMeasureGrp += Isis::PvlKeyword("NewLocation",  LocationString(mtInterestResults[measure].mdBestSample, 
                                                                                 mtInterestResults[measure].mdBestLine));
                pvlMeasureGrp += Isis::PvlKeyword("DeltaSample",  mtInterestResults[measure].miDeltaSample);
                pvlMeasureGrp += Isis::PvlKeyword("DeltaLine",    mtInterestResults[measure].miDeltaLine);
                pvlMeasureGrp += Isis::PvlKeyword("Reference",    "true");
              }
              else {
                double dSample = measureCamera->Sample();
                double dLine   = measureCamera->Line();

                MeasureValidationResults results = 
                  ValidStandardOptions(dSample, dLine, measureCube);
                if(!results.isValid()) {
                  iNumIgnore++;
                  pvlMeasureGrp += Isis::PvlKeyword("Ignored",   "Failed Validation Test");
                  newMeasure.SetIgnore(true);
                }
                pvlMeasureGrp += Isis::PvlKeyword("NewLocation", LocationString(dSample, dLine));
                pvlMeasureGrp += Isis::PvlKeyword("DeltaSample", (int)abs((int)dSample - (int)newMeasure.GetSample()));
                pvlMeasureGrp += Isis::PvlKeyword("DeltaLine",   (int)abs((int)dLine - (int)newMeasure.GetLine()));
                pvlMeasureGrp += Isis::PvlKeyword("Reference",   "false");
                newMeasure.SetCoordinate(dSample, dLine);
              }
            }
            else {
              iNumIgnore++;
              pvlMeasureGrp += Isis::PvlKeyword("Ignored", "True");
              newMeasure.SetIgnore(true);
              if(!measureCamera->InCube()) {
                pvlMeasureGrp += Isis::PvlKeyword("Comments", "New location is not in the Image");
              }
            }
          }
          // No best interest, ignore the measure
          else {
            iNumIgnore++;
            pvlMeasureGrp += Isis::PvlKeyword("Ignored", "True");
            newMeasure.SetIgnore(true);
          }

          if(newMeasure != origPnt[measure]) {
            iMeasuresModified ++;
          }

          pvlMeasureGrp += Isis::PvlKeyword("BestInterest",   mtInterestResults[measure].mdInterest);
          pvlMeasureGrp += Isis::PvlKeyword("EmissionAngle",  mtInterestResults[measure].mdEmission);
          pvlMeasureGrp += Isis::PvlKeyword("IncidenceAngle", mtInterestResults[measure].mdIncidence);
          pvlMeasureGrp += Isis::PvlKeyword("Resolution",     mtInterestResults[measure].mdResolution);
          pvlMeasureGrp += Isis::PvlKeyword("DNValue",        mtInterestResults[measure].mdDn);
          pvlPointObj += pvlMeasureGrp;
        } // Measures Loop

        // Check the ignored measures number
        if((newPnt.Size() - iNumIgnore) < 2) {
          newPnt.SetIgnore(true);
          pvlPointObj += Isis::PvlKeyword("Ignored", "Good Measures less than 2");
        }

        iNumIgnore = 0;

        if(newPnt != origPnt) {
          iPointsModified ++;
        }
          
        if(!newPnt.IsIgnored() && iBestMeasureIndex != iOrigRefIndex) {
          iRefChanged ++;
          PvlGroup pvlRefChangeGrp("ReferenceChangeDetails");
          pvlRefChangeGrp += Isis::PvlKeyword("PrevSerialNumber", mtInterestResults[iOrigRefIndex].msSerialNum);
          pvlRefChangeGrp += Isis::PvlKeyword("PrevBestInterest", mtInterestResults[iOrigRefIndex].mdInterest);
          pvlRefChangeGrp += Isis::PvlKeyword("PrevLocation",     LocationString(mtInterestResults[iOrigRefIndex].mdOrigSample,
                                                                                 mtInterestResults[iOrigRefIndex].mdOrigLine));
          pvlRefChangeGrp += Isis::PvlKeyword("NewSerialNumber",  mtInterestResults[iBestMeasureIndex].msSerialNum);
          pvlRefChangeGrp += Isis::PvlKeyword("NewBestInterest",  mtInterestResults[iBestMeasureIndex].mdInterest);
          pvlRefChangeGrp += Isis::PvlKeyword("NewLocation",      LocationString(mtInterestResults[iBestMeasureIndex].mdBestSample,
                                                                                 mtInterestResults[iBestMeasureIndex].mdBestLine));

          // Log info, if Point not locked, apriori source == Reference and a new reference
          if(newPnt.GetAprioriSurfacePointSource() == ControlPoint::SurfacePointSource::Reference) {
            pvlRefChangeGrp += Isis::PvlKeyword("AprioriSource", "Reference is the source and has changed");
          }
        
          pvlPointObj += pvlRefChangeGrp;
        }
        else {
          pvlPointObj += Isis::PvlKeyword("Reference", "No Change");
        }
        // Clean up the results structure
        delete [] mtInterestResults;
      }
      else {
        // Process Ignored, non Tie points or Measures=0
        int iComment = 1;
        
        if(iOrigRefIndex < 0) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "No Measures in the Point");
        }

        if(newPnt.IsIgnored()) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Point was originally Ignored");
        }
        
        if (newPnt.GetType() == ControlPoint::Tie) {
          std::string sComment = "Comment" + iComment++;
          pvlPointObj += Isis::PvlKeyword(sComment, "Not a Tie Point");
        }
        
        if (iNumMeasuresLocked > 0 && !bRefLocked){
          pvlPointObj += Isis::PvlKeyword("Error", "Point has a Measure with EditLock set to true "
                                                   "but not a Reference");
        }

        for(int measure = 0; measure < newPnt.Size(); measure++) {
          newPnt[measure].SetDateTime();
          newPnt[measure].SetChooserName("Application cnetref(Interest)");
        }
      } // End of if point is of type tie

      mPvlLog += pvlPointObj;

      mStatus.CheckStatus();
    } // Point loop

    // Basic Statistics
    mStatisticsGrp += Isis::PvlKeyword("TotalPoints",      pNewNet.Size());
    mStatisticsGrp += Isis::PvlKeyword("PointsIgnored",    (pNewNet.Size() - pNewNet.NumValidPoints()));
    mStatisticsGrp += Isis::PvlKeyword("PointsModified",   iPointsModified);
    mStatisticsGrp += Isis::PvlKeyword("ReferenceChanged", iRefChanged);
    mStatisticsGrp += Isis::PvlKeyword("TotalMeasures",    pNewNet.NumMeasures());
    mStatisticsGrp += Isis::PvlKeyword("MeasuresModified", iMeasuresModified);

    mPvlLog += mStatisticsGrp;
  }

  /**
   * InterestByPoint - Find the interest of all measures in a Point
   * and store all the results in Interest Results structure.
   *
   * @author Sharmila Prasad (6/8/2010)
   *  
   * @param pCnetPoint - Control Point for which the best interest is calculated
   */
  int InterestOperator::InterestByPoint(ControlPoint &pCnetPoint) {
    // Find the overlap this point is inside of if the overlap list is entered
    const geos::geom::MultiPolygon *overlapPoly = NULL;
    
    if(mbOverlaps) {
      overlapPoly = FindOverlap(pCnetPoint);
      if(overlapPoly == NULL) {
        string msg = "Unable to find overlap polygon for point [" +
            pCnetPoint.GetId() + "]";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }
    }

    std::vector <PvlGroup> pvlGrpVector;

    // Create an array of Interest Results structure of measures size
    mtInterestResults = new InterestResults[pCnetPoint.Size()];

    int iBestMeasureIndex     = -1;
    double dBestInterestValue = Isis::Null;

    for(int measure = 0; measure < pCnetPoint.Size(); ++measure) {
      ControlMeasure origMsr = pCnetPoint[measure];
      std::string sn = origMsr.GetCubeSerialNumber();

      // Do not process Ignored Measures
      if(!origMsr.IsIgnored()) {
        InitInterestResults(measure);
        Cube *inCube = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn));

        // Set the clipping polygon for this point
        // Convert the lon/lat overlap polygon to samp/line using the UGM for
        // this image
        if(mbOverlaps) {
          UniversalGroundMap unvGround = UniversalGroundMap(*inCube);
          geos::geom::MultiPolygon *poly = PolygonTools::LatLonToSampleLine(*overlapPoly, &unvGround);
          SetClipPolygon(*poly);
          delete poly;
        }

        // Run the interest operator on this measurment
        if(InterestByMeasure(measure, origMsr, *inCube)) {
          if(dBestInterestValue == Isis::Null || CompareInterests(mtInterestResults[measure].mdInterest, 
                                                                  dBestInterestValue)) {
            dBestInterestValue = mtInterestResults[measure].mdInterest;
            iBestMeasureIndex = measure;
          }
        }
      }
    }
    return iBestMeasureIndex;
  }

  /**
   * Find the interest by Measure given the index to store the results in the
   * InterestResults structure
   *
   * @author Sharmila Prasad (6/10/2010)
   *  
   * @param piMeasure    - Index for Interest Results structure
   * @param pCnetMeasure - Control Measure for which the best interest 
   *                       is calculated
   *
   * @return bool
   */
  bool InterestOperator::InterestByMeasure(int piMeasure, ControlMeasure &pCnetMeasure, 
                                           Cube &pCube) {
    std::string serialNum = pCnetMeasure.GetCubeSerialNumber();

    int iOrigSample = (int)(pCnetMeasure.GetSample() + 0.5);
    int iOrigLine   = (int)(pCnetMeasure.GetLine() + 0.5);

    mtInterestResults[piMeasure].msSerialNum    = serialNum;
    mtInterestResults[piMeasure].mdOrigSample   = pCnetMeasure.GetSample();
    mtInterestResults[piMeasure].mdOrigLine     = pCnetMeasure.GetLine();

    int pad = Padding();
    Chip chip(2 * p_deltaSamp + p_samples + pad, 2 * p_deltaLine + p_lines + pad);
    chip.TackCube(iOrigSample, iOrigLine);
    if(p_clipPolygon != NULL) chip.SetClipPolygon(*p_clipPolygon);
    chip.Load(pCube);

    // Walk the search chip and find the best interest
    int iBestSamp = 0;
    int iBestLine = 0;
    double dSmallestDist = DBL_MAX;
    double dBestInterest = Isis::Null;
    int iLines   =  2 * p_deltaLine + p_lines / 2 + 1;
    int iSamples =  2 * p_deltaSamp + p_samples / 2 + 1;
    bool bCalculateInterest = false;
    for(int lin = p_lines / 2 + 1; lin <= iLines; lin++) {
      for(int samp = p_samples / 2 + 1; samp <= iSamples; samp++) {
        // Cannot take dnValues from the chip as it contains the interpolated dnValue
        // hence get the dn values directly from the cube
        chip.SetChipPosition((double)samp, (double)lin);

        bCalculateInterest = false;

        MeasureValidationResults results = 
          ValidStandardOptions(chip.CubeSample(), chip.CubeLine(), &pCube);
        if(results.isValid()) {
          bCalculateInterest = true;
        }

        if(bCalculateInterest) {
          Chip subChip = chip.Extract(p_samples + pad, p_lines + pad, samp, lin);
          double interest = Interest(subChip);

          if(interest != Isis::Null) {
            if((dBestInterest == Isis::Null) || CompareInterests(interest, dBestInterest)) {
              double dist = std::sqrt(std::pow(iOrigSample - samp, 2.0) + 
                                      std::pow(iOrigLine - lin, 2.0));
              if(interest == dBestInterest && dist > dSmallestDist) {
                continue;
              }
              else {
                dBestInterest = interest;
                dSmallestDist = dist;
                iBestSamp = samp;
                iBestLine = lin;

                mtInterestResults[piMeasure].mdEmission   = mdEmissionAngle;
                mtInterestResults[piMeasure].mdIncidence  = mdIncidenceAngle;
                mtInterestResults[piMeasure].mdDn         = mdDnValue;
                mtInterestResults[piMeasure].mdResolution = mdResolution ;
                mtInterestResults[piMeasure].mbValid      = true;
              }
            }
          }
        }
      }
    }

    // Check to see if we went through the interest chip and never got a interest at
    // any location.But record the Emission, Incidence Angles and DN Value for the failed 
    // Measure at the original location
    if(dBestInterest == Isis::Null || dBestInterest < p_minimumInterest) {
      // Get the Camera
      Camera *camera;
      try {
        camera = pCube.Camera();
      }
      catch(Isis::iException &e) {
        std::string msg = "Cannot Create Camera for Image:" + mSerialNumbers.Filename(serialNum);
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }

      if(camera->SetImage(iOrigSample, iOrigLine)) {
        Portal inPortal(1, 1, pCube.PixelType());
        inPortal.SetPosition(iOrigSample, iOrigLine, 1);
        pCube.Read(inPortal);

        mtInterestResults[piMeasure].mdInterest   = dBestInterest;
        mtInterestResults[piMeasure].mdBestSample = Isis::Null;
        mtInterestResults[piMeasure].mdBestLine   = Isis::Null;
        mtInterestResults[piMeasure].mdOrigSample = iOrigSample;
        mtInterestResults[piMeasure].mdOrigLine   = iOrigLine;
        mtInterestResults[piMeasure].mdEmission   = camera->EmissionAngle();
        mtInterestResults[piMeasure].mdIncidence  = camera->IncidenceAngle();
        mtInterestResults[piMeasure].mdDn         = inPortal[0];
        mtInterestResults[piMeasure].mdResolution = camera->PixelResolution();
        mtInterestResults[piMeasure].mbValid      = false;
      }
      return false;
    }

    chip.SetChipPosition(iBestSamp, iBestLine);
    mtInterestResults[piMeasure].mdInterest    = dBestInterest;
    mtInterestResults[piMeasure].mdBestSample  = chip.CubeSample();
    mtInterestResults[piMeasure].mdBestLine    = chip.CubeLine();
    mtInterestResults[piMeasure].miDeltaSample = (int)abs(mtInterestResults[piMeasure].mdBestSample - 
                                                          iOrigSample);
    mtInterestResults[piMeasure].miDeltaLine   = (int)abs(mtInterestResults[piMeasure].mdBestLine - 
                                                          iOrigLine);
    return true;
  }

  /**
   * This method searches for an overlap in the ImageOverlapSet that belongs
   * to the given control point. Only exact SN matches are accepted.
   *
   */
  const geos::geom::MultiPolygon *InterestOperator::FindOverlap(ControlPoint &pCnetPoint) {
    int exactMatchIndex = -1;

    for(int overlapIndex = 0; ((exactMatchIndex == -1) && (overlapIndex < mOverlaps.Size())); 
         overlapIndex ++) {
      const Isis::ImageOverlap *overlap = mOverlaps[overlapIndex];

      // Exact matches only; skip if # SNs don't match
      if(overlap->Size() != pCnetPoint.Size()) continue;

      // If # SNs match and each SN is contained in both then we're good, there
      // should never be two measures with the same SN
      int numMatches = 0;

      for(int measureIndex = 0; measureIndex < pCnetPoint.Size(); measureIndex ++) {
        if(measureIndex == numMatches) {
          const ControlMeasure &controlMeasure = pCnetPoint[measureIndex];
          iString serialNum = controlMeasure.GetCubeSerialNumber();
          if(overlap->HasSerialNumber(serialNum)) {
            numMatches++;
          }
        }
      }

      if(numMatches == pCnetPoint.Size()) {
        exactMatchIndex = overlapIndex;
      }
    }

    if(exactMatchIndex < 0) {
      return (FindOverlapByImageFootPrint(pCnetPoint));
    }

    return mOverlaps[exactMatchIndex]->Polygon();
  }

  /**
   * Find image overlaps by getting intersection of the individual image footprints
   * when an exact match in the overlaplist fails
   *
   * @author Sharmila Prasad (7/1/2010)
   *
   * @param pCnetPoint - Overlaps for the Control Point
   *
   * @return const geos::geom::MultiPolygon*
   */
  const geos::geom::MultiPolygon *InterestOperator::FindOverlapByImageFootPrint
                                                    (Isis::ControlPoint &pCnetPoint) {
    ImagePolygon measPolygon1, measPolygon2, measPolygon3;
    geos::geom::Geometry *geomIntersect1, *geomIntersect2;

    // Create Multipolygon for the first Control Measure
    std::string sn1 = pCnetPoint[0].GetCubeSerialNumber();
    Cube *inCube1 = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn1));
    inCube1->Read((Blob &)measPolygon1);

    // Create Multipolygon for the Second Control Measure
    std::string sn2 = pCnetPoint[1].GetCubeSerialNumber();
    Cube *inCube2 = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn2));
    inCube2->Read((Blob &)measPolygon2);

    // Get the interesection for the first 2 polgons
    geomIntersect1 = PolygonTools::Intersect((const geos::geom::Geometry *)measPolygon1.Polys(), 
                                             (const geos::geom::Geometry *)measPolygon2.Polys());

    for(int measureIndex = 2; measureIndex < pCnetPoint.Size(); measureIndex ++) {
      std::string sn3 = pCnetPoint[measureIndex].GetCubeSerialNumber();
      Cube *inCube3 = mCubeMgr.OpenCube(mSerialNumbers.Filename(sn3));
      inCube3->Read((Blob &)measPolygon3);

      // Get the intersection of the intersection and the measure Image Polygon
      geomIntersect2 = PolygonTools::Intersect(geomIntersect1, 
                                     (const geos::geom::Geometry *)measPolygon3.Polys());
      geomIntersect1 = geomIntersect2;
    }
    return (geos::geom::MultiPolygon *)geomIntersect1;
  }

  /**
   * This virtual method must return if the 1st fit is equal to or better
   * than the second fit.
   *
   * @param int1  1st interestAmount
   * @param int2  2nd interestAmount
   */
  bool InterestOperator::CompareInterests(double int1, double int2) {
    return(int1 >= int2);
  }


  // add this object's group to the pvl
  void InterestOperator::AddGroup(Isis::PvlObject &obj) {
    Isis::PvlGroup group;
    obj.AddGroup(group);
  }


  /**
   * Sets the clipping polygon for the chip. The coordinates must be in
   * (sample,line) order.
   *
   * @param clipPolygon  The polygons used to clip the chip
   */
  void InterestOperator::SetClipPolygon(const geos::geom::MultiPolygon &clipPolygon) {
    if(p_clipPolygon != NULL) delete p_clipPolygon;
    p_clipPolygon = PolygonTools::CopyMultiPolygon(clipPolygon);
  }

  /**
   * Sets an offset to pass in larger chips if operator requires it
   * This is used to offset the subchip size passed into Interest
   *
   * @return int   Amount to add to both x & y total sizes
   */
  int InterestOperator::Padding() {
    return 0;
  }

  /**
   * This function returns the keywords that this object was
   * created from.
   *
   * @return PvlGroup The keywords this object used in
   *         initialization
   */
  PvlGroup InterestOperator::Operator() {
    return mOperatorGrp;
  }
}
