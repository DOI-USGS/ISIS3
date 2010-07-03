#include "CnetValidMeasure.h"
#include "Cube.h"
#include "Camera.h"
#include "CubeManager.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"
#include "UniversalGroundMap.h"

namespace Isis {

  /**
   * Constructor - Initializes the data members and
   * parses the input Pvl . The Pvl Def File is optional.
   *
   * @author Sharmila Prasad (5/13/2010)
   *
   * @param pPvl
   */
  CnetValidMeasure::CnetValidMeasure(Pvl *pPvl) {
    InitStdOptions();

    if(pPvl != 0) {
      Parse(*pPvl);
    }
    else {
      InitStdOptionsGroup();
    }
    mStatisticsGrp = PvlGroup("Statistics");
  }

  /**
   * Constructor with a reference to Pvl Def file. Used for
   * Interest Operator where Def File is a requirement
   *
   * @author Sharmila Prasad (6/8/2010)
   *
   * @param pPvl
   */
  CnetValidMeasure::CnetValidMeasure(Pvl &pPvl) {
    InitStdOptions();
    Parse(pPvl);
    mStatisticsGrp = PvlGroup("Statistics");
  }

  /**
   * Init all the standard options to default
   *
   * @author Sharmila Prasad (6/8/2010)
   */
  void CnetValidMeasure::InitStdOptions(void) {
    mdMinEmissionAngle  = 0;
    mdMaxEmissionAngle  = 135;
    mdMinIncidenceAngle = 0;
    mdMaxIncidenceAngle = 135;;
    miPixelsFromEdge    = 0;
    mdMinResolution     = 0;
    mdMaxResolution     = DBL_MAX;
    mdMinDN             = Isis::ValidMinimum;
    mdMaxDN             = Isis::ValidMaximum;
    miPixelsFromEdge    = 0;
    mdMetersFromEdge    = 0;
  }

  /**
   * Set the Standard Options group for logging.
   *
   * @author Sharmila Prasad (6/8/2010)
   */
  void CnetValidMeasure::InitStdOptionsGroup(void) {
    mStdOptionsGrp = PvlGroup("StandardOptions");

    mStdOptionsGrp += Isis::PvlKeyword("MinDN",          mdMinDN);
    mStdOptionsGrp += Isis::PvlKeyword("MaxDN",          mdMaxDN);
    mStdOptionsGrp += Isis::PvlKeyword("MinEmission",    mdMinEmissionAngle);
    mStdOptionsGrp += Isis::PvlKeyword("MaxEmission",    mdMaxEmissionAngle);
    mStdOptionsGrp += Isis::PvlKeyword("MinIncidence",   mdMinIncidenceAngle);
    mStdOptionsGrp += Isis::PvlKeyword("MaxIncidence",   mdMaxIncidenceAngle);
    mStdOptionsGrp += Isis::PvlKeyword("MinResolution",  mdMinResolution);
    mStdOptionsGrp += Isis::PvlKeyword("MaxResolution",  mdMaxResolution);
    mStdOptionsGrp += Isis::PvlKeyword("PixelsFromEdge", miPixelsFromEdge);
    mStdOptionsGrp += Isis::PvlKeyword("MetersFromEdge", mdMetersFromEdge);
  }

  /**
   * Destructor: clean up stuff relevant for this class
   *
   * @author Sharmila Prasad (6/3/2010)
   */
  CnetValidMeasure::~CnetValidMeasure() {
    mCubeMgr.CleanCubes();
  }

  /**
   * Read Serial Numbers from specified file and populate the Cube and UniversalGround Maps
   * using the serial numbers
   *
   * @author Sharmila Prasad (6/3/2010)
   *
   * @param psSerialNumfile
   */
  void CnetValidMeasure::ReadSerialNumbers(std::string psSerialNumfile) {
    mSerialNumbers = SerialNumberList(psSerialNumfile, true, &mStatus);

    mCubeMgr.SetNumOpenCubes(50);
  }

  /**
   * Virtual member that parses the common Cnet Options and
   * checks for their validity
   *
   * @author Sharmila Prasad (5/13/2010)
   *
   * @param pvl
   */
  void CnetValidMeasure::Parse(Pvl &pvlDef) {
    mPvlOpGrp = pvlDef.FindGroup("Operator", Pvl::Traverse);

    mStdOptionsGrp = PvlGroup("StandardOptions");

    ValidatePvlDN();
    ValidatePvlEmissionAngle();
    ValidatePvlIncidenceAngle();
    ValidatePvlResolution();
    ValidatePvlFromEdge();

    mPvlLog += mStdOptionsGrp;
  }

  /**
   * Validate a measure for all the Standard Options
   *
   * @author sprasad (6/22/2010)
   *
   * @param piSample
   * @param piLine
   * @param pCube
   *
   * @return bool
   */
  bool CnetValidMeasure::ValidStandardOptions(double pdSample, double pdLine, Cube *pCube, PvlGroup *pMeasureGrp) {
    // Get the Camera
    Camera *measureCamera;
    try {
      measureCamera = pCube->Camera();
    }
    catch(Isis::iException &e) {
      std::string msg = "Cannot Create Camera for Image:" + pCube->Filename();
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    measureCamera->SetImage(pdSample, pdLine);

    mdEmissionAngle  = measureCamera->EmissionAngle();
    mdIncidenceAngle = measureCamera->IncidenceAngle();
    mdResolution     = measureCamera->PixelResolution();

    Isis::Portal inPortal(1, 1, pCube->PixelType());
    inPortal.SetPosition(pdSample, pdLine, 1);
    pCube->Read(inPortal);
    mdDnValue = inPortal[0];

    if(pMeasureGrp != NULL) {
      *pMeasureGrp += Isis::PvlKeyword("EmissionAngle",  mdEmissionAngle);
      *pMeasureGrp += Isis::PvlKeyword("IncidenceAngle", mdIncidenceAngle);
      *pMeasureGrp += Isis::PvlKeyword("DNValue",        mdDnValue);
      *pMeasureGrp += Isis::PvlKeyword("Resolution",     mdResolution);
    }

    if(ValidEmissionAngle(mdEmissionAngle) && ValidIncidenceAngle(mdIncidenceAngle) &&
        ValidDnValue(mdDnValue) && ValidResolution(mdResolution) &&
        PixelsFromEdge((int)pdSample, (int)pdLine, pCube) && MetersFromEdge((int)pdSample, (int)pdLine, pCube)) {
      return true;
    }
    return false;
  }

  /**
   * Validate and Read the Pixels and Meters from Edge Standard
   * Options
   *
   * @author sprasad (6/22/2010)
   */
  void CnetValidMeasure::ValidatePvlFromEdge(void) {
    // Parse the Pixels from edge
    if(mPvlOpGrp.HasKeyword("PixelsFromEdge")) {
      miPixelsFromEdge = mPvlOpGrp["PixelsFromEdge"];
      if(miPixelsFromEdge < 0) {
        miPixelsFromEdge = 0;
      }
    }
    mStdOptionsGrp += Isis::PvlKeyword("PixelsFromEdge", miPixelsFromEdge);

    // Parse the Meters from edge
    if(mPvlOpGrp.HasKeyword("MetersFromEdge")) {
      mdMetersFromEdge = mPvlOpGrp["MetersFromEdge"];
      if(mdMetersFromEdge < 0) {
        mdMetersFromEdge = 0;
      }
    }
    mStdOptionsGrp += Isis::PvlKeyword("MetersFromEdge", mdMetersFromEdge);
  }

  /**
   * Validate the Min and Max Resolution Values set by the user in the Operator pvl file.
   * If not set then set the options to default and enter their names in the Unused Group.
   * If the user set values are invalid then exception is thrown.
   *
   * @author Sharmila Prasad (6/4/2010)
   */
  void CnetValidMeasure::ValidatePvlResolution(void) {
    if(mPvlOpGrp.HasKeyword("MinResolution"))
      mdMinResolution = mPvlOpGrp["MinResolution"];
    else {
      mdMinResolution = 0;
    }
    mStdOptionsGrp += Isis::PvlKeyword("MinResolution", mdMinResolution);

    if(mPvlOpGrp.HasKeyword("MaxResolution"))
      mdMaxResolution = mPvlOpGrp["MaxResolution"];
    else {
      mdMaxResolution = DBL_MAX;
    }
    mStdOptionsGrp += Isis::PvlKeyword("MaxResolution", mdMaxResolution);

    if(mdMinResolution < 0 || mdMaxResolution < 0) {
      std::string msg = "Invalid Resolution value(s), Resolution must be greater than zero";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    if(mdMaxResolution < mdMinResolution) {
      std::string msg = "MinResolution must be less than MaxResolution";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  /**
   * Validate the Min and Max Dn Values set by the user in the Operator pvl file.
   * If not set then set the options to default and enter their names in the Unused Group.
   * If the user set values are invalid then exception is thrown.
   *
   * @author Sharmila Prasad (5/10/2010)
   *
   */
  void CnetValidMeasure::ValidatePvlDN(void) {
    if(mPvlOpGrp.HasKeyword("MinDN"))
      mdMinDN = mPvlOpGrp["MinDN"];
    else {
      mdMinDN = Isis::ValidMinimum;
    }
    mStdOptionsGrp += Isis::PvlKeyword("MinDN", mdMinDN);

    if(mPvlOpGrp.HasKeyword("MaxDN"))
      mdMaxDN = mPvlOpGrp["MaxDN"];
    else {
      mdMaxDN = Isis::ValidMaximum;
    }
    mStdOptionsGrp += Isis::PvlKeyword("MaxDN", mdMaxDN);

    if(mdMaxDN < mdMinDN) {
      std::string msg = "MinDN must be less than MaxDN";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  /**
   * ValidateEmissionAngle: Validate the Min and Max Emission Values set by the user in the Operator pvl file.
   * If not set then set the options to default and enter their names in the Unused Group.
   * If the user set values are invalid then exception is thrown, the valid range being [0-135]
   *
   * @author Sharmila Prasad (5/10/2010)
   *
   */
  void CnetValidMeasure::ValidatePvlEmissionAngle(void) {
    if(mPvlOpGrp.HasKeyword("MinEmission")) {
      mdMinEmissionAngle = mPvlOpGrp["MinEmission"];
      if(mdMinEmissionAngle < 0 || mdMinEmissionAngle > 135) {
        std::string msg = "Invalid Min Emission Angle, Valid Range is [0-135]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    mStdOptionsGrp += Isis::PvlKeyword("MinEmission", mdMinEmissionAngle);

    if(mPvlOpGrp.HasKeyword("MaxEmission")) {
      mdMaxEmissionAngle = mPvlOpGrp["MaxEmission"];
      if(mdMaxEmissionAngle < 0 || mdMaxEmissionAngle > 135) {
        std::string msg = "Invalid Max Emission Angle, Valid Range is [0-135]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    mStdOptionsGrp += Isis::PvlKeyword("MaxEmission", mdMaxEmissionAngle);

    if(mdMaxEmissionAngle < mdMinEmissionAngle) {
      std::string msg = "Min EmissionAngle must be less than Max EmissionAngle";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

  }

  /**
   * ValidateIncidenceAngle: Validate the Min and Max Incidence Values set by the user in the Operator pvl file.
   * If not set then set the options to default and enter their names in the Unused Group.
   * If the user set values are invalid then exception is thrown, the valid range being [0-135]
   *
   * @author Sharmila Prasad (5/10/2010)
   *
   *
   */
  void CnetValidMeasure::ValidatePvlIncidenceAngle(void) {
    if(mPvlOpGrp.HasKeyword("MinIncidence")) {
      mdMinIncidenceAngle = mPvlOpGrp["MinIncidence"];
      if(mdMinIncidenceAngle < 0 || mdMinIncidenceAngle > 135) {
        std::string msg = "Invalid Min Incidence Angle, Valid Range is [0-135]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    mStdOptionsGrp += Isis::PvlKeyword("MinIncidence", mdMinIncidenceAngle);

    if(mPvlOpGrp.HasKeyword("MaxIncidence")) {
      mdMaxIncidenceAngle = mPvlOpGrp["MaxIncidence"];
      if(mdMaxIncidenceAngle < 0 || mdMaxIncidenceAngle > 135) {
        std::string msg = "Invalid Max Incidence Angle, Valid Range is [0-135]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    mStdOptionsGrp += Isis::PvlKeyword("MaxIncidence", mdMaxIncidenceAngle);

    if(mdMaxIncidenceAngle < mdMinIncidenceAngle) {
      std::string msg = "Min IncidenceAngle must be less than Max IncidenceAngle";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Validates an Emission angle by comparing with the min and max values in the def file.
   * If Emission Angle is greater or lesser than the max/min values in the def file or the defaults
   * it returns false else true.
   *
   * @author Sharmila Prasad (3/30/2010)
   *
   *
   * @return bool
   */
  bool CnetValidMeasure::ValidEmissionAngle(double pdEmissionAngle) {
    if(pdEmissionAngle < mdMinEmissionAngle || pdEmissionAngle > mdMaxEmissionAngle) {
      return false;
    }
    return true;
  }

  /**
   * Validates an Incidence angle by comparing with the min and max values in the def file.
   * If Incidence Angle is greater or lesser than the max/min values in the def file or the defaults
   * it returns false else true.
   *
   * @author Sharmila Prasad (5/10/2010)
   *
   *
   * @return bool
   */
  bool CnetValidMeasure::ValidIncidenceAngle(double pdIncidenceAngle) {
    if(pdIncidenceAngle < mdMinIncidenceAngle || pdIncidenceAngle > mdMaxIncidenceAngle) {
      return false;
    }
    return true;
  }

  /**
   * Validates Dn Value by comparing against the Min and Max DN Values set in the
   * def file or the defaults.
   *
   * @author Sharmila Prasad (3/30/2010)
   *
   * @return bool
   */
  bool CnetValidMeasure::ValidDnValue(double pdDnValue) {
    if(Isis::IsSpecial(pdDnValue) || pdDnValue < mdMinDN || pdDnValue >  mdMaxDN) {
      return false;
    }
    return true;
  }

  /**
   * Validates Dn Value by comparing against the Min and Max DN Values set in the
   * def file or the defaults.
   *
   * @author Sharmila Prasad (6/4/2010)
   *
   * @return bool
   */
  bool CnetValidMeasure::ValidResolution(double pdResolution) {
    if(pdResolution < mdMinResolution || pdResolution >  mdMaxResolution) {
      return false;
    }
    return true;
  }

  /**
   * Validate if a point has a valid lat, lon for that camera
   *
   * @author Sharmila Prasad (6/4/2010)
   *
   * @param pCamera
   * @param piSample
   * @param piLine
   *
   * @return bool
   */
  bool CnetValidMeasure::ValidLatLon(Camera *pCamera, int piSample, int piLine) {
    return true;
  }

  /**
   * Validate if a point is user defined number of pixels from the edge
   *
   * @author Sharmila Prasad (6/21/2010)
   *
   * @param piSample
   * @param piLine
   * @param pCube
   *
   * @return bool
   */
  bool CnetValidMeasure::PixelsFromEdge(int piSample, int piLine, Cube *pCube) {
    if(miPixelsFromEdge <= 0) {
      return true;
    }

    int iNumSamples = pCube->Samples();
    int iNumLines   = pCube->Lines();

    // test right
    if((iNumSamples - piSample) < miPixelsFromEdge)  {
      return false;
    }

    // test left
    if((piSample - miPixelsFromEdge) <= 0)  {
      return false;
    }

    // test down
    if((iNumLines - piLine) < miPixelsFromEdge)  {
      return false;
    }

    // test up
    if((piLine - miPixelsFromEdge) <= 0) {
      return false;
    }

    return true;
  }

  /**
   * Validate if a point is user defined number of meters from the edge
   *
   * @author Sharmila Prasad (6/21/2010)
   *
   * @param piSample
   * @param piLine
   * @param pCube
   *
   * @return bool
   */
  bool CnetValidMeasure::MetersFromEdge(int piSample, int piLine, Cube *pCube) {
    if(mdMetersFromEdge <= 0) {
      return true;
    }

    int iNumSamples = pCube->Samples();
    int iNumLines   = pCube->Lines();

    try {
      // Get the image's camera to get pixel resolution
      Camera *camera = pCube->Camera();
      double resMetersTotal = 0;
      bool bMinDistance     = false;

      // test top
      for(int line = piLine - 1; line > 0; line--) {
        camera->SetImage(piSample, line);
        double resolution = camera->PixelResolution();
        resMetersTotal += resolution;
        if(resMetersTotal >= mdMetersFromEdge) {
          bMinDistance = true;
          break;
        }
      }
      if(!bMinDistance) {
        return false;
      }

      // test bottom
      bMinDistance   = false;
      resMetersTotal = 0;
      for(int line = piLine + 1; line <= iNumLines; line++) {
        camera->SetImage(piSample, line);
        double resolution = camera->PixelResolution();
        resMetersTotal += resolution;
        if(resMetersTotal >= mdMetersFromEdge) {
          bMinDistance = true;
          break;
        }
      }
      if(!bMinDistance) {
        return false;
      }

      // test left
      resMetersTotal = 0;
      bMinDistance   = false;
      for(int sample = piSample - 1; sample > 0; sample--) {
        camera->SetImage(sample, piLine);
        double resolution = camera->PixelResolution();
        resMetersTotal += resolution;
        if(resMetersTotal >= mdMetersFromEdge) {
          bMinDistance = true;
          break;
        }
      }
      if(!bMinDistance) {
        return false;
      }

      // test right
      resMetersTotal = 0;
      bMinDistance   = false;
      for(int sample = piSample + 1; sample <= iNumSamples; sample++) {
        camera->SetImage(sample, piLine);
        double resolution = camera->PixelResolution();
        resMetersTotal += resolution;
        if(resMetersTotal >= mdMetersFromEdge) {
          return true;
        }
      }
      return false;
    }
    catch(iException &e) {
      std::string msg = "Cannot Create Camera for Image:" + pCube->Filename();
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }

};

