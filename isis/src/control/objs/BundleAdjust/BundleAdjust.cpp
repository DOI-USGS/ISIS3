#include "BundleAdjust.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "SpecialPixel.h"
#include "BasisFunction.h"
#include "LeastSquares.h"
#include "CameraGroundMap.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "ControlPoint.h"
#include "SpicePosition.h"
#include "Application.h"
#include "Camera.h"
#include "CSVReader.h"
#include "SurfacePoint.h"
#include "Latitude.h"
#include "Longitude.h"
#include "iTime.h"

#include "boost/numeric/ublas/matrix_sparse.hpp"
#include "boost/numeric/ublas/vector_proxy.hpp"
#include "boost/lexical_cast.hpp"

using namespace boost::numeric::ublas;

namespace Isis {

static void cholmod_error_handler(int nStatus, const char* file, int nLineNo,
      const char* message) {

    std::string errlog;

    errlog = "SPARSE: ";
    errlog += message;

    PvlGroup gp(errlog);

    gp += PvlKeyword("File",file);
    gp += PvlKeyword("Line_Number", nLineNo);
    gp += PvlKeyword("Status", nStatus);

    Application::Log(gp);

    errlog += ". (See print.prt for details)";
    throw iException::Message(iException::Math, errlog, _FILEINFO_);
  }

  BundleAdjust::BundleAdjust(const std::string &cnetFile,
                             const std::string &cubeList,
                             bool bPrintSummary) {
    Progress progress;
    m_bCleanUp = true;
    m_pCnet = new Isis::ControlNet(cnetFile, &progress);
    m_pSnList = new Isis::SerialNumberList(cubeList);
    m_pHeldSnList = NULL;
    m_bPrintSummary = bPrintSummary;
    m_strCnetFilename = cnetFile;
    m_strOutputFilePrefix = "";
    m_bDeltack = false;

    Init(&progress);
  }

  BundleAdjust::BundleAdjust(const std::string &cnetFile,
                             const std::string &cubeList,
                             const std::string &heldList,
                             bool bPrintSummary) {
    Progress progress;
    m_bCleanUp = true;
    m_pCnet = new Isis::ControlNet(cnetFile, &progress);
    m_pSnList = new Isis::SerialNumberList(cubeList);
    m_pHeldSnList = new Isis::SerialNumberList(heldList);
    m_bPrintSummary = bPrintSummary;
    m_strCnetFilename = cnetFile;
    m_strOutputFilePrefix = "";
    m_bDeltack = false;

    Init(&progress);
  }

  BundleAdjust::BundleAdjust(Isis::ControlNet &cnet,
                             Isis::SerialNumberList &snlist,
                             bool bPrintSummary) {
    m_bCleanUp = false;
    m_pCnet = &cnet;
    m_pSnList = &snlist;
    m_pHeldSnList = NULL;
    m_bPrintSummary = bPrintSummary;
    m_dConvergenceThreshold = 0.;    // This is needed for deltack???
    m_strCnetFilename = "";
    m_strOutputFilePrefix = "";
    m_bDeltack = true;

    Init();
  }

  BundleAdjust::BundleAdjust(Isis::ControlNet &cnet,
                             Isis::SerialNumberList &snlist,
                             Isis::SerialNumberList &heldsnlist,
                             bool bPrintSummary) {
    m_bCleanUp = false;
    m_pCnet = &cnet;
    m_pSnList = &snlist;
    m_pHeldSnList = &heldsnlist;
    m_bPrintSummary = bPrintSummary;
    m_strCnetFilename = "";
    m_strOutputFilePrefix = "";
    m_bDeltack = false;

    Init();
  }

  BundleAdjust::~BundleAdjust() {
    // If we have ownership 
    if (m_bCleanUp) {
      delete m_pCnet;
      delete m_pSnList;

      if (m_nHeldImages > 0)
        delete m_pHeldSnList;

      if (m_bObservationMode)
        delete m_pObsNumList;
    }

    if ( m_pLsq )
      delete m_pLsq;

    if ( m_strSolutionMethod == "SPARSE" )
      freeCholMod();
  }

  bool BundleAdjust::ReadSCSigmas(const std::string &scsigmasList) {
    CSVReader csv;
    csv.setSkip(20);

    try {
      csv.read(scsigmasList);
    }
    catch (iException &e) {
      std::string msg = "Failed to read spacecraft sigmas file";
      throw Isis::iException::Message(iException::Io, msg, _FILEINFO_);
    }

    int nrows = csv.rows();
    m_SCWeights.resize(nrows);

    for (int i = 0; i < nrows; i++) {
      CSVReader::CSVAxis row = csv.getRow(i);
      int ntokens = row.dim();
      int nsigmas = ntokens - 2;

      SpacecraftWeights &scs = m_SCWeights[i];

      scs.SpacecraftName = row[0];
      scs.InstrumentId = row[1];
      scs.weights.resize(6);

      for (int j = 0; j < nsigmas; j++) {
        std::string str = row[j+2];

        double d = atof(str.c_str());
        if (d == 0.0)
          continue;

        if (j < 3)
          scs.weights[j] = 1.0e+6 / (d * d); // position - input units are m and converted to km, km/s, km/s/s
        else
          scs.weights[j] = 1.0 / (DEG2RAD * DEG2RAD * d * d); // angles - input units are decimal degrees, converted to rads, rads/s, rads/s/s
      }
    }

    return true;
  }

/**
   * Initialize solution parameters
   *
   * @internal
   *   @history 2011-08-14 Debbie A. Cook - Opt out of network validation
   *                      for deltack network in order to allow
   *                      a single measure on a point
   */
  void BundleAdjust::Init(Progress *progress) {
//printf("BOOST_UBLAS_CHECK_ENABLE = %d\n", BOOST_UBLAS_CHECK_ENABLE);
//printf("BOOST_UBLAS_TYPE_CHECK = %d\n", BOOST_UBLAS_TYPE_CHECK);

//    m_pProgressBar = progress;

    m_dError = DBL_MAX;
    m_bSimulatedData = true;
    m_bObservationMode = false;
    m_strSolutionMethod = "SPECIALK";
    m_pObsNumList = NULL;
    m_pLsq = NULL;
    m_dElapsedTimeErrorProp = 0.0;
    m_dElapsedTime = 0.0;

    // Get the cameras set up for all images
    m_pCnet->SetImages(*m_pSnList, progress);

    // clear JigsawRejected flags
    m_pCnet->ClearJigsawRejected();

    m_nHeldImages = 0;
    int nImages = m_pSnList->Size();

    // Create the image index
    if (m_pHeldSnList != NULL) {
      //Check to make sure held images are in the control net
      CheckHeldList();

      // Get a count of held images too
      int count = 0;
      for ( int i = 0; i < nImages; i++ ) {
        if ( m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)) )
          m_nHeldImages++;

        m_nImageIndexMap.push_back(count);
        count++;
      }
    }
    else {
      for (int i = 0; i < nImages; i++)
        m_nImageIndexMap.push_back(i);
    }

    FillPointIndexMap();

    // Set default variables to solve for
    m_bSolveTwist = true;
    m_bSolveRadii = false;
    m_bErrorPropagation = false;
    m_bMaxIterationsReached = false;
    m_cmatrixSolveType = AnglesOnly;
    m_spacecraftPositionSolveType = Nothing;
    m_nckDegree = 2;
    m_nsolveCamDegree = m_nckDegree;
    m_nNumberCameraCoefSolved = 1;
    m_nUnknownParameters = 0;
    m_bOutputStandard = true;
    m_bOutputCSV = true;
    m_bOutputResiduals = true;

    m_dGlobalLatitudeAprioriSigma = 1000.0;
    m_dGlobalLongitudeAprioriSigma = 1000.0;
    m_dGlobalRadiusAprioriSigma = 1000.0;
//    m_dGlobalSurfaceXAprioriSigma = 1000.0;
//    m_dGlobalSurfaceYAprioriSigma = 1000.0;
//    m_dGlobalSurfaceZAprioriSigma = 1000.0;
    m_dGlobalSpacecraftPositionAprioriSigma = -1.0;
    m_dGlobalSpacecraftVelocityAprioriSigma = -1.0;
    m_dGlobalSpacecraftAccelerationAprioriSigma = -1.0;

//    m_dGlobalCameraAnglesAprioriSigma = -1.0;
//    m_dGlobalCameraAngularVelocityAprioriSigma = -1.0;
//    m_dGlobalCameraAngularAccelerationAprioriSigma = -1.0;

    m_dGlobalSpacecraftPositionWeight = 0.0;
    m_dGlobalSpacecraftVelocityWeight = 0.0;
    m_dGlobalSpacecraftAccelerationWeight = 0.0;
    m_dGlobalCameraAnglesWeight = 0.0;
    m_dGlobalCameraAngularVelocityWeight = 0.0;
    m_dGlobalCameraAngularAccelerationWeight = 0.0;

    m_dConvergenceThreshold = 1.0e-10;
    m_nRejectedObservations = 0;

    if (!m_bSolveRadii)
      m_dGlobalRadiusAprioriSigma *= -1.0;

    // (must be a smarter way)
    // get target body radii and body
    // specific conversion factors between
    // radians and meters
    // need validity checks and different
    // conversion factors for lat and long
    m_BodyRadii[0] = m_BodyRadii[1] = m_BodyRadii[2] = Distance();
    Camera *pCamera = m_pCnet->Camera(0);
    if (pCamera) {
      pCamera->Radii(m_BodyRadii);  // meters

//      printf("radii: %lf %lf %lf\n",m_BodyRadii[0],m_BodyRadii[1],m_BodyRadii[2]);

      if (m_BodyRadii[0] >= Distance(0, Distance::Meters)) {
        m_dMTR = 0.001 / m_BodyRadii[0].GetKilometers(); // at equator
        m_dRTM = 1.0 / m_dMTR;
      }
//      printf("MTR: %lf\nRTM: %lf\n",m_dMTR,m_dRTM);
    }

    // TODO:  Need to have some validation code to make sure everything is
    // on the up-and-up with the control network.  Add checks for multiple
    // networks, images without any points, and points on images removed from
    // the control net (when we start adding software to remove points with high
    // residuals) and ?.  For "deltack" a single measure on a point is allowed
    // so skip the test.
    if (!m_bDeltack) validateNetwork();
  }

  /**
   * control network validation - on the very real chance that the net
   * has not been checked before running the bundle
   *
   * checks implemented for ...
   *  (1) images with 0 or 1 measures
   * @internal
   *   @history  2011-08-4 Debbie A. Cook - Changed error message to 
   *                        indicate it fails with one measure as 
   *                        well as no measures.
   */
  bool BundleAdjust::validateNetwork() {
    printf("Validating network...\n");

    // verify measures exist for all images
    int nimagesWithInsufficientMeasures = 0;
    std::string msg = "Images with one or less measures:\n";
    int nImages = m_pSnList->Size();
    for (int i = 0; i < nImages; i++) {
      int nMeasures =
        m_pCnet->GetNumberOfMeasuresInImage(m_pSnList->SerialNumber(i));

      if ( nMeasures > 1 )
        continue;

      nimagesWithInsufficientMeasures++;
      msg += m_pSnList->Filename(i) + ": " + iString(nMeasures) + "\n";
    }
    if ( nimagesWithInsufficientMeasures > 0 ) {
        throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    return true;
  }

  /**
   * Initializations for Cholmod sparse matrix package
   */
  bool BundleAdjust::initializeCholMod() {

      if( m_nRank <= 0 )
          return false;

      m_pTriplet = NULL;

      cholmod_start(&m_cm);

      // set user-defined cholmod error handler
      m_cm.error_handler = cholmod_error_handler;

      // testing not using metis
      m_cm.nmethods = 1;
      m_cm.method[0].ordering = CHOLMOD_AMD;

      // set size of sparse block normal equations matrix
      m_SparseNormals.setNumberOfColumns( Images() );

      return true;
  }

  /**
   * Initializations for Cholmod sparse matrix package
   */
  bool BundleAdjust::freeCholMod() {

    cholmod_free_triplet(&m_pTriplet, &m_cm);
    cholmod_free_sparse(&m_N, &m_cm);
    cholmod_free_factor(&m_L, &m_cm);

    cholmod_finish(&m_cm);

    return true;
  }

  /**
   * This method fills the point index map and needs to know the solution
   * method in order to work properly.
   */
  void BundleAdjust::FillPointIndexMap() {

    // Create a lookup table of ignored, and fixed points
    // TODO  Deal with edit lock points
    m_nFixedPoints = m_nIgnoredPoints = 0;
    int count = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();

    for (int i = 0; i < nObjectPoints; i++)     {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if (point->IsIgnored()) {
        m_nPointIndexMap.push_back(-1);
        m_nIgnoredPoints++;
        continue;
      }

      else if (point->GetType() == ControlPoint::Fixed) {
        m_nFixedPoints++;

        if ( m_strSolutionMethod == "SPECIALK"  ||
             m_strSolutionMethod == "SPARSE" ||
             m_strSolutionMethod == "OLDSPARSE" ) {
          m_nPointIndexMap.push_back(count);
          count++;
        }
        else
          m_nPointIndexMap.push_back(-1);
      }

      else {
        m_nPointIndexMap.push_back(count);
        count++;
      }
    }
  }

  /**
   * This method checks all cube files in the held list to make sure they are in the
   * input list.
   */
  void BundleAdjust::CheckHeldList() {
    for (int ih = 0; ih < m_pHeldSnList->Size(); ih++) {
      if (!(m_pSnList->HasSerialNumber(m_pHeldSnList->SerialNumber(ih)))) {
        std::string msg = "Held image [" + m_pHeldSnList->SerialNumber(ih)
                          + "not in FROMLIST";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }

  /**
   * This method determines the number of partials per image and
   * per point.  It is based on the variables to be solved for
   * (e.g., twist, radii, cmatrix velocity, cmatrix acceleration,
   * etc)
   */
  void BundleAdjust::ComputeNumberPartials() {
    m_nNumImagePartials = 0;

    if (m_cmatrixSolveType != None) {
      // Solve for ra/dec always
      m_nNumImagePartials = 2;

      // Do we solve for twist
      if (m_bSolveTwist)
        m_nNumImagePartials++;

      // Do we solve for angles only, +velocity, or +velocity and acceleration, or all coefficients
      m_nNumImagePartials *= m_nNumberCameraCoefSolved;
      /*      if (m_cmatrixSolveType == AnglesVelocity) {
              m_nNumImagePartials *= 2;
            }
            else if (m_cmatrixSolveType == AnglesVelocityAcceleration) {
              m_nNumImagePartials *= 3;
            }*/
    }

    if (m_spacecraftPositionSolveType != Nothing) {
      // Solve for position always.
      m_nNumImagePartials += 3;

      // Do we solve for position and velocity, position, velocity and acceleration, or position only
      if (m_spacecraftPositionSolveType == PositionVelocity)
        m_nNumImagePartials += 3;
      else if (m_spacecraftPositionSolveType == PositionVelocityAcceleration)
        m_nNumImagePartials += 6;
    }

    // Revised to solve for x/y/z always
    // Solve for lat/lon always
    // 2010-03-01 KLE now always solving for all 3 coordinates,
    // but now, we "hold", "fix", or constrain via weights
    m_nNumPointPartials = 3;


    // Test code to match old test runs which don't solve for radius
    if ( m_strSolutionMethod != "SPECIALK"  &&
        m_strSolutionMethod != "SPARSE" &&
        m_strSolutionMethod != "OLDSPARSE" ) {
      m_nNumPointPartials = 2;

      if (m_bSolveRadii) 
        m_nNumPointPartials++;
    }
  }

  /**
   * Weighting for image parameter
   * ComputeNumberPartials must be called first
   */
  void BundleAdjust::ComputeImageParameterWeights() {
    // size and initialize to 0.0
    m_dImageParameterWeights.resize(m_nNumImagePartials);
    for (int i = 0; i < m_nNumImagePartials; i++)
      m_dImageParameterWeights[i] = 0.0;

    int nIndex = 0;
    if (m_spacecraftPositionSolveType == PositionOnly) {
      m_dImageParameterWeights[0] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[1] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[2] = m_dGlobalSpacecraftPositionWeight;
      nIndex += 3;
    }
    else if (m_spacecraftPositionSolveType == PositionVelocity) {
      m_dImageParameterWeights[0] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[1] = m_dGlobalSpacecraftVelocityWeight;
      m_dImageParameterWeights[2] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[3] = m_dGlobalSpacecraftVelocityWeight;
      m_dImageParameterWeights[4] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[5] = m_dGlobalSpacecraftVelocityWeight;
      nIndex += 6;
    }
    else if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
      m_dImageParameterWeights[0] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[1] = m_dGlobalSpacecraftVelocityWeight;
      m_dImageParameterWeights[2] = m_dGlobalSpacecraftAccelerationWeight;
      m_dImageParameterWeights[3] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[4] = m_dGlobalSpacecraftVelocityWeight;
      m_dImageParameterWeights[5] = m_dGlobalSpacecraftAccelerationWeight;
      m_dImageParameterWeights[6] = m_dGlobalSpacecraftPositionWeight;
      m_dImageParameterWeights[7] = m_dGlobalSpacecraftVelocityWeight;
      m_dImageParameterWeights[8] = m_dGlobalSpacecraftAccelerationWeight;
      nIndex += 9;
    }

    if (m_cmatrixSolveType == AnglesOnly) {
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;

      if (m_bSolveTwist)
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
    }
    else if (m_cmatrixSolveType == AnglesVelocity) {
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      if (m_bSolveTwist) {
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      }
    }
    if (m_cmatrixSolveType >= AnglesVelocityAcceleration) {
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationWeight;

      if (m_bSolveTwist) {
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationWeight;
      }
    }    
  }

  /**
   * This method turns on observation mode and creates the observation number list.
   * It also checks to make sure the held image list is consistent for all images in
   * an observation.
   *
   * Is this still going to be necessary.  Is the code that uses it still intact?
   */
  void BundleAdjust::SetObservationMode(bool observationMode) {
    m_bObservationMode = observationMode;

    if (!m_bObservationMode)
      return;

    // Create the observation number list
    m_pObsNumList = new Isis::ObservationNumberList(m_pSnList);

    if (m_pHeldSnList == NULL)
      return;

    // make sure ALL images in an observation are held if any are
    for (int ih = 0; ih < m_pHeldSnList->Size(); ih++) {
      for (int isn = 0; isn < m_pSnList->Size(); isn++) {
        if (m_pHeldSnList->ObservationNumber(ih) != m_pSnList->ObservationNumber(isn))
          continue;

        if (m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(isn)))
          continue;

        std::string msg = "Cube file " + m_pSnList->Filename(isn)
                          + " must be held since it is on the same observation as held cube "
                          + m_pHeldSnList->Filename(ih);
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }

  /**
   * Set decomposition method. Choices are...
   * SpecialK (dense normal equations matrix)
   * Sparse (Cholmod sparse normal equations matrix)
   */
  void BundleAdjust::SetDecompositionMethod(DecompositionMethod method) {
    m_decompositionMethod = method;
  }

  /**
   * For which camera angle coefficients do we solve?
   */
  void BundleAdjust::SetSolveCmatrix(CmatrixSolveType type) {
    m_cmatrixSolveType = type;

    switch (type) {
      case BundleAdjust::AnglesOnly:
        m_nNumberCameraCoefSolved = 1;
        break;
      case BundleAdjust::AnglesVelocity:
        m_nNumberCameraCoefSolved = 2;
        break;
      case BundleAdjust::AnglesVelocityAcceleration:
        m_nNumberCameraCoefSolved = 3;
        break;
      case BundleAdjust::All:
        m_nNumberCameraCoefSolved = m_nsolveCamDegree + 1;
        break;
      default:
        m_nNumberCameraCoefSolved = 0;
        break;
    }

    m_dGlobalCameraAnglesAprioriSigma.resize(m_nNumberCameraCoefSolved);
    for( int i = 0; i < m_nNumberCameraCoefSolved; i++ )
        m_dGlobalCameraAnglesAprioriSigma[i] = -1.0;

    // Make sure the degree of the polynomial the user selected for
    // the camera angles fit is sufficient for the selected CAMSOLVE
    if (m_nNumberCameraCoefSolved > m_nsolveCamDegree + 1) {
      std::string msg = "Selected SolveCameraDegree " + iString(m_nsolveCamDegree)
                        + " is not sufficient for the CAMSOLVE";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  /**
   * Determine the number of columns we will need for the least
   * squares. When we create a row of data we will store all the
   * image partials first and then the point partials
   *
   */
  int BundleAdjust::BasisColumns() {
    m_nImageParameters = Observations() * m_nNumImagePartials;

    int nPointParameterColumns = m_pCnet->GetNumValidPoints() * m_nNumPointPartials;

    if (m_strSolutionMethod != "SPECIALK" &&
        m_strSolutionMethod != "SPARSE" &&
        m_strSolutionMethod != "OLDSPARSE")
      nPointParameterColumns -= m_nFixedPoints * m_nNumPointPartials;

    return m_nImageParameters + nPointParameterColumns;
  }

  /**
   * Initializes matrices and parameters for bundle adjustment
   */
  void BundleAdjust::Initialize() {

    // size of reduced normals matrix
    m_nRank = m_nNumImagePartials * Observations();

    int n3DPoints = m_pCnet->GetNumValidPoints();

    if ( m_decompositionMethod == SPECIALK ) {
      m_Normals.resize(m_nRank);           // set size of reduced normals matrix
      m_Normals.clear();                   // zero all elements
      m_Qs_SPECIALK.resize(n3DPoints);
    }
    else if ( m_decompositionMethod == CHOLMOD ) {
      m_Qs_CHOLMOD.resize(n3DPoints);
    }

    m_nUnknownParameters = m_nRank + 3 * n3DPoints;

    m_nRejectedObservations = 0;

    m_Image_Solution.resize(m_nRank);
    m_Image_Corrections.resize(m_nRank);
    m_NICs.resize(n3DPoints);
    m_Point_Corrections.resize(n3DPoints);
    m_Point_Weights.resize(n3DPoints);
    m_Point_AprioriSigmas.resize(n3DPoints);

    // initialize NICS, Qs, and point correction vectors to zero
    for (int i = 0; i < n3DPoints; i++) {
      m_NICs[i].clear();

      // TODO_CHOLMOD: is this needed with new cholmod implementation?
      if ( m_decompositionMethod == SPECIALK )
        m_Qs_SPECIALK[i].clear();
      else if ( m_decompositionMethod == CHOLMOD )
        m_Qs_CHOLMOD[i].clear();

      m_Point_Corrections[i].clear();
      m_Point_Weights[i].clear();
      m_Point_AprioriSigmas[i].clear();
    }

    m_bConverged = false;                // flag indicating convergence
    m_bError = false;                    // flag indicating general bundle error

    // convert apriori sigmas into weights (if they're negative or zero, we don't use them)
    SetSpaceCraftWeights();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // initializations for cholmod
    if ( m_strSolutionMethod == "SPARSE" )
      initializeCholMod();
  }

  void BundleAdjust::SetSpaceCraftWeights() {

    if (m_dGlobalSpacecraftPositionAprioriSigma > 0.0) {
      m_dGlobalSpacecraftPositionWeight
      = 1.0 / (m_dGlobalSpacecraftPositionAprioriSigma * m_dGlobalSpacecraftPositionAprioriSigma * 1.0e-6);
    }

    if (m_dGlobalSpacecraftVelocityAprioriSigma > 0.0) {
      m_dGlobalSpacecraftVelocityWeight
      = 1.0 / (m_dGlobalSpacecraftVelocityAprioriSigma * m_dGlobalSpacecraftVelocityAprioriSigma * 1.0e-6);
    }

    if (m_dGlobalSpacecraftAccelerationAprioriSigma > 0.0) {
      m_dGlobalSpacecraftAccelerationWeight
      = 1.0 / (m_dGlobalSpacecraftAccelerationAprioriSigma * m_dGlobalSpacecraftAccelerationAprioriSigma * 1.0e-6);
    }

    if ( m_nNumberCameraCoefSolved >= 1 ) {
        if ( m_dGlobalCameraAnglesAprioriSigma[0] > 0.0 ) {
            m_dGlobalCameraAnglesWeight
                    = 1.0 / (m_dGlobalCameraAnglesAprioriSigma[0] * m_dGlobalCameraAnglesAprioriSigma[0] * DEG2RAD * DEG2RAD);
        }
    }

//    if (m_dGlobalCameraAngularVelocityAprioriSigma > 0.0) {
    if ( m_nNumberCameraCoefSolved >= 2 ) {
        if ( m_dGlobalCameraAnglesAprioriSigma[1] > 0.0 ) {
            m_dGlobalCameraAngularVelocityWeight
                    = 1.0 / (m_dGlobalCameraAnglesAprioriSigma[1]  * m_dGlobalCameraAnglesAprioriSigma[1]  * DEG2RAD * DEG2RAD);
        }
    }

//    if (m_dGlobalCameraAngularAccelerationAprioriSigma > 0.0) {
    if ( m_nNumberCameraCoefSolved >= 3 ) {
        if( m_dGlobalCameraAnglesAprioriSigma[2] > 0.0 ) {
            m_dGlobalCameraAngularAccelerationWeight
                    = 1.0 / (m_dGlobalCameraAnglesAprioriSigma[2]  * m_dGlobalCameraAnglesAprioriSigma[2]  * DEG2RAD * DEG2RAD);
        }
    }
  }

  /**
   * The solve method is an least squares solution for updating the camera
   * pointing.  It is iterative as the equations are non-linear.  If it does
   * not iterate to a solution in maxIterations it will throw an error.  During
   * each iteration it is updating portions of the control net, as well as the
   * instrument pointing in the camera
   .  An error is thrown if it does not
   * converge in the maximum iterations.  However, even if an error is thrown
   * the control network will contain the errors at each control measure.
   *
   * @param tol             Maximum pixel error for any control network
   *                        measurement
   * @param maxIterations   Maximum iterations, if tolerance is never
   *                        met an iException will be thrown.
   */
  bool BundleAdjust::SolveCholesky() {
//   double averageError;
    std::vector<int> observationInitialValueIndex;  // image index for observation inital values
    int iIndex = -1;                                // image index for initial spice for an observation
    int oIndex = -1;                                // Index of current observation

    ComputeNumberPartials();

    if (m_bObservationMode)
      observationInitialValueIndex.assign(m_pObsNumList->ObservationSize(), -1);

    //    std::cout << observationInitialValueIndex << std::endl;

    for (int i = 0; i < Images(); i++) {
      Camera *pCamera = m_pCnet->Camera(i);

      if (m_bObservationMode) {
        oIndex = i;
        oIndex = m_pObsNumList->ObservationNumberMapIndex(oIndex);   // get observation index for this image
        iIndex = observationInitialValueIndex[oIndex];          // get image index for initial observation values
      }

      if (m_cmatrixSolveType != None) {
        // For observations, find the index of the first image and use its polynomial for the observation
        // initial coefficient values.  Initialize indices to -1

        // Fit the camera pointing to an equation
        SpiceRotation *pSpiceRot = pCamera->InstrumentRotation();

        if (!m_bObservationMode) {
          pSpiceRot->SetPolynomialDegree(m_nckDegree);   // Set the ck polynomial fit degree
          pSpiceRot->SetPolynomial();
          pSpiceRot->SetPolynomialDegree(m_nsolveCamDegree);   // Update to the solve polynomial fit degree
        }
        else {
          // Index of image to use for initial values is set already so set polynomial to initial values
          if (iIndex >= 0) {
            SpiceRotation *pOrot = m_pCnet->Camera(iIndex)->InstrumentRotation(); //Observation rotation
            std::vector<double> anglePoly1, anglePoly2, anglePoly3;
            pOrot->GetPolynomial(anglePoly1, anglePoly2, anglePoly3);
            double baseTime = pOrot->GetBaseTime();
            double timeScale = pOrot->GetTimeScale();
            pSpiceRot->SetPolynomialDegree(m_nsolveCamDegree);   // Update to the solve polynomial fit degree
            pSpiceRot->SetOverrideBaseTime(baseTime, timeScale);
            pSpiceRot->SetPolynomial(anglePoly1, anglePoly2, anglePoly3);
          }
          else {
            // Index of image to use for inital observation values has not been assigned yet so use this image
            pSpiceRot->SetPolynomialDegree(m_nckDegree);
            pSpiceRot->SetPolynomial();
            pSpiceRot->SetPolynomialDegree(m_nsolveCamDegree);   // Update to the solve polynomial fit degree
            observationInitialValueIndex[oIndex] = i;
          }
        }
      }

      if (m_spacecraftPositionSolveType != Nothing) {
        // Set the spacecraft position to an equation
        SpicePosition *pSpicePos = pCamera->InstrumentPosition();

        if (!m_bObservationMode)
          pSpicePos->SetPolynomial();
        else {
          // Index of image to use for initial values is set already so set polynomial to initial values
          if (iIndex >= 0) {
            SpicePosition *pOpos = m_pCnet->Camera(iIndex)->InstrumentPosition(); //Observation position
            std::vector<double> posPoly1, posPoly2, posPoly3;
            pOpos->GetPolynomial(posPoly1, posPoly2, posPoly3);
            double baseTime = pOpos->GetBaseTime();
            double timeScale = pOpos->GetTimeScale();
            pSpicePos->SetPolynomial();
            pSpicePos->SetOverrideBaseTime(baseTime, timeScale);
            pSpicePos->SetPolynomial(posPoly1, posPoly2, posPoly3);
          }
          else {
            // Index of image to use for inital observation values has not been assigned yet so use this image
            pSpicePos->SetPolynomial();
            observationInitialValueIndex[oIndex] = i;
          }
        }
      }
    }

    Initialize();

    ComputeImageParameterWeights();

    // Compute the apriori lat/lons for each nonheld point
    m_pCnet->ComputeApriori(); // original location

    InitializePointWeights(); // testing - moved from ::Initalize function
    InitializePoints(); // New method to set apriori sigmas and surface points

    // Initialize solution parameters
    double sigmaXY, sigmaHat, sigmaX, sigmaY;
    sigmaXY = sigmaHat = sigmaX = sigmaY = 0.0;
    m_nIteration = 1;

    clock_t t1 = clock();

    double dvtpv = 0.0;
    double dSigma0_previous = 0.0;

    Progress progress;

    for (;;) {
      printf("starting iteration %d\n", m_nIteration);
      clock_t iterationclock1 = clock();

      // send notification to UI indicating "new iteration"
      // UI.Notify(BundleEvent.NEW_ITERATION);

      // zero normals (after iteration 0)
      if (m_nIteration != 1) {
        if ( m_decompositionMethod == SPECIALK )
          m_Normals.clear();
        else if ( m_decompositionMethod == CHOLMOD )
          m_SparseNormals.zeroBlocks();
      }

      // form normal equations
//      clock_t formNormalsclock1 = clock();
//      printf("starting FormNormals\n");

      if (!formNormalEquations()) {
        m_bConverged = false;
        m_bError = true;
        break;
      }
//      clock_t formNormalsclock2 = clock();
//      double dFormNormalsTime = ((formNormalsclock2-formNormalsclock1)/(double)CLOCKS_PER_SEC);
//      printf("FormNormals Elapsed Time: %20.10lf\n",dFormNormalsTime);

      // solve system
//      clock_t Solveclock1 = clock();
//      printf("Starting Solve System\n");

      if (!solveSystem()) {
        printf("solve failed!\n");
        m_bConverged = false;
        m_bError = true;
        break;
      }

//      clock_t Solveclock2 = clock();
//      double dSolveTime = ((Solveclock2-Solveclock1)/(double)CLOCKS_PER_SEC);
//      printf("Solve Elapsed Time: %20.10lf\n",dSolveTime);

      // apply parameter corrections
//      clock_t Correctionsclock1 = clock();
      applyParameterCorrections();
//      clock_t Correctionsclock2 = clock();
//      double dCorrectionsTime = ((Correctionsclock2-Correctionsclock1)/(double)CLOCKS_PER_SEC);
//      printf("Corrections Elapsed Time: %20.10lf\n",dCorrectionsTime);

      // compute residuals
//      clock_t Residualsclock1 = clock();

      dvtpv = ComputeResiduals();
//      clock_t Residualsclock2 = clock();
//      double dResidualsTime = ((Residualsclock2-Residualsclock1)/(double)CLOCKS_PER_SEC);
//      printf("Residuals Elapsed Time: %20.10lf\n",dResidualsTime);

      // flag outliers
      if ( m_bOutlierRejection ) {
          ComputeRejectionLimit(); // compute outlier rejection limit
          FlagOutliers();
      }

      // variance of unit weight (also reference variance, variance factor, etc.)
      m_nDegreesOfFreedom =
        m_nObservations + (m_nConstrainedPointParameters + m_nConstrainedImageParameters) - m_nUnknownParameters;

      if (m_nDegreesOfFreedom > 0) 
        m_dSigma0 = dvtpv / m_nDegreesOfFreedom;
      else if (m_bDeltack && m_nDegreesOfFreedom == 0)
        m_dSigma0 = dvtpv;
      else {
        std::string msg = "Degrees of Freedom " + iString(m_nDegreesOfFreedom) 
            + " is invalid (&lt;= 0)!";
      throw Isis::iException::Message(iException::Io, msg, _FILEINFO_);
    }

      m_dSigma0 = sqrt(m_dSigma0);

      printf("Iteration: %d\nSigma0: %20.10lf\n", m_nIteration, m_dSigma0);
      printf("Observations: %d\nConstrained Parameters:%d\nUnknowns: %d\nDegrees of Freedom: %d\n",
             m_nObservations, m_nConstrainedPointParameters, m_nUnknownParameters, m_nDegreesOfFreedom);

      // check for convergence
      if ( !m_bDeltack ) {
        if (fabs(dSigma0_previous - m_dSigma0) <= m_dConvergenceThreshold) {
          m_bLastIteration = true;

          m_bConverged = true;
          printf("Bundle has converged\n");
          break;
        }
      }
      else {
        int nconverged = 0;
        int numimgparam = m_Image_Solution.size();
        for (int ij = 0; ij < numimgparam; ij++) {
          if (fabs(m_Image_Solution(ij)) > m_dConvergenceThreshold) 
            break;
          else
            nconverged++;
        }

        if ( nconverged == numimgparam ) {
          m_bConverged = true;
          m_bLastIteration = true;
          printf("Deltack Bundle has converged\n");
          break;
        }
      }  

      clock_t iterationclock2 = clock();
      double dIterationTime = ((iterationclock2 - iterationclock1) / (double)CLOCKS_PER_SEC);
      printf("End of Iteration %d\nElapsed Time: %20.10lf\n", m_nIteration, dIterationTime);

      // send notification to UI indicating "new iteration"
      // UI.Notify(BundleEvent.END_ITERATION);

      // check for maximum iterations
      if (m_nIteration >= m_nMaxIterations) {
        m_bMaxIterationsReached = true;
        break;
      }

      SpecialKIterationSummary();

      m_nIteration++;

      dSigma0_previous = m_dSigma0;
    }

    if ( m_bConverged && m_bErrorPropagation ) {
      clock_t terror1 = clock();
      printf("\nStarting Error Propagation\n");
      errorPropagation();
      printf("\nError Propagation Complete\n");
      clock_t terror2 = clock();
      m_dElapsedTimeErrorProp = ((terror2 - terror1) / (double)CLOCKS_PER_SEC);
    }

    clock_t t2 = clock();
    m_dElapsedTime = ((t2 - t1) / (double)CLOCKS_PER_SEC);

    WrapUp();

    printf("\nGenerating report files\n");
    Output();

    printf("\nBundle complete\n");

    SpecialKIterationSummary();

    return true;

    std::string msg = "Need to return something here, or just change the whole darn thing? [";
//    msg += iString(tol) + "] in less than [";
//    msg += iString(m_nMaxIterations) + "] iterations";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  /**
   * Forming the least-squares normal equations matrix.
   */
  bool BundleAdjust::formNormalEquations() {
    if ( m_decompositionMethod == CHOLMOD )
      return formNormalEquations_CHOLMOD();
    else
      return formNormalEquations_SPECIALK();

    return false;
  }

  /**
   * solve normal equations system.
   */
  bool BundleAdjust::solveSystem() {
    if ( m_decompositionMethod == CHOLMOD )
      return solveSystem_CHOLMOD();
    else
      return solveSystem_SPECIALK();

    return false;
  }

  /**
   * Forming the least-squares normal equations matrix via cholmod.
   */
  bool BundleAdjust::formNormalEquations_CHOLMOD() {
//    if (m_pProgressBar != NULL)
//    {
//      m_pProgressBar->SetText("Forming Normal Equations...");
//      m_pProgressBar->SetMaximumSteps(m_pCnet->Size());
//      m_pProgressBar->CheckStatus();
//    }
    bool bStatus = false;

    m_nObservations = 0;
    m_nConstrainedPointParameters = 0;

    static matrix<double> coeff_image;
    static matrix<double> coeff_point3D(2, 3);
    static vector<double> coeff_RHS(2);
    static symmetric_matrix<double, upper> N22(3);                // 3x3 upper triangular
//    static compressed_matrix< double> N12(m_nRank, 3);            // image parameters x 3 (should this be compressed? We only make one, so probably not)
//    static SparseBlockColumnMatrix N12;
    SparseBlockColumnMatrix N12;
    static vector<double> n2(3);                                  // 3x1 vector
    compressed_vector<double> n1(m_nRank);                        // image parameters x 1
//  vector<double> nj(m_nRank);                                   // image parameters x 1

    m_nj.resize(m_nRank);

    coeff_image.resize(2, m_nNumImagePartials);
    //N12.resize(m_nRank, 3);

    // clear N12, n1, and nj
    N12.clear();
    n1.clear();
    m_nj.clear();

    // clear static matrices
//    coeff_image.clear();
    coeff_point3D.clear();
    coeff_RHS.clear();
    N22.clear();
    n2.clear();

    // loop over 3D points
    int nGood3DPoints = 0;
    int nRejected3DPoints = 0;
    int nPointIndex = 0;
    int nImageIndex;
    int n3DPoints = m_pCnet->GetNumPoints();

//    char buf[1056];
//    sprintf(buf,"\n\t                      Points:%10d\n", n3DPoints);
//    m_fp_log << buf;
//    printf("%s", buf);

    printf("\n\n");

    for (int i = 0; i < n3DPoints; i++) {

        const ControlPoint *point = m_pCnet->GetPoint(i);

        if ( point->IsIgnored() )
            continue;

        if( point->IsRejected() ) {
            nRejected3DPoints++;
//            sprintf(buf, "\tRejected %s - 3D Point %d of %d RejLimit = %lf\n", point.Id().c_str(),nPointIndex,n3DPoints,m_dRejectionLimit);
//            m_fp_log << buf;

            nPointIndex++;
            continue;
        }

        // send notification to UI indicating index of point currently being processed
        // m_nProcessedPoint = i+1;
        // UI.Notify(BundleEvent.NEW_POINT_PROCESSED);

        if ( i != 0 ) {
            N22.clear();
//            N12.clear();
            N12.wipe();
            n2.clear();
        }

      int nMeasures = point->GetNumMeasures();
      // loop over measures for this point
      for (int j = 0; j < nMeasures; j++) {

          const ControlMeasure *measure = point->GetMeasure(j);
          if ( measure->IsIgnored() )
              continue;

          // flagged as "JigsawFail" implies this measure has been rejected
          // TODO  IsRejected is obsolete -- replace code or add to ControlMeasure
          if (measure->IsRejected())
              continue;

          // printf("   Processing Measure %d of %d\n", j,nMeasures);

          // fill non-zero indices for this point - do we have to do this?
          // see BundleDistanceConstraints.java for code snippet (line 926)

          // Determine the image index
          nImageIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());
          if ( m_bObservationMode )
              nImageIndex = ImageIndex(nImageIndex)/m_nNumImagePartials;

        bStatus = ComputePartials_DC(coeff_image, coeff_point3D, coeff_RHS,
                                  *measure, *point);

//        std::cout << coeff_image << std::endl;
//        std::cout << coeff_point3D << std::endl;
//        std::cout << coeff_RHS << std::endl;

        if ( !bStatus )
            continue;     // this measure should be flagged as rejected

        // update number of observations
        m_nObservations += 2;

      formNormals1_CHOLMOD(N22, N12, n1, n2, coeff_image, coeff_point3D,
                             coeff_RHS, nImageIndex);
    } // end loop over this points measures

      formNormals2_CHOLMOD(N22, N12, n2, m_nj, nPointIndex, i);
      nPointIndex++;

//      if (m_pProgressBar != NULL)
//          m_pProgressBar->CheckStatus();

      nGood3DPoints++;

  } // end loop over 3D points

    // finally, form the reduced normal equations
    formNormals3_CHOLMOD(n1, m_nj);

//    std::cout << m_Normals << std::endl;
//    m_SparseNormals.print();

    // update number of unknown parameters
    m_nUnknownParameters = m_nRank + 3 * nGood3DPoints;

    return bStatus;
}

  bool BundleAdjust::formNormals1_CHOLMOD(symmetric_matrix<double, upper>&N22,
      SparseBlockColumnMatrix& N12, compressed_vector<double>& n1,
      vector<double>& n2, matrix<double>& coeff_image,
      matrix<double>& coeff_point3D, vector<double>& coeff_RHS,
                                          int nImageIndex) {
    int i;

    static vector<double> n1_image(m_nNumImagePartials);
    n1_image.clear();

    // form N11 (normals for photo)
    static symmetric_matrix<double, upper> N11(m_nNumImagePartials);
    N11.clear();

//    std::cout << "image" << std::endl << coeff_image << std::endl;
//    std::cout << "point" << std::endl << coeff_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_RHS << std::endl;

    N11 = prod(trans(coeff_image), coeff_image);

//    std::cout << "N11" << std::endl << N11 << std::endl;

    int t = m_nNumImagePartials * nImageIndex;

    // insert submatrix at column, row
    m_SparseNormals.InsertMatrixBlock(nImageIndex, nImageIndex,
        m_nNumImagePartials, m_nNumImagePartials );

    (*(*m_SparseNormals[nImageIndex])[nImageIndex]) += N11;

//    std::cout << (*(*m_SparseNormals[nImageIndex])[nImageIndex]) << std::endl;

    // form N12_Image
    static matrix<double> N12_Image(m_nNumImagePartials, 3);
    N12_Image.clear();

    N12_Image = prod(trans(coeff_image), coeff_point3D);


//    printf("N12 before insert\n");
//    std::cout << N12 << std::endl;

//    std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
//    for (i = 0; i < m_nNumImagePartials; i++)
//      for (j = 0; j < 3; j++)
//        N12(i + t, j) += N12_Image(i, j);
    N12.InsertMatrixBlock(nImageIndex, m_nNumImagePartials, 3);
    *N12[nImageIndex] += N12_Image;

//    printf("N12\n");
//    std::cout << N12 << std::endl;

    // form n1
    n1_image = prod(trans(coeff_image), coeff_RHS);

//    std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    for (i = 0; i < m_nNumImagePartials; i++)
      n1(i + t) += n1_image(i);

    // form N22
    N22 += prod(trans(coeff_point3D), coeff_point3D);

//    std::cout << "N22" << std::endl << N22 << std::endl;

    // form n2
    n2 += prod(trans(coeff_point3D), coeff_RHS);

//    std::cout << "n2" << std::endl << n2 << std::endl;

    return true;
  }

  bool BundleAdjust::formNormals2_CHOLMOD(symmetric_matrix<double, upper>&N22,
      SparseBlockColumnMatrix& N12, vector<double>& n2, vector<double>& nj,
      int nPointIndex, int i) {

    bounded_vector<double, 3>& NIC = m_NICs[nPointIndex];
    SparseBlockRowMatrix& Q = m_Qs_CHOLMOD[nPointIndex];

    NIC.clear();
    Q.zeroBlocks();

    // weighting of 3D point parameters
//    const ControlPoint *point = m_pCnet->GetPoint(i);
    ControlPoint *point = m_pCnet->GetPoint(i); //TODO: what about this const business, regarding SetAdjustedSurfacePoint below???

    bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
    bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

//    std::cout << "Point" << point->GetId() << "weights" << std::endl << weights << std::endl;

//    std::cout << "corrections" << std::endl << corrections << std::endl;

    if (weights[0] > 0.0) {
      N22(0, 0) += weights[0];
      n2(0) += (-weights[0] * corrections(0));
      m_nConstrainedPointParameters++;
    }

    if (weights[1] > 0.0) {
      N22(1, 1) += weights[1];
      n2(1) += (-weights[1] * corrections(1));
      m_nConstrainedPointParameters++;
    }

    if (weights[2] > 0.0) {
      N22(2, 2) += weights[2];
      n2(2) += (-weights[2] * corrections(2));
      m_nConstrainedPointParameters++;
    }

//    std::cout << "N22 before inverse" << std::endl << N22 << std::endl;
    // invert N22
    Invert_3x3(N22);
//    std::cout << "N22 after inverse" << std::endl << N22 << std::endl;

    // save upper triangular covariance matrix for error propagation
    // TODO:  The following method does not exist yet (08-13-2010)
    SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();
    SurfacePoint.SetSphericalMatrix(N22);
    point->SetAdjustedSurfacePoint(SurfacePoint);
//    point->GetAdjustedSurfacePoint().SetSphericalMatrix(N22);

// TODO  Test to make sure spherical matrix is truly set.  Try to read it back

    // Next 3 lines obsolete because only the matrix is stored and sigmas are calculated from it.
//    point->SetSigmaLatitude(N22(0,0));
//    point->SetSigmaLongitude(N22(1,1));
//    point->SetSigmaRadius(N22(2,2));

    // form Q (this is N22{-1} * N12{T})
//    clock_t FormQ1 = clock();
//    Q = prod(N22, trans(N12));
    product_ATransB(N22, N12, Q);
//    clock_t FormQ2 = clock();
//    double dFormQTime = ((FormQ2-FormQ1)/(double)CLOCKS_PER_SEC);
//    printf("FormQ Elapsed Time: %20.10lf\n",dFormQTime);

//    std::cout << "Q: " << Q << std::endl;
//    Q.print();

    // form product of N22(inverse) and n2; store in NIC
//    clock_t FormNIC1 = clock();
    NIC = prod(N22, n2);
//    clock_t FormNIC2 = clock();
//    double dFormNICTime = ((FormNIC2-FormNIC1)/(double)CLOCKS_PER_SEC);
//    printf("FormNIC Elapsed Time: %20.10lf\n",dFormNICTime);

    // accumulate -R directly into reduced normal equations
//  clock_t AccumIntoNormals1 = clock();
//  m_Normals -= prod(N12,Q);
//  printf("starting AmultAdd_CNZRows\n");
//    AmultAdd_CNZRows(-1.0, N12, Q, m_Normals);
    AmultAdd_CNZRows_CHOLMOD(-1.0, N12, Q);
//  clock_t AccumIntoNormals2 = clock();
//  double dAccumIntoNormalsTime = ((AccumIntoNormals2-AccumIntoNormals1)/(double)CLOCKS_PER_SEC);
//  printf("Accum Into Normals Elapsed Time: %20.10lf\n",dAccumIntoNormalsTime);

    // accumulate -nj
//    clock_t Accum_nj_1 = clock();
//    m_nj -= prod(trans(Q),n2);
    transA_NZ_multAdd_CHOLMOD(-1.0, Q, n2, m_nj);
//    clock_t Accum_nj_2 = clock();
//    double dAccumnjTime = ((Accum_nj_2-Accum_nj_1)/(double)CLOCKS_PER_SEC);
//    printf("Accum nj Elapsed Time: %20.10lf\n",dAccumnjTime);

    return true;
  }

  /**
   * apply weighting for spacecraft position, velocity, acceleration and camera
   * angles, angular velocities, angular accelerations
   * if so stipulated (legalese)
   */
  bool BundleAdjust::formNormals3_CHOLMOD(compressed_vector<double>& n1,
                                  vector<double>& nj) {

    //  std::cout << m_dImageParameterWeights << std::endl;

    m_nConstrainedImageParameters = 0;

    int n = 0;

    for ( int i = 0; i < m_SparseNormals.size(); i++ ) {
      matrix<double>* diagonalBlock = m_SparseNormals.getBlock(i,i);
      if ( !diagonalBlock )
        continue;

      for (int j = 0; j < m_nNumImagePartials; j++) {
        if (m_dImageParameterWeights[j] > 0.0) {
          (*diagonalBlock)(j,j) += m_dImageParameterWeights[j];
          m_nj[n] -= m_dImageParameterWeights[j] * m_Image_Corrections[n];
          m_nConstrainedImageParameters++;
        }

        n++;
      }
    }

    // add n1 to nj
    m_nj += n1;

    return true;
  }

  /**
   * Forming the least-squares normal equations matrix via specialk.
   */
  bool BundleAdjust::formNormalEquations_SPECIALK() {
//    if (m_pProgressBar != NULL)
//    {
//      m_pProgressBar->SetText("Forming Normal Equations...");
//      m_pProgressBar->SetMaximumSteps(m_pCnet->Size());
//      m_pProgressBar->CheckStatus();
//    }
    bool bStatus = false;

    m_nObservations = 0;
    m_nConstrainedPointParameters = 0;

    static matrix<double> coeff_image;
    static matrix<double> coeff_point3D(2, 3);
    static vector<double> coeff_RHS(2);
    static symmetric_matrix<double, upper> N22(3);     // 3x3 upper triangular
    static matrix< double> N12(m_nRank, 3);            // image parameters x 3 (should this be compressed? We only make one, so probably not)
    static vector<double> n2(3);                       // 3x1 vector
    compressed_vector<double> n1(m_nRank);             // image parameters x 1

    m_nj.resize(m_nRank);

    coeff_image.resize(2, m_nNumImagePartials);
    N12.resize(m_nRank, 3);

    // clear N12, n1, and nj
    N12.clear();
    n1.clear();
    m_nj.clear();

    // clear static matrices
    coeff_point3D.clear();
    coeff_RHS.clear();
    N22.clear();
    n2.clear();

    // loop over 3D points
    int nGood3DPoints = 0;
    int nRejected3DPoints = 0;
    int nPointIndex = 0;
    int nImageIndex;
    int n3DPoints = m_pCnet->GetNumPoints();

//    char buf[1056];
//    sprintf(buf,"\n\t                      Points:%10d\n", n3DPoints);
//    m_fp_log << buf;
//    printf("%s", buf);

    for (int i = 0; i < n3DPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if ( point->IsIgnored() )
        continue;

      if( point->IsRejected() ) {
        nRejected3DPoints++;
//      sprintf(buf, "\tRejected %s - 3D Point %d of %d RejLimit = %lf\n", point.Id().c_str(),nPointIndex,n3DPoints,m_dRejectionLimit);
//      m_fp_log << buf;

        nPointIndex++;
        continue;
      }

      // send notification to UI indicating index of point currently being processed
      // m_nProcessedPoint = i+1;
      // UI.Notify(BundleEvent.NEW_POINT_PROCESSED);

      if ( i != 0 ) {
          N22.clear();
          N12.clear();
          n2.clear();
      }

      // loop over measures for this point
      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);
        if ( measure->IsIgnored() )
          continue;

        // flagged as "JigsawFail" implies this measure has been rejected
        // TODO  IsRejected is obsolete -- replace code or add to ControlMeasure
        if (measure->IsRejected())
          continue;

        // printf("   Processing Measure %d of %d\n", j,nMeasures);

        // fill non-zero indices for this point - do we have to do this?
        // see BundleDistanceConstraints.java for code snippet (line 926)

        // Determine the image index
        nImageIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());
        if ( m_bObservationMode )
          nImageIndex = ImageIndex(nImageIndex)/m_nNumImagePartials;

        bStatus = ComputePartials_DC(coeff_image, coeff_point3D, coeff_RHS,
                                  *measure, *point);

//        std::cout << coeff_image << std::endl;
//        std::cout << coeff_point3D << std::endl;
//        std::cout << coeff_RHS << std::endl;

        if ( !bStatus )
          continue;     // this measure should be flagged as rejected

        // update number of observations
        m_nObservations += 2;

        formNormals1_SPECIALK(N22, N12, n1, n2, coeff_image, coeff_point3D,
                             coeff_RHS, nImageIndex);
      } // end loop over this points measures

      formNormals2_SPECIALK(N22, N12, n2, m_nj, nPointIndex, i);
      nPointIndex++;

//    if (m_pProgressBar != NULL)
//      m_pProgressBar->CheckStatus();

      nGood3DPoints++;

    } // end loop over 3D points

    // finally, form the reduced normal equations
    formNormals3_SPECIALK(n1, m_nj);

//  std::cout << m_Normals << std::endl;
//  m_SparseNormals.print();

    // update number of unknown parameters
    m_nUnknownParameters = m_nRank + 3 * nGood3DPoints;

    return bStatus;
  }

  bool BundleAdjust::formNormals1_SPECIALK(symmetric_matrix<double, upper>&N22,
      matrix<double>& N12, compressed_vector<double>& n1, vector<double>& n2,
      matrix<double>& coeff_image, matrix<double>& coeff_point3D,
      vector<double>& coeff_RHS, int nImageIndex) {
    int i, j;

    static vector<double> n1_image(m_nNumImagePartials);
    n1_image.clear();

    // form N11 (normals for photo)
    static symmetric_matrix<double, upper> N11(m_nNumImagePartials);
    N11.clear();

//    std::cout << "image" << std::endl << coeff_image << std::endl;
//    std::cout << "point" << std::endl << coeff_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_RHS << std::endl;

    N11 = prod(trans(coeff_image), coeff_image);

//    std::cout << "N11" << std::endl << N11 << std::endl;

    // insert into reduced normal equations
    int t = m_nNumImagePartials * nImageIndex;
    for (i = 0; i < m_nNumImagePartials; i++)
      for (j = i; j < m_nNumImagePartials; j++)
        m_Normals(i + t, j + t) += N11(i, j);

    // form N12_Image
    static matrix<double> N12_Image(m_nNumImagePartials, 3);
    N12_Image.clear();

    N12_Image = prod(trans(coeff_image), coeff_point3D);


//    printf("N12 before insert\n");
//    std::cout << N12 << std::endl;

//    std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
    for (i = 0; i < m_nNumImagePartials; i++)
      for (j = 0; j < 3; j++)
        N12(i + t, j) += N12_Image(i, j);

//    printf("N12\n");
//    std::cout << N12 << std::endl;

    // form n1
    n1_image = prod(trans(coeff_image), coeff_RHS);

//    std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    for (i = 0; i < m_nNumImagePartials; i++)
      n1(i + t) += n1_image(i);

    // form N22
    N22 += prod(trans(coeff_point3D), coeff_point3D);

//    std::cout << "N22" << std::endl << N22 << std::endl;

    // form n2
    n2 += prod(trans(coeff_point3D), coeff_RHS);

//    std::cout << "n2" << std::endl << n2 << std::endl;

    return true;
  }

  bool BundleAdjust::formNormals2_SPECIALK(symmetric_matrix<double, upper>&N22,
      matrix<double>& N12, vector<double>& n2, vector<double>& nj,
      int nPointIndex, int i) {

      bounded_vector<double, 3>& NIC = m_NICs[nPointIndex];
      compressed_matrix<double>& Q = m_Qs_SPECIALK[nPointIndex];

      NIC.clear();
      Q.clear();

      // weighting of 3D point parameters
  //    const ControlPoint *point = m_pCnet->GetPoint(i);
      ControlPoint *point = m_pCnet->GetPoint(i); //TODO: what about this const business, regarding SetAdjustedSurfacePoint below???

      bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
      bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

  //    std::cout << "Point" << point->GetId() << "weights" << std::endl << weights << std::endl;

  //    std::cout << "corrections" << std::endl << corrections << std::endl;

      if (weights[0] > 0.0) {
        N22(0, 0) += weights[0];
        n2(0) += (-weights[0] * corrections(0));
        m_nConstrainedPointParameters++;
      }

      if (weights[1] > 0.0) {
        N22(1, 1) += weights[1];
        n2(1) += (-weights[1] * corrections(1));
        m_nConstrainedPointParameters++;
      }

      if (weights[2] > 0.0) {
        N22(2, 2) += weights[2];
        n2(2) += (-weights[2] * corrections(2));
        m_nConstrainedPointParameters++;
      }

  //    std::cout << "N22 before inverse" << std::endl << N22 << std::endl;
      // invert N22
      Invert_3x3(N22);
  //    std::cout << "N22 after inverse" << std::endl << N22 << std::endl;

      // save upper triangular covariance matrix for error propagation
      // TODO:  The following method does not exist yet (08-13-2010)
      SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();
      SurfacePoint.SetSphericalMatrix(N22);
      point->SetAdjustedSurfacePoint(SurfacePoint);
  //    point->GetAdjustedSurfacePoint().SetSphericalMatrix(N22);

  // TODO  Test to make sure spherical matrix is truly set.  Try to read it back

      // Next 3 lines obsolete because only the matrix is stored and sigmas are calculated from it.
  //    point->SetSigmaLatitude(N22(0,0));
  //    point->SetSigmaLongitude(N22(1,1));
  //    point->SetSigmaRadius(N22(2,2));

      // form Q (this is N22{-1} * N12{T})
  //    clock_t FormQ1 = clock();
      Q = prod(N22, trans(N12));
  //    clock_t FormQ2 = clock();
  //    double dFormQTime = ((FormQ2-FormQ1)/(double)CLOCKS_PER_SEC);
  //    printf("FormQ Elapsed Time: %20.10lf\n",dFormQTime);

  //    std::cout << "Q: " << Q << std::endl;

      // form product of N22(inverse) and n2; store in NIC
  //    clock_t FormNIC1 = clock();
      NIC = prod(N22, n2);
  //    clock_t FormNIC2 = clock();
  //    double dFormNICTime = ((FormNIC2-FormNIC1)/(double)CLOCKS_PER_SEC);
  //    printf("FormNIC Elapsed Time: %20.10lf\n",dFormNICTime);

      // accumulate -R directly into reduced normal equations
  //  clock_t AccumIntoNormals1 = clock();
  //  m_Normals -= prod(N12,Q);
  //  printf("starting AmultAdd_CNZRows\n");
      AmultAdd_CNZRows_SPECIALK(-1.0, N12, Q, m_Normals);
  //  clock_t AccumIntoNormals2 = clock();
  //  double dAccumIntoNormalsTime = ((AccumIntoNormals2-AccumIntoNormals1)/(double)CLOCKS_PER_SEC);
  //  printf("Accum Into Normals Elapsed Time: %20.10lf\n",dAccumIntoNormalsTime);

      // accumulate -nj
  //    clock_t Accum_nj_1 = clock();
  //    m_nj -= prod(trans(Q),n2);
      transA_NZ_multAdd_SPECIALK(-1.0, Q, n2, m_nj);
  //    clock_t Accum_nj_2 = clock();
  //    double dAccumnjTime = ((Accum_nj_2-Accum_nj_1)/(double)CLOCKS_PER_SEC);
  //    printf("Accum nj Elapsed Time: %20.10lf\n",dAccumnjTime);

      return true;
    }

  /**
   * apply weighting for spacecraft position, velocity, acceleration and camera
   * angles, angular velocities, angular accelerations
   * if so stipulated (legalese)
   */
  bool BundleAdjust::formNormals3_SPECIALK(compressed_vector<double>& n1,
      vector<double>& nj) {

  //  std::cout << m_dImageParameterWeights << std::endl;

    m_nConstrainedImageParameters = 0;

    int n = 0;
    do {
      for (int j = 0; j < m_nNumImagePartials; j++) {
        if (m_dImageParameterWeights[j] > 0.0) {
          m_Normals(n, n) += m_dImageParameterWeights[j];
          m_nj[n] -= m_dImageParameterWeights[j] * m_Image_Corrections[n];
          m_nConstrainedImageParameters++;
        }

        n++;
      }

    }
    while (n < m_nRank);

    // add n1 to nj
    m_nj += n1;

    return true;
  }


  bool BundleAdjust::InitializePointWeights() {
    // TODO:  Get working as is then convert to use new classes (Angles, etc.) and xyz with radius constraints
//  Distance dAprioriXSigma;
//  Distance dAprioriYSigma;
//  Distance dAprioriZSigma;
//  double dWtLat = 0.0;
//  double dWtLong = 0.0;
//  double dWtRadius = 0.0;
    double d;

    int n3DPoints = m_pCnet->GetNumPoints();
    int nPointIndex = 0;
    for (int i = 0; i < n3DPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
//      {
//        nPointIndex++;
        continue;
//      }

      SurfacePoint aprioriSurfacePoint = point->GetAprioriSurfacePoint();

      bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
      bounded_vector<double, 3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

//      std::cout << weights << std::endl;

//      if( point->Held() || point->Type() == ControlPoint::Fixed )
      if (point->GetType() == ControlPoint::Fixed) {
        weights[0] = 1.0e+50;
        weights[1] = 1.0e+50;
        weights[2] = 1.0e+50;
      }
      else {
          if( point->IsLatitudeConstrained() ) {
            apriorisigmas[0] = point->GetAprioriSurfacePoint().GetLatSigmaDistance().GetMeters();
              weights[0] = point->GetAprioriSurfacePoint().GetLatWeight();
          }
          else if( m_dGlobalLatitudeAprioriSigma > 0.0 ) {
              apriorisigmas[0] = m_dGlobalLatitudeAprioriSigma;
              d = m_dGlobalLatitudeAprioriSigma*m_dMTR;
              weights[0] = 1.0/(d*d);
          }

//      dAprioriSigmaX = point->GetAprioriSurfacePoint().GetXSigma();
//      if(  dAprioriSigmaX <= 0.0 || dAprioriSigmaX >= 1000.0 ) {
//        if( m_dGlobalSurfaceXAprioriSigma > 0.0 )
//          dAprioriSigmaX = m_dGlobalSurfaceXAprioriSigma;
//      }
//      apriorisigmas[0] = dAprioriSigmaX;
//
//      if( dAprioriSigmaX > 0.0 && dAprioriSigmaX < 1000.0  ) {
//        // We bundle in km and store in meters
//        d = dAprioriSigmaX * 0.001;
//        weights[0] = 1.0/(d*d);
//      }

//     if(  m_dGlobalLongitudeAprioriSigma > 0.0 )
//       dAprioriSigmaLon = m_dGlobalLongitudeAprioriSigma;
//     else
//       dAprioriSigmaLon = point->AprioriSigmaLongitude();

          if( point->IsLongitudeConstrained() ) {
            apriorisigmas[1] = point->GetAprioriSurfacePoint().GetLonSigmaDistance().GetMeters();
              weights[1] = point->GetAprioriSurfacePoint().GetLonWeight();
          }
          else if( m_dGlobalLongitudeAprioriSigma > 0.0 ) {
              apriorisigmas[1] = m_dGlobalLongitudeAprioriSigma;
              d = m_dGlobalLongitudeAprioriSigma*m_dMTR;
              weights[1] = 1.0/(d*d);
          }

//      dAprioriSigmaY = point->GetAprioriSurfacePoint().GetYSigma();
//        if(  dAprioriSigmaY <= 0.0 || dAprioriSigmaY >= 1000.0 ) {
//          if( m_dGlobalSurfaceYAprioriSigma > 0.0 )
//            dAprioriSigmaY = m_dGlobalSurfaceYAprioriSigma;
//        }
//        apriorisigmas[1] = dAprioriSigmaY;
//
//        if( dAprioriSigmaY > 0.0 && dAprioriSigmaY < 1000.0  ) {
//          // We bundle in km and store in meters
//          d = dAprioriSigmaY * 0.001;
//          weights[1] = 1.0/(d*d);
//        }

//      if(  m_dGlobalRadiusAprioriSigma > 0.0 )
//        dAprioriSigmaRad = m_dGlobalRadiusAprioriSigma;
//      else
//        dAprioriSigmaRad = point->AprioriSigmaRadius();

          if ( !m_bSolveRadii )
              weights[2] = 1.0e+50;
          else {
              if( point->IsRadiusConstrained() ) {
                apriorisigmas[2] = point->GetAprioriSurfacePoint().GetLocalRadiusSigma().GetMeters();
                  weights[2] = point->GetAprioriSurfacePoint().GetLocalRadiusWeight();
              }
              else if( m_dGlobalRadiusAprioriSigma > 0.0 ) {
                  apriorisigmas[2] = m_dGlobalRadiusAprioriSigma;
                  d = m_dGlobalRadiusAprioriSigma*0.001;
                  weights[2] = 1.0/(d*d);
              }
          }

//      dAprioriSigmaZ = point->GetAprioriSurfacePoint().GetZSigma();
//        if(  dAprioriSigmaZ <= 0.0 || dAprioriSigmaZ >= 1000.0 ) {
//          if( m_dGlobalSurfaceZAprioriSigma > 0.0 )
//            dAprioriSigmaZ = m_dGlobalSurfaceZAprioriSigma;
//        }
//        apriorisigmas[2] = dAprioriSigmaZ;
//
//        if( dAprioriSigmaZ > 0.0 && dAprioriSigmaZ < 1000.0  ) {
//          // We bundle in km and store in meters
//          d = dAprioriSigmaZ * 0.001;
//          weights[2] = 1.0/(d*d);
//        }

      }

//      printf("LatWt: %20.10lf LonWt: %20.10lf RadWt: %20.10lf\n",weights[0],weights[1],weights[2]);

      // TODO: do we need the four lines below??????
      // 2011-05-19 KLE: NOOOO! this causes apriori sigmas to be set if the user has input
      // global point weights. This causes a mess in the adjustment and the output
      // net is corrupted with an invalid apriori covariance matrix
//      try {
//        aprioriSurfacePoint.SetSphericalSigmasDistance(Distance(apriorisigmas[0], Distance::Meters),
//          Distance(apriorisigmas[1], Distance::Meters),
//          Distance(apriorisigmas[2], Distance::Meters));
//      }
//      catch (iException &e) {
//        std::string msg = "Required target radii not available for converting lat/lon sigmas from meters ";
//        msg += "for control point " + Isis::iString(point->GetId());
//        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
//    }
//
//      point->SetAprioriSurfacePoint(aprioriSurfacePoint);
      nPointIndex++;
    }

    return true;
  }

// This method will definitely need to be revisited
  void BundleAdjust::InitializePoints() {
    int n3DPoints = m_pCnet->GetNumPoints();

    for (int i = 0; i < n3DPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);

      if (point->IsIgnored())
        continue;

      SurfacePoint aprioriSurfacePoint = point->GetAprioriSurfacePoint();
      point->SetAdjustedSurfacePoint(aprioriSurfacePoint);
    }

  }

  void BundleAdjust::product_AV(double alpha, bounded_vector<double,3>& v2,
      SparseBlockRowMatrix& Q, vector< double >& v1) {

    QMapIterator<int, matrix<double>*> iQ(Q);

    while ( iQ.hasNext() ) {
      iQ.next();

      int ncol = iQ.key();

      int t = ncol*m_nNumImagePartials;

      v2 += alpha * prod(*(iQ.value()),subrange(v1,t,t+m_nNumImagePartials));
    }
  }

  // C = A x B(transpose) where
  //    A is a boost matrix
  //    B is a SparseBlockColumn matrix
  //    C is a SparseBlockRowMatrix
  //    each block of B and C are boost matrices
  bool BundleAdjust::product_ATransB(symmetric_matrix <double,upper>& N22,
      SparseBlockColumnMatrix& N12, SparseBlockRowMatrix& Q) {

    QMapIterator<int, matrix<double>*> iN12(N12);

    while ( iN12.hasNext() ) {
      iN12.next();

      int ncol = iN12.key();

      // insert submatrix in Q at block "ncol"
      Q.InsertMatrixBlock(ncol, 3, m_nNumImagePartials);

      *(Q[ncol]) = prod(N22,trans(*(iN12.value())));
    }

    return true;
  }

  // NOTE: A = N12, B = Q
  void BundleAdjust::AmultAdd_CNZRows_CHOLMOD(double alpha,
      SparseBlockColumnMatrix& N12, SparseBlockRowMatrix& Q) {

    if (alpha == 0.0)
      return;

    // now multiply blocks and subtract from m_SparseNormals
    QMapIterator<int, matrix<double>*> iN12(N12);
    QMapIterator<int, matrix<double>*> iQ(Q);

    while ( iN12.hasNext() ) {
      iN12.next();

      int nrow = iN12.key();
//      matrix<double> a = *(iN12.value());
      matrix<double> *a = iN12.value();

      while ( iQ.hasNext() ) {
        iQ.next();

        int ncol = iQ.key();
        if ( nrow > ncol )
          continue;

        // insert submatrix at column, row
        m_SparseNormals.InsertMatrixBlock(ncol, nrow,
            m_nNumImagePartials, m_nNumImagePartials );

        (*(*m_SparseNormals[ncol])[nrow]) -= prod(*a,*(iQ.value()));
      }
      iQ.toFront();
    }
  }


  void BundleAdjust::AmultAdd_CNZRows_SPECIALK(double alpha, matrix<double>& A, compressed_matrix<double>& B,
                                      symmetric_matrix<double, upper, column_major>& C)
  {
    if (alpha == 0.0)
      return;

    register int i, j, k, ii, jj;
    double d;

    int nColsA = A.size2();

    // create array of non-zero indices of matrix B
    std::vector<int> nz(B.nnz() / B.size1());

    // iterators for B
    typedef compressed_matrix<double>::iterator1 it_row;
    typedef compressed_matrix<double>::iterator2 it_col;

    it_row itr = B.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();
    for (i = 0; i < nzlength; ++i) {
      ii = nz[i];
      for (j = i; j < nzlength; ++j) {
        jj = nz[j];
        d = 0.0;

        for (k = 0; k < nColsA; ++k)
          d += A(ii, k) * B(k, jj);

        C(ii, jj) += alpha * d;
      }
    }
  }

  void BundleAdjust::transA_NZ_multAdd_CHOLMOD(double alpha,
      SparseBlockRowMatrix& Q, vector<double>& n2, vector<double>& m_nj) {

    if (alpha == 0.0)
      return;

    QMapIterator<int, matrix<double>*> iQ(Q);

    while ( iQ.hasNext() ) {
      iQ.next();

      int nrow = iQ.key();
      matrix<double>* m = iQ.value();

      vector<double> v = prod(trans(*m),n2);

      int t = nrow*m_nNumImagePartials;

      for( unsigned i = 0; i < v.size(); i++ )
        m_nj(t+i) += alpha*v(i);
    }
  }

  void BundleAdjust::transA_NZ_multAdd_SPECIALK(double alpha, compressed_matrix<double>& A,
  vector<double>& B, vector<double>& C) {

    if (alpha == 0.0)
      return;

    register int i, j, ii;
    double d;

    int nRowsA = A.size1();

    // create array of non-zero indices of matrix A
    std::vector<int> nz(A.nnz() / A.size1());

    typedef compressed_matrix<double>::iterator1 it_row;
    typedef compressed_matrix<double>::iterator2 it_col;

    it_row itr = A.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();
    for (i = 0; i < nzlength; ++i) {
      ii = nz[i];
      d = 0.0;

      for (j = 0; j < nRowsA; ++j)
        d += A(j, ii) * B(j);

      C(ii) += alpha * d;
    }
  }

  // new
  void BundleAdjust::AmulttransBNZ(matrix<double>& A,
      compressed_matrix<double>& B, matrix<double> &C, double alpha) {

    if ( alpha == 0.0 )
      return;

    register int i,j,k,kk;
    double d;

    int nRowsB = B.size1();

    // create array of non-zero indices of matrix B
    std::vector<int> nz(B.nnz() / nRowsB);

    typedef compressed_matrix<double>::iterator1 it_row;
    typedef compressed_matrix<double>::iterator2 it_col;

    it_row itr = B.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();

    int nRowsA = A.size1();
    int nColsC = C.size2();

    for ( i = 0; i < nRowsA; ++i ) {
      for (j = 0; j < nColsC; ++j) {
        d = 0;

        for (k = 0; k < nzlength; ++k) {
          kk = nz[k];
          d += A(i, kk) * B(j, kk);
        }

        C(i,j) += alpha * d;
      }
    }

  }

  // new
  void BundleAdjust::ANZmultAdd(compressed_matrix<double>& A,
      symmetric_matrix<double, upper, column_major>& B,
      matrix<double>& C, double alpha) {

    if ( alpha == 0.0 )
      return;

    register int i,j,k,kk;
    double d;

    int nRowsA = A.size1();

    // create array of non-zero indices of matrix A
    std::vector<int> nz(A.nnz() /nRowsA);

    typedef compressed_matrix<double>::iterator1 it_row;
    typedef compressed_matrix<double>::iterator2 it_col;

    it_row itr = A.begin1();
    int nIndex = 0;
    for (it_col itc = itr.begin(); itc != itr.end(); ++itc) {
      nz[nIndex] = itc.index2();
      nIndex++;
    }

    int nzlength = nz.size();

    int nColsC = C.size2();    
    for ( i = 0; i < nRowsA; ++i ) {
      for ( j = 0; j < nColsC; ++j ) {
        d = 0;

        for ( k = 0; k < nzlength; ++k ) {
          kk = nz[k];
          d += A(i, kk) * B(kk, j);
        }

        C(i, j) += alpha * d;
      }
    }

  }

  bool BundleAdjust::solveSystem_CHOLMOD() {

    // load cholmod triplet
    if ( !loadCholmodTriplet() ) {
      std::string msg = "CHOLMOD: Failed to load Triplet matrix";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // convert triplet to sparse matrix
//      FILE * pFile1;
//      pFile1 = fopen ("//work//users//kedmundson//Normals.txt" , "w");
    m_N = cholmod_triplet_to_sparse(m_pTriplet,  m_pTriplet->nnz, &m_cm);
//      cholmod_write_sparse(pFile1, m_N, 0, 0, &m_cm);
//      fclose(pFile1);

    // analyze matrix
    clock_t t1 = clock();
    m_L = cholmod_analyze(m_N, &m_cm); // should we analyze just 1st iteration?
    clock_t t2 = clock();
    double delapsedtime = ((t2-t1)/(double)CLOCKS_PER_SEC);
    //printf("cholmod Analyze Elapsed Time: %20.10lf\n",delapsedtime);

    // create cholmod cholesky factor (LDLT?)
    t1 = clock();
    cholmod_factorize(m_N, m_L, &m_cm);
    t2 = clock();
    delapsedtime = ((t2-t1)/(double)CLOCKS_PER_SEC);
    //printf("cholmod Factorize Elapsed Time: %20.10lf\n",delapsedtime);

    // check for "matrix not positive definite" error
    if ( m_cm.status == CHOLMOD_NOT_POSDEF ) {
      std::string msg = "matrix NOT positive-definite: failure at column "
          + m_L->minor;
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

//      FILE * pFile2;
//      pFile2 = fopen ("//work1//kedmundson//factor.txt" , "w");
//      cholmod_sparse* factor   = cholmod_factor_to_sparse(L, &c);
//      cholmod_write_sparse(pFile2, factor, 0, 0, &c);
//      fclose(pFile2);

    // cholmod solution and right-hand side vectors
    cholmod_dense *x, *b;

    // initialize right-hand side vector
    b = cholmod_zeros (m_N->nrow, 1, m_N->xtype, &m_cm);

    // copy right-hand side vector into b
    double *px = (double*)b->x;
    for ( int i = 0; i < m_nRank; i++ )
      px[i] = m_nj[i];

//      FILE * pFile3;
//      pFile3 = fopen ("//work1//kedmundson//rhs.txt" , "w");
//      cholmod_write_dense(pFile3, b, 0, &c);
//      fclose(pFile3);

    // cholmod solve
    t1 = clock();
    x = cholmod_solve (CHOLMOD_A, m_L, b, &m_cm) ;
    t2 = clock();
    delapsedtime = ((t2-t1)/(double)CLOCKS_PER_SEC);
    //printf("cholmod Solution Elapsed Time: %20.10lf\n",delapsedtime);

//      FILE * pFile4;
//      pFile4 = fopen ("//work//users//kedmundson//solution.txt" , "w");
//      cholmod_write_dense(pFile4, x, 0, &m_cm);
//      fclose(pFile4);

    // copy solution vector x out into m_Image_Solution
    double *sx = (double*)x->x;
    for ( int i = 0; i < m_nRank; i++ )
      m_Image_Solution[i] = sx[i];

    // free cholmod structures
    cholmod_free_sparse(&m_N, &m_cm); // necessary?
    cholmod_free_dense(&b, &m_cm);
    cholmod_free_dense(&x, &m_cm);

    return true;
  }

  bool BundleAdjust::loadCholmodTriplet() {
    //printf("Starting CholMod conversion to triplet\n");
    double d;

    if ( m_nIteration == 1 ) {
      int nelements = m_SparseNormals.numberOfElements();
      //printf("Matrix rank: %d # of Triplet elements: %d", m_nRank,nelements);
      m_pTriplet = cholmod_allocate_triplet(m_nRank, m_nRank, nelements,
          -1, CHOLMOD_REAL, &m_cm);

      if ( !m_pTriplet ) {
        printf("Triplet allocation failure");
        return false;
      }

      m_pTriplet->nnz = 0;
    }

    int* Ti = (int*) m_pTriplet->i;
    int* Tj = (int*) m_pTriplet->j;
    double* v = (double*)m_pTriplet->x;

    int nentries = 0;

    int nblockcolumns = m_SparseNormals.size();
    for ( int ncol = 0; ncol < nblockcolumns; ncol++ ) {

      SparseBlockColumnMatrix* sbc = m_SparseNormals[ncol];

      if ( !sbc ) {
        printf("SparseBlockColumnMatrix retrieval failure at column %d", ncol);
        return false;
      }

      QMapIterator<int, matrix<double>*> it(*sbc);

      while ( it.hasNext() ) {
        it.next();

        int nrow = it.key();

        matrix<double>* m = it.value();
        if ( !m ) {
          printf("matrix block retrieval failure at column %d, row %d", ncol,nrow);
          printf("Total # of block columns: %d", nblockcolumns);
          printf("Total # of blocks: %d", m_SparseNormals.numberOfBlocks());
          return false;
        }

        if ( ncol == nrow )  {   // diagonal block (upper-triangular)
          for ( unsigned ii = 0; ii < m->size1(); ii++ ) {
            for (unsigned jj = ii; jj < m->size2(); jj++) {
              d = m->at_element(ii,jj);
              int ncolindex = jj+ncol*m_nNumImagePartials;
              int nrowindex = ii+nrow*m_nNumImagePartials;

              if ( m_nIteration == 1 ) {
                Ti[nentries] = ncolindex;
                Tj[nentries] = nrowindex;
                m_pTriplet->nnz++;
              }

//              printf("UT - writing to row: %d column: %d\n", ncolindex, nrowindex);
              v[nentries] = d;

              nentries++;
            }
          }
//          printf("\n");
        }
        else {                // off-diagonal block (square)
          for ( unsigned ii = 0; ii < m->size1(); ii++ ) {
            for ( unsigned jj = 0; jj < m->size2(); jj++ ) {
              d = m->at_element(ii,jj);
              int ncolindex = jj+ncol*m_nNumImagePartials;
              int nrowindex = ii+nrow*m_nNumImagePartials;

              if ( m_nIteration ==1 ) {
                Ti[nentries] = nrowindex;
                Tj[nentries] = ncolindex;
                m_pTriplet->nnz++;
              }

              v[nentries] = d;

//              printf("     writing to row: %d column: %d\n", ncolindex, nrowindex);

              nentries++;
            }
          }
//          printf("\n");
        }
      }
    }

    return true;
  }

  bool BundleAdjust::solveSystem_SPECIALK() {
    // decomposition (this is UTDU - need to test row vs column major
    // storage and access, and use of matrix iterators)
//    clock_t CholeskyUtDUclock1 = clock();
//    printf("Starting Cholesky\n");
//    cholesky_decompose(m_Normals);
    if ( !CholeskyUT_NOSQR() )
      return false;
//    clock_t CholeskyUtDUclock2 = clock();
//    double dCholeskyTime = ((CholeskyUtDUclock2-CholeskyUtDUclock1)/(double)CLOCKS_PER_SEC);
//    printf("Cholesky Elapsed Time: %20.10lf\n",dCholeskyTime);

//    cholesky_solve(m_Normals, m_nj);

    // solve via back-substitution
//    clock_t BackSubclock1 = clock();
//    printf("Starting Back Substitution\n");
    if (!CholeskyUT_NOSQR_BackSub(m_Normals, m_Image_Solution, m_nj))
      return false;
//    clock_t BackSubclock2 = clock();
//    double dBackSubTime = ((BackSubclock1-BackSubclock2)/(double)CLOCKS_PER_SEC);
//    printf("Back Substitution Elapsed Time: %20.10lf\n",dBackSubTime);

//    std::cout << m_Image_Solution << std::endl;

    return true;
  }

  bool BundleAdjust::CholeskyUT_NOSQR() {
    int i, j, k;
    double sum, divisor;
    double den;
    double d1, d2;

    int nRows = m_Normals.size1();
//    comment here
//        for( i = 0; i < nRows; i++ )
//        {
//          for( j = i; j < nRows; j++ )
//          {
//            printf("%lf ",m_Normals(i,j));
//          }
//          printf("\n");
//        }
//    comment here
    for (i = 0; i < nRows; i++) {
//    printf("Cholesky Row %d of %d\n",i+1,nRows);
      sum = 0.0;

      for (j = 0; j < i; j++) {
//      sum += m_Normals(j,i-j) * m_Normals(j,i-j) * m_Normals(j,0);   // old way

        d1 = m_Normals(j, i);
        if (d1 == 0.0)
          continue;
        sum += d1 * d1 * m_Normals(j, j);  // new way
      }

//    m_Normals(i,0) -= sum;                                           // old
      m_Normals(i, i) -= sum;                                          // new

      // check for divide by 0
//    den = m_Normals(i,0);                                            // old
      den = m_Normals(i, i);                                           // new
      if (fabs(den) < 1e-100)
        return false;

      divisor = 1.0 / den;

      for (j = (i + 1); j < nRows; j++) {
        sum = 0.0;

        for (k = 0; k < i; k++) {
//        sum += m_Normals(k,j-k) * m_Normals(k,i-k) * m_Normals(k,0); // old

          d1 = m_Normals(k, j);
          if (d1 == 0.0)
            continue;

          d2 = m_Normals(k, i);
          if (d2 == 0.0)
            continue;

          sum += d1 * d2 * m_Normals(k, k); // new
        }

//      m_Normals(i,j-i) = (m_Normals(i,j-i) - sum) * divisor;         // old
        m_Normals(i, j)   = (m_Normals(i, j) - sum) * divisor;         // new
      }

      // decompose right-hand side
      sum = 0.0;
      for (k = 0; k < i; k++) {
        d1 = m_nj(k);
        if (d1 == 0.0)
          continue;

        d2 = m_Normals(k, i);
        if (d2 == 0.0)
          continue;

        sum += d1 * d2 * m_Normals(k, k);
      }

      m_nj(i) = (m_nj(i) - sum) * divisor;
    }

    return true;
  }

  bool BundleAdjust::CholeskyUT_NOSQR_BackSub(symmetric_matrix<double, upper, column_major>& m, vector<double>& s,
      vector<double>& rhs)
//  bool BundleAdjust::CholeskyUT_NOSQR_BackSub(symmetric_matrix<double,lower>& m, vector<double>& s,
//                                              vector<double>& rhs)
  {
    int i, j;
    double sum;
    double d1, d2;

    int nRows = m.size1();

    s(nRows - 1) = rhs(nRows - 1);

    for (i = nRows - 2; i >= 0; i--) {
      sum = 0.0;

      for (j = i + 1; j < nRows; j++) {
        d1 = m(i, j);
        if (d1 == 0.0)
          continue;

        d2 = s(j);
        if (d2 == 0.0)
          continue;

        sum += d1 * d2;
      }

      s(i) = rhs(i) - sum;
    }

//    std::cout << s << std::endl;

    return true;
  }

  bool BundleAdjust::CholeskyUT_NOSQR_Inverse() {
    int i, j, k;
    double div, sum;
    double colk, tmpkj, tmpkk;

    // create temporary copy, inverse will be stored in m_Normals
    symmetric_matrix<double, upper, column_major> tmp = m_Normals;
//    symmetric_matrix<double,lower> tmp = m_Normals;

    // solution vector
    vector<double> s(m_nRank);

    // initialize column vector
    vector<double> column(m_nRank);
    column.clear();
    column(0) = 1.0;

    for (i = 0; i < m_nRank; i++) {
      // set current column of identity
      column.clear();
      column(i) = 1.0;

      // factorize current column of identity matrix
      for (j = 0; j < m_nRank; j++) {
        div = 1.0 / tmp(j, j);
        sum = 0.0;

        for (k = 0; k < j; k++) {
          colk = column(k);
          tmpkj = tmp(k, j);
          tmpkk = tmp(k, k);

          if (colk == 0.0 || tmpkj == 0.0 || tmpkk == 0.0)
            continue;

          sum += colk * tmpkj * tmpkk;
        }

        column(j) = (column(j) - sum) * div;
      }

      // back-substitution
      if (!CholeskyUT_NOSQR_BackSub(tmp, s, column))
        return false;

      // store solution in corresponding column of inverse (replacing column in
      // m_Normals)
      for (j = 0; j <= i; j++)
        m_Normals(j, i) = s(j);
    }

    return true;
  }

  bool BundleAdjust::cholmod_Inverse() {
    int i, j;

    // allocate matrix inverse
    m_Normals.resize(m_nRank);

    cholmod_dense *x;        // solution vector
    cholmod_dense *b;        // right-hand side (column vectors of identity)

    b = cholmod_zeros ( m_nRank, 1, CHOLMOD_REAL, &m_cm ) ;
    double* pb = (double*)b->x;

    for ( i = 0; i < m_nRank; i++ ) {
      if ( i > 0 )
        pb[i-1] = 0.0;
      pb[i] = 1.0;

      x = cholmod_solve ( CHOLMOD_A, m_L, b, &m_cm ) ;
      double* px = (double*)x->x;

      // store solution in corresponding column of inverse (replacing column in
      // m_Normals)
      for (j = 0; j <= i; j++)
        m_Normals(j, i) = px[j];
    }

    cholmod_free_dense(&x,&m_cm);
    cholmod_free_dense(&b,&m_cm);

    return true;
  }

  bool BundleAdjust::Invert_3x3(symmetric_matrix<double, upper>& m) {
    double det;
    double den;

    symmetric_matrix<double, upper> c = m;

    den = m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
          - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
          + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));

    // check for divide by zero
    if (fabs(den) < 1.0e-100)
      return false;

    det = 1.0 / den;

    m(0, 0) = (c(1, 1) * c(2, 2) - c(1, 2) * c(2, 1)) * det;
    m(0, 1) = (c(0, 2) * c(2, 1) - c(0, 1) * c(2, 2)) * det;
    m(0, 2) = (c(0, 1) * c(1, 2) - c(0, 2) * c(1, 1)) * det;
    m(1, 1) = (c(0, 0) * c(2, 2) - c(0, 2) * c(2, 0)) * det;
    m(1, 2) = (c(0, 2) * c(1, 0) - c(0, 0) * c(1, 2)) * det;
    m(2, 2) = (c(0, 0) * c(1, 1) - c(0, 1) * c(1, 0)) * det;

    return true;
  }

  /**
   * compute partials for measure
   */
  bool BundleAdjust::ComputePartials_DC(matrix<double>& coeff_image, matrix<double>& coeff_point3D,
                                     vector<double>& coeff_RHS, const ControlMeasure &measure,
                                     const ControlPoint &point) {

    // additional vectors
    std::vector<double> d_lookB_WRT_LAT;
    std::vector<double> d_lookB_WRT_LON;
    std::vector<double> d_lookB_WRT_RAD;

    Camera *pCamera = NULL;

    double dMeasuredx, dComputedx, dMeasuredy, dComputedy;
    double deltax, deltay;
    double dObservationSigma;
    double dObservationWeight;

    pCamera = measure.Camera();

    // clear partial derivative matrices and vectors
    coeff_image.clear();
    coeff_point3D.clear();
    coeff_RHS.clear();

    // no need to call SetImage for framing camera ( CameraType  = 0 )
    if (pCamera->GetCameraType() != 0) {
      // Set the Spice to the measured point
      // but, can this be simplified???
      pCamera->SetImage(measure.GetSample(), measure.GetLine());
    }

    //Compute the look vector in instrument coordinates based on time of observation and apriori lat/lon/radius
    if (!(pCamera->GroundMap()->GetXY(point.GetAdjustedSurfacePoint(), &dComputedx, &dComputedy))) {
      std::string msg = "Unable to map apriori surface point for measure ";
      msg += measure.GetCubeSerialNumber() + " on point " + point.GetId() + " into focal plane";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // partials for fixed point w/r lat, long, radius in Body-Fixed
    d_lookB_WRT_LAT = pCamera->GroundMap()->PointPartial(point.GetAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = pCamera->GroundMap()->PointPartial(point.GetAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Longitude);
    d_lookB_WRT_RAD = pCamera->GroundMap()->PointPartial(point.GetAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Radius);

//    std::cout << "d_lookB_WRT_LAT" << d_lookB_WRT_LAT << std::endl;
//    std::cout << "d_lookB_WRT_LON" << d_lookB_WRT_LON << std::endl;
//    std::cout << "d_lookB_WRT_RAD" << d_lookB_WRT_RAD << std::endl;

    int nIndex = 0;

    if (m_spacecraftPositionSolveType != Nothing) {
//      SpicePosition* pInstPos = pCamera->InstrumentPosition();

      // Add the partial for the x coordinate of the position (differentiating
      // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
      for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, icoef,
                                              &coeff_image(0, nIndex),
                                              &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partial for the y coordinate of the position
      for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, icoef,
                                              &coeff_image(0, nIndex),
                                              &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partial for the z coordinate of the position
      for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, icoef,
                                              &coeff_image(0, nIndex),
                                              &coeff_image(1, nIndex));
        nIndex++;
      }

    }

    if (m_cmatrixSolveType != None) {

      // Add the partials for ra
      for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
        pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension,
                                                 icoef, &coeff_image(0, nIndex),
                                                 &coeff_image(1, nIndex));
        nIndex++;
      }


      // Add the partials for dec
      for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
        pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination,
                                                 icoef, &coeff_image(0, nIndex),
                                                 &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partial for twist if necessary
      if (m_bSolveTwist) {
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist,
                                                   icoef, &coeff_image(0, nIndex),
                                                   &coeff_image(1, nIndex));
          nIndex++;
        }
      }
    }

    // partials for 3D point
    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LAT, &coeff_point3D(0, 0),
                                       &coeff_point3D(1, 0));
    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LON, &coeff_point3D(0, 1),
                                       &coeff_point3D(1, 1));

    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RAD, &coeff_point3D(0, 2),
                                       &coeff_point3D(1, 2));

//    std::cout << coeff_point3D << std::endl;
    // right-hand side (measured - computed)
    dMeasuredx = measure.GetFocalPlaneMeasuredX();
    dMeasuredy = measure.GetFocalPlaneMeasuredY();

    deltax = dMeasuredx - dComputedx;
    deltay = dMeasuredy - dComputedy;

    coeff_RHS(0) = deltax;
    coeff_RHS(1) = deltay;

    dObservationSigma = 1.4 * pCamera->PixelPitch();
    dObservationWeight = 1.0 / dObservationSigma;

//    std::cout << "Measuredx " << dMeasuredx << " Measuredy = " << dMeasuredy << std::endl;
//    std::cout << "dComputedx " << dComputedx << " dComputedy = " << dComputedy << std::endl;
//    std::cout << coeff_image << std::endl;
//    std::cout << coeff_point3D << std::endl;
//    std::cout << dMeasuredx << " " << dComputedx << std::endl << dMeasuredy << " " << dComputedy << std::endl;
//    std::cout << coeff_RHS << std::endl;

    // multiply coefficients by observation weight
    coeff_image *= dObservationWeight;
    coeff_point3D *= dObservationWeight;
    coeff_RHS *= dObservationWeight;

    m_Statsx.AddData(deltax);
    m_Statsy.AddData(deltay);

    return true;
  }


  /**
   * compute partials for measure
   */
  /*
  bool BundleAdjust::ComputePartials(matrix<double>& coeff_image, matrix<double>& coeff_point3D,
                                     vector<double>& coeff_RHS, const ControlMeasure &measure,
                                     const ControlPoint &point) {

    // additional vectors
//    double pB[3];                        // Point on surface
//    std::vector<double> sB(3);           // Spacecraft position in body-fixed coordinates
//    std::vector<double> lookB(3);        // "look" vector in body-fixed coordinates
//    std::vector<double> lookC(3);        // "look" vector in camera coordinates
//    std::vector<double> lookJ(3);        // "look" vector in J2000 coordinates
//    std::vector<double> d_lookJ;
//    std::vector<double> d_lookC;
    std::vector<double> d_lookB_WRT_LAT;
    std::vector<double> d_lookB_WRT_LON;
    std::vector<double> d_lookB_WRT_RAD;

//    std::vector<double> TC; // platform to camera  (constant rotation matrix)
//    std::vector<double> TB; // J2000 to platform (time-based rotation matrix)
//    std::vector<double> CJ; // J2000 to Camera (product of TB and TC)

    Camera *pCamera = NULL;
//    double fl;

    double dMeasuredx, dComputedx, dMeasuredy, dComputedy;
    double deltax, deltay;
    double dObservationSigma;
    // Compute fixed point in body-fixed coordinates km
//    latrec_c((double) point->GetAdjustedSurfacePoint().GetLocalRadius().GetKilometers(),
//             (double) point->GetAdjustedSurfacePoint().GetLongitude().GetRadians(),
//             (double) point->GetAdjustedSurfacePoint().GetLatitude().GetRadians(),
//             pB);

    double dObservationWeight;

    // auxiliary variables
//    double NX_C,NY_C,D_C;
//    double NX,NY;
//    double a1,a2,a3;
//    double z1,z2,z3,z4;

//    double dTime = -1.0;

    // partials for fixed point w/r lat, long, radius in Body-Fixed (?)
    // need to verify this
    // For now move entire point partial below.  Maybe later be more efficient  ----DC
    // and split the GetdXYdPoint method of CameraGroundMap into PointPartial part
    // and the rest like Ken has it.

//    d_lookB_WRT_LAT = PointPartial(point,WRT_Latitude);
//    d_lookB_WRT_LON = PointPartial(point,WRT_Longitude);
//    d_lookB_WRT_RAD = PointPartial(point,WRT_Radius);

    pCamera = measure.Camera();

    // Compute fixed point in body-fixed coordinates

//  printf("Lat: %20.10lf  Long: %20.10lf   Radius: %20.10lf\n",point.UniversalLatitude(),point.UniversalLongitude(),point.Radius());

//    latrec_c( point.Radius() * 0.001,
//             (point.UniversalLongitude() * DEG2RAD),
//             (point.UniversalLatitude() * DEG2RAD),
//             pB);

    // clear partial derivative matrices and vectors
    coeff_image.clear();
    coeff_point3D.clear();
    coeff_RHS.clear();

    // Get focal length with direction
//    fl = pCamera->DistortionMap()->UndistortedFocalPlaneZ();

    // no need to call SetImage for framing camera ( CameraType  = 0 )
    if (pCamera->GetCameraType() != 0) {
      // Set the Spice to the measured point
      // but, can this be simplified???
      pCamera->SetImage(measure.GetSample(), measure.GetLine());
    }

    //Compute the look vector in instrument coordinates based on time of observation and apriori lat/lon/radius
    if (!(pCamera->GroundMap()->GetXY(point.GetAdjustedSurfacePoint(), &dComputedx, &dComputedy))) {
      std::string msg = "Unable to map apriori surface point for measure ";
      msg += measure.GetCubeSerialNumber() + " on point " + point.GetId() + " into focal plane";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // partials for fixed point w/r lat, long, radius in Body-Fixed
    d_lookB_WRT_LAT = pCamera->GroundMap()->PointPartial(point.GetAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = pCamera->GroundMap()->PointPartial(point.GetAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Longitude);
    d_lookB_WRT_RAD = pCamera->GroundMap()->PointPartial(point.GetAdjustedSurfacePoint(),
                      CameraGroundMap::WRT_Radius);

//    std::cout << "d_lookB_WRT_LAT" << d_lookB_WRT_LAT << std::endl;
//    std::cout << "d_lookB_WRT_LON" << d_lookB_WRT_LON << std::endl;
//    std::cout << "d_lookB_WRT_RAD" << d_lookB_WRT_RAD << std::endl;


    //    SpiceRotation* pBodyRot = pCamera->BodyRotation();

    // "InstumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
    // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
//    sB = pBodyRot->ReferenceVector(pCamera->InstrumentPosition()->Coordinate());

//    lookB[0] = pB[0] - sB[0];
//    lookB[1] = pB[1] - sB[1];
//    lookB[2] = pB[2] - sB[2];

    // get look vector in the camera frame
//    lookJ = pBodyRot->J2000Vector( lookB );

//    SpiceRotation* pInstRot = pCamera->InstrumentRotation();
//    lookC = pInstRot->ReferenceVector(lookJ);

    // get J2000 to camera rotation matrix
//    CJ = pCamera->InstrumentRotation()->Matrix();

//    std::cout << CJ << std::endl;

    // collinearity auxiliaries
//    NX_C = CJ[0]*lookJ[0] + CJ[1]*lookJ[1] + CJ[2]*lookJ[2];
//    NY_C = CJ[3]*lookJ[0] + CJ[4]*lookJ[1] + CJ[5]*lookJ[2];
//     D_C = CJ[6]*lookJ[0] + CJ[7]*lookJ[1] + CJ[8]*lookJ[2];
//      a1 = fl/D_C;
//      a2 = NX_C/D_C;
//      a3 = NY_C/D_C;

    int nIndex = 0;

    if (m_spacecraftPositionSolveType != Nothing) {
//      SpicePosition* pInstPos = pCamera->InstrumentPosition();

      // Add the partial for the x coordinate of the position (differentiating
      // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
//      coeff_image(0,nIndex) = a1 * (CJ[6]*a2 - CJ[0]);
//      coeff_image(1,nIndex) = a1 * (CJ[6]*a3 - CJ[3]);

      for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, icoef, &coeff_image(0, nIndex), &coeff_image(1, nIndex));
        nIndex++;
      }

//      std::cout << coeff_image << std::endl;
//      if ( m_spacecraftPositionSolveType > PositionOnly ) {
//        dTime = pInstPos->EphemerisTime() - pInstPos->GetBaseTime();
//        dTime = dTime/pInstRot->GetTimeScale();
//
//        coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//        coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//        nIndex++;
//
//        if ( m_spacecraftPositionSolveType == PositionVelocityAcceleration ) {
//          coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//          coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//          nIndex++;
//        }
//      }

      // Add the partial for the y coordinate of the position
//      coeff_image(0,nIndex) = a1 * (CJ[7]*a2 - CJ[1]);
//      coeff_image(1,nIndex) = a1 * (CJ[7]*a3 - CJ[4]);
      for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, icoef, &coeff_image(0, nIndex), &coeff_image(1, nIndex));
        nIndex++;
      }

//      if ( m_spacecraftPositionSolveType > PositionOnly ) {
//         coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//         coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//         nIndex++;
//
//         if ( m_spacecraftPositionSolveType == PositionVelocityAcceleration ) {
//           coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//           coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//           nIndex++;
//         }
//      }
//
      // Add the partial for the z coordinate of the position
//      coeff_image(0,nIndex) = a1 * (CJ[8]*a2 - CJ[2]);
//      coeff_image(1,nIndex) = a1 * (CJ[8]*a3 - CJ[5]);
      for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
        pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, icoef, &coeff_image(0, nIndex), &coeff_image(1, nIndex));
        nIndex++;
      }

//      if ( m_spacecraftPositionSolveType > PositionOnly ) {
//        coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//        coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//        nIndex++;
//
//        if ( m_spacecraftPositionSolveType == PositionVelocityAcceleration ) {
//          coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//          coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//          nIndex++;
//        }
//      }
    }
//
    if (m_cmatrixSolveType != None) {
//      TC = pInstRot->ConstantMatrix();
//      TB = pInstRot->TimeBasedMatrix();
//
//      dTime = pInstRot->EphemerisTime() - pInstRot->GetBaseTime();
//      dTime = dTime/pInstRot->GetTimeScale();
//
//      // additional collinearity auxiliaries
//      NX = TB[0]*lookJ[0] + TB[1]*lookJ[1] + TB[2]*lookJ[2];
//      NY = TB[3]*lookJ[0] + TB[4]*lookJ[1] + TB[5]*lookJ[2];

      // Add the partials for ra
      for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
//        if(  icoef == 0 )
//        {
//          z1 = -TB[1]*lookJ[0]+TB[0]*lookJ[1];
//          z2 = -TB[4]*lookJ[0]+TB[3]*lookJ[1];
//          z3 = -TB[7]*lookJ[0]+TB[6]*lookJ[1];
//          z4 =  TC[6]*z1+TC[7]*z2+TC[8]*z3;
//
//          coeff_image(0,nIndex) = a1*(TC[0]*z1+TC[1]*z2+TC[2]*z3 - z4*a2);
//          coeff_image(1,nIndex) = a1*(TC[3]*z1+TC[4]*z2+TC[5]*z3 - z4*a3);
//          nIndex++;
//        }
//        else
//        {
//          coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//          coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//          nIndex++;
//        }
        pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension, icoef, &coeff_image(0, nIndex), &coeff_image(1, nIndex));
        nIndex++;
      }


      // Add the partials for dec
      for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
//        if(  icoef == 0 )
//        {
//          d_lookC = pInstRot->ToReferencePartial(lookJ,SpiceRotation::WRT_Declination, icoef);
//          coeff_image(0,nIndex) = fl * LowDHigh(lookC,d_lookC,0);
//          coeff_image(1,nIndex) = fl * LowDHigh(lookC,d_lookC,1);
//          nIndex++;
//        }
//        else
//        {
//          coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//          coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//          nIndex++;
//        }
        pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination, icoef, &coeff_image(0, nIndex), &coeff_image(1, nIndex));
        nIndex++;
      }

      // Add the partial for twist if necessary
      if (m_bSolveTwist) {
//        z1 = TC[6]*NY-TC[7]*NX;
//
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
//          if(  icoef == 0 )
//          {
//            coeff_image(0,nIndex) = a1*(((TC[0]*NY-TC[1]*NX) - z1*a2));
//            coeff_image(1,nIndex) = a1*(((TC[3]*NY-TC[4]*NX) - z1*a3));
//            nIndex++;
//          }
//          else
//          {
//            coeff_image(0,nIndex) = coeff_image(0,nIndex-1) * dTime;
//            coeff_image(1,nIndex) = coeff_image(1,nIndex-1) * dTime;
//            nIndex++;
//          }
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist, icoef, &coeff_image(0, nIndex), &coeff_image(1, nIndex));
          nIndex++;
        }
      }
    }

    // partials for 3D point
//    d_lookJ = pBodyRot->J2000Vector(d_lookB_WRT_LAT);
//    d_lookC = pInstRot->ReferenceVector(d_lookJ);

//    coeff_point3D(0,0) = fl * LowDHigh(lookC,d_lookC,0);
//    coeff_point3D(1,0) = fl * LowDHigh(lookC,d_lookC,1);
//
//    d_lookJ = pBodyRot->J2000Vector(d_lookB_WRT_LON);
//    d_lookC = pInstRot->ReferenceVector(d_lookJ);
//
//    coeff_point3D(0,1) = fl * LowDHigh(lookC,d_lookC,0);
//    coeff_point3D(1,1) = fl * LowDHigh(lookC,d_lookC,1);
//
//    d_lookJ = pBodyRot->J2000Vector(d_lookB_WRT_RAD);
//    d_lookC = pInstRot->ReferenceVector(d_lookJ);
//
//    coeff_point3D(0,2) = fl * LowDHigh(lookC,d_lookC,0);
//    coeff_point3D(1,2) = fl * LowDHigh(lookC,d_lookC,1);

    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LAT, &coeff_point3D(0, 0),
                                       &coeff_point3D(1, 0));
    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LON, &coeff_point3D(0, 1),
                                       &coeff_point3D(1, 1));

    // test added to check old test case that didn't solve for radius
    //    if (m_bSolveRadii || m_strSolutionMethod == "SPARSE")
      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RAD, &coeff_point3D(0, 2),
                                       &coeff_point3D(1, 2));

//    std::cout << coeff_point3D << std::endl;
    // right-hand side (measured - computed)
    dMeasuredx = measure.GetFocalPlaneMeasuredX();
//    dComputedx = lookC[0] * fl / lookC[2];

    dMeasuredy = measure.GetFocalPlaneMeasuredY();
//    dComputedy = lookC[1] * fl / lookC[2];

    deltax = dMeasuredx - dComputedx;
    deltay = dMeasuredy - dComputedy;

    coeff_RHS(0) = deltax;
    coeff_RHS(1) = deltay;

    dObservationSigma = 1.4 * pCamera->PixelPitch();
    dObservationWeight = 1.0 / dObservationSigma;

//    std::cout << "Measuredx " << dMeasuredx << " Measuredy = " << dMeasuredy << std::endl;
//    std::cout << "dComputedx " << dComputedx << " dComputedy = " << dComputedy << std::endl;
//    std::cout << coeff_image << std::endl;
//    std::cout << coeff_point3D << std::endl;
//    std::cout << dMeasuredx << " " << dComputedx << std::endl << dMeasuredy << " " << dComputedy << std::endl;
//    std::cout << coeff_RHS << std::endl;

    // multiply coefficients by observation weight
    coeff_image *= dObservationWeight;
    coeff_point3D *= dObservationWeight;
    coeff_RHS *= dObservationWeight;

    m_Statsx.AddData(deltax);
    m_Statsy.AddData(deltay);

    return true;
  }
  */

  /**
  * The solve method is an least squares solution for updating the camera
  * pointing.  It is iterative as the equations are non-linear.  If it does
  * not iterate to a solution in maxIterations it will throw an error.  During
  * each iteration it is updating portions of the control net, as well as the
  * instrument pointing in the camera
  .  An error is thrown if it does not
  * converge in the maximum iterations.  However, even if an error is thrown
  * the control network will contain the errors at each control measure.
  *
  * @param tol             Maximum pixel error for any control network
  *                        measurement
  * @param maxIterations   Maximum iterations, if tolerance is never
  *                        met an iException will be thrown.
  */
  double BundleAdjust::Solve() {
    double averageError;
    std::vector<int> observationInitialValueIndex;  // image index for observation inital values
    int iIndex = -1;                                // image index for initial spice for an observation
    int oIndex = -1;                                // Index of current observation

    ComputeNumberPartials();

    ComputeImageParameterWeights();

//    InitializePoints();

    if (m_bObservationMode)
      observationInitialValueIndex.assign(m_pObsNumList->ObservationSize(), -1);

    for (int i = 0; i < Images(); i++) {
      Camera *pCamera = m_pCnet->Camera(i);

      if (m_bObservationMode) {
        oIndex = i;
        oIndex = m_pObsNumList->ObservationNumberMapIndex(oIndex);   // get observation index for this image
        iIndex = observationInitialValueIndex[oIndex];          // get image index for initial observation values
      }

      if (m_cmatrixSolveType != None) {
        // For observations, find the index of the first image and use its polynomial for the observation
        // initial coefficient values.  Initialize indices to -1

        // Fit the camera pointing to an equation
        SpiceRotation *pSpiceRot = pCamera->InstrumentRotation();

        if (!m_bObservationMode) {
          pSpiceRot->SetPolynomialDegree(m_nckDegree);   // Set the ck polynomial fit degree
          pSpiceRot->SetPolynomial();
          pSpiceRot->SetPolynomialDegree(m_nsolveCamDegree);   // Update to the solve polynomial fit degree
        }
        else {
          // Index of image to use for initial values is set already so set polynomial to initial values
          if (iIndex >= 0) {
            SpiceRotation *pOrot = m_pCnet->Camera(iIndex)->InstrumentRotation(); //Observation rotation
            std::vector<double> anglePoly1, anglePoly2, anglePoly3;
            pOrot->GetPolynomial(anglePoly1, anglePoly2, anglePoly3);
            double baseTime = pOrot->GetBaseTime();
            double timeScale = pOrot->GetTimeScale();
            pSpiceRot->SetPolynomialDegree(m_nsolveCamDegree);   // Update to the solve polynomial fit degree
            pSpiceRot->SetOverrideBaseTime(baseTime, timeScale);
            pSpiceRot->SetPolynomial(anglePoly1, anglePoly2, anglePoly3);
          }
          else {
            // Index of image to use for inital observation values has not been assigned yet so use this image
            pSpiceRot->SetPolynomialDegree(m_nckDegree);
            pSpiceRot->SetPolynomial();
            pSpiceRot->SetPolynomialDegree(m_nsolveCamDegree);   // Update to the solve polynomial fit degree
            observationInitialValueIndex[oIndex] = i;
          }
        }
      }

      if (m_spacecraftPositionSolveType != Nothing) {
        // Set the spacecraft position to an equation
        SpicePosition *pSpicePos = pCamera->InstrumentPosition();

        if (!m_bObservationMode)
          pSpicePos->SetPolynomial();
        else {
          // Index of image to use for initial values is set already so set polynomial to initial values
          if (iIndex >= 0) {
            SpicePosition *pOpos = m_pCnet->Camera(iIndex)->InstrumentPosition(); //Observation position
            std::vector<double> posPoly1, posPoly2, posPoly3;
            pOpos->GetPolynomial(posPoly1, posPoly2, posPoly3);
            double baseTime = pOpos->GetBaseTime();
            double timeScale = pOpos->GetTimeScale();
            pSpicePos->SetPolynomial();
            pSpicePos->SetOverrideBaseTime(baseTime, timeScale);
            pSpicePos->SetPolynomial(posPoly1, posPoly2, posPoly3);
          }
          else {
            // Index of image to use for inital observation values has not been assigned yet so use this image
            pSpicePos->SetPolynomial();
            observationInitialValueIndex[oIndex] = i;
          }
        }
      }
    }

    // Compute the apriori lat/lons for each nonheld point
    m_pCnet->ComputeApriori();

    // Initialize solution parameters
    double sigmaXY, sigmaHat, sigmaX, sigmaY;
    sigmaXY = sigmaHat = sigmaX = sigmaY = 0.;
    m_nIteration = -1;

    clock_t t1 = clock();

    // Create the basis function and prep for a least squares solution
    m_nBasisColumns = BasisColumns();
    BasisFunction basis("Bundle", m_nBasisColumns, m_nBasisColumns);
    if (m_strSolutionMethod == "OLDSPARSE") {
      m_pLsq = new Isis::LeastSquares(basis, true,
                                      m_pCnet->GetNumValidMeasures() * 2, m_nBasisColumns, true);
        SetParameterWeights();
    }
    else
      m_pLsq = new Isis::LeastSquares(basis);

    // set size of partial derivative vectors
    m_dxKnowns.resize(m_nBasisColumns);
    m_dyKnowns.resize(m_nBasisColumns);

    double dprevious_Sigma0 = 10;
    m_dSigma0 = 0.0;

    Progress progress;

    while (m_nIteration < m_nMaxIterations) {
      m_nIteration++;

      m_pCnet->ComputeResiduals();
      m_dError = m_pCnet->GetMaximumResidual();
      averageError = m_pCnet->AverageResidual();

      // kle testing - print residual(?) statistics
//       printf("Iteration #%d\n\tAverage Error: %lf\n\tSigmaXY: %lf\n\tSigmaHat: %lf\n\tSigmaX: %lf\n\tSigmaY: %lf\n",
//              m_nIteration, averageError, sigmaXY, sigmaHat, sigmaX, sigmaY);

//      if (m_bPrintSummary)
      IterationSummary(averageError, sigmaXY, sigmaHat, sigmaX, sigmaY);

      // these vectors hold statistics for right-hand sides (observed - computed)
      m_Statsx.Reset();
      m_Statsy.Reset();

      // these vectors hold statistics for true residuals
      m_Statsrx.Reset();
      m_Statsry.Reset();
      m_Statsrxy.Reset();

      if ( m_nIteration == 0 )
        sigmaHat = 10.0;

      // we've converged
      if (fabs(dprevious_Sigma0 - m_dSigma0) <= m_dConvergenceThreshold) {
        clock_t t2 = clock();

        m_bConverged = true;

        m_dElapsedTime = ((t2 - t1) / (double)CLOCKS_PER_SEC);

        // retrieve vectors of image and point parameter corrections
        GetSparseParameterCorrections();

        if (m_bErrorPropagation) {
          progress.SetText("Performing Error Propagation...");

          //          printf("start error prop\n");
          clock_t terror1 = clock();
          if (m_pLsq->SparseErrorPropagation())
            SetPostBundleSigmas();
          clock_t terror2 = clock();
          m_dElapsedTimeErrorProp = ((terror2 - terror1) / (double)CLOCKS_PER_SEC);
          //          printf("end error prop\n");
        }

        ComputeBundleStatistics();
        Output();

        return m_dError;
    }

      dprevious_Sigma0 = m_dSigma0;

      if ( m_nIteration > 0 )
        m_pLsq->Reset();

      // Loop through the control net and add the partials for each point
      // need generic 'AddPartials' function which calls necessary partials
      // function dependent on sensor, i.e., frame, pushframe, linescan, radar?
      int nObjectPoints = m_pCnet->GetNumPoints();
      for (int i = 0; i < nObjectPoints; i++)
        AddPartials(i);

      // Try to solve the iteration
      try {
        if (m_strSolutionMethod == "SVD") {
          m_pLsq->Solve(Isis::LeastSquares::SVD);
        }
        else if (m_strSolutionMethod == "QRD") {
          m_pLsq->Solve(Isis::LeastSquares::QRD);
        }
        // next is the old SPARSE solution
        else {

          int zeroColumn = m_pLsq->Solve(Isis::LeastSquares::SPARSE);

          if (zeroColumn != 0) {
            std::string msg;
            int imageColumns = Observations() * m_nNumImagePartials;
            if (zeroColumn <= imageColumns) {
              msg = "Solution matrix has a column of zeros which probably ";
              msg += "indicates an image with no points.  Running the program, ";
              msg += "cnetcheck, before jigsaw should catch these problems.";
            }
            else {
              msg = "Solution matrix has a column of zeros which probably ";
              msg += "indicates a point with no measures.  Running the program, ";
              msg += "cnetcheck, before jigsaw should catch these problems.";
            }
            throw Isis::iException::Message(iException::Math, msg, _FILEINFO_);
          }
        }
      }
      catch (iException &e) {
        std::string msg = "Unable to solve in BundleAdjust, ";
        msg += "Iteration " + Isis::iString(m_nIteration) + " of ";
        msg += Isis::iString(m_nMaxIterations) + ", Sigma0 = ";
        msg += Isis::iString(m_dConvergenceThreshold);
        throw Isis::iException::Message(iException::Math, msg, _FILEINFO_);
      }

      // Ok take the results and put them back into the camera blobs
      Update(basis);

      // get residuals and load into statistics vectors
      std::vector<double> residuals = m_pLsq->Residuals();
      int nresiduals = residuals.size();
      for (int i = 0; i < nresiduals; i++) {
        m_Statsrx.AddData(residuals[i]);
        m_Statsry.AddData(residuals[i+1]);
        i++;
      }

      m_Statsrxy.AddData(&residuals[0], nresiduals);

      m_nObservations = m_pLsq->Knowns();
      m_nUnknownParameters = m_nBasisColumns;

      //Compute stats for residuals
      double drms_rx  = sqrt(m_Statsrx.SumSquare() / (m_nObservations / 2));
      double drms_ry  = sqrt(m_Statsry.SumSquare() / (m_nObservations / 2));
      double drms_rxy = sqrt(m_Statsrxy.SumSquare() / m_nObservations);
      double davg_rxy = m_Statsrxy.Average();
      printf("avg rxy: %20.10lf\nrms x: %20.10lf\nrms y: %20.10lf\nrms xy: %20.10lf\n", davg_rxy, drms_rx, drms_ry, drms_rxy);

      sigmaXY = sqrt((m_Statsx.SumSquare() + m_Statsy.SumSquare()) / m_pLsq->Knowns());
      m_nDegreesOfFreedom = m_pLsq->GetDegreesOfFreedom();
      sigmaHat = (m_nObservations - m_nBasisColumns) ?
                 (sqrt((m_Statsx.SumSquare() + m_Statsy.SumSquare()) / m_nDegreesOfFreedom))
                 : 0.;

      m_dSigma0 = m_pLsq->GetSigma0();

      printf("Observations: %d   Unknowns: %d\n", m_nObservations, m_nUnknownParameters);
      printf("SigmaHat: %20.10lf   Sigma0: %20.10lf\n", sigmaHat, m_dSigma0);

      sigmaX = m_Statsx.TotalPixels() ?
               sqrt(m_Statsx.SumSquare() / m_Statsx.TotalPixels()) : 0.;
      sigmaY = m_Statsy.TotalPixels() ?
               sqrt(m_Statsy.SumSquare() / m_Statsy.TotalPixels()) : 0.;
    }

    std::string msg = "Did not converge to Sigma0 criteria [";
    msg += iString(m_dConvergenceThreshold) + "] in less than [";
    msg += iString(m_nMaxIterations) + "] iterations";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  /**
   * Retrieve parameter correction vector for old sparse least-squares object
   * and parse into m_Image_Corrections and m_Point_Corrections vectors so we
   * can use the same output as SpecialK and Cholmod Sparse solutions
   */
  void BundleAdjust::GetSparseParameterCorrections() {

      int nValidPoints = m_pCnet->GetNumValidPoints();
      int nTotalPoints = m_pCnet->GetNumPoints();
      int nPointCorrections = 3 * nValidPoints;
      m_Point_Corrections.resize(nValidPoints);

      m_dEpsilons = m_pLsq->GetEpsilons();
      int nCorrections = m_dEpsilons.size();
      int nImageCorrections = nCorrections - nPointCorrections;
      m_Image_Corrections.resize(nImageCorrections);

      // fill image corrections
      for( int i = 0; i < nImageCorrections; i++ )
          m_Image_Corrections[i] = m_dEpsilons[i];

      // fill point corrections
      int nindex = nImageCorrections;
      int nPointIndex = 0;
      for ( int i = 0; i < nTotalPoints; i++ ) {

          const ControlPoint *point = m_pCnet->GetPoint(i);
          if ( point->IsIgnored() )
              continue;

          bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

          corrections[0] = m_dEpsilons[nindex];
          corrections[1] = m_dEpsilons[nindex+1];
          corrections[2] = m_dEpsilons[nindex+2];

          nindex += 3;
          nPointIndex++;
      }
  }

  /**
   * Populate the least squares matrix with measures for a point
   * specific to frame cameras (for now)
   */
  void BundleAdjust::AddPartials(int nPointIndex) {
    const ControlPoint *point = m_pCnet->GetPoint(nPointIndex);

    if (point->IsIgnored())
      return;

    // pointers to partial derivative vectors
    // ***** can this be a 2xm gmm sparse matrix?
    // ***** or 2 1xm gmm sparse vectors?
    double *px = &m_dxKnowns[0];
    double *py = &m_dyKnowns[0];

    // additional vectors
    std::vector<double> d_lookB_WRT_LAT;
    std::vector<double> d_lookB_WRT_LON;
    std::vector<double> d_lookB_WRT_RAD;

    std::vector<double> TC; // platform to camera  (constant rotation matrix)
    std::vector<double> TB; // J2000 to platform (time-based rotation matrix)
    std::vector<double> CJ; // J2000 to Camera (product of TB and TC)

    Camera *pCamera = NULL;

    int nIndex;
    double dMeasuredx, dMeasuredy;
    double deltax, deltay;
    double dObservationSigma;
    double dObservationWeight;

    // partials for fixed point w/r lat, long, radius in Body-Fixed km
    d_lookB_WRT_LAT = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetAdjustedSurfacePoint(),
                        CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetAdjustedSurfacePoint(),
                        CameraGroundMap::WRT_Longitude);

    // Test to match old test run that didn't solve for radius
    if (m_bSolveRadii || m_strSolutionMethod == "OLDSPARSE")
    d_lookB_WRT_RAD = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetAdjustedSurfacePoint(),
                        CameraGroundMap::WRT_Radius);

//    std::cout << "d_lookB_WRT_LAT" << d_lookB_WRT_LAT << std::endl;
//    std::cout << "d_lookB_WRT_LON" << d_lookB_WRT_LON << std::endl;
//    std::cout << "d_lookB_WRT_RAD" << d_lookB_WRT_RAD << std::endl;

    int nObservations = point->GetNumMeasures();
    for (int i = 0; i < nObservations; i++) {
      const ControlMeasure *measure = point->GetMeasure(i);
      if (measure->IsIgnored())
        continue;


      // zero partial derivative vectors
      memset(px, 0, m_nBasisColumns * sizeof(double));
      memset(py, 0, m_nBasisColumns * sizeof(double));

      pCamera = measure->Camera();

      // no need to call SetImage for framing camera ( CameraType  = 0 )
      if (pCamera->GetCameraType() != 0) {
        // Set the Spice to the measured point
        // but, can this be simplified???
        if (!pCamera->SetImage(measure->GetSample(), measure->GetLine()))
          printf("\n***Call to Camera::SetImage failed - need to handle this***\n");
      }

      //Compute the look vector in instrument coordinates based on time of observation and apriori lat/lon/radius
      double dComputedx, dComputedy;
      if (!(pCamera->GroundMap()->GetXY(point->GetAdjustedSurfacePoint(), &dComputedx, &dComputedy))) {
        std::string msg = "Unable to map apriori surface point for measure ";
        msg += measure->GetCubeSerialNumber() + " on point " + point->GetId() + " into focal plane";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      // Determine the image index
      nIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());
      nIndex = ImageIndex(nIndex);

      if (m_spacecraftPositionSolveType != Nothing) {

        // Add the partial for the x coordinate of the position (differentiating
        // point(x,y,z) - spacecraftPosition(x,y,z) in J2000

        for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
          pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, icoef,
                                                &px[nIndex], &py[nIndex]);
          nIndex++;
        }

        // Add the partial for the y coordinate of the position
        for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
          pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, icoef, &px[nIndex], &py[nIndex]);
          nIndex++;
        }

        // Add the partial for the z coordinate of the position
        for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
          pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, icoef, &px[nIndex], &py[nIndex]);
          nIndex++;
        }
      }

      if (m_cmatrixSolveType != None) {

        // Add the partials for ra
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension, icoef, &px[nIndex], &py[nIndex]);
          nIndex++;
        }

        // Add the partials for dec
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination, icoef, &px[nIndex], &py[nIndex]);
          nIndex++;
        }

        // Add the partial for twist if necessary
        if (m_bSolveTwist) {
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist, icoef, &px[nIndex], &py[nIndex]);
            nIndex++;
          }
        }
      }

      // partials for 3D point
      if (point->GetType() != ControlPoint::Fixed  ||
          m_strSolutionMethod == "SPECIALK"  ||
          m_strSolutionMethod == "SPARSE"  ||
          m_strSolutionMethod == "OLDSPARSE") {
        nIndex = PointIndex(nPointIndex);
        pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LAT, &px[nIndex],
                                         &py[nIndex]);
        nIndex++;
        pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LON, &px[nIndex],
                                         &py[nIndex]);
        nIndex++;

        // test added to check old test case that didn't solve for radii
        if (m_bSolveRadii || m_strSolutionMethod == "OLDSPARSE")
          pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RAD, &px[nIndex],
                                         &py[nIndex]);

      }
      // right-hand side (measured - computed)
      dMeasuredx = measure->GetFocalPlaneMeasuredX();
      dMeasuredy = measure->GetFocalPlaneMeasuredY();

      deltax = dMeasuredx - dComputedx;
      deltay = dMeasuredy - dComputedy;

      dObservationSigma = 1.4 * pCamera->PixelPitch();
      dObservationWeight = 1.0 / (dObservationSigma * dObservationSigma);

//       std::cout << "yKnowns = ";
//       for (int i=0; i<m_dyKnowns.size(); i++)
//         std::cout << "          " << m_dyKnowns[i] << std::endl;
//       std::cout << std::endl;
//       std::cout << "deltax and deltay = " << deltax << " " << deltay << " " << dObservationWeight << std::endl;



      m_pLsq->AddKnown(m_dxKnowns, deltax, dObservationWeight);
      m_pLsq->AddKnown(m_dyKnowns, deltay, dObservationWeight);

      m_Statsx.AddData(deltax);
      m_Statsy.AddData(deltay);
    }
  }




  /**
   * Populate the least squares matrix with measures for a point
   * specific to frame cameras (for now)
   */
  /*
  void BundleAdjust::AddPartials(int nPointIndex) {
    const ControlPoint *point = m_pCnet->GetPoint(nPointIndex);

    if (point->IsIgnored())
      return;

    // pointers to partial derivative vectors
    // ***** can this be a 2xm gmm sparse matrix?
    // ***** or 2 1xm gmm sparse vectors?
    double *px = &m_dxKnowns[0];
    double *py = &m_dyKnowns[0];

    // additional vectors
//     double pB[3];                        // Point on surface
    std::vector<double> sB(3);           // Spacecraft position in body-fixed coordinates
    std::vector<double> lookB(3);        // "look" vector in body-fixed coordinates
    std::vector<double> lookC(3);        // "look" vector in camera coordinates
    std::vector<double> lookJ(3);        // "look" vector in J2000 coordinates
    std::vector<double> d_lookJ;
    std::vector<double> d_lookC;
    std::vector<double> d_lookB_WRT_LAT;
    std::vector<double> d_lookB_WRT_LON;
    std::vector<double> d_lookB_WRT_RAD;

    std::vector<double> TC; // platform to camera  (constant rotation matrix)
    std::vector<double> TB; // J2000 to platform (time-based rotation matrix)
    std::vector<double> CJ; // J2000 to Camera (product of TB and TC)

    Camera *pCamera = NULL;
//     double fl;

    int nIndex;
//    double dMeasuredx,dComputedx,dMeasuredy,dComputedy;
    double dMeasuredx, dMeasuredy;
    double deltax, deltay;
    double dObservationSigma;
    double dObservationWeight;

    // auxiliary variables
//     double NX_C, NY_C, D_C;
//     double NX, NY;
//     double a1, a2, a3;
//     double z1, z2, z3, z4;

//     double dTime = -1.0;

    // partials for fixed point w/r lat, long, radius in Body-Fixed km
    d_lookB_WRT_LAT = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetAdjustedSurfacePoint(),
                        CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetAdjustedSurfacePoint(),
                        CameraGroundMap::WRT_Longitude);

    // Test to match old test run that didn't solve for radius
    if (m_bSolveRadii || m_strSolutionMethod == "SPARSE")
    d_lookB_WRT_RAD = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetAdjustedSurfacePoint(),
                        CameraGroundMap::WRT_Radius);

//    std::cout << "d_lookB_WRT_LAT" << d_lookB_WRT_LAT << std::endl;
//    std::cout << "d_lookB_WRT_LON" << d_lookB_WRT_LON << std::endl;
//    std::cout << "d_lookB_WRT_RAD" << d_lookB_WRT_RAD << std::endl;

    // Compute fixed point in body-fixed coordinates km
//     latrec_c((double) point->GetAdjustedSurfacePoint().GetLocalRadius().GetKilometers(),
//              (double) point->GetAdjustedSurfacePoint().GetLongitude().GetRadians(),
//              (double) point->GetAdjustedSurfacePoint().GetLatitude().GetRadians(),
//              pB);

    int nObservations = point->GetNumMeasures();
    for (int i = 0; i < nObservations; i++) {
      const ControlMeasure *measure = point->GetMeasure(i);
      if (measure->IsIgnored())
        continue;


      // zero partial derivative vectors
      memset(px, 0, m_nBasisColumns * sizeof(double));
      memset(py, 0, m_nBasisColumns * sizeof(double));

      pCamera = measure->Camera();

      // Get focal length with direction
//       fl = pCamera->DistortionMap()->UndistortedFocalPlaneZ();

      // no need to call SetImage for framing camera ( CameraType  = 0 )
      if (pCamera->GetCameraType() != 0) {
        // Set the Spice to the measured point
        // but, can this be simplified???
        if (!pCamera->SetImage(measure->GetSample(), measure->GetLine()))
          printf("\n***Call to Camera::SetImage failed - need to handle this***\n");
      }

      //Compute the look vector in instrument coordinates based on time of observation and apriori lat/lon/radius
      double dComputedx, dComputedy;
      if (!(pCamera->GroundMap()->GetXY(point->GetAdjustedSurfacePoint(), &dComputedx, &dComputedy))) {
        std::string msg = "Unable to map apriori surface point for measure ";
        msg += measure->GetCubeSerialNumber() + " on point " + point->GetId() + " into focal plane";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      // May need to do back of planet test here TODO
//       SpiceRotation *pBodyRot = pCamera->BodyRotation();

      // "InstumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
      // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
//       sB = pBodyRot->ReferenceVector(pCamera->InstrumentPosition()->Coordinate());

//       lookB[0] = pB[0] - sB[0];
//       lookB[1] = pB[1] - sB[1];
//       lookB[2] = pB[2] - sB[2];

      // get look vector in the camera frame
//       lookJ = pBodyRot->J2000Vector(lookB);
//       SpiceRotation *pInstRot = pCamera->InstrumentRotation();
//       lookC = pInstRot->ReferenceVector(lookJ);

      // get J2000 to camera rotation matrix
//       CJ = pCamera->InstrumentRotation()->Matrix();

      // collinearity auxiliaries
//       NX_C = CJ[0] * lookJ[0] + CJ[1] * lookJ[1] + CJ[2] * lookJ[2];
//       NY_C = CJ[3] * lookJ[0] + CJ[4] * lookJ[1] + CJ[5] * lookJ[2];
//       D_C = CJ[6] * lookJ[0] + CJ[7] * lookJ[1] + CJ[8] * lookJ[2];
//       a1 = fl / D_C;
//       a2 = NX_C / D_C;
//       a3 = NY_C / D_C;

      // Determine the image index
      nIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());
      nIndex = ImageIndex(nIndex);

      if (m_spacecraftPositionSolveType != Nothing) {
//         SpicePosition *pInstPos = pCamera->InstrumentPosition();

        // Add the partial for the x coordinate of the position (differentiating
        // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
        // ***TODO*** check derivative with scale added to dTime
//         px[nIndex] = a1 * (CJ[6] * a2 - CJ[0]);
//         py[nIndex] = a1 * (CJ[6] * a3 - CJ[3]);

        for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
          pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, icoef,
                                                &px[nIndex], &py[nIndex]);
          nIndex++;
        }

//         nIndex++;

//         if (m_spacecraftPositionSolveType > PositionOnly) {
//           dTime = pInstPos->EphemerisTime() - pInstPos->GetBaseTime();
//           dTime = dTime / pInstPos->GetTimeScale();

//           px[nIndex] = px[nIndex-1] * dTime;
//           py[nIndex] = py[nIndex-1] * dTime;
//           nIndex++;

//           if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
//             px[nIndex] = px[nIndex-1] * dTime;
//             py[nIndex] = py[nIndex-1] * dTime;
//             nIndex++;
//           }
//         }

        // Add the partial for the y coordinate of the position
        for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
          pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, icoef, &px[nIndex], &py[nIndex]);
          nIndex++;
        }
//         px[nIndex] = a1 * (CJ[7] * a2 - CJ[1]);
//         py[nIndex] = a1 * (CJ[7] * a3 - CJ[4]);
//         nIndex++;

//         if (m_spacecraftPositionSolveType > PositionOnly) {
//           px[nIndex] = px[nIndex-1] * dTime;
//           py[nIndex] = py[nIndex-1] * dTime;
//           nIndex++;

//           if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
//             px[nIndex] = px[nIndex-1] * dTime;
//             py[nIndex] = py[nIndex-1] * dTime;
//             nIndex++;
//           }
//         }

        // Add the partial for the z coordinate of the position
//         px[nIndex] = a1 * (CJ[8] * a2 - CJ[2]);
//         py[nIndex] = a1 * (CJ[8] * a3 - CJ[5]);
        for (int icoef = 0; icoef < m_spacecraftPositionSolveType; icoef++) {
          pCamera->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, icoef, &px[nIndex], &py[nIndex]);
          nIndex++;
        }
//         nIndex++;

//         if (m_spacecraftPositionSolveType > PositionOnly) {
//           px[nIndex] = px[nIndex-1] * dTime;
//           py[nIndex] = py[nIndex-1] * dTime;

//           nIndex++;

//           if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
//             px[nIndex] = px[nIndex-1] * dTime;
//             py[nIndex] = py[nIndex-1] * dTime;
//             nIndex++;
//           }
//         }
      }

      if (m_cmatrixSolveType != None) {
        
//         TC = pInstRot->ConstantMatrix();
//         TB = pInstRot->TimeBasedMatrix();

//         dTime = pInstRot->EphemerisTime() - pInstRot->GetBaseTime();
//         dTime = dTime / pInstRot->GetTimeScale();

        // additional collinearity auxiliaries
//         NX = TB[0] * lookJ[0] + TB[1] * lookJ[1] + TB[2] * lookJ[2];
//         NY = TB[3] * lookJ[0] + TB[4] * lookJ[1] + TB[5] * lookJ[2];


        // Add the partials for ra
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension, icoef, &px[nIndex], &py[nIndex]);
          
//           if (icoef == 0) {
//             z1 = -TB[1] * lookJ[0] + TB[0] * lookJ[1];
//             z2 = -TB[4] * lookJ[0] + TB[3] * lookJ[1];
//             z3 = -TB[7] * lookJ[0] + TB[6] * lookJ[1];
//             z4 =  TC[6] * z1 + TC[7] * z2 + TC[8] * z3;

//             px[nIndex] = a1 * (TC[0] * z1 + TC[1] * z2 + TC[2] * z3 - z4 * a2);
//             py[nIndex] = a1 * (TC[3] * z1 + TC[4] * z2 + TC[5] * z3 - z4 * a3);
//           }
//           else {
//             px[nIndex] = px[nIndex-1] * dTime;
//             py[nIndex] = py[nIndex-1] * dTime;
//           }
          
          nIndex++;
        }

        // Add the partials for dec
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination, icoef, &px[nIndex], &py[nIndex]);
//        if(  icoef == 0 ) {
//          d_lookC = pInstRot->ToReferencePartial(lookJ,SpiceRotation::WRT_Declination, icoef);
//          px[nIndex] = fl * LowDHigh(lookC,d_lookC,0);
//          py[nIndex] = fl * LowDHigh(lookC,d_lookC,1);
//        }
//        else {
//          px[nIndex] = px[nIndex-1] * dTime;
//          py[nIndex] = py[nIndex-1] * dTime;
//        }
//
//        double z1 = -TB[1]*lookJ[0]+TB[0]*lookJ[1];
//        double z2 = -TB[4]*lookJ[0]+TB[3]*lookJ[1];
//        double z3 = -TB[7]*lookJ[0]+TB[6]*lookJ[1];

//        t1 = a1*(TC[0]*z1+TC[1]*z2+TC[2]*z3 - (TC[6]*z1+TC[7]*z2+TC[8]*z3)*a2);
//        t2 = a1*(TC[3]*z1+TC[4]*z2+TC[5]*z3 - (TC[6]*z1+TC[7]*z2+TC[8]*z3)*a3);

//        printf("xKnowns[%d]: %lf t1: %lf\n", nIndex,m_dxKnowns[nIndex],t1);
//        printf("yKnowns[%d]: %lf t2: %lf\n", nIndex,m_dyKnowns[nIndex],t2);
          nIndex++;
        }

        // Add the partial for twist if necessary
        if (m_bSolveTwist) {
          
//           z1 = TC[6] * NY - TC[7] * NX;
          
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            pCamera->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist, icoef, &px[nIndex], &py[nIndex]);
            
//             if (icoef == 0) {
//               px[nIndex] = a1 * (((TC[0] * NY - TC[1] * NX) - z1 * a2));
//               py[nIndex] = a1 * (((TC[3] * NY - TC[4] * NX) - z1 * a3));
//             }
//             else {
//               px[nIndex] = px[nIndex-1] * dTime;
//               py[nIndex] = py[nIndex-1] * dTime;
//             }
          
            nIndex++;
          }
 //   nIndex = (Images() - m_nHeldImages) * m_nNumImagePartials;
        }
      }

      // partials for 3D point
      if (point->GetType() != ControlPoint::Fixed  ||
          m_strSolutionMethod == "SPECIALK"  ||
          m_strSolutionMethod == "SPARSE") {
        nIndex = PointIndex(nPointIndex);
        pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LAT, &px[nIndex],
                                         &py[nIndex]);
        nIndex++;
        pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LON, &px[nIndex],
                                         &py[nIndex]);
        nIndex++;

        // test added to check old test case that didn't solve for radii
        if (m_bSolveRadii || m_strSolutionMethod == "SPARSE")
          pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RAD, &px[nIndex],
                                         &py[nIndex]);

//    d_lookJ = pBodyRot->J2000Vector(d_lookB_WRT_LAT);
//    d_lookC = pInstRot->ReferenceVector(d_lookJ);
//
//    px[nIndex] = fl * LowDHigh(lookC,d_lookC,0);
//    py[nIndex] = fl * LowDHigh(lookC,d_lookC,1);
//    nIndex++;
//
//    d_lookJ = pBodyRot->J2000Vector(d_lookB_WRT_LON);
//    d_lookC = pInstRot->ReferenceVector(d_lookJ);
//
//    px[nIndex] = fl * LowDHigh(lookC,d_lookC,0);
//    py[nIndex] = fl * LowDHigh(lookC,d_lookC,1);
//    nIndex++;
//
//    d_lookJ = pBodyRot->J2000Vector(d_lookB_WRT_RAD);
//    d_lookC = pInstRot->ReferenceVector(d_lookJ);
//
//    px[nIndex] = fl * LowDHigh(lookC,d_lookC,0);
//    py[nIndex] = fl * LowDHigh(lookC,d_lookC,1);
//
      }
      // right-hand side (measured - computed)
      dMeasuredx = measure->GetFocalPlaneMeasuredX();
      //      dComputedx = lookC[0] * fl / lookC[2];

      dMeasuredy = measure->GetFocalPlaneMeasuredY();
      //      dComputedy = lookC[1] * fl / lookC[2];

      deltax = dMeasuredx - dComputedx;
      deltay = dMeasuredy - dComputedy;

      dObservationSigma = 1.4 * pCamera->PixelPitch();
      dObservationWeight = 1.0 / (dObservationSigma * dObservationSigma);
      //      dObservationWeight = 1.0 / dObservationSigma;

      // test to match old runs
      //      dObservationWeight = 1.0;

//       std::cout << "yKnowns = ";
//       for (int i=0; i<m_dyKnowns.size(); i++)
//         std::cout << "          " << m_dyKnowns[i] << std::endl;
//       std::cout << std::endl;
//       std::cout << "deltax and deltay = " << deltax << " " << deltay << " " << dObservationWeight << std::endl;



      m_pLsq->AddKnown(m_dxKnowns, deltax, dObservationWeight);
      m_pLsq->AddKnown(m_dyKnowns, deltay, dObservationWeight);

      m_Statsx.AddData(deltax);
      m_Statsy.AddData(deltay);
    }
  }
*/



  /**
   * Triangulates all points (including control points).
   *
   * @param tol             Maximum pixel error for any control network
   *                        measurement
   * @param maxIterations   Maximum iterations, if tolerance is never
   *                        met an iException will be thrown.
   * @return integer        Number of points triangulated
   *                        successfully
   */
  int BundleAdjust::Triangulation(bool bDoApproximation) {
    int nSuccessfullyTriangulated = 0;

    // Loop over all points in control net
    // We will triangulate all points, ultimately using this as a rudimentary means of outlier detection.
    // if the point is control, we triangulate but don't update the coordinates
    int nControlNetPoints = m_pCnet->GetNumPoints();
    for (int i = 0;  i < nControlNetPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if (point->IsIgnored())
        return nSuccessfullyTriangulated;

      if (bDoApproximation) {
        ApproximatePoint_ClosestApproach(*point, i);
//        if( !ApproximatePoint_ClosestApproach() )
        // mark point somehow to ignore it
      }

      // triangulate point
      TriangulatePoint(*point);
//    if( !TriangulatePoint(point) )
//    {
//      flag point to ignore
//    }

      nSuccessfullyTriangulated++;
    }

    return nSuccessfullyTriangulated;
  }

  /**
   * Triangulates an individual point.
   *
   * @param rpoint          reference to control point
   * @param maxIterations   Maximum iterations, if tolerance is never
   *                        met an iException will be thrown.
   * @return integer        Number of points triangulated
   *                        successfully
   */
  bool BundleAdjust::TriangulatePoint(const ControlPoint &rPoint) {
    return true;
  }

  /**
   * Calculates approximate point coordinates by the method of the
   * closest approach of two vectors
   *
   * @param rpoint          reference to control point
   * @param maxIterations   Maximum iterations, if tolerance is never
   *                        met an iException will be thrown.
   * @return bool           flags success or failure
   */
  bool BundleAdjust::ApproximatePoint_ClosestApproach(const ControlPoint &rPoint, int nIndex) {
    ControlMeasure measure1, measure2;
    Camera *pCamera1 = 0;
    Camera *pCamera2 = 0;
    CameraDistortionMap *pDistortionMap1 = 0;
    CameraDistortionMap *pDistortionMap2 = 0;
    CameraFocalPlaneMap *pFocalPlaneMap1 = 0;
    double BaseVector[3];
    double Camera1Position[3]; // note: units are body-fixed in kilometers
    double Camera2Position[3]; // note: units are body-fixed in kilometers
    double Camera1LookVector[3];
    double Camera2LookVector[3];
    double Camera1XCamera2[3];
    double ClosestPoint1[3];
    double ClosestPoint2[3];
    double AveragePoint[3];
    double d;
    double blabla;

    int nClosetApproaches = 0;

//  const char* buf = rPoint.Id().c_str();

    // loop over observations (in Astro Terms "measures")
    int nObservations = rPoint.GetNumMeasures();
    for (int i = 0; i < nObservations - 1; i++) {
      measure1 = *rPoint.GetMeasure(i);
      if (measure1.IsIgnored())
        continue;

      // get camera and distortion map for observation i
      pCamera1 = measure1.Camera();
      if (!pCamera1)
        continue;

      pDistortionMap1 = pCamera1->DistortionMap();
      if (!pDistortionMap1)
        continue;

      pFocalPlaneMap1 = pCamera1->FocalPlaneMap();
      if (!pDistortionMap1)
        continue;

      pCamera1->SetImage(measure1.GetSample(), measure1.GetLine());

      pCamera1->InstrumentPosition(Camera1Position);

//    double TCD = pCamera1->TargetCenterDistance();

      Camera1LookVector[0] =  pDistortionMap1->UndistortedFocalPlaneX();
      Camera1LookVector[1] =  pDistortionMap1->UndistortedFocalPlaneY();
      Camera1LookVector[2] =  pDistortionMap1->UndistortedFocalPlaneZ();

      // normalize vector (make into a unit vector)
      d = Camera1LookVector[0] * Camera1LookVector[0] +
          Camera1LookVector[1] * Camera1LookVector[1] +
          Camera1LookVector[2] * Camera1LookVector[2];

      if (d <= 0.0)
        return false;

      d = sqrt(d);

      Camera1LookVector[0] /= d;
      Camera1LookVector[1] /= d;
      Camera1LookVector[2] /= d;

      // rotate into J2000
      std::vector<double> dummy1(3);
      dummy1[0] = Camera1LookVector[0];
      dummy1[1] = Camera1LookVector[1];
      dummy1[2] = Camera1LookVector[2];
      dummy1 = pCamera1->InstrumentRotation()->J2000Vector(dummy1);

      // rotate into body-fixed
      dummy1 = pCamera1->BodyRotation()->ReferenceVector(dummy1);

      Camera1LookVector[0] = dummy1[0];
      Camera1LookVector[1] = dummy1[1];
      Camera1LookVector[2] = dummy1[2];

      // Get the look vector in the camera frame and the instrument rotation
//      lookJ = cam->BodyRotation()->J2000Vector( lookB );
//      SpiceRotation *instRot = cam->InstrumentRotation();
//      lookC = instRot->ReferenceVector( lookJ);

      for (int j = i + 1; j < nObservations; j++) {
        measure2 = *rPoint.GetMeasure(j);
        if (measure2.IsIgnored())
          continue;

        pCamera2 = measure2.Camera();
        if (!pCamera2)
          continue;

        pDistortionMap2 = pCamera2->DistortionMap();
        if (!pDistortionMap2)
          continue;

        pCamera2->SetImage(measure2.GetSample(), measure2.GetLine());

        pCamera2->InstrumentPosition(Camera2Position);

        Camera2LookVector[0] = pDistortionMap2->UndistortedFocalPlaneX();
        Camera2LookVector[1] = pDistortionMap2->UndistortedFocalPlaneY();
        Camera2LookVector[2] = pDistortionMap2->UndistortedFocalPlaneZ();

        // normalize vector (make into a unit vector)
        d = Camera2LookVector[0] * Camera2LookVector[0] +
            Camera2LookVector[1] * Camera2LookVector[1] +
            Camera2LookVector[2] * Camera2LookVector[2];

        if (d <= 0.0)
          return false;

        d = sqrt(d);

        Camera2LookVector[0] /= d;
        Camera2LookVector[1] /= d;
        Camera2LookVector[2] /= d;

        // rotate into J2000
        dummy1[0] = Camera2LookVector[0];
        dummy1[1] = Camera2LookVector[1];
        dummy1[2] = Camera2LookVector[2];
        dummy1 = pCamera2->InstrumentRotation()->J2000Vector(dummy1);

        // rotate into body-fixed
        dummy1 = pCamera2->BodyRotation()->ReferenceVector(dummy1);

        Camera2LookVector[0] = dummy1[0];
        Camera2LookVector[1] = dummy1[1];
        Camera2LookVector[2] = dummy1[2];

        // base vector between Camera1 and Camera2
        BaseVector[0] = Camera2Position[0] - Camera1Position[0];
        BaseVector[1] = Camera2Position[1] - Camera1Position[1];
        BaseVector[2] = Camera2Position[2] - Camera1Position[2];

        // cross product of Camera1LookVector and Camera2LookVector (SPICE routine)
        vcrss_c(Camera1LookVector, Camera2LookVector, Camera1XCamera2);

        // magnitude^^2 of cross product
        double dmag2 = Camera1XCamera2[0] * Camera1XCamera2[0] +
                       Camera1XCamera2[1] * Camera1XCamera2[1] +
                       Camera1XCamera2[2] * Camera1XCamera2[2];

        // if dmag2 == 0 (or smaller than some epsilon?), then lines are parallel and we're done
        if (dmag2 == 0.0)
          return false;

        SpiceDouble dMatrix[3][3];
        dMatrix[0][0] = BaseVector[0];
        dMatrix[0][1] = BaseVector[1];
        dMatrix[0][2] = BaseVector[2];
        dMatrix[1][0] = Camera2LookVector[0];
        dMatrix[1][1] = Camera2LookVector[1];
        dMatrix[1][2] = Camera2LookVector[2];
        dMatrix[2][0] = Camera1XCamera2[0];
        dMatrix[2][1] = Camera1XCamera2[1];
        dMatrix[2][2] = Camera1XCamera2[2];

        blabla = (double)det_c(dMatrix);

        double t1 = blabla / dmag2;

//        printf("blabla = %lf dmag2 = %lf t1 = %lf\n",blabla,dmag2,t1);

        dMatrix[1][0] = Camera1LookVector[0];
        dMatrix[1][1] = Camera1LookVector[1];
        dMatrix[1][2] = Camera1LookVector[2];

        blabla = (double)det_c(dMatrix);

        double t2 = blabla / dmag2;

//        printf("blabla = %lf dmag2 = %lf t1 = %lf\n",blabla,dmag2,t2);
//        printf("i=%d j=%d\n",i,j);

//        printf("look1: %20.10lf %20.10lf %20.10lf\n",Camera1LookVector[0],Camera1LookVector[1],Camera1LookVector[2]);
//        printf("look2: %20.10lf %20.10lf %20.10lf\n",Camera2LookVector[0],Camera2LookVector[1],Camera2LookVector[2]);

        ClosestPoint1[0] = Camera1Position[0] + t1 * Camera1LookVector[0];
        ClosestPoint1[1] = Camera1Position[1] + t1 * Camera1LookVector[1];
        ClosestPoint1[2] = Camera1Position[2] + t1 * Camera1LookVector[2];

        ClosestPoint2[0] = Camera2Position[0] + t2 * Camera2LookVector[0];
        ClosestPoint2[1] = Camera2Position[1] + t2 * Camera2LookVector[1];
        ClosestPoint2[2] = Camera2Position[2] + t2 * Camera2LookVector[2];

        AveragePoint[0] = (ClosestPoint1[0] + ClosestPoint2[0]) * 0.5;
        AveragePoint[1] = (ClosestPoint1[1] + ClosestPoint2[1]) * 0.5;
        AveragePoint[2] = (ClosestPoint1[2] + ClosestPoint2[2]) * 0.5;

        nClosetApproaches++;
      }
    }

//    AveragePoint[0] /= nClosetApproaches;
//    AveragePoint[1] /= nClosetApproaches;
//    AveragePoint[2] /= nClosetApproaches;

    // convert from body-fixed XYZ to lat,long,radius
//    SpiceDouble rectan[3];
//    rectan[0] = AveragePoint[0];
//    rectan[1] = AveragePoint[1];
//    rectan[2] = AveragePoint[2];

    SpiceDouble lat, lon, rad;
    reclat_c(AveragePoint, &rad, &lon, &lat);

//  double avglat = rPoint.UniversalLatitude();
//  double avglon = rPoint.UniversalLongitude();
//  double avgrad = rPoint.Radius();

    // set the apriori control net value to the closest approach version
    m_pCnet->GetPoint(nIndex)->SetAdjustedSurfacePoint(
        SurfacePoint(
          Latitude(lat, Angle::Radians),
          Longitude(lon, Angle::Radians),
          Distance(rad, Distance::Kilometers)));

    // Compute fixed point in body-fixed coordinates
    double pB[3];
    latrec_c((double) rPoint.GetAdjustedSurfacePoint().GetLocalRadius().GetKilometers(),
             (double) rPoint.GetAdjustedSurfacePoint().GetLongitude().GetRadians(),
             (double) rPoint.GetAdjustedSurfacePoint().GetLatitude().GetRadians(),
             pB);

//    printf("%s: %lf   %lf   %lf\n",rPoint.Id().c_str(), AveragePoint[0],AveragePoint[1],AveragePoint[2]);
//    printf("    %lf   %lf   %lf\n", avglat,avglon,avgrad);
//    printf("    %lf   %lf   %lf\n", lat,lon,rad);
//    printf("    %lf   %lf   %lf\n",pB[0],pB[1],pB[2]);

    return true;
  }

  /**
   * apply parameter corrections
   */
  void BundleAdjust::applyParameterCorrections() {
    if ( m_decompositionMethod == CHOLMOD )
      return applyParameterCorrections_CHOLMOD();
    else
      return applyParameterCorrections_SPECIALK();
  }

  /**
   * apply parameter corrections
   */
  void BundleAdjust::applyParameterCorrections_CHOLMOD() {
//    std::cout << "image corrections: " << m_Image_Corrections << std::endl;
//    std::cout << "   image solution: " << m_Image_Solution << std::endl;

      int index;
      int currentindex = -1;
      bool bsameindex = false;

      // Update selected spice for each image
      int nImages = Images();
      for (int i = 0; i < nImages; i++) {

          if ( m_nHeldImages > 0 )
              if ((m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i))))
                  continue;

          Camera *pCamera = m_pCnet->Camera(i);
          index = ImageIndex(i);
          if( index == currentindex )
              bsameindex = true;
          else
              bsameindex = false;

          currentindex = index;

      if (m_spacecraftPositionSolveType != Nothing) {
        SpicePosition *pInstPos = pCamera->InstrumentPosition();
        std::vector<double> abcX(3), abcY(3), abcZ(3);
        pInstPos->GetPolynomial(abcX, abcY, abcZ);

//        printf("X0:%20.10lf X1:%20.10lf X2:%20.10lf\n",abcX[0],abcX[1],abcX[2]);

        // Update the X coordinate coefficient(s) and sum parameter correction
        abcX[0] += m_Image_Solution(index);
        if ( !bsameindex )
            m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcX[1] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcX[2] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        // Update the Y coordinate coefficient(s)
        abcY[0] += m_Image_Solution(index);
        if ( !bsameindex )
            m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcY[1] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcY[2] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        // Update the Z coordinate coefficient(s)
        abcZ[0] += m_Image_Solution(index);
        if ( !bsameindex )
            m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcZ[1] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcZ[2] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        pInstPos->SetPolynomial(abcX, abcY, abcZ);
      }

      if (m_cmatrixSolveType != None) {
        SpiceRotation *pInstRot = pCamera->InstrumentRotation();
        std::vector<double> coefRA(m_nNumberCameraCoefSolved),
            coefDEC(m_nNumberCameraCoefSolved),
            coefTWI(m_nNumberCameraCoefSolved);
        pInstRot->GetPolynomial(coefRA, coefDEC, coefTWI);

        // Update right ascension coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefRA[icoef] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;
        }

        // Update declination coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefDEC[icoef] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;
        }

        if (m_bSolveTwist) {
          // Update twist coefficient(s)
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            coefTWI[icoef] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        pInstRot->SetPolynomial(coefRA, coefDEC, coefTWI);
      }
    }

    // Update lat/lon for each control point
    double dLatCorr, dLongCorr, dRadCorr;
    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      if( point->IsRejected() ) {
          nPointIndex++;
          continue;
      }

      // get NIC, Q, and correction vector for this point
      bounded_vector<double, 3>& NIC = m_NICs[nPointIndex];
      SparseBlockRowMatrix& Q = m_Qs_CHOLMOD[nPointIndex];
      bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

//      printf("Q\n");
//      std::cout << Q << std::endl;

//      printf("NIC\n");
//      std::cout << NIC << std::endl;

//      std::cout << m_Image_Solution << std::endl;

      // subtract product of Q and nj from NIC
//      NIC -= prod(Q, m_Image_Solution);
      product_AV(-1.0, NIC, Q, m_Image_Solution);

      // get point parameter corrections
      dLatCorr = NIC(0);
      dLongCorr = NIC(1);
      dRadCorr = NIC(2);

//      printf("Point %s Corrections\n Latitude: %20.10lf\nLongitude: %20.10lf\n   Radius: %20.10lf\n",point->GetId().c_str(),dLatCorr, dLongCorr, dRadCorr);
//      std::cout <<"Point " <<  point->GetId().c_str() << " Corrections\n" << "Latitude: " << dLatCorr << std::endl << "Longitude: " << dLongCorr << std::endl << "Radius: " << dRadCorr << std::endl;

      double dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
      double dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
      double dRad = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();

      dLat += RAD2DEG * dLatCorr;
      dLon += RAD2DEG * dLongCorr;

      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if (dLat < -90.0) {
        dLat = -180.0 - dLat;
        dLon = dLon + 180.0;
      }
      if (dLat > 90.0) {
        dLat = 180.0 - dLat;
        dLon = dLon + 180.0;
      }
      while (dLon > 360.0) dLon = dLon - 360.0;
      while (dLon < 0.0) dLon = dLon + 360.0;

      dRad += 1000.*dRadCorr;

//      std::cout << corrections << std::endl;

      // sum and save corrections
      corrections(0) += dLatCorr;
      corrections(1) += dLongCorr;
      corrections(2) += dRadCorr;

//      std::cout << corrections << std::endl;

      SurfacePoint surfacepoint = point->GetAdjustedSurfacePoint();

      surfacepoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                              Longitude(dLon, Angle::Degrees),
                                              Distance(dRad, Distance::Meters));

      point->SetAdjustedSurfacePoint(surfacepoint);

      nPointIndex++;

      // testing
      // Compute fixed point in body-fixed coordinates
//      double pB[3];
//      latrec_c( dRad * 0.001,
//               (dLon * DEG2RAD),
//               (dLat * DEG2RAD),
//               pB);
//      printf("%s %lf %lf %lf\n",point->Id().c_str(),pB[0],pB[1],pB[2]);
    } // end loop over point corrections
  }

  /**
   * apply parameter corrections
   */
  void BundleAdjust::applyParameterCorrections_SPECIALK() {
//    std::cout << "image corrections: " << m_Image_Corrections << std::endl;
//    std::cout << "   image solution: " << m_Image_Solution << std::endl;

      int index;
      int currentindex = -1;
      bool bsameindex = false;

      // Update selected spice for each image
      int nImages = Images();
      for (int i = 0; i < nImages; i++) {

          if ( m_nHeldImages > 0 )
              if ((m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i))))
                  continue;

          Camera *pCamera = m_pCnet->Camera(i);
          index = ImageIndex(i);
          if( index == currentindex )
              bsameindex = true;
          else
              bsameindex = false;

          currentindex = index;

      if (m_spacecraftPositionSolveType != Nothing) {
        SpicePosition *pInstPos = pCamera->InstrumentPosition();
        std::vector<double> abcX(3), abcY(3), abcZ(3);
        pInstPos->GetPolynomial(abcX, abcY, abcZ);

//        printf("X0:%20.10lf X1:%20.10lf X2:%20.10lf\n",abcX[0],abcX[1],abcX[2]);

        // Update the X coordinate coefficient(s) and sum parameter correction
        abcX[0] += m_Image_Solution(index);
        if ( !bsameindex )
            m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcX[1] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcX[2] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        // Update the Y coordinate coefficient(s)
        abcY[0] += m_Image_Solution(index);
        if ( !bsameindex )
            m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcY[1] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcY[2] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        // Update the Z coordinate coefficient(s)
        abcZ[0] += m_Image_Solution(index);
        if ( !bsameindex )
            m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcZ[1] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcZ[2] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        pInstPos->SetPolynomial(abcX, abcY, abcZ);
      }

      if (m_cmatrixSolveType != None) {
        SpiceRotation *pInstRot = pCamera->InstrumentRotation();
        std::vector<double> coefRA(m_nNumberCameraCoefSolved),
            coefDEC(m_nNumberCameraCoefSolved),
            coefTWI(m_nNumberCameraCoefSolved);
        pInstRot->GetPolynomial(coefRA, coefDEC, coefTWI);

        // Update right ascension coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefRA[icoef] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;
        }

        // Update declination coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefDEC[icoef] += m_Image_Solution(index);
          if ( !bsameindex )
              m_Image_Corrections(index) += m_Image_Solution(index);
          index++;
        }

        if (m_bSolveTwist) {
          // Update twist coefficient(s)
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            coefTWI[icoef] += m_Image_Solution(index);
            if ( !bsameindex )
                m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        pInstRot->SetPolynomial(coefRA, coefDEC, coefTWI);
      }
    }

    // Update lat/lon for each control point
    double dLatCorr, dLongCorr, dRadCorr;
    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      if( point->IsRejected() ) {
          nPointIndex++;
          continue;
      }

      // get NIC, Q, and correction vector for this point
      bounded_vector<double, 3>& NIC = m_NICs[nPointIndex];
      compressed_matrix<double>& Q = m_Qs_SPECIALK[nPointIndex];
      bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

//      printf("Q\n");
//      std::cout << Q << std::endl;

//      printf("NIC\n");
//      std::cout << NIC << std::endl;

//      std::cout << m_Image_Solution << std::endl;

      // subtract product of Q and nj from NIC
      NIC -= prod(Q, m_Image_Solution);

      // get point parameter corrections
      dLatCorr = NIC(0);
      dLongCorr = NIC(1);
      dRadCorr = NIC(2);

//      printf("Point %s Corrections\n Latitude: %20.10lf\nLongitude: %20.10lf\n   Radius: %20.10lf\n",point->GetId().c_str(),dLatCorr, dLongCorr, dRadCorr);
//      std::cout <<"Point " <<  point->GetId().c_str() << " Corrections\n" << "Latitude: " << dLatCorr << std::endl << "Longitude: " << dLongCorr << std::endl << "Radius: " << dRadCorr << std::endl;

      double dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
      double dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
      double dRad = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();

      dLat += RAD2DEG * dLatCorr;
      dLon += RAD2DEG * dLongCorr;

      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if (dLat < -90.0) {
        dLat = -180.0 - dLat;
        dLon = dLon + 180.0;
      }
      if (dLat > 90.0) {
        dLat = 180.0 - dLat;
        dLon = dLon + 180.0;
      }
      while (dLon > 360.0) dLon = dLon - 360.0;
      while (dLon < 0.0) dLon = dLon + 360.0;

      dRad += 1000.*dRadCorr;

//      std::cout << corrections << std::endl;

      // sum and save corrections
      corrections(0) += dLatCorr;
      corrections(1) += dLongCorr;
      corrections(2) += dRadCorr;

//      std::cout << corrections << std::endl;

      SurfacePoint surfacepoint = point->GetAdjustedSurfacePoint();

      surfacepoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                              Longitude(dLon, Angle::Degrees),
                                              Distance(dRad, Distance::Meters));

      point->SetAdjustedSurfacePoint(surfacepoint);

      nPointIndex++;

      // testing
      // Compute fixed point in body-fixed coordinates
//      double pB[3];
//      latrec_c( dRad * 0.001,
//               (dLon * DEG2RAD),
//               (dLat * DEG2RAD),
//               pB);
//      printf("%s %lf %lf %lf\n",point->Id().c_str(),pB[0],pB[1],pB[2]);
    } // end loop over point corrections
  }

  double BundleAdjust::ComputeResiduals() {
    double vtpv = 0.0;
    double vtpv_control = 0.0;
    double vtpv_image = 0.0;
    double dWeight;
    double v, vx, vy;

    // clear residual stats vectors
    m_Statsrx.Reset();
    m_Statsry.Reset();
    m_Statsrxy.Reset();

//    m_drms_rx  = sqrt(m_Statsrx.SumSquare()/(m_nObservations/2));
//    m_drms_ry  = sqrt(m_Statsry.SumSquare()/(m_nObservations/2));
//    m_drms_rxy = sqrt(m_Statsrxy.SumSquare()/m_nObservations);

    // vtpv for image coordinates
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      point->ComputeResiduals_Millimeters();

      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored())
          continue;

        dWeight = 1.4 * (measure->Camera())->PixelPitch();
        dWeight = 1.0 / dWeight;
        dWeight *= dWeight;

        vx = measure->GetSampleResidual();
        vy = measure->GetLineResidual();

        //        std::cout << "vx vy" << vx << " " << vy << std::endl;

        // if rejected, don't include in statistics
        if (measure->IsRejected())
          continue;

        m_Statsrx.AddData(vx);
        m_Statsry.AddData(vy);
        m_Statsrxy.AddData(vx);
        m_Statsrxy.AddData(vy);

//      printf("Point: %s rx: %20.10lf  ry: %20.10lf\n",point->Id().c_str(),rx,ry);

        vtpv += vx * vx * dWeight + vy * vy * dWeight;
      }
    }

//     std::cout << "vtpv image = " << vtpv << std::endl;
//     std::cout << "dWeight = " << dWeight << std::endl;

    // add vtpv from constrained 3D points
    int nPointIndex = 0;
    for (int i = 0; i < nObjectPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if ( point->IsIgnored() )
        continue;

      // get weight and correction vector for this point
      bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
      bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

      //printf("Point: %s PointIndex: %d Loop(i): %d\n",point->GetId().c_str(),nPointIndex,i);
      //std::cout << weights << std::endl;
      //std::cout << corrections << std::endl;

      if( weights[0] > 0.0 )
          vtpv_control += corrections[0] * corrections[0] * weights[0];
      if( weights[1] > 0.0 )
          vtpv_control += corrections[1] * corrections[1] * weights[1];
      if( weights[2] > 0.0 )
          vtpv_control += corrections[2] *corrections[2] * weights[2];

      nPointIndex++;
    }

//    std::cout << "vtpv control = " << vtpv_control << std::endl;

    // add vtpv from constrained image parameters
    int n = 0;
    do {
      for (int j = 0; j < m_nNumImagePartials; j++) {
        if (m_dImageParameterWeights[j] > 0.0) {
          v = m_Image_Corrections[n];
          vtpv_image += v * v * m_dImageParameterWeights[j];
        }

        n++;
      }

    }
    while (n < m_nRank);

//    std::cout << "vtpv_image = " << vtpv_image << std::endl;

    vtpv = vtpv + vtpv_control + vtpv_image;

    // Compute rms for all image coordinate residuals
    // separately for x, y, then x and y together
    m_drms_rx = m_Statsrx.Rms();
    m_drms_ry = m_Statsry.Rms();
    m_drms_rxy = m_Statsrxy.Rms();

    return vtpv;
  }

  bool BundleAdjust::WrapUp() {
    // compute residuals in pixels

    // vtpv for image coordinates
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      point->ComputeResiduals();
    }

    ComputeBundleStatistics();

    return true;
  }

  bool BundleAdjust::ComputeBundleStatistics() {
      int nImages = Images();
       int nMeasures;
      int nImageIndex;
      double vsample;
      double vline;

      m_rmsImageSampleResiduals.resize(nImages);
      m_rmsImageLineResiduals.resize(nImages);
      m_rmsImageResiduals.resize(nImages);

      // load image coordinate residuals into statistics vectors
      int nObservation = 0;
      int nObjectPoints = m_pCnet->GetNumPoints();
      for (int i = 0; i < nObjectPoints; i++) {

          const ControlPoint *point = m_pCnet->GetPoint(i);
          if ( point->IsIgnored() )
              continue;

          if ( point->IsRejected() )
              continue;

          nMeasures = point->GetNumMeasures();
          for (int j = 0; j < nMeasures; j++) {

              const ControlMeasure *measure = point->GetMeasure(j);
              if ( measure->IsIgnored() )
                  continue;

              if ( measure->IsRejected() )
                  continue;

              vsample = fabs(measure->GetSampleResidual());
              vline = fabs(measure->GetLineResidual());

              // Determine the image index
              nImageIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());

              // add residuals to pertinent vector
              m_rmsImageSampleResiduals[nImageIndex].AddData(vsample);
              m_rmsImageLineResiduals[nImageIndex].AddData(vline);
              m_rmsImageResiduals[nImageIndex].AddData(vline);
              m_rmsImageResiduals[nImageIndex].AddData(vsample);

              nObservation++;
          }
      }

      if( m_bErrorPropagation )
      {
          m_rmsImageXSigmas.resize(nImages);
          m_rmsImageYSigmas.resize(nImages);
          m_rmsImageZSigmas.resize(nImages);
          m_rmsImageRASigmas.resize(nImages);
          m_rmsImageDECSigmas.resize(nImages);
          m_rmsImageTWISTSigmas.resize(nImages);

          // compute stats for point sigmas
          Statistics sigmaLatitude;
          Statistics sigmaLongitude;
          Statistics sigmaRadius;

          double dSigmaLat, dSigmaLong, dSigmaRadius;

          int nPoints = m_pCnet->GetNumPoints();
          for ( int i = 0; i < nPoints; i++ ) {

              const ControlPoint *point = m_pCnet->GetPoint(i);
              if ( point->IsIgnored() )
                  continue;

              dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().GetMeters();
              dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().GetMeters();
              dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().GetMeters();

              sigmaLatitude.AddData(dSigmaLat);
              sigmaLongitude.AddData(dSigmaLong);
              sigmaRadius.AddData(dSigmaRadius);

              if ( i > 0 ) {
                  if ( dSigmaLat > m_dmaxSigmaLatitude ) {
                      m_dmaxSigmaLatitude = dSigmaLat;
                      m_idMaxSigmaLatitude = point->GetId();
                  }
                  if ( dSigmaLong > m_dmaxSigmaLongitude ) {
                      m_dmaxSigmaLongitude = dSigmaLong;
                      m_idMaxSigmaLongitude = point->GetId();
                  }
                  if ( dSigmaRadius > m_dmaxSigmaRadius ) {
                      m_dmaxSigmaRadius = dSigmaRadius;
                      m_idMaxSigmaRadius = point->GetId();
                  }
                  if ( dSigmaLat < m_dminSigmaLatitude ) {
                      m_dminSigmaLatitude = dSigmaLat;
                      m_idMinSigmaLatitude = point->GetId();
                  }
                  if ( dSigmaLong < m_dminSigmaLongitude ) {
                      m_dminSigmaLongitude = dSigmaLong;
                      m_idMinSigmaLongitude = point->GetId();
                  }
                  if ( dSigmaRadius < m_dminSigmaRadius ) {
                      m_dminSigmaRadius = dSigmaRadius;
                      m_idMinSigmaRadius = point->GetId();
                  }
              }
              else {
                  m_dmaxSigmaLatitude = dSigmaLat;
                  m_dmaxSigmaLongitude = dSigmaLong;
                  m_dmaxSigmaRadius = dSigmaRadius;
                  m_dminSigmaLatitude = dSigmaLat;
                  m_dminSigmaLongitude = dSigmaLong;
                  m_dminSigmaRadius = dSigmaRadius;
              }
          }

          m_drms_sigmaLat = sigmaLatitude.Rms();
          m_drms_sigmaLon = sigmaLongitude.Rms();
          m_drms_sigmaRad = sigmaRadius.Rms();
      }

      return true;
  }

  bool BundleAdjust::ComputeRejectionLimit() {

      double vx, vy;

      int nResiduals = m_nObservations / 2;

      std::vector<double> x_residuals;
      std::vector<double> y_residuals;

      x_residuals.resize(nResiduals);
      y_residuals.resize(nResiduals);

      // load absolute value of residuals into vectors
      int nObservation = 0;
      int nObjectPoints = m_pCnet->GetNumPoints();
      for (int i = 0; i < nObjectPoints; i++) {

          const ControlPoint *point = m_pCnet->GetPoint(i);
          if ( point->IsIgnored() )
              continue;

          if ( point->IsRejected() )
              continue;

          int nMeasures = point->GetNumMeasures();
          for (int j = 0; j < nMeasures; j++) {

              const ControlMeasure *measure = point->GetMeasure(j);
              if ( measure->IsIgnored() )
                  continue;

              if ( measure->IsRejected() )
                  continue;

              vx = measure->GetSampleResidual();
              vy = measure->GetLineResidual();

              x_residuals[nObservation] = fabs(vx);
              y_residuals[nObservation] = fabs(vy);

              nObservation++;
          }
      }

//      std::cout << "x residuals\n" << x_residuals << std::endl;
//      std::cout << "y_residuals\n" << y_residuals << std::endl;

      // sort vectors
      std::sort(x_residuals.begin(), x_residuals.end());
      std::sort(y_residuals.begin(), y_residuals.end());

//      std::cout << "x residuals sorted\n" << x_residuals << std::endl;
//      std::cout << "y_residuals sorted\n" << y_residuals << std::endl;

      double xmedian, ymedian;
      double xmad, ymad;

      int nmidpoint = nResiduals / 2;

      if ( nResiduals % 2 ) {
          xmedian = x_residuals[nmidpoint];
          ymedian = y_residuals[nmidpoint];
      }
      else {
          xmedian = (x_residuals[nmidpoint-1] + x_residuals[nmidpoint]) / 2;
          ymedian = (y_residuals[nmidpoint-1] + y_residuals[nmidpoint]) / 2;
      }

      // compute M.A.D.
      for (int i = 0; i < nResiduals; i++) {
          x_residuals[i] = fabs(x_residuals[i] - xmedian);
          y_residuals[i] = fabs(y_residuals[i] - ymedian);
      }

      std::sort(x_residuals.begin(), x_residuals.end());
      std::sort(y_residuals.begin(), y_residuals.end());

      if ( nResiduals % 2 ) {
          xmad = 1.4826 * x_residuals[nmidpoint];
          ymad = 1.4826 * y_residuals[nmidpoint];
      }
      else {
          xmad = 1.4826 * (x_residuals[nmidpoint-1] + x_residuals[nmidpoint]) / 2;
          ymad = 1.4826 * (y_residuals[nmidpoint-1] + y_residuals[nmidpoint]) / 2;
      }

      m_dRejectionLimit = 10.0 * std::max(xmad, ymad);

      return true;
  }

  bool BundleAdjust::FlagOutliers() {
    double vx, vy;
    int nRejected;
    int ntotalrejected = 0;

    int nIndexMaxResidual = -1;
    double dMaxResidual;
    double dSumSquares;
    double dUsedRejectionLimit = m_dRejectionLimit;

    if ( m_dRejectionLimit < 0.05 )
        dUsedRejectionLimit = 0.05;

    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if ( point->IsIgnored() )
        continue;

      point->ZeroNumberOfRejectedMeasures();

      nRejected = 0;
      dMaxResidual = -1.0;

      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {

          ControlMeasure *measure = point->GetMeasure(j);
          if ( measure->IsIgnored() )
              continue;

          vx = measure->GetSampleResidual();
          vy = measure->GetLineResidual();

          if ( fabs(vx) < dUsedRejectionLimit && fabs(vy) < dUsedRejectionLimit ) {
              if( measure->IsRejected() ) {
                  printf("Coming back in: %s\r",point->GetId().c_str());
                  m_pCnet->DecrementNumberOfRejectedMeasuresInImage(measure->GetCubeSerialNumber());
              }

              measure->SetRejected(false);
              continue;
          }

          // if it's still rejected, skip it
          if ( measure->IsRejected() ) {
              nRejected++;
              ntotalrejected++;
              continue;
          }

          dSumSquares = vx * vx + vy * vy;

          if ( dSumSquares > dMaxResidual ) {
              dMaxResidual = dSumSquares;
              nIndexMaxResidual = j;
          }
      }

      // no observations above the current rejection limit for this 3D point
      if ( dMaxResidual == -1.0 ) {
          point->SetNumberOfRejectedMeasures(nRejected);
          continue;
      }

      // this is another kluge - if we only have two observations
      // we won't reject (for now)
//      if ((nMeasures - (nRejected + 1)) < 2) {
//          point->SetNumberOfRejectedMeasures(nRejected);
//          continue;
//      }

      // otherwise, we have at least one observation
      // for this point whose residual is above the
      // current rejection limit - we'll flag the
      // worst of these as rejected
      ControlMeasure *rejected = point->GetMeasure(nIndexMaxResidual);
      rejected->SetRejected(true);
      nRejected++;
      point->SetNumberOfRejectedMeasures(nRejected);
      m_pCnet->IncrementNumberOfRejectedMeasuresInImage(rejected->GetCubeSerialNumber());
      ntotalrejected++;

      // do we still have sufficient remaining observations for this 3D point?
      if( ( nMeasures-nRejected ) < 2 ) {
          point->SetRejected(true);
          printf("Rejecting Entire Point: %s\r",point->GetId().c_str());
      }
      else
          point->SetRejected(false);

//      int ndummy = point->GetNumberOfRejectedMeasures();
//      printf("Rejected for point %s = %d\n", point->GetId().c_str(), ndummy);
//      printf("%s: %20.10lf  %20.10lf*\n",point->GetId().c_str(), rejected->GetSampleResidual(), rejected->GetLineResidual());
  }

    m_nRejectedObservations = 2*ntotalrejected;

    printf("\n\t       Rejected Observations:%10d (Rejection Limit:%12.5lf\n",
           m_nRejectedObservations, dUsedRejectionLimit);

    return true;
}

  /**
   * error propagation.
   */
  bool BundleAdjust::errorPropagation() {
    if ( m_decompositionMethod == CHOLMOD )
      return errorPropagation_CHOLMOD();
    else
      return errorPropagation_SPECIALK();

    return false;
  }

  bool BundleAdjust::errorPropagation_SPECIALK() {

    // create inverse of normal equations matrix
    if ( !CholeskyUT_NOSQR_Inverse() )
        return false;

    matrix<double> T(3, 3);
    matrix<double> QS(3, m_nRank);
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    double t;

    double dSigma02 = m_dSigma0 * m_dSigma0;

    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
        if ( point->IsIgnored() )
            continue;

        if ( point->IsRejected() )
            continue;

        T.clear();
        QS.clear();

        // get corresponding Q matrix
        compressed_matrix<double>& Q = m_Qs_SPECIALK[nPointIndex];

        // form QS
        QS = prod(Q, m_Normals);

        // form T
        T = prod(QS, trans(Q));

        // Ask Ken what is happening here...Setting just the sigmas is not very accurate
        // Shouldn't we be updating and setting the matrix???  TODO
        SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();

        dSigmaLat = SurfacePoint.GetLatSigma().GetRadians();
        dSigmaLong = SurfacePoint.GetLonSigma().GetRadians();
        dSigmaRadius = SurfacePoint.GetLocalRadiusSigma().GetMeters();

//      std::cout << dSigmaLat << " " << dSigmaLong << " " << dSigmaRadius << std::endl;

//      dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().GetMeters();
//      dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().GetMeters();
//      dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().GetMeters();

        t = dSigmaLat*dSigmaLat + T(0, 0);
        Distance tLatSig(sqrt(dSigma02 * t) * m_dRTM, Distance::Meters);

        t = dSigmaLong*dSigmaLong + T(1, 1);
        t = sqrt(dSigma02 * t) * m_dRTM;
        Distance tLonSig(
            t * cos(point->GetAdjustedSurfacePoint().GetLatitude().GetRadians()),
            Distance::Meters);

        t = dSigmaRadius*dSigmaRadius + T(2, 2);
        t = sqrt(dSigma02 * t) * 1000.0;

        SurfacePoint.SetSphericalSigmasDistance(tLatSig, tLonSig,
            Distance(t, Distance::Meters));

        point->SetAdjustedSurfacePoint(SurfacePoint);

       nPointIndex++;
    }

    return true;
  }

  bool BundleAdjust::errorPropagation_CHOLMOD() {

    // free unneeded memory
    cholmod_free_triplet(&m_pTriplet, &m_cm);
    cholmod_free_sparse(&m_N, &m_cm);
    m_SparseNormals.wipe();

    // create inverse of normal equations matrix
    if ( !cholmod_Inverse() )
      return false;

    matrix<double> T(3, 3);
    matrix<double> QS(3, m_nRank);
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    double t;

    // TODO_CHOLMOD: would rather not make copies like this
    // must be a better way????
    compressed_matrix<double> Q(3,m_nRank);

    double dSigma02 = m_dSigma0 * m_dSigma0;

    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {

      ControlPoint *point = m_pCnet->GetPoint(i);
      if ( point->IsIgnored() )
        continue;

      if ( point->IsRejected() )
        continue;

      T.clear();
      QS.clear();

      // get corresponding Q matrix
      SparseBlockRowMatrix& Qblock = m_Qs_CHOLMOD[nPointIndex];

      // copy into compressed boost matrix
      // TODO_CHOLMOD: don't want to do this
      Qblock.copyToBoost(Q);

//      Qblock.print();
//      std::cout << "Copy of Q" << Q << std::endl;

      // form QS
      QS = prod(Q, m_Normals);

//      ANZmultAdd(Q, m_Normals, QS);

      // form T
      T = prod(QS, trans(Q));

//      AmulttransBNZ(QS, Q, T);

//      std::cout << "T" << std::endl << T << std::endl;

      // Ask Ken what is happening here...Setting just the sigmas is not very
      // accurate
      // Shouldn't we be updating and setting the matrix???  TODO
      SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();

      dSigmaLat = SurfacePoint.GetLatSigma().GetRadians();
      dSigmaLong = SurfacePoint.GetLonSigma().GetRadians();
      dSigmaRadius = SurfacePoint.GetLocalRadiusSigma().GetMeters();

//    std::cout << dSigmaLat << " " << dSigmaLong << " " << dSigmaRadius << std::endl;

//      dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().GetMeters();
//      dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().GetMeters();
//      dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().GetMeters();

      t = dSigmaLat*dSigmaLat + T(0, 0);
      Distance tLatSig(sqrt(dSigma02 * t) * m_dRTM, Distance::Meters);

      t = dSigmaLong*dSigmaLong + T(1, 1);
      t = sqrt(dSigma02 * t) * m_dRTM;
      Distance tLonSig(
          t * cos(point->GetAdjustedSurfacePoint().GetLatitude().GetRadians()),
          Distance::Meters);

      t = dSigmaRadius*dSigmaRadius + T(2, 2);
      t = sqrt(dSigma02 * t) * 1000.0;

      SurfacePoint.SetSphericalSigmasDistance(tLatSig, tLonSig,
                                              Distance(t, Distance::Meters));

      point->SetAdjustedSurfacePoint(SurfacePoint);

      nPointIndex++;
    }

    return true;
  }

  /**
   * This method uses the basis function after the matrix has been solved.
   * The coefficients of the basis function represent the new right ascension,
   * declination, and twist values of the camera.  Each is a polynomial based on
   * time.  For example, ra = A + B * (t - t0) + C * (t - t0)^2.  However,
   * as the function we were solving was non-linear we had to take the
   * dervative to linearize.  Therefore we have the change in ra, dec, and
   * twist. Really we have the change in A, B, and C.
   */
  void BundleAdjust::Update(BasisFunction &basis) {
    // Update selected spice for each image
    int nImages = Images();
    for (int i = 0; i < nImages; i++) {
      if (m_nHeldImages > 0)
        if ((m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))) continue;

      Camera *pCamera = m_pCnet->Camera(i);
      int index = i;
      index = ImageIndex(index);
      if (m_spacecraftPositionSolveType != Nothing) {
        SpicePosition *pInstPos = pCamera->InstrumentPosition();
        std::vector<double> abcX(3), abcY(3), abcZ(3);
        pInstPos->GetPolynomial(abcX, abcY, abcZ);

//        printf("X0:%20.10lf X1:%20.10lf X2:%20.10lf\n",abcX[0],abcX[1],abcX[2]);

        // Update the X coordinate coefficient(s)
        abcX[0] += basis.Coefficient(index);
        index++;
        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcX[1] += basis.Coefficient(index);
          index++;
          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcX[2] += basis.Coefficient(index);
          index++;
          }
        }

        // Update the Y coordinate coefficient(s)
        abcY[0] += basis.Coefficient(index);
        index++;
        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcY[1] += basis.Coefficient(index);
          index++;
          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcY[2] += basis.Coefficient(index);
            index++;
          }
        }

        // Update the Z coordinate cgoefficient(s)
        abcZ[0] += basis.Coefficient(index);
        index++;
        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcZ[1] += basis.Coefficient(index);
          index++;
          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcZ[2] += basis.Coefficient(index);
            index++;
          }
        }

        pInstPos->SetPolynomial(abcX, abcY, abcZ);
      }

      if (m_cmatrixSolveType != None) {
        SpiceRotation *pInstRot = pCamera->InstrumentRotation();
        std::vector<double> coefRA(m_nNumberCameraCoefSolved),
            coefDEC(m_nNumberCameraCoefSolved),
            coefTWI(m_nNumberCameraCoefSolved);
        pInstRot->GetPolynomial(coefRA, coefDEC, coefTWI);

        // Update right ascension coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefRA[icoef] += basis.Coefficient(index);
          index++;
        }

        // Update declination coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefDEC[icoef] += basis.Coefficient(index);
          index++;
        }

        if (m_bSolveTwist) {
          // Update twist coefficient(s)
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            coefTWI[icoef] += basis.Coefficient(index);
            index++;
          }
        }

        pInstRot->SetPolynomial(coefRA, coefDEC, coefTWI);
      }
    }

    // Update lat/lon for each control point
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;
      if (m_strSolutionMethod != "SPECIALK" &&
          m_strSolutionMethod != "SPARSE"  &&
          m_strSolutionMethod != "OLDSPARSE"  &&
          point->GetType() == ControlPoint::Fixed)
        continue;

      double dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
      double dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
      double dRad = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();
      int index = PointIndex(i);
      dLat += RAD2DEG * (basis.Coefficient(index));
      index++;
      dLon += RAD2DEG * (basis.Coefficient(index));
      index++;

      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if (dLat < -90.0) {
        dLat = -180.0 - dLat;
        dLon = dLon + 180.0;
      }
      if (dLat > 90.0) {
        dLat = 180.0 - dLat;
        dLon = dLon + 180.0;
      }
      while (dLon > 360.0) dLon = dLon - 360.0;
      while (dLon < 0.0) dLon = dLon + 360.0;

      // test to match results of old test case that didn't solve for radius
      if ( m_bSolveRadii || m_strSolutionMethod == "OLDSPARSE") {
      dRad += 1000.*basis.Coefficient(index);
      index++;
      }

      // testing
      // Compute fixed point in body-fixed coordinates
      double pB[3];
      latrec_c(dRad * 0.001,
               (dLon * DEG2RAD),
               (dLat * DEG2RAD),
               pB);
//      printf("%s %lf %lf %lf\n",point->Id().c_str(),pB[0],pB[1],pB[2]);

      /*      else {  // Recompute radius to match updated lat/lon... Should this be removed?
              ControlMeasure &m = ((*m_pCnet)[i])[0];
              Camera *cam = m.Camera();
              cam->SetUniversalGround(lat, lon);
              rad = cam->LocalRadius(); //meters
           }*/

      SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();
      SurfacePoint.SetSphericalCoordinates(Latitude(dLat, Angle::Degrees),
                                              Longitude(dLon, Angle::Degrees),
                                              Distance(dRad, Distance::Meters));
      point->SetAdjustedSurfacePoint(SurfacePoint);
    }
  }

  //! Return index to basis function for ith point
  int BundleAdjust::PointIndex(int i) const {
    int nIndex;

    if (!m_bObservationMode)
      nIndex = Images() * m_nNumImagePartials;
    else
      nIndex = Observations() * m_nNumImagePartials;

    nIndex += m_nPointIndexMap[i] * m_nNumPointPartials;

    return nIndex;
  }

  //! Return index to basis function for ith image
  int BundleAdjust::ImageIndex (int i) const
  {
    if ( !m_bObservationMode )
      return m_nImageIndexMap[i] * m_nNumImagePartials;
    else
      return m_pObsNumList->ObservationNumberMapIndex(i) * m_nNumImagePartials;
  }

  //! Return the ith filename in the cube list file given to constructor
  std::string BundleAdjust::Filename(int i) {
//    std::string serialNumber = (*m_pSnList)[i];
//    return m_pSnList->Filename(serialNumber);
    return m_pSnList->Filename(i);
  }


  //! Return whether the ith file in the cube list is held
  bool BundleAdjust::IsHeld(int i) {
    if ( m_nHeldImages > 0 )
         if ((m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i))))
           return true;
    return false;
  }

  //! Return a table cmatrix for the ith cube in the cube list given to the
  //! constructor
  Table BundleAdjust::Cmatrix(int i) {
    return m_pCnet->Camera(i)->InstrumentRotation()->Cache("InstrumentPointing");
  }

  //! Return a table spacecraft vector for the ith cube in the cube list given to the
  //! constructor
  Table BundleAdjust::SpVector(int i) {
    return m_pCnet->Camera(i)->InstrumentPosition()->Cache("InstrumentPosition");
  }

  //! Return the number of observations in list given to the constructor
  int BundleAdjust::Observations() const {
    if (!m_bObservationMode)
      return m_pSnList->Size();
//    return m_pSnList->Size() - m_nHeldImages;
    else
      return m_pObsNumList->ObservationSize();
  }

//std::vector<double> BundleAdjust::PointPartial(Isis::ControlPoint &point, PartialDerivative wrt)
//{
//  double lat = point.GetAdjustedSurfacePoint().GetLatitude();  // Radians
//  double lon = point.GetAdjustedSurfacePoint().GetLongitude();
//  double sinLon = sin(lon);
//  double cosLon = cos(lon);
//  double sinLat = sin(lat);
//  double cosLat = cos(lat);
//  double radius = point.GetAdjustedSurfacePoint().GetLocalRadius() * 0.001;  /km
//
//  std::vector<double> v(3);
//
//  if (wrt == WRT_Latitude)
//  {
//    v[0] = -radius * sinLat * cosLon;
//    v[1] = -radius * sinLon * sinLat;
//    v[2] =  radius * cosLat;
//  }
//  else if (wrt == WRT_Longitude)
//  {
//    v[0] = -radius * cosLat * sinLon;
//    v[1] =  radius * cosLat * cosLon;
//    v[2] =  0.0;
//  }
//  else
//  {
//    v[0] = cosLon * cosLat;
//    v[1] = sinLon * cosLat;
//    v[2] = sinLat;
//  }
//
//  return v;
//}
//
  /**
    * This method creates a iteration summary and creates an iteration group for
    * the Sparse BundleAdjust summary.
    *
    * @param it              Iteration number
    *
    * @param it_time         Iteration time
    *
    * @param avErr           Average error or iteration (pixels)
    *
    * @param sigmaXY         Standard deviation of coordinates (mm)
    *
    * @param sigmaHat        Aposteriori standard deviation of unit weight (mm)
    *
    * @param sigmaX          Standard deviation of deltax (mm)
    *
    * @param sigmaY          Standard deviation of deltay (mm)
    *
    */
  void BundleAdjust::IterationSummary(double avErr, double sigmaXY, double sigmaHat,
                                      double sigmaX, double sigmaY) {
    //Add this iteration to the summary pvl
    std::string itlog = "Iteration" + iString(m_nIteration);
    PvlGroup gp(itlog);

    gp += PvlKeyword("MaximumError", m_dError, "pixels");
    gp += PvlKeyword("AverageError", avErr, "pixels");
    gp += PvlKeyword("SigmaXY", sigmaXY, "mm");
//    gp += PvlKeyword("SigmaHat", sigmaHat, "mm");
    gp += PvlKeyword("Sigma0", m_dSigma0, "mm");
    gp += PvlKeyword("SigmaX", sigmaX, "mm");
    gp += PvlKeyword("SigmaY", sigmaY, "mm");

    std::ostringstream ostr;
    ostr<<gp<<endl;
    m_iterationSummary += QString::fromStdString(ostr.str());
    if (m_bPrintSummary) Application::Log(gp);
  }

  /* This method creates a iteration summary and creates an iteration group for
  * the SpecialK BundleAdjust summary.
  */
  void BundleAdjust::SpecialKIterationSummary() {
    std::string itlog;
    if ( m_bConverged )
        itlog = "Iteration" + iString(m_nIteration) + ": Final";
    else
        itlog = "Iteration" + iString(m_nIteration);
    PvlGroup gp(itlog);

    gp += PvlKeyword("Sigma0", m_dSigma0);
    gp += PvlKeyword("Observations", m_nObservations);
    gp += PvlKeyword("Constrained_Point_Parameters", m_nConstrainedPointParameters);
    gp += PvlKeyword("Constrained_Image_Parameters", m_nConstrainedImageParameters);
    gp += PvlKeyword("Unknown_Parameters", m_nUnknownParameters);
    gp += PvlKeyword("Degrees_of_Freedom", m_nDegreesOfFreedom);
    gp += PvlKeyword("Rejected_Measures", m_nRejectedObservations/2);

    if ( m_bConverged ) {
        gp += PvlKeyword("Converged", "TRUE");
        gp += PvlKeyword("TotalElapsedTime", m_dElapsedTime);

        if ( m_bErrorPropagation )
            gp += PvlKeyword("ErrorPropationElapsedTime", m_dElapsedTimeErrorProp);
    }

    std::ostringstream ostr;
    ostr<<gp<<endl;
    m_iterationSummary += QString::fromStdString(ostr.str());
    if (m_bPrintSummary) Application::Log(gp);
  }

  /**
   * set parameter weighting for old SPARSE solution
   *
   * @history 2011-04-19 Debbie A. Cook - Added initialization to m_Point_AprioriSigmas
   *                      for old sparse method case
   */
  bool BundleAdjust::SetParameterWeights() {

    SetSpaceCraftWeights();
    ComputeImageParameterWeights();

    m_dParameterWeights.resize(m_nBasisColumns);
    std::fill_n(m_dParameterWeights.begin(), m_nBasisColumns, 0.0);
    m_nConstrainedImageParameters = 0;
    m_nConstrainedPointParameters = 0;

    int nconstraintsperimage = std::count_if( m_dImageParameterWeights.begin(),
                                              m_dImageParameterWeights.end(),
                                              std::bind2nd(std::greater<double>(),0.0));

    int nWtIndex = 0;
    int nCurrentIndex = -1;
    int nImages = m_pSnList->Size();
    for( int i = 0; i < nImages; i++ ) {

        nWtIndex = ImageIndex(i);
        if ( nWtIndex == nCurrentIndex )
            continue;

        nCurrentIndex = nWtIndex;

        if ( m_pHeldSnList != NULL &&
             (m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i))) ) {
            std::fill_n(m_dParameterWeights.begin()+nWtIndex, m_nNumImagePartials, 1.0e+50);
            m_nConstrainedImageParameters += m_nNumImagePartials;
        }
        else {
            std::copy(m_dImageParameterWeights.begin(),
                      m_dImageParameterWeights.end(),
                      m_dParameterWeights.begin()+nWtIndex);
            m_nConstrainedImageParameters += nconstraintsperimage;
        }
    }

    // loop over 3D object points
    nWtIndex = m_nImageParameters;
    int nObjectPoints = m_pCnet->GetNumPoints();

    m_Point_AprioriSigmas.resize(nObjectPoints);

    // initialize sigmas to zero
    for (int i = 0; i < nObjectPoints; i++) {
      m_Point_AprioriSigmas[i].clear();
    }

    int nPointIndex = 0;
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      bounded_vector<double, 3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

       SurfacePoint aprioriSurfacePoint = point->GetAprioriSurfacePoint();
//       // How do I solve the compile error regarding the const?
//       //      point->SetAprioriSurfacePoint(aprioriSurfacePoint);

      if (point->GetType() == ControlPoint::Fixed) {
        m_dParameterWeights[nWtIndex] = 1.0e+50;
        m_dParameterWeights[nWtIndex+1] = 1.0e+50;
        m_dParameterWeights[nWtIndex+2] = 1.0e+50;
        m_nConstrainedPointParameters += 3;
      }
      else {
        if( point->IsLatitudeConstrained() ) {
          apriorisigmas[0] = point->GetAprioriSurfacePoint().GetLatSigmaDistance().GetMeters();
          m_dParameterWeights[nWtIndex] = point->GetAprioriSurfacePoint().GetLatWeight();
          m_nConstrainedPointParameters++;
        }
        else if( m_dGlobalLatitudeAprioriSigma > 0.0 ) {
          apriorisigmas[0] = m_dGlobalLatitudeAprioriSigma;
          m_dParameterWeights[nWtIndex] = 1. / (m_dGlobalLatitudeAprioriSigma*m_dMTR);
          m_dParameterWeights[nWtIndex] *= m_dParameterWeights[nWtIndex];
          m_nConstrainedPointParameters++;
        }
      
        if( point->IsLongitudeConstrained() ) {
          apriorisigmas[1] = point->GetAprioriSurfacePoint().GetLonSigmaDistance().GetMeters();
          m_dParameterWeights[nWtIndex+1] = point->GetAprioriSurfacePoint().GetLonWeight();
          m_nConstrainedPointParameters++;
        }
        else if( m_dGlobalLongitudeAprioriSigma > 0.0 ) {
          apriorisigmas[1] = m_dGlobalLongitudeAprioriSigma;
          m_dParameterWeights[nWtIndex+1] = 1. / (m_dGlobalLongitudeAprioriSigma*m_dMTR);
          m_dParameterWeights[nWtIndex+1] *= m_dParameterWeights[nWtIndex+1];
          m_nConstrainedPointParameters++;
        }

        if ( !m_bSolveRadii ) {
          m_dParameterWeights[nWtIndex+2] = 1.0e+50;
          m_nConstrainedPointParameters++;
        }
        else if( point->IsRadiusConstrained() ) {
          apriorisigmas[2] = point->GetAprioriSurfacePoint().GetLocalRadiusSigma().GetMeters();
          m_dParameterWeights[nWtIndex+2] = point->GetAprioriSurfacePoint().GetLocalRadiusWeight();
          m_nConstrainedPointParameters++;
        }
        else if( m_dGlobalRadiusAprioriSigma > 0.0 ) {
          apriorisigmas[2] = m_dGlobalRadiusAprioriSigma;
          m_dParameterWeights[nWtIndex+2] = 1000. / m_dGlobalRadiusAprioriSigma;
          m_dParameterWeights[nWtIndex+2] *= m_dParameterWeights[nWtIndex+2];
          m_nConstrainedPointParameters++;
        }
      }

      nWtIndex += 3;
      nPointIndex++;
    }

    m_pLsq->SetParameterWeights(m_dParameterWeights);

    // Next call added by DAC 02-04-2011...Is this correct? Ask Ken
    m_pLsq->SetNumberOfConstrainedParameters(
      m_nConstrainedPointParameters+m_nConstrainedImageParameters);

    return true;
  }

  void BundleAdjust::SetPostBundleSigmas() {
    double dSigmaLat;
    double dSigmaLong;
    double dSigmaRadius;

    // get reference to the covariance matrix from the least-squares object
    gmm::row_matrix<gmm::rsvector<double> > lsqCovMatrix = m_pLsq->GetCovarianceMatrix();

    int nIndex = m_nImageParameters;

    int nPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      dSigmaLat = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
      dSigmaLat *= m_dRTM;
      nIndex++;

      dSigmaLong = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
      dSigmaLong *= m_dRTM * cos(point->GetAdjustedSurfacePoint().GetLatitude().GetRadians()); // Lat in radians
      nIndex++;

      dSigmaRadius = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));

      nIndex++;

      SurfacePoint SurfacePoint = point->GetAdjustedSurfacePoint();

      SurfacePoint.SetSphericalSigmasDistance(
          Distance(dSigmaLat, Distance::Meters),
          Distance(dSigmaLong, Distance::Meters),
          Distance(dSigmaRadius, Distance::Kilometers));    

      point->SetAdjustedSurfacePoint(SurfacePoint);
    }
  }

  /**
   * output bundle results to file
   */
  bool BundleAdjust::Output() {

      if ( m_bOutputStandard ) {
          if( m_bConverged && m_bErrorPropagation )
              OutputWithErrorPropagation();
          else
              OutputNoErrorPropagation();
      }

      if ( m_bOutputCSV ) {
          OutputPointsCSV();
          OutputImagesCSV();
      }

      if ( m_bOutputResiduals )
          OutputResiduals();

    return true;
  }

  bool BundleAdjust::OutputHeader(std::ofstream& fp_out) {

      if (!fp_out)
          return false;

      char buf[1056];
      int nImages = Images();
      int nValidPoints = m_pCnet->GetNumValidPoints();
      int nInnerConstraints = 0;
      int nDistanceConstraints = 0;
      int nDegreesOfFreedom = m_nObservations + m_nConstrainedPointParameters + m_nConstrainedImageParameters - m_nUnknownParameters;
      int nConvergenceCriteria = 1;

      if (!m_bDeltack)
        sprintf(buf, "JIGSAW: BUNDLE ADJUSTMENT\n=========================\n");
      else
        sprintf(buf, "JIGSAW (DELTACK or QTIE): BUNDLE ADJUSTMENT\n=========================\n");

      fp_out << buf;
      sprintf(buf, "\n                       Run Time: %s", Isis::iTime::CurrentLocalTime().c_str());
      fp_out << buf;
      sprintf(buf,"\n               Network Filename: %s", m_strCnetFilename.c_str());
      fp_out << buf;
      sprintf(buf,"\n                     Network Id: %s", m_pCnet->GetNetworkId().c_str());
      fp_out << buf;
      sprintf(buf,"\n            Network Description: %s", m_pCnet->Description().c_str());
      fp_out << buf;
      sprintf(buf,"\n                         Target: %s", m_pCnet->GetTarget().c_str());
      fp_out << buf;
      sprintf(buf,"\n\n                   Linear Units: kilometers");
      fp_out << buf;
      sprintf(buf,"\n                  Angular Units: decimal degrees");
      fp_out << buf;
      sprintf(buf, "\n\nINPUT: SOLVE OPTIONS\n====================\n");
      fp_out << buf;
      m_bObservationMode ? sprintf(buf, "\n                   OBSERVATIONS: ON"):
              sprintf(buf, "\n                   OBSERVATIONS: OFF");
      fp_out << buf;
      m_bSolveRadii ? sprintf(buf, "\n                         RADIUS: ON"):
              sprintf(buf, "\n                         RADIUS: OFF");
      fp_out << buf;
      sprintf(buf,"\n                  SOLUTION TYPE: %s", m_strSolutionMethod.c_str());
      fp_out << buf;
      m_bErrorPropagation ? sprintf(buf, "\n              ERROR PROPAGATION: ON"):
              sprintf(buf, "\n              ERROR PROPAGATION: OFF");
      fp_out << buf;
      m_bOutlierRejection ? sprintf(buf, "\n              OUTLIER REJECTION: ON"):
              sprintf(buf, "\n              OUTLIER REJECTION: OFF");
      fp_out << buf;
      sprintf(buf, "\n\nINPUT: CONVERGENCE CRITERIA\n===========================\n");
      fp_out << buf;
      sprintf(buf,"\n                         SIGMA0: %e",m_dConvergenceThreshold);
      fp_out << buf;
      sprintf(buf,"\n             MAXIMUM ITERATIONS: %d",m_nMaxIterations);
      fp_out << buf;
      sprintf(buf, "\n\nINPUT: CAMERA POINTING OPTIONS\n==============================\n");
      fp_out << buf;
      switch (m_cmatrixSolveType) {
        case BundleAdjust::AnglesOnly:
          sprintf(buf,"\n                       CAMSOLVE: ANGLES");
          break;
        case BundleAdjust::AnglesVelocity:
          sprintf(buf,"\n                       CAMSOLVE: ANGLES, VELOCITIES");
          break;
        case BundleAdjust::AnglesVelocityAcceleration:
          sprintf(buf,"\n                       CAMSOLVE: ANGLES, VELOCITIES, ACCELERATIONS");
          break;
        case BundleAdjust::All:
          sprintf(buf,"\n                       CAMSOLVE: ALL POLYNOMIAL COEFFICIENTS (%d)",m_nsolveCamDegree);
          break;
      case BundleAdjust::None:
          sprintf(buf,"\n                       CAMSOLVE: NONE");
          break;
      default:
        break;
      }
      fp_out << buf;
      m_bSolveTwist ? sprintf(buf, "\n                          TWIST: ON"):
              sprintf(buf, "\n                          TWIST: OFF");
      fp_out << buf;
      sprintf(buf, "\n\nINPUT: SPACECRAFT OPTIONS\n=========================\n");
      fp_out << buf;
      switch (m_spacecraftPositionSolveType) {
        case Nothing:
          sprintf(buf,"\n                        SPSOLVE: NONE");
          break;
        case PositionOnly:
          sprintf(buf,"\n                        SPSOLVE: POSITION");
          break;
        case PositionVelocity:
          sprintf(buf,"\n                        SPSOLVE: POSITION, VELOCITIES");
          break;
        case PositionVelocityAcceleration:
          sprintf(buf,"\n                        SPSOLVE: POSITION, VELOCITIES, ACCELERATIONS");
          break;
        default:
          break;
      }
      fp_out << buf;
      sprintf(buf, "\n\nINPUT: GLOBAL IMAGE PARAMETER UNCERTAINTIES\n===========================================\n");
      fp_out << buf;
      (m_dGlobalLatitudeAprioriSigma == -1) ? sprintf(buf,"\n               POINT LATITUDE SIGMA: N/A"):
              sprintf(buf,"\n               POINT LATITUDE SIGMA: %lf (meters)",m_dGlobalLatitudeAprioriSigma);
      fp_out << buf;
      (m_dGlobalLongitudeAprioriSigma == -1) ? sprintf(buf,"\n              POINT LONGITUDE SIGMA: N/A"):
              sprintf(buf,"\n              POINT LONGITUDE SIGMA: %lf (meters)",m_dGlobalLongitudeAprioriSigma);
      fp_out << buf;
      (m_dGlobalRadiusAprioriSigma == -1) ? sprintf(buf,"\n                 POINT RADIUS SIGMA: N/A"):
              sprintf(buf,"\n                 POINT RADIUS SIGMA: %lf (meters)",m_dGlobalRadiusAprioriSigma);
      fp_out << buf;
      (m_dGlobalSpacecraftPositionAprioriSigma == -1) ? sprintf(buf,"\n          SPACECRAFT POSITION SIGMA: N/A"):
              sprintf(buf,"\n          SPACECRAFT POSITION SIGMA: %lf (meters)",m_dGlobalSpacecraftPositionAprioriSigma);
      fp_out << buf;
      (m_dGlobalSpacecraftVelocityAprioriSigma == -1) ? sprintf(buf,"\n          SPACECRAFT VELOCITY SIGMA: N/A"):
              sprintf(buf,"\n          SPACECRAFT VELOCITY SIGMA: %lf (m/s)",m_dGlobalSpacecraftVelocityAprioriSigma);
      fp_out << buf;
      (m_dGlobalSpacecraftAccelerationAprioriSigma == -1) ? sprintf(buf,"\n      SPACECRAFT ACCELERATION SIGMA: N/A"):
              sprintf(buf,"\n      SPACECRAFT ACCELERATION SIGMA: %lf (m/s/s)",m_dGlobalSpacecraftAccelerationAprioriSigma);
      fp_out << buf;

      if (m_nNumberCameraCoefSolved < 1 || m_dGlobalCameraAnglesAprioriSigma[0] == -1)
        sprintf(buf,"\n                CAMERA ANGLES SIGMA: N/A");
      else
        sprintf(buf,"\n                CAMERA ANGLES SIGMA: %lf (dd)",m_dGlobalCameraAnglesAprioriSigma[0]);
      fp_out << buf;

      if (m_nNumberCameraCoefSolved < 2 || m_dGlobalCameraAnglesAprioriSigma[1] == -1)
        sprintf(buf,"\n      CAMERA ANGULAR VELOCITY SIGMA: N/A");
      else
        sprintf(buf,"\n      CAMERA ANGULAR VELOCITY SIGMA: %lf (dd/s)",m_dGlobalCameraAnglesAprioriSigma[1]);
      fp_out << buf;

      if (m_nNumberCameraCoefSolved < 3 || m_dGlobalCameraAnglesAprioriSigma[2] == -1)
        sprintf(buf,"\n  CAMERA ANGULAR ACCELERATION SIGMA: N/A");
      else
        sprintf(buf,"\n  CAMERA ANGULAR ACCELERATION SIGMA: %lf (dd/s/s)",m_dGlobalCameraAnglesAprioriSigma[2]);
      fp_out << buf;

      sprintf(buf, "\n\nJIGSAW: RESULTS\n===============\n");
      fp_out << buf;
      sprintf(buf,"\n                         Images: %6d",nImages);
      fp_out << buf;
      sprintf(buf,"\n                         Points: %6d",nValidPoints);
      fp_out << buf;
      sprintf(buf,"\n                 Total Measures: %6d",(m_nObservations+m_nRejectedObservations)/2);
      fp_out << buf;
      sprintf(buf,"\n             Total Observations: %6d",m_nObservations+m_nRejectedObservations);
      fp_out << buf;
      sprintf(buf,"\n              Good Observations: %6d",m_nObservations);
      fp_out << buf;
      sprintf(buf,"\n          Rejected Observations: %6d",m_nRejectedObservations);
      fp_out << buf;

      if( m_nConstrainedPointParameters > 0 ) {
          sprintf(buf, "\n   Constrained Point Parameters: %6d",m_nConstrainedPointParameters);
          fp_out << buf;
      }
      if( m_nConstrainedImageParameters > 0 ){
          sprintf(buf, "\n   Constrained Image Parameters: %6d",m_nConstrainedImageParameters);
          fp_out << buf;
      }
      sprintf(buf,"\n                       Unknowns: %6d",m_nUnknownParameters);
      fp_out << buf;
      if( nInnerConstraints > 0) {
          sprintf(buf, "\n      Inner Constraints: %6d",nInnerConstraints);
          fp_out << buf;
      }
      if( nDistanceConstraints > 0) {
          sprintf(buf, "\n   Distance Constraints: %d",nDistanceConstraints);
          fp_out << buf;
      }

      sprintf(buf,"\n             Degrees of Freedom: %6d",nDegreesOfFreedom);
      fp_out << buf;
      sprintf(buf,"\n           Convergence Criteria: %6.3g",m_dConvergenceThreshold);
      fp_out << buf;

      if( nConvergenceCriteria == 1 ) {
          sprintf(buf, "(Sigma0)");
          fp_out << buf;
      }

      sprintf(buf, "\n                     Iterations: %6d",m_nIteration);
      fp_out << buf;

      if( m_nIteration >= m_nMaxIterations ) {
          sprintf(buf, "(Maximum reached)");
          fp_out << buf;
      }

    //      sprintf(buf, "\n                         Sigma0: %6.4lf\n",m_dSigma0);
      sprintf(buf, "\n                         Sigma0: %30.20lf\n",m_dSigma0);
      fp_out << buf;
      sprintf(buf, " Error Propagation Elapsed Time: %6.4lf (seconds)\n",m_dElapsedTimeErrorProp);
      fp_out << buf;
      sprintf(buf, "             Total Elapsed Time: %6.4lf (seconds)\n",m_dElapsedTime);
      fp_out << buf;

      sprintf(buf,"\nIMAGE MEASURES SUMMARY\n==========================\n\n");
      fp_out << buf;

      int nMeasures;
      int nRejectedMeasures;
      int nUsed;

      for (int i = 0; i < nImages; i++) {
        // ImageIndex(i) retrieves index into the normal equations matrix
        // for Image(i)
        double rmsSampleResiduals = m_rmsImageSampleResiduals[i].Rms();
        double rmsLineResiduals = m_rmsImageLineResiduals[i].Rms();
        double rmsLandSResiduals = m_rmsImageResiduals[i].Rms();

        nMeasures = 
            m_pCnet->GetNumberOfMeasuresInImage(m_pSnList->SerialNumber(i));
        nRejectedMeasures =
            m_pCnet->GetNumberOfJigsawRejectedMeasuresInImage(
                                              m_pSnList->SerialNumber(i));
        nUsed = nMeasures - nRejectedMeasures;

        if (nUsed == nMeasures)
          sprintf(buf,"%s   %5d of %5d %6.3lf %6.3lf %6.3lf\n",
                  m_pSnList->Filename(i).c_str(),
                  (nMeasures-nRejectedMeasures), nMeasures,
                  rmsSampleResiduals,rmsLineResiduals,rmsLandSResiduals);
        else
          sprintf(buf,"%s   %5d of %5d* %6.3lf %6.3lf %6.3lf\n",
                  m_pSnList->Filename(i).c_str(),
                  (nMeasures-nRejectedMeasures), nMeasures,
                  rmsSampleResiduals,rmsLineResiduals,rmsLandSResiduals);
        fp_out << buf;
      }

      return true;
  }

/**
 * output bundle results to file with error propagation
 */
    bool BundleAdjust::OutputWithErrorPropagation() {

      std:: string ofname("bundleout.txt");
      if( m_strOutputFilePrefix.length() != 0 )
          ofname = m_strOutputFilePrefix + "_" + ofname;

      std::ofstream fp_out(ofname.c_str(), std::ios::out);
      if (!fp_out)
          return false;

      char buf[1056];
      bool bHeld = false;
      std::vector<double> PosX(3);
      std::vector<double> PosY(3);
      std::vector<double> PosZ(3);
      std::vector<double> coefRA(m_nNumberCameraCoefSolved);
      std::vector<double> coefDEC(m_nNumberCameraCoefSolved);
      std::vector<double> coefTWI(m_nNumberCameraCoefSolved);
      std::vector<double> angles;
      Camera *pCamera = NULL;
      SpicePosition *pSpicePosition = NULL;
      SpiceRotation *pSpiceRotation = NULL;

      int nImages = Images();
      double dSigma=0;
      int nIndex = 0;
      bool bSolveSparse = false;

      gmm::row_matrix<gmm::rsvector<double> > lsqCovMatrix;

//      std::cout << m_Image_Corrections << std::endl;

      if( m_strSolutionMethod == "OLDSPARSE" )
      {
          lsqCovMatrix = m_pLsq->GetCovarianceMatrix();  // get reference to the covariance matrix from the least-squares object
          bSolveSparse = true;
      }

      OutputHeader(fp_out);

      sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION\n==========================\n");
      fp_out << buf;

      for ( int i = 0; i < nImages; i++ ) {

          if ( m_nHeldImages > 0 && m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)) )
              bHeld = true;

          pCamera = m_pCnet->Camera(i);
          if ( !pCamera )
              continue;

          // ImageIndex(i) retrieves index into the normal equations matrix for Image(i)
          nIndex = ImageIndex(i) ;

          pSpicePosition = pCamera->InstrumentPosition();
          if ( !pSpicePosition )
              continue;

          pSpiceRotation = pCamera->InstrumentRotation();
          if ( !pSpiceRotation )
              continue;

          // for frame cameras we directly retrieve the Exterior Orientation (i.e. position
          // and orientation angles). For others (linescan, radar) we retrieve the polynomial
          // coefficients from which the Exterior Orientation parameters are derived.
          if ( m_spacecraftPositionSolveType > 0 )
              pSpicePosition->GetPolynomial(PosX, PosY, PosZ);
          else { // not solving for position
              std::vector <double> coordinate(3);
              coordinate = pSpicePosition->GetCenterCoordinate();
              PosX[0] = coordinate[0];
              PosY[0] = coordinate[1];
              PosZ[0] = coordinate[2];
          }

          if ( m_cmatrixSolveType > 0 )
              pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
          //          else { // frame camera
          else { // This is for m_cmatrixSolveType = None and no polynomial fit has occurred
            angles = pSpiceRotation->GetCenterAngles();
            coefRA.push_back(angles.at(0));
            coefDEC.push_back(angles.at(1));
            coefTWI.push_back(angles.at(2));
          }

          sprintf(buf, "\nImage Full File Name: %s\n", m_pSnList->Filename(i).c_str());
          fp_out << buf;
          sprintf(buf, "\nImage Serial Number: %s\n", m_pSnList->SerialNumber(i).c_str());
          fp_out << buf;
          sprintf(buf, "\n    Image         Initial              Total               Final             Initial           Final\n"
                  "Parameter         Value              Correction            Value             Accuracy          Accuracy\n");
          fp_out << buf;

          if ( m_spacecraftPositionSolveType == 0 ) {
              sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", PosX[0], 0.0, PosX[0], 0.0, "N/A");
              fp_out << buf;
              sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", PosY[0], 0.0, PosY[0], 0.0, "N/A");
              fp_out << buf;
              sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", PosZ[0], 0.0, PosZ[0], 0.0, "N/A");
              fp_out << buf;
          }
          else if ( m_spacecraftPositionSolveType == 1 ) {
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;

              sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
          }
          else if (m_spacecraftPositionSolveType == 2) {
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Xv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosX[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosX[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Yv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosY[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosY[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Zv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosZ[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosZ[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
          }
          else if ( m_spacecraftPositionSolveType == 3 ) {
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Xv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosX[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosX[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Xa%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosX[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosX[2], m_dGlobalSpacecraftAccelerationAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        Y%17.8f%21.8f%20.8f%18.8lf%18.8lf\n",
                      PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Yv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosY[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosY[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Ya%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosY[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosY[2], m_dGlobalSpacecraftAccelerationAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "        Z%17.8f%21.8f%20.8f%18.8lf%18.8lf\n",
                      PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Zv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosZ[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosZ[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
              if( bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              sprintf(buf, "       Za%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                      PosZ[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex),
                      PosZ[2], m_dGlobalSpacecraftAccelerationAprioriSigma, dSigma);
              fp_out << buf;
              nIndex++;
          }

          if( m_nNumberCameraCoefSolved > 0 ) {
              char strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
              std::ostringstream ostr;
              for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {
                  if( i ==0 )
                      ostr << "  " << strcoeff;
                  else if ( i == 1 )
                      ostr << " " << strcoeff << "t";
                  else
                      ostr << strcoeff << "t" << i;
                  if( bSolveSparse )
                      dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
                  else
                      dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
                  if( i == 0 ) {
                      sprintf(buf, " RA (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                              ostr.str().c_str(),(coefRA[i] - m_Image_Corrections(nIndex)) * RAD2DEG,
                              m_Image_Corrections(nIndex) * RAD2DEG, coefRA[i] * RAD2DEG,
                              m_dGlobalCameraAnglesAprioriSigma[i], dSigma * RAD2DEG);
                  }
                  else {
                      sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                              ostr.str().c_str(),(coefRA[i] - m_Image_Corrections(nIndex)) * RAD2DEG,
                              m_Image_Corrections(nIndex) * RAD2DEG, coefRA[i] * RAD2DEG,
                              m_dGlobalCameraAnglesAprioriSigma[i], dSigma * RAD2DEG);
                  }
                  fp_out << buf;
                  ostr.str("");
                  strcoeff--;
                  nIndex++;
              }
              strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
              for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {
                  if( i ==0 )
                      ostr << "  " << strcoeff;
                  else if ( i == 1 )
                      ostr << " " << strcoeff << "t";
                  else
                      ostr << strcoeff << "t" << i;
                  if( bSolveSparse )
                      dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
                  else
                      dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
                  if( i == 0 ) {
                      sprintf(buf, "DEC (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                              ostr.str().c_str(),(coefDEC[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                              m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[i]*RAD2DEG,
                              m_dGlobalCameraAnglesAprioriSigma[i], dSigma * RAD2DEG);
                  }
                  else {
                      sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                              ostr.str().c_str(),(coefDEC[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                              m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[i]*RAD2DEG,
                              m_dGlobalCameraAnglesAprioriSigma[i], dSigma * RAD2DEG);
                  }
                  fp_out << buf;
                  ostr.str("");
                  strcoeff--;
                  nIndex++;
              }
              if ( !m_bSolveTwist ) {
                  sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                          coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
                  fp_out << buf;
              }
              else {
                  strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
                  for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {
                      if( i ==0 )
                          ostr << "  " << strcoeff;
                      else if ( i == 1 )
                          ostr << " " << strcoeff << "t";
                      else
                          ostr << strcoeff << "t" << i;
                      if( bSolveSparse )
                          dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
                      else
                          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
                      if( i == 0 ) {
                          sprintf(buf, "TWI (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                                  ostr.str().c_str(),(coefTWI[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                                  m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[i]*RAD2DEG,
                                  m_dGlobalCameraAnglesAprioriSigma[i], dSigma * RAD2DEG);
                      }
                      else {
                          sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                                  ostr.str().c_str(),(coefTWI[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                                  m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[i]*RAD2DEG,
                                  m_dGlobalCameraAnglesAprioriSigma[i], dSigma * RAD2DEG);
                      }
                      fp_out << buf;
                      ostr.str("");
                      strcoeff--;
                      nIndex++;
                  }
              }
          }

          else{
              sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                      coefRA[0]*RAD2DEG, 0.0, coefRA[0]*RAD2DEG, 0.0, "N/A");
              fp_out << buf;
              sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                      coefDEC[0]*RAD2DEG, 0.0, coefDEC[0]*RAD2DEG, 0.0, "N/A");
              fp_out << buf;
              sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                      coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
              fp_out << buf;
          }
      }

      // output point data
      sprintf(buf, "\n\n\nPOINTS UNCERTAINTY SUMMARY\n==========================\n\n");
      fp_out << buf;
      sprintf(buf, " RMS Sigma Latitude(m)%20.8lf\n",
              m_drms_sigmaLat);
      fp_out << buf;
      sprintf(buf, " MIN Sigma Latitude(m)%20.8lf at %s\n",
              m_dminSigmaLatitude,m_idMinSigmaLatitude.c_str());
      fp_out << buf;
      sprintf(buf, " MAX Sigma Latitude(m)%20.8lf at %s\n\n",
              m_dmaxSigmaLatitude,m_idMaxSigmaLatitude.c_str());
      fp_out << buf;
      sprintf(buf, "RMS Sigma Longitude(m)%20.8lf\n",
              m_drms_sigmaLon);
      fp_out << buf;
      sprintf(buf, "MIN Sigma Longitude(m)%20.8lf at %s\n",
              m_dminSigmaLongitude,m_idMinSigmaLongitude.c_str());
      fp_out << buf;
      sprintf(buf, "MAX Sigma Longitude(m)%20.8lf at %s\n\n",
              m_dmaxSigmaLongitude,m_idMaxSigmaLongitude.c_str());
      fp_out << buf;
      sprintf(buf, "   RMS Sigma Radius(m)%20.8lf\n",
              m_drms_sigmaRad);
      fp_out << buf;
      sprintf(buf, "   MIN Sigma Radius(m)%20.8lf at %s\n",
              m_dminSigmaRadius,m_idMinSigmaRadius.c_str());
      fp_out << buf;
      sprintf(buf, "   MAX Sigma Radius(m)%20.8lf at %s\n",
              m_dmaxSigmaRadius,m_idMaxSigmaRadius.c_str());
      fp_out << buf;

      // output point data
      sprintf(buf, "\n\nPOINTS SUMMARY\n==============\n%103sSigma          Sigma              Sigma\n"
              "           Label         Status     Rays    RMS        Latitude       Longitude          Radius"
              "        Latitude       Longitude          Radius\n", "");
      fp_out << buf;

      int nRays = 0;
      double dLat, dLon, dRadius;
      double dSigmaLat, dSigmaLong, dSigmaRadius;
      double cor_lat_dd, cor_lon_dd, cor_rad_m;
      double cor_lat_m, cor_lon_m;
      double dLatInit, dLonInit, dRadiusInit;
      int nGoodRays;
      double dResidualRms;
      std::string strStatus;
      int nPointIndex = 0;

      int nPoints = m_pCnet->GetNumPoints();
      for ( int i = 0; i < nPoints; i++ ) {

          const ControlPoint *point = m_pCnet->GetPoint(i);
          if ( point->IsIgnored() )
              continue;

          nRays = point->GetNumMeasures();
          dResidualRms = point->GetResidualRms();         
          dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
          dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
          dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();
          dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().GetMeters();
          dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().GetMeters();
          dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().GetMeters();
          nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

          if (point->GetType() == ControlPoint::Fixed)
              strStatus = "FIXED";
          else if (point->GetType() == ControlPoint::Constrained) 
              strStatus = "CONSTRAINED";
          else if (point->GetType() == ControlPoint::Free)
              strStatus = "FREE";
          else
              strStatus = "UNKNOWN";

          sprintf(buf, "%16s%15s%5d of %d%6.2lf%16.8lf%16.8lf%16.8lf%16.8lf%16.8lf%16.8lf\n",
                  point->GetId().c_str(), strStatus.c_str(), nGoodRays, nRays, dResidualRms, dLat,
                  dLon, dRadius * 0.001, dSigmaLat, dSigmaLong, dSigmaRadius);
          fp_out << buf;
          nPointIndex++;
      }

      // output point data
      sprintf(buf, "\n\nPOINTS DETAIL\n=============\n\n");
      fp_out << buf;

      nPointIndex = 0;
      for ( int i = 0; i < nPoints; i++ ) {

          const ControlPoint *point = m_pCnet->GetPoint(i);
          if ( point->IsIgnored() )
              continue;

          nRays = point->GetNumMeasures();
          dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
          dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
          dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();
          dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().GetMeters();
          dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().GetMeters();
          dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().GetMeters();
          nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

          // point corrections and initial sigmas
          bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];
          bounded_vector<double, 3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

          cor_lat_dd = corrections[0] * Isis::RAD2DEG;
          cor_lon_dd = corrections[1] * Isis::RAD2DEG;
          cor_rad_m  = corrections[2] * 1000.0;

          cor_lat_m = corrections[0] * m_dRTM;
          cor_lon_m = corrections[1] * m_dRTM * cos(dLat*Isis::DEG2RAD);

          dLatInit = dLat - cor_lat_dd;
          dLonInit = dLon - cor_lon_dd;
          dRadiusInit = dRadius - (corrections[2] * 1000.0);

          if (point->GetType() == ControlPoint::Fixed)
              strStatus = "FIXED";
          else if (point->GetType() == ControlPoint::Constrained)
              strStatus = "CONSTRAINED";
          else if (point->GetType() == ControlPoint::Free)
              strStatus = "FREE";
          else
              strStatus = "UNKNOWN";

          sprintf(buf, " Label: %s\nStatus: %s\n  Rays: %d of %d\n",
                  point->GetId().c_str(), strStatus.c_str(), nGoodRays, nRays);
          fp_out << buf;

          sprintf(buf, "\n     Point         Initial               Total               Total              Final             Initial             Final\n"
                  "Coordinate          Value             Correction          Correction            Value             Accuracy          Accuracy\n"
                  "                 (dd/dd/km)           (dd/dd/km)           (Meters)           (dd/dd/km)          (Meters)          (Meters)\n");
          fp_out << buf;

          sprintf(buf, "  LATITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18.8lf\n",
                  dLatInit, cor_lat_dd, cor_lat_m, dLat, apriorisigmas[0], dSigmaLat);
          fp_out << buf;

          sprintf(buf, " LONGITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18.8lf\n",
                  dLonInit, cor_lon_dd, cor_lon_m, dLon, apriorisigmas[1], dSigmaLong);
          fp_out << buf;

          sprintf(buf, "    RADIUS%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18.8lf\n\n",
                  dRadiusInit * 0.001, corrections[2], cor_rad_m, dRadius * 0.001,
                  apriorisigmas[2], dSigmaRadius);

          fp_out << buf;
          nPointIndex++;
      }

      fp_out.close();

      return true;
  }

   /**
   * output bundle results to file with no error propagation
   *
   * @internal
   * @history 2011-05-22 Debbie A. Cook - Added commas to make csv header lines
   *                      consistent for comparer   
   * @history 2011-06-05 Debbie A. Cook - Fixed output of spacecraft position
   *                      when it is not part of the bundle
   * @history 2011-07-26 Debbie A. Cook - Omitted output of camera angles for 
   *                      radar, which only has spacecraft position
   */
  bool BundleAdjust::OutputNoErrorPropagation() {

      std:: string ofname("bundleout.txt");
      if( !m_strOutputFilePrefix.empty() )
          ofname = m_strOutputFilePrefix + "_" + ofname;

      std::ofstream fp_out(ofname.c_str(), std::ios::out);
      if (!fp_out)
          return false;

      char buf[1056];

    bool bHeld = false;
    std::vector<double> PosX(3);
    std::vector<double> PosY(3);
    std::vector<double> PosZ(3);
    std::vector<double> coefRA(m_nNumberCameraCoefSolved);
    std::vector<double> coefDEC(m_nNumberCameraCoefSolved);
    std::vector<double> coefTWI(m_nNumberCameraCoefSolved);
    std::vector<double> angles;
    Camera *pCamera = NULL;
    SpicePosition *pSpicePosition = NULL;
    SpiceRotation *pSpiceRotation = NULL;
    int nIndex = 0;
    int nImages = Images();

    OutputHeader(fp_out);

    sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION ***J2000***\n======================================\n");
    fp_out << buf;

    for (int i = 0; i < nImages; i++) {
      if (m_nHeldImages > 0 && m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))
        bHeld = true;

      pCamera = m_pCnet->Camera(i);
      if (!pCamera)
        continue;

      nIndex = ImageIndex(i);

      pSpicePosition = pCamera->InstrumentPosition();
      if (!pSpicePosition)
        continue;

      pSpiceRotation = pCamera->InstrumentRotation();
      if (!pSpiceRotation)
        continue;

      // for frame cameras we directly retrieve the Exterior Orientation (i.e. position
      // and orientation angles). For others (linescan, radar) we retrieve the polynomial
      // coefficients from which the Exterior Orientation paramters are derived.
      // This is incorrect...Correction below
      // For all instruments we retrieve the polynomial coefficients from which the
      // Exterior Orientation parameters are derived.  For framing cameras, a single
      // coefficient for each coordinate is returned.
      if ( m_spacecraftPositionSolveType > 0 )
          pSpicePosition->GetPolynomial(PosX, PosY, PosZ);
      //      else { // frame camera
      else { // This is for m_spacecraftPositionSolveType = None and no polynomial fit has occurred
          std::vector <double> coordinate(3);
          coordinate = pSpicePosition->GetCenterCoordinate();
          PosX[0] = coordinate[0];
          PosY[0] = coordinate[1];
          PosZ[0] = coordinate[2];
      }

      if ( m_cmatrixSolveType > 0 ) {
//          angles = pSpiceRotation->Angles(3,1,3);
          pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
      }
      else { // frame camera
        angles = pSpiceRotation->GetCenterAngles();
        coefRA.push_back(angles.at(0));
        coefDEC.push_back(angles.at(1));
        coefTWI.push_back(angles.at(2));
      }

      sprintf(buf, "\nImage Full File Name: %s\n", m_pSnList->Filename(i).c_str());
      fp_out << buf;
      sprintf(buf, "\n Image Serial Number: %s\n", m_pSnList->SerialNumber(i).c_str());
      fp_out << buf;
      sprintf(buf, "\n    Image         Initial              Total               Final             Initial           Final\n"
              "Parameter         Value              Correction            Value             Accuracy          Accuracy\n");
      fp_out << buf;

      if (m_spacecraftPositionSolveType == 0) {
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18s%18s\n", PosX[0], 0.0, PosX[0], "N/A", "N/A");
        fp_out << buf;
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18s%18s\n", PosY[0], 0.0, PosY[0], "N/A", "N/A");
        fp_out << buf;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18s%18s\n", PosZ[0], 0.0, PosZ[0], "N/A", "N/A");
        fp_out << buf;
      }
      else if (m_spacecraftPositionSolveType == 1) {
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
      }
      else if (m_spacecraftPositionSolveType == 2) {
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Xv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosX[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[1], m_dGlobalSpacecraftVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Yv%17.8lf%21.8lf%20.8lf%18.8lf%18.s\n",
                PosY[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[1], m_dGlobalSpacecraftVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Zv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosZ[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[1], m_dGlobalSpacecraftVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
      }
      else if (m_spacecraftPositionSolveType == 3) {
        sprintf(buf, "       X0%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       X1%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosX[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[1], m_dGlobalSpacecraftVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       X2%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosX[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[2], m_dGlobalSpacecraftAccelerationAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Y0%17.8f%21.8f%20.8f%18.8lf%18s\n",
                PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Y1%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosY[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[1], m_dGlobalSpacecraftVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Y2%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosY[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[2], m_dGlobalSpacecraftAccelerationAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Z0%17.8f%21.8f%20.8f%18.8lf%18s\n",
                PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Z1%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosZ[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[1], m_dGlobalSpacecraftVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "       Z2%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosZ[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[2], m_dGlobalSpacecraftAccelerationAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
      }

      if( m_nNumberCameraCoefSolved > 0 ) {
          char strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
          std::ostringstream ostr;
          for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {
              if( i ==0 )
                  ostr << "  " << strcoeff;
              else if ( i == 1 )
                  ostr << " " << strcoeff << "t";
              else
                  ostr << strcoeff << "t" << i;
              if( i == 0 ) {                 
                  sprintf(buf, " RA (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                          ostr.str().c_str(),(coefRA[i] - m_Image_Corrections(nIndex)) * RAD2DEG,
                          m_Image_Corrections(nIndex) * RAD2DEG, coefRA[i] * RAD2DEG,
                          m_dGlobalCameraAnglesAprioriSigma[i], "N/A");
              }
              else {
                  sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                          ostr.str().c_str(),(coefRA[i] - m_Image_Corrections(nIndex)) * RAD2DEG,
                          m_Image_Corrections(nIndex) * RAD2DEG, coefRA[i] * RAD2DEG,
                          m_dGlobalCameraAnglesAprioriSigma[i], "N/A");
              }
              fp_out << buf;
              ostr.str("");
              strcoeff--;
              nIndex++;
          }
          strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
          for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {
              if( i ==0 )
                  ostr << "  " << strcoeff;
              else if ( i == 1 )
                  ostr << " " << strcoeff << "t";
              else
                  ostr << strcoeff << "t" << i;
              if( i == 0 ) {
                  sprintf(buf, "DEC (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                          ostr.str().c_str(),(coefDEC[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                          m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[i]*RAD2DEG,
                          m_dGlobalCameraAnglesAprioriSigma[i], "N/A");
              }
              else {
                  sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                          ostr.str().c_str(),(coefDEC[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                          m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[i]*RAD2DEG,
                          m_dGlobalCameraAnglesAprioriSigma[i], "N/A");
              }
              fp_out << buf;
              ostr.str("");
              strcoeff--;
              nIndex++;
          }
          if ( !m_bSolveTwist ) {
              sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                      coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
              fp_out << buf;
          }
          else {
              strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
              for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {
                  if( i ==0 )
                      ostr << "  " << strcoeff;
                  else if ( i == 1 )
                      ostr << " " << strcoeff << "t";
                  else
                      ostr << strcoeff << "t" << i;
                  if( i == 0 ) {
                      sprintf(buf, "TWI (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                              ostr.str().c_str(),(coefTWI[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                              m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[i]*RAD2DEG,
                              m_dGlobalCameraAnglesAprioriSigma[i], "N/A");
                  }
                  else {
                      sprintf(buf, "    (%s)%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                              ostr.str().c_str(),(coefTWI[i] - m_Image_Corrections(nIndex))*RAD2DEG,
                              m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[i]*RAD2DEG,
                              m_dGlobalCameraAnglesAprioriSigma[i], "N/A");
                  }
                  fp_out << buf;
                  ostr.str("");
                  strcoeff--;
                  nIndex++;
              }
          }
      }
      else{
          sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  coefRA[0]*RAD2DEG, 0.0, coefRA[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
          sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  coefDEC[0]*RAD2DEG, 0.0, coefDEC[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
      }
  }

    fp_out << "\n\n\n";

    // output point data
    sprintf(buf,"\nPOINTS SUMMARY\n==============\n%99sSigma           Sigma           Sigma\n"
                "           Label      Status     Rays   RMS        Latitude       Longitude          Radius"
                "        Latitude        Longitude       Radius\n","");
    fp_out << buf;

    int nRays = 0;
    double dResidualRms;
    double dLat,dLon,dRadius;
    double cor_lat_dd,cor_lon_dd,cor_rad_m;
    double cor_lat_m,cor_lon_m;
    double dLatInit,dLonInit,dRadiusInit;
    int nGoodRays;

    std::string strStatus;

    int nPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nPoints; i++) {

        const ControlPoint* point = m_pCnet->GetPoint(i);
        if( point->IsIgnored() )
            continue;

        nRays = point->GetNumMeasures();
        dResidualRms = point->GetResidualRms();
        dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
        dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
        dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();
        nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

        if( point->GetType() == ControlPoint::Fixed)
            strStatus = "FIXED";
        else if( point->GetType() == ControlPoint::Constrained)
            strStatus = "CONSTRAINED";
        else if( point->GetType() == ControlPoint::Free)
            strStatus = "FREE";
        else
            strStatus = "UNKNOWN";

        sprintf(buf, "%16s%12s%4d of %d%6.2lf%16.8lf%16.8lf%16.8lf%11s%16s%16s\n",
                point->GetId().c_str(), strStatus.c_str(), nGoodRays, nRays, dResidualRms, dLat,
                dLon, dRadius * 0.001,"N/A","N/A","N/A");

        fp_out << buf;
    }

    sprintf(buf,"\n\nPOINTS DETAIL\n=============\n\n");
    fp_out << buf;

    int nPointIndex = 0;
    for (int i = 0; i < nPoints; i++) {

        const ControlPoint* point = m_pCnet->GetPoint(i);
        if( point->IsIgnored() )
            continue;

        nRays = point->GetNumMeasures();
        dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
        dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
        dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().GetMeters();
        nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

        // point corrections and initial sigmas
        bounded_vector<double,3>& corrections = m_Point_Corrections[nPointIndex];
        bounded_vector<double,3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

        cor_lat_dd = corrections[0]*Isis::RAD2DEG;
        cor_lon_dd = corrections[1]*Isis::RAD2DEG;
        cor_rad_m  = corrections[2]*1000.0;

        cor_lat_m = corrections[0]*m_dRTM;
        cor_lon_m = corrections[1]*m_dRTM*cos(dLat*Isis::DEG2RAD);

        dLatInit = dLat-cor_lat_dd;
        dLonInit = dLon-cor_lon_dd;
        dRadiusInit = dRadius-(corrections[2]*1000.0);

        if( point->GetType() == ControlPoint::Fixed)
            strStatus = "FIXED";
        else if( point->GetType() == ControlPoint::Constrained)
            strStatus = "CONSTRAINED";
        else if( point->GetType() == ControlPoint::Free)
            strStatus = "FREE";
        else
            strStatus = "UNKNOWN";

        sprintf(buf," Label: %s\nStatus: %s\n  Rays: %d of %d\n",
                point->GetId().c_str(),strStatus.c_str(),nGoodRays,nRays);
        fp_out << buf;

        sprintf(buf,"\n     Point         Initial               Total               Total              Final             Initial             Final\n"
                "Coordinate          Value             Correction          Correction            Value             Accuracy          Accuracy\n"
                "                 (dd/dd/km)           (dd/dd/km)           (Meters)           (dd/dd/km)          (Meters)          (Meters)\n");
        fp_out << buf;

        sprintf(buf,"  LATITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18s\n",
                dLatInit, cor_lat_dd, cor_lat_m, dLat, apriorisigmas[0], "N/A");
        fp_out << buf;

        sprintf(buf," LONGITUDE%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18s\n",
                dLonInit, cor_lon_dd, cor_lon_m, dLon, apriorisigmas[1], "N/A");
        fp_out << buf;

        sprintf(buf,"    RADIUS%17.8lf%21.8lf%20.8lf%20.8lf%18.8lf%18s\n\n",
                dRadiusInit*0.001, corrections[2], cor_rad_m, dRadius*0.001, apriorisigmas[2], "N/A");

        fp_out << buf;

        nPointIndex++;
    }

    fp_out.close();

    return true;
  }

  /**
   * output point data to csv file
   */
  bool BundleAdjust::OutputPointsCSV() {
    char buf[1056];

    std:: string ofname("bundleout_points.csv");
    if( !m_strOutputFilePrefix.empty() )
        ofname = m_strOutputFilePrefix + "_" + ofname;

    std::ofstream fp_out(ofname.c_str(), std::ios::out);
    if (!fp_out)
        return false;

    int nPoints = m_pCnet->GetNumPoints();

    double dLat, dLon, dRadius;
    double dX, dY, dZ;
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    std::string strStatus;
    double cor_lat_m;
    double cor_lon_m;
    double cor_rad_m;
    int nMeasures, nRejectedMeasures;
    double dResidualRms;

    // print column headers
    if (m_bErrorPropagation) {
      sprintf(buf, "Point,Point,Accepted,Rejected,Residual,3-d,3-d,3-d,Sigma,"
              "Sigma,Sigma,Correction,Correction,Correction,Coordinate,"
              "Coordinate,Coordinate\nID,,,,,Latitude,Longitude,Radius,"
              "Latitude,Longitude,Radius,Latitude,Longitude,Radius,X,Y,Z\n"
              "Label,Status,Measures,Measures,RMS,(dd),(dd),(km),(m),(m),(m),"
              "(m),(m),(m),(km),(km),(km)\n");
    }
    else {
      sprintf(buf, "Point,Point,Accepted,Rejected,Residual,3-d,3-d,3-d,"
              "Correction,Correction,Correction,Coordinate,Coordinate,"
              "Coordinate\n,,,,,Latitude,Longitude,Radius,Latitude,"
              "Longitude,Radius,X,Y,Z\nLabel,Status,Measures,Measures,"
              "RMS,(dd),(dd),(km),(m),(m),(m),(km),(km),(km)\n");
    }
    fp_out << buf;

    int nPointIndex = 0;
    for (int i = 0; i < nPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if ( !point )
          continue;

      if ( point->IsIgnored() || point->IsRejected() )
        continue;

      dLat = point->GetAdjustedSurfacePoint().GetLatitude().GetDegrees();
      dLon = point->GetAdjustedSurfacePoint().GetLongitude().GetDegrees();
      dRadius = point->GetAdjustedSurfacePoint().GetLocalRadius().GetKilometers();
      dX = point->GetAdjustedSurfacePoint().GetX().GetKilometers();
      dY = point->GetAdjustedSurfacePoint().GetY().GetKilometers();
      dZ = point->GetAdjustedSurfacePoint().GetZ().GetKilometers();
      nMeasures = point->GetNumMeasures();
      nRejectedMeasures = point->GetNumberOfRejectedMeasures();
      dResidualRms = point->GetResidualRms();

      // point corrections and initial sigmas
      bounded_vector<double,3>& corrections = m_Point_Corrections[nPointIndex];
      cor_lat_m = corrections[0]*m_dRTM;
      cor_lon_m = corrections[1]*m_dRTM*cos(dLat*Isis::DEG2RAD);
      cor_rad_m  = corrections[2]*1000.0;

      if (point->GetType() == ControlPoint::Fixed)
        strStatus = "FIXED";
      else if (point->GetType() == ControlPoint::Constrained)
        strStatus = "CONSTRAINED";
      else if (point->GetType() == ControlPoint::Free)
        strStatus = "FREE";
      else
        strStatus = "UNKNOWN";

      if (m_bErrorPropagation) {
        dSigmaLat = point->GetAdjustedSurfacePoint().GetLatSigmaDistance().GetMeters();
        dSigmaLong = point->GetAdjustedSurfacePoint().GetLonSigmaDistance().GetMeters();
        dSigmaRadius = point->GetAdjustedSurfacePoint().GetLocalRadiusSigma().GetMeters();

        sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                point->GetId().c_str(), strStatus.c_str(), nMeasures, nRejectedMeasures, dResidualRms, dLat, dLon, dRadius,
                dSigmaLat, dSigmaLong, dSigmaRadius, cor_lat_m, cor_lon_m, cor_rad_m, dX, dY, dZ);
      }
      else
        sprintf(buf, "%s,%s,%d,%d,%6.2lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                point->GetId().c_str(), strStatus.c_str(), nMeasures, nRejectedMeasures, dResidualRms, dLat, dLon, dRadius,
                cor_lat_m, cor_lon_m, cor_rad_m, dX, dY, dZ);

      fp_out << buf;

      nPointIndex++;
    }

    fp_out.close();

    return true;
  }

  /**
   * output image coordinate residuals to comma-separated-value
   * file
   */
  bool BundleAdjust::OutputResiduals() {
    char buf[1056];

    std:: string ofname("residuals.csv");
    if( !m_strOutputFilePrefix.empty() )
        ofname = m_strOutputFilePrefix + "_" + ofname;

    std::ofstream fp_out(ofname.c_str(), std::ios::out);
    if (!fp_out)
        return false;

    // output column headers
    sprintf(buf, ",,,x image,y image,Measured,Measured,sample,line,Residual Vector\n");
    fp_out << buf;
    sprintf(buf, "Point,Image,Image,coordinate,coordinate,Sample,Line,residual,residual,Magnitude\n");
    fp_out << buf;
    sprintf(buf, "Label,Filename,Serial Number,(mm),(mm),(pixels),(pixels),(pixels),(pixels),(pixels),Rejected\n");
    fp_out << buf;

    int nImageIndex;

    //    printf("output residuals!!!\n");

    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      int nObservations = point->GetNumMeasures();
      for (int j = 0; j < nObservations; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored())
          continue;

        Camera *pCamera = measure->Camera();
        if (!pCamera)
          continue;

        // Determine the image index
        nImageIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());

        if (measure->IsRejected())
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,*\n",
                  point->GetId().c_str(), m_pSnList->Filename(nImageIndex).c_str(), m_pSnList->SerialNumber(nImageIndex).c_str(),
                  measure->GetFocalPlaneMeasuredX(), measure->GetFocalPlaneMeasuredY(), measure->GetSample(),
                  measure->GetLine(), measure->GetSampleResidual(), measure->GetLineResidual(),
                  measure->GetResidualMagnitude());
        else
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                  point->GetId().c_str(), m_pSnList->Filename(nImageIndex).c_str(), m_pSnList->SerialNumber(nImageIndex).c_str(),
                  measure->GetFocalPlaneMeasuredX(), measure->GetFocalPlaneMeasuredY(), measure->GetSample(),
                  measure->GetLine(), measure->GetSampleResidual(), measure->GetLineResidual(), measure->GetResidualMagnitude());
        fp_out << buf;
      }
    }

    fp_out.close();

    return true;
  }

  /**
   * output image data to csv file
   */
  bool BundleAdjust::OutputImagesCSV() {
      char buf[1056];

      std:: string ofname("bundleout_images.csv");
      if( !m_strOutputFilePrefix.empty() )
          ofname = m_strOutputFilePrefix + "_" + ofname;

      std::ofstream fp_out(ofname.c_str(), std::ios::out);
      if (!fp_out)
          return false;

      // setup column headers
      std::vector<std::string> output_columns;

      output_columns.push_back("Image,");

      output_columns.push_back("rms,");
      output_columns.push_back("rms,");
      output_columns.push_back("rms,");

      if ( m_spacecraftPositionSolveType <= 1 ) {
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("X,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Y,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Z,");
      }
      else if ( m_spacecraftPositionSolveType == 2 ) {
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("X,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Xv,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Y,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Yv,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Z,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Zv,");
      }
      else if ( m_spacecraftPositionSolveType == 3 ) {
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("X,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Xv,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Xa,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Y,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Yv,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Ya,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Z,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Zv,");
          for(int i = 0; i < 5; i++ )
              output_columns.push_back("Za,");
      }

      char strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
      std::ostringstream ostr;
      for( int i = 0; i < m_nNumberCameraCoefSolved; i++) {
          if( i ==0 )
              ostr << strcoeff;
          else if ( i == 1 )
              ostr << strcoeff << "t";
          else
              ostr << strcoeff << "t" << i;
          for( int j = 0; j < 5; j++ ) {
              if( m_nNumberCameraCoefSolved == 1 )
                  output_columns.push_back("RA,");
              else {
                  std::string str = "RA(";
                  str += ostr.str().c_str();
                  str += "),";
                  output_columns.push_back(str);
              }
          }
          ostr.str("");
          strcoeff--;
      }
      strcoeff = 'a' + m_nNumberCameraCoefSolved -1;
      for( int i = 0; i < m_nNumberCameraCoefSolved; i++) {
          if( i ==0 )
              ostr << strcoeff;
          else if ( i == 1 )
              ostr << strcoeff << "t";
          else
              ostr << strcoeff << "t" << i;
          for( int j = 0; j < 5; j++ ) {
              if( m_nNumberCameraCoefSolved == 1 )
                  output_columns.push_back("DEC,");
              else {
                  std::string str = "DEC(";
                  str += ostr.str().c_str();
                  str += "),";
                  output_columns.push_back(str);
              }
          }
          ostr.str("");
          strcoeff--;
      }
      strcoeff = 'a' + m_nNumberCameraCoefSolved -1;

      for ( int i = 0; i < m_nNumberCameraCoefSolved; i++) {

        if ( i ==0 )
          ostr << strcoeff;
        else if ( i == 1 )
          ostr << strcoeff << "t";
        else
          ostr << strcoeff << "t" << i;

        for ( int j = 0; j < 5; j++ ) {

          if ( m_nNumberCameraCoefSolved == 1 || !m_bSolveTwist) {
            output_columns.push_back("TWIST,");
          }
          else {
            std::string str = "TWIST(";
            str += ostr.str().c_str();
            str += "),";
            output_columns.push_back(str);
          }
        }

        ostr.str("");
        strcoeff--;
        if (!m_bSolveTwist) break;
      }

      // print first column header to buffer and output to file
      int ncolumns = output_columns.size();
      for ( int i = 0; i < ncolumns; i++) {
          std::string str = output_columns.at(i);
          sprintf(buf, "%s", (const char*)str.c_str());
          fp_out << buf;
      }
      sprintf(buf, "\n");
      fp_out << buf;

      output_columns.clear();
      output_columns.push_back("Filename,");

      output_columns.push_back("sample res,");
      output_columns.push_back("line res,");
      output_columns.push_back("total res,");

      int nparams = 0;
      if ( m_spacecraftPositionSolveType <= 1 )
          nparams = 3;
      else if ( m_spacecraftPositionSolveType == 2 )
          nparams = 6;
      else if ( m_spacecraftPositionSolveType == 3 )
          nparams = 9;
      int numCameraAnglesSolved = 2;
      if (m_bSolveTwist) numCameraAnglesSolved++;
      nparams += numCameraAnglesSolved*m_nNumberCameraCoefSolved;
      if (!m_bSolveTwist) nparams += 1; // Report on twist only
      for(int i = 0; i < nparams; i++ ) {
          output_columns.push_back("Initial,");
          output_columns.push_back("Correction,");
          output_columns.push_back("Final,");
          output_columns.push_back("Apriori Sigma,");
          output_columns.push_back("Adj Sigma,");
      }

      // print second column header to buffer and output to file
      ncolumns = output_columns.size();
      for( int i = 0; i < ncolumns; i++) {
          std::string str = output_columns.at(i);
          sprintf(buf, "%s", (const char*)str.c_str());
          fp_out << buf;
      }
      sprintf(buf, "\n");
      fp_out << buf;

      Camera *pCamera = NULL;
      SpicePosition *pSpicePosition = NULL;
      SpiceRotation *pSpiceRotation = NULL;

      int nImages = Images();
      double dSigma = 0.;
      int nIndex = 0;
      bool bSolveSparse = false;
      bool bHeld = false;
      std::vector<double> PosX(3);
      std::vector<double> PosY(3);
      std::vector<double> PosZ(3);
      std::vector<double> coefRA(m_nNumberCameraCoefSolved);
      std::vector<double> coefDEC(m_nNumberCameraCoefSolved);
      std::vector<double> coefTWI(m_nNumberCameraCoefSolved);
      std::vector<double> angles;

      output_columns.clear();

      gmm::row_matrix<gmm::rsvector<double> > lsqCovMatrix;
      if (m_strSolutionMethod == "OLDSPARSE" && m_bErrorPropagation) {
//      Get reference to the covariance matrix from the least-squares object
        lsqCovMatrix = m_pLsq->GetCovarianceMatrix();
        bSolveSparse = true;
      }

      std::vector<double> BFP(3);

      for ( int i = 0; i < nImages; i++ ) {

        if (m_nHeldImages > 0 &&
            m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)) )
          bHeld = true;

        pCamera = m_pCnet->Camera(i);
        if ( !pCamera )
          continue;

        // ImageIndex(i) retrieves index into the normal equations matrix for
        //  Image(i)
        nIndex = ImageIndex(i) ;

        pSpicePosition = pCamera->InstrumentPosition();
        if ( !pSpicePosition )
          continue;

        pSpiceRotation = pCamera->InstrumentRotation();
        if ( !pSpiceRotation )
            continue;

        // for frame cameras we directly retrieve the J2000 Exterior Orientation
        // (i.e. position and orientation angles). For others (linescan, radar)
        //  we retrieve the polynomial coefficients from which the Exterior
        // Orientation parameters are derived.
        if ( m_spacecraftPositionSolveType > 0 )
          pSpicePosition->GetPolynomial(PosX, PosY, PosZ);
        else { // not solving for position so report state at center of image
          std::vector <double> coordinate(3);
          coordinate = pSpicePosition->GetCenterCoordinate();

          PosX[0] = coordinate[0];
          PosY[0] = coordinate[1];
          PosZ[0] = coordinate[2];
        }

        if ( m_cmatrixSolveType > 0 )
          pSpiceRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
//          else { // frame camera
        else if (pCamera->GetCameraType() != 3) {
// This is for m_cmatrixSolveType = None (except Radar which has no pointing)
// and no polynomial fit has occurred
          angles = pSpiceRotation->GetCenterAngles();
          coefRA.push_back(angles.at(0));
          coefDEC.push_back(angles.at(1));
          coefTWI.push_back(angles.at(2));
        }

        // clear column vector
        output_columns.clear();

        // add filename
        output_columns.push_back(m_pSnList->Filename(i).c_str());

        // add rms of sample, line, total image coordinate residuals
        output_columns.push_back(boost::lexical_cast<std::string>
            (m_rmsImageSampleResiduals[i].Rms()));
        output_columns.push_back(boost::lexical_cast<std::string>
            (m_rmsImageLineResiduals[i].Rms()));
        output_columns.push_back(boost::lexical_cast<std::string>
            (m_rmsImageResiduals[i].Rms()));

        // add all XYZ(J2000) spacecraft position parameters to column vector
        if ( m_spacecraftPositionSolveType == 0 ) {
            output_columns.push_back(boost::lexical_cast<std::string>(PosX[0]));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back(boost::lexical_cast<std::string>(PosX[0]));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back("N/A");
            output_columns.push_back(boost::lexical_cast<std::string>(PosY[0]));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back(boost::lexical_cast<std::string>(PosY[0]));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back("N/A");
            output_columns.push_back(boost::lexical_cast<std::string>(PosZ[0]));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back(boost::lexical_cast<std::string>(PosZ[0]));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back("N/A");
        }
        else if ( m_spacecraftPositionSolveType == 1 ) {
          if ( m_bErrorPropagation && m_bConverged ) {
            if ( bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");

          nIndex++;
          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosY[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosY[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");

          nIndex++;
          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosZ[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosZ[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;
        }
        else if (m_spacecraftPositionSolveType == 2) {

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");

          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[1] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[1]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftVelocityAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
            nIndex++;

            if ( m_bErrorPropagation && m_bConverged ) {
              if (bSolveSparse )
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
            }
            output_columns.push_back(boost::lexical_cast<std::string>
                (PosY[0] - m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (PosY[0]));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_dGlobalSpacecraftPositionAprioriSigma));

            if ( m_bErrorPropagation && m_bConverged )
              output_columns.push_back(boost::lexical_cast<std::string>
                  (dSigma));
            else
              output_columns.push_back("N/A");

            nIndex++;

            if ( m_bErrorPropagation && m_bConverged ) {
              if (bSolveSparse )
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
            }

            output_columns.push_back(boost::lexical_cast<std::string>
                (PosY[1] - m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (PosY[1]));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_dGlobalSpacecraftVelocityAprioriSigma));

            if ( m_bErrorPropagation && m_bConverged )
              output_columns.push_back(boost::lexical_cast<std::string>
                  (dSigma));
            else
              output_columns.push_back("N/A");

            nIndex++;

            if ( m_bErrorPropagation && m_bConverged ) {
              if (bSolveSparse )
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
            }
            output_columns.push_back(boost::lexical_cast<std::string>
                (PosZ[0] - m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (PosZ[0]));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_dGlobalSpacecraftPositionAprioriSigma));

            if ( m_bErrorPropagation && m_bConverged )
              output_columns.push_back(boost::lexical_cast<std::string>
                  (dSigma));
            else
              output_columns.push_back("N/A");
            nIndex++;

            if ( m_bErrorPropagation && m_bConverged ) {
              if (bSolveSparse )
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
            }

            output_columns.push_back(boost::lexical_cast<std::string>
                (PosZ[1] - m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_Image_Corrections(nIndex)));
            output_columns.push_back(boost::lexical_cast<std::string>
                (PosZ[1]));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_dGlobalSpacecraftVelocityAprioriSigma));

            if ( m_bErrorPropagation && m_bConverged )
              output_columns.push_back(boost::lexical_cast<std::string>
                  (dSigma));
            else
              output_columns.push_back("N/A");
            nIndex++;
        }
        else if ( m_spacecraftPositionSolveType == 3 ) {
          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[1] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[1]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftVelocityAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosX[2] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosX[2]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftAccelerationAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosY[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosY[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }
          output_columns.push_back(boost::lexical_cast<std::string>
              (PosY[1] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosY[1]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftVelocityAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosY[2] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosY[2]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftAccelerationAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosZ[0] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosZ[0]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftPositionAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if ( m_bErrorPropagation && m_bConverged ) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosZ[1] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosZ[1]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftVelocityAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;

          if (m_bErrorPropagation) {
            if (bSolveSparse )
              dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
            else
              dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          }

          output_columns.push_back(boost::lexical_cast<std::string>
              (PosZ[2] - m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_Image_Corrections(nIndex)));
          output_columns.push_back(boost::lexical_cast<std::string>(PosZ[2]));
          output_columns.push_back(boost::lexical_cast<std::string>
              (m_dGlobalSpacecraftAccelerationAprioriSigma));

          if ( m_bErrorPropagation && m_bConverged )
            output_columns.push_back(boost::lexical_cast<std::string>
                (dSigma));
          else
            output_columns.push_back("N/A");
          nIndex++;
        }

        if ( m_nNumberCameraCoefSolved > 0 ) {
          for ( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {

            if ( m_bErrorPropagation && m_bConverged ) {
              if (bSolveSparse )
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
            }

            output_columns.push_back(boost::lexical_cast<std::string>
                ((coefRA[i] - m_Image_Corrections(nIndex)) * RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_Image_Corrections(nIndex) * RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>
                (coefRA[i] * RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>(
                m_dGlobalCameraAnglesAprioriSigma[i]));

            if ( m_bErrorPropagation && m_bConverged )
              output_columns.push_back(boost::lexical_cast<std::string>
                  (dSigma * RAD2DEG));
            else
              output_columns.push_back("N/A");
            nIndex++;
          }
          for ( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {

            if ( m_bErrorPropagation && m_bConverged ) {
              if (bSolveSparse )
                dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
              else
                dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
            }

            output_columns.push_back(boost::lexical_cast<std::string>
                ((coefDEC[i] - m_Image_Corrections(nIndex)) * RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_Image_Corrections(nIndex) * RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>
                (coefDEC[i] * RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>
                (m_dGlobalCameraAnglesAprioriSigma[i]));

            if ( m_bErrorPropagation && m_bConverged )
              output_columns.push_back(boost::lexical_cast<std::string>
                  (dSigma * RAD2DEG));
            else
              output_columns.push_back("N/A");
            nIndex++;
          }
          if ( !m_bSolveTwist ) {
            output_columns.push_back(boost::lexical_cast<std::string>
                (coefTWI[0]*RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back(boost::lexical_cast<std::string>
                (coefTWI[0]*RAD2DEG));
            output_columns.push_back(boost::lexical_cast<std::string>(0.0));
            output_columns.push_back("N/A");
          }
          else {
            for( int i = 0; i < m_nNumberCameraCoefSolved; i++ ) {

              if ( m_bErrorPropagation && m_bConverged ) {
                if (bSolveSparse )
                  dSigma = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
                else
                  dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
              }

              output_columns.push_back(boost::lexical_cast<std::string>
                  ((coefTWI[i] - m_Image_Corrections(nIndex)) * RAD2DEG));
              output_columns.push_back(boost::lexical_cast<std::string>
                  (m_Image_Corrections(nIndex) * RAD2DEG));
              output_columns.push_back(boost::lexical_cast<std::string>
                  (coefTWI[i] * RAD2DEG));
              output_columns.push_back(boost::lexical_cast<std::string>
                  (m_dGlobalCameraAnglesAprioriSigma[i]));

              if ( m_bErrorPropagation && m_bConverged )
                output_columns.push_back(boost::lexical_cast<std::string>
                    (dSigma * RAD2DEG));
              else
                output_columns.push_back("N/A");
              nIndex++;
            }
          }
        }

        else if ( pCamera->GetCameraType() != 3 ) {
          output_columns.push_back(boost::lexical_cast<std::string>
              (coefRA[0]*RAD2DEG));
          output_columns.push_back(boost::lexical_cast<std::string>(0.0));
          output_columns.push_back(boost::lexical_cast<std::string>
              (coefRA[0]*RAD2DEG));
          output_columns.push_back(boost::lexical_cast<std::string>(0.0));
          output_columns.push_back("N/A");
          output_columns.push_back(boost::lexical_cast<std::string>
              (coefDEC[0]*RAD2DEG));
          output_columns.push_back(boost::lexical_cast<std::string>(0.0));
          output_columns.push_back(boost::lexical_cast<std::string>
              (coefDEC[0]*RAD2DEG));
          output_columns.push_back(boost::lexical_cast<std::string>(0.0));
          output_columns.push_back("N/A");
          output_columns.push_back(boost::lexical_cast<std::string>
              (coefTWI[0]*RAD2DEG));
          output_columns.push_back(boost::lexical_cast<std::string>(0.0));
          output_columns.push_back(boost::lexical_cast<std::string>
              (coefTWI[0]*RAD2DEG));
          output_columns.push_back(boost::lexical_cast<std::string>(0.0));
          output_columns.push_back("N/A");
        }

        // print column vector to buffer and output to file
        int ncolumns = output_columns.size();
        for ( int i = 0; i < ncolumns; i++) {
          std::string str = output_columns.at(i);

          if (i < ncolumns-1)
            sprintf(buf, "%s,", (const char*)str.c_str());
          else
            sprintf(buf, "%s", (const char*)str.c_str());
          fp_out << buf;
        }
        sprintf(buf, "\n");
        fp_out << buf;
      }

      fp_out.close();

      return true;
  }

  /**
   * This method sets the solution method for solving the matrix and fills
   * the point index map, which is dependent on the solution method.
   */
  void BundleAdjust::SetSolutionMethod(std::string str) {
    m_strSolutionMethod = str;
    FillPointIndexMap();
  }

}
