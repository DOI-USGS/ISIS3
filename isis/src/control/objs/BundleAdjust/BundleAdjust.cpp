#include "BundleAdjust.h"

#include <iomanip>
#include <fstream>

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

#include "boost/numeric/ublas/matrix_sparse.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "boost/numeric/ublas/vector.hpp"
#include "cholesky.hpp"
#include "boost/numeric/ublas/matrix_expression.hpp"

using namespace boost::numeric::ublas;


namespace Isis {

  BundleAdjust::BundleAdjust(const std::string &cnetFile,
                             const std::string &cubeList,
                             bool bPrintSummary) {
    Progress progress;
    m_bCleanUp = true;
    m_pCnet = new Isis::ControlNet(cnetFile, &progress);
    m_pSnList = new Isis::SerialNumberList(cubeList);
    m_pHeldSnList = NULL;
    m_bPrintSummary = bPrintSummary;
    p_cnetFile = cnetFile;

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
    p_cnetFile = cnetFile;

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
    p_cnetFile = "";

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
    p_cnetFile = "";

    Init();
  }

  BundleAdjust::~BundleAdjust() {
    if (m_bCleanUp) {
      delete m_pCnet;
      delete m_pSnList;

      if (m_nHeldImages > 0)
        delete m_pHeldSnList;

      if (m_bObservationMode)
        delete m_pObsNumList;
    }

// Next 2 lines may be needed for deltack and other cases
// if Special K doesn't work for them
//
//    if ( m_pLsq )
//      delete m_pLsq;
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

  void BundleAdjust::Init(Progress *progress) {
//printf("BOOST_UBLAS_CHECK_ENABLE = %d\n", BOOST_UBLAS_CHECK_ENABLE);
//printf("BOOST_UBLAS_TYPE_CHECK = %d\n", BOOST_UBLAS_TYPE_CHECK);

//    m_pProgressBar = progress;

    m_dError = DBL_MAX;
    m_bOutputCSV = false;
    m_bSimulatedData = true;
    m_bObservationMode = false;    // Handled with constraints now...make sure answers match
    m_strSolutionMethod = "SPECIALK";
    m_pObsNumList = NULL;
    m_pLsq = NULL;

    // Get the cameras set up for all images
    m_pCnet->SetImages(*m_pSnList, progress);

    m_nHeldImages = 0;
    int nImages = m_pSnList->Size();

    int count;

    if (m_pHeldSnList != NULL) {
      //Check to make sure held images are in the control net
      CheckHeldList();

      // Set all points on held images to held, using measurement on held image
      // to get lat/lon/radius of point OBSOLETE way of holding image -- delete these lines once code is tested.
//      ApplyHeldList();

      // Create a lookup table of held images  OBSOLETE -- done with constraints -- delete these lines once code is tested.
//      count = 0;
//      for ( int i = 0; i < nImages; i++ ) {
//        if ( m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)) ) {
//          m_nImageIndexMap.push_back(-1);
//          m_nHeldImages++;
//        }
//        else {
//          m_nImageIndexMap.push_back(count);
//          count++;
//        }
//      }
    }
    else {
      for (int i = 0; i < nImages; i++)
        m_nImageIndexMap.push_back(i);
    }

    // Create a lookup table of ignored, held, and ground points
    m_nHeldPoints = m_nGroundPoints = m_nIgnoredPoints = 0;
    count = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();

    for (int i = 0; i < nObjectPoints; i++)     {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if (point->IsIgnored()) {
        m_nPointIndexMap.push_back(-1);
        m_nIgnoredPoints++;
        continue;
      }

      if (point->GetType() == ControlPoint::Ground)
        m_nGroundPoints++;

      m_nPointIndexMap.push_back(count);
      count++;
    }

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

    m_dGlobalLatitudeAprioriSigma = 1000.0;
    m_dGlobalLongitudeAprioriSigma = 1000.0;
    m_dGlobalRadiusAprioriSigma = 1000.0;
//    m_dGlobalSurfaceXAprioriSigma = 1000.0;
//    m_dGlobalSurfaceYAprioriSigma = 1000.0;
//    m_dGlobalSurfaceZAprioriSigma = 1000.0;
    m_dGlobalSpacecraftPositionAprioriSigma = -1.0;
    m_dGlobalSpacecraftVelocityAprioriSigma = -1.0;
    m_dGlobalSpacecraftAccelerationAprioriSigma = -1.0;
    m_dGlobalCameraAnglesAprioriSigma = -1.0;
    m_dGlobalCameraAngularVelocityAprioriSigma = -1.0;
    m_dGlobalCameraAngularAccelerationAprioriSigma = -1.0;

    m_dGlobalSpacecraftPositionWeight = -1.0;
    m_dGlobalSpacecraftVelocityWeight = -1.0;
    m_dGlobalSpacecraftAccelerationWeight = -1.0;
    m_dGlobalCameraAnglesWeight = -1.0;
    m_dGlobalCameraAngularVelocityWeight = -1.0;
    m_dGlobalCameraAngularAccelerationWeight = -1.0;

    if (!m_bSolveRadii)
      m_dGlobalRadiusAprioriSigma *= -1.0;

    m_dConvergenceThreshold = 1.0e-10;

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
      pCamera->Radii(m_BodyRadii);

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
    // residuals) and ?.
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
   * This method loads heavy constraints on image parameters of images in the held list.
   * The implementation will be done later after a mechanism for loading individual
   * image constraints has been established.
   */
  void BundleAdjust::ApplyHeldList() {
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
  }

  /**
   * Weighting for image parameter
   * ComputeNumberPartials must be called first
   */
  void BundleAdjust::ComputeImageParameterWeights() {
    // size and initialize to -1.0
    m_dImageParameterWeights.resize(m_nNumImagePartials);
    for (int i = 0; i < m_nNumImagePartials; i++)
      m_dImageParameterWeights[i] = -1.0;

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
    if (m_cmatrixSolveType == AnglesVelocityAcceleration) {
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationAprioriSigma;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationAprioriSigma;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
      m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationAprioriSigma;

      if (m_bSolveTwist) {
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAnglesWeight;
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularVelocityWeight;
        m_dImageParameterWeights[nIndex++] = m_dGlobalCameraAngularAccelerationAprioriSigma;
      }
    }

//    std::cout << m_dImageParameterWeights << std::endl;
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

    if (m_nHeldImages != 0)
      m_pObsNumList->Remove(m_pHeldSnList);

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

    int nPointParameterColumns = m_pCnet->GetNumValidPoints() * 3;

    return m_nImageParameters + nPointParameterColumns;
  }

  /**
   * Determine the number of constrained parameters
   *
   */
  int BundleAdjust::ComputeConstrainedParameters() {
//  m_nConstrainedImageParameters = Observations() * (6 - m_nNumImagePartials);

//  int nConstrained = m_nConstrainedPointParameters + m_nConstrainedImageParameters;

//  m_pLsq->SetNumberOfConstrainedParameters(nConstrained);

//    m_pLsq->SetNumberOfConstrainedParameters(m_nConstrainedPointParameters);

    return m_nConstrainedPointParameters;
  }

  /**
   * Initializes matrices and parameters for bundle adjustment
   */
  void BundleAdjust::Initialize() {
    m_nRank = m_nNumImagePartials * Observations();   // determine size of reduced normal equations matrix

    m_Normals.resize(m_nRank);                        // set size of reduced normal equations matrix
    m_Normals.clear();                                // zero all elements

    int n3DPoints = m_pCnet->GetNumValidPoints();

    m_nUnknownParameters = m_nRank + 3 * n3DPoints;

//  m_nRejectedObservations = 0;

    m_Image_Solution.resize(m_nRank);
    m_Image_Corrections.resize(m_nRank);
    m_NICs.resize(n3DPoints);
    m_Point_Corrections.resize(n3DPoints);
    m_Point_Weights.resize(n3DPoints);
    m_Point_AprioriSigmas.resize(n3DPoints);
    m_Qs.resize(n3DPoints);

    // initialize NICS, Qs, and point correction vectors to zero
    for (int i = 0; i < n3DPoints; i++) {
      m_NICs[i].clear();
      m_Qs[i].clear();
      m_Point_Corrections[i].clear();
      m_Point_Weights[i].clear();
      m_Point_AprioriSigmas[i].clear();
    }

    m_bConverged = false;                             // flag indicating convergence
    m_bError = false;                                 // flag indicating general bundle error

//    InitializePointWeights();

//  InitializeImageWeights();

    // convert apriori sigmas into weights (if they're negative or zero, we don't use them)

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

    if (m_dGlobalCameraAnglesAprioriSigma > 0.0) {
      m_dGlobalCameraAnglesWeight
      = 1.0 / (m_dGlobalCameraAnglesAprioriSigma * m_dGlobalCameraAnglesAprioriSigma * DEG2RAD * DEG2RAD);
    }

    if (m_dGlobalCameraAngularVelocityAprioriSigma > 0.0) {
      m_dGlobalCameraAngularVelocityWeight
      = 1.0 / (m_dGlobalSpacecraftAccelerationAprioriSigma * m_dGlobalSpacecraftAccelerationAprioriSigma * DEG2RAD * DEG2RAD);
    }

    if (m_dGlobalCameraAngularAccelerationAprioriSigma > 0.0) {
      m_dGlobalSpacecraftAccelerationWeight
      = 1.0 / (m_dGlobalSpacecraftAccelerationAprioriSigma * m_dGlobalSpacecraftAccelerationAprioriSigma * DEG2RAD * DEG2RAD);
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
  bool BundleAdjust::SolveSpecialK() {
//   double averageError;
    std::vector<int> observationInitialValueIndex;  // image index for observation inital values
    int iIndex = -1;                                // image index for initial spice for an observation
    int oIndex = -1;                                // Index of current observation

    ComputeNumberPartials();

    if (m_bObservationMode)
      observationInitialValueIndex.assign(m_pObsNumList->ObservationSize(), -1);

    for (int i = 0; i < Images(); i++) {
      if (m_nHeldImages > 0)
        if ((m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))) continue;

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
      if (m_nIteration != 1)
        m_Normals.clear();

      // form normal equations
//      clock_t formNormalsclock1 = clock();
//      printf("starting FormNormals\n");

      if (!FormNormalEquations()) {
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

      if (!SolveSystem()) {
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
      ApplyParameterCorrections();
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
      if (m_bOutlierRejection) {
        // compute outlier rejection limit
        ComputeRejectionLimit();

        FlagOutliers();
      }

      // variance of unit weight (also reference variance, variance factor, etc.)
      m_nDegreesOfFreedom =
        m_nObservations + (m_nConstrainedPointParameters + m_nConstrainedImageParameters) - m_nUnknownParameters;

      m_dSigma0 = dvtpv / m_nDegreesOfFreedom;
      m_dSigma0 = sqrt(m_dSigma0);

      printf("Iteration: %d\nSigma0: %20.10lf\n", m_nIteration, m_dSigma0);
      printf("Observations: %d\nConstrained Parameters:%d\nUnknowns: %d\nDegrees of Freedom: %d\n",
             m_nObservations, m_nConstrainedPointParameters, m_nUnknownParameters, m_nDegreesOfFreedom);

      // check for convergence
      if (fabs(dSigma0_previous - m_dSigma0) <= m_dConvergenceThreshold) {
        m_bLastIteration = true;
        m_bConverged = true;
        break;
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

      m_nIteration++;

      dSigma0_previous = m_dSigma0;
    }

    if (m_bConverged && m_bErrorPropagation) {
      clock_t terror1 = clock();
      ErrorPropagation();
      clock_t terror2 = clock();
      m_dElapsedTimeErrorProp = ((terror2 - terror1) / (double)CLOCKS_PER_SEC);
    }

    clock_t t2 = clock();
    m_dElapsedTime = ((t2 - t1) / (double)CLOCKS_PER_SEC);

    WrapUp();

//    printf("Start Output\n");
    Output();
//    printf("End Output\n");

//    printf("Start Output Points CSV\n");
    OutputPointsCSV();
//    printf("End Output Points\n");
//    printf("Start Output Images CSV\n");
    OutputImagesCSV();
//    printf("End Output Images\n");
//    printf("Start Output Residuals CSV\n");
    OutputResiduals();
//    printf("End Output Residuals CSV\n");

    return true;

    std::string msg = "Need to return something here, or just change the whole darn thing? [";
//    msg += iString(tol) + "] in less than [";
//    msg += iString(m_nMaxIterations) + "] iterations";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  /**
   * Forming the least-squares normal equations matrix.
   */
  bool BundleAdjust::FormNormalEquations() {
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
    static matrix<double> N12;                                    // image parameters x 3 (should this be compressed? We only make one, so probably not)
    static vector<double> n2(3);                                  // 3x1 vector
    compressed_vector<double> n1(m_nRank);                        // image parameters x 1
//  vector<double> nj(m_nRank);                                   // image parameters x 1

    m_nj.resize(m_nRank);

    coeff_image.resize(2, m_nNumImagePartials);
    N12.resize(m_nRank, 3);

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
    int nPointIndex = 0;
    int nImageIndex;
    int n3DPoints = m_pCnet->GetNumPoints();

    for (int i = 0; i < n3DPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);

//      std::cout << "   processing point " << i << " with id = " << point->Id() << std::endl;


      if (point->IsIgnored())
        continue;

//    printf("Processing %s - 3D Point %d of %d\n", point->Id().c_str(),nPointIndex,n3DPoints);

      // flagged as "JigsawHighSigma" implies this point has
      // insufficient number of observations (kluge on this flag - need specific flag)
//      if( point->Status() == ControlPoint::JigsawHighSigma )
//        continue;

      // send notification to UI indicating index of point currently being processed
      // m_nProcessedPoint = i+1;
      // UI.Notify(BundleEvent.NEW_POINT_PROCESSED);

      if (i != 0) {
        N22.clear();
        N12.clear();
        n2.clear();
      }

      int nMeasures = point->GetNumMeasures();

      // set up non-zero indices for Q matrix - Q will be sparse matrix - do we have to do this?
//    if( m_nIterations == 0 )
//    {
//      m_nonZeroColumns = m_nNumImagePartials * nMeasures;
//      nonZeroIndices = new int[3][m_nonZeroColumns];
//    }

      // loop over measures for this point
      for (int j = 0; j < nMeasures; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);

        if (measure->IsIgnored())
          continue;

        // flagged as "JigsawFail" implies this measure has been rejected
        // TODO  IsRejected is obsolete -- replace code or add to ControlMeasure
        if (measure->IsRejected()) {
          printf("skipping rejected observation for %s\n",
                 point->GetId().c_str());
          continue;
        }

        if (m_nHeldImages > 0 && m_pHeldSnList->HasSerialNumber(measure->GetCubeSerialNumber()))
          continue;

//        printf("   Processing Measure %d of %d\n", j,nMeasures);

        // fill non-zero indices for this point - do we have to do this?
        // see BundleDistanceConstraints.java for code snippet (line 926)

        // Determine the image index
        nImageIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());

//        std::cout << "  About to call ComputePartials..." << std::endl;


        bStatus = ComputePartials(coeff_image, coeff_point3D, coeff_RHS,
                                  *measure, *point);

//        std::cout << coeff_image << std::endl;
//        std::cout << coeff_point3D << std::endl;
//        std::cout << coeff_RHS << std::endl;

        if (!bStatus)
          continue;     // this measure should be flagged as rejected

        // update number of observations
        m_nObservations += 2;

        FormNormalEquations1(N22, N12, n1, n2, coeff_image, coeff_point3D,
                             coeff_RHS, nImageIndex);
      } // end loop over this points measures

      FormNormalEquations2(N22, N12, n2, m_nj, nPointIndex, i);
      nPointIndex++;

//      if (m_pProgressBar != NULL)
//        m_pProgressBar->CheckStatus();

    } // end loop over 3D points

    // finally, form the reduced normal equations
    FormNormalEquations3(n1, m_nj);

    // count zeroes
//    int nzeroes = 0;
//    for( int i = 0; i < m_nRank; i++ )
//      for( int j = i; j < m_nRank; j++ )
//        if( m_Normals(i,j) == 0.0 )
//          nzeroes++;

//    printf("Zeros: %d of %d\n",nzeroes,(m_nRank*(m_nRank+1)/2));

//    std::cout << m_Normals << std::endl;

    // update number of unknown parameters

//    m_nUnknownParameters = m_nRank + 3 * nGood3DPoints;

    return bStatus;
  }

  bool BundleAdjust::FormNormalEquations1(symmetric_matrix<double, upper>&N22, matrix<double>& N12,
                                          compressed_vector<double>& n1, vector<double>& n2,
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

  bool BundleAdjust::FormNormalEquations2(symmetric_matrix<double, upper>&N22, matrix<double>& N12,
                                          vector<double>& n2, vector<double>& nj, int nPointIndex, int i) {
//    if( m_nIteration == 0 )
//    {
//      m_NICs(nPointIndex).clear();
//    }

//    m_NICs(nPointIndex).clear();
    bounded_vector<double, 3>& NIC = m_NICs[nPointIndex];
    compressed_matrix<double>& Q = m_Qs[nPointIndex];

    NIC.clear();
    Q.clear();

    // weighting of 3D point parameters
    const ControlPoint *point = m_pCnet->GetPoint(i);

    bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
    bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

//    std::cout << "weights" << std::endl << weights << std::endl;

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

    // invert N22
    Invert_3x3(N22);

    // save upper triangular covariance matrix for error propagation
    // TODO:  The following method does not exist yet (08-13-2010)
    // Can N22 be cast to vector type? Or should SurfacePoint be changed to use matrix type?
    point->GetSurfacePoint().SetSphericalMatrix(N22);

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
    AmultAdd_CNZRows(-1.0, N12, Q, m_Normals);
//  clock_t AccumIntoNormals2 = clock();
//  double dAccumIntoNormalsTime = ((AccumIntoNormals2-AccumIntoNormals1)/(double)CLOCKS_PER_SEC);
//  printf("Accum Into Normals Elapsed Time: %20.10lf\n",dAccumIntoNormalsTime);

    // accumulate -nj
//    clock_t Accum_nj_1 = clock();
//    m_nj -= prod(trans(Q),n2);
    transA_NZ_multAdd(-1.0, Q, n2, m_nj);
//    clock_t Accum_nj_2 = clock();
//    double dAccumnjTime = ((Accum_nj_2-Accum_nj_1)/(double)CLOCKS_PER_SEC);
//    printf("Accum nj Elapsed Time: %20.10lf\n",dAccumnjTime);

    return true;
  }

  bool BundleAdjust::InitializePointWeights() {
    // TODO:  Get working as is then convert to use new classes (Angles, etc.) and xyz with radius constraints
    double dAprioriSigmaLat;
    double dAprioriSigmaLon;
    double dAprioriSigmaRad;
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
      aprioriSurfacePoint.SetRadii(Distance(m_BodyRadii[0]),
                                   Distance(m_BodyRadii[1]),
                                   Distance(m_BodyRadii[2]));
      point->SetAprioriSurfacePoint(aprioriSurfacePoint);

      bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
      bounded_vector<double, 3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

//      std::cout << weights << std::endl;

//      if( point->Held() || point->Type() == ControlPoint::Ground )
      if (point->GetType() == ControlPoint::Ground) {
        weights[0] = 1.0e+25;
        weights[1] = 1.0e+25;
        weights[2] = 1.0e+25;
      }
      else {
//      if(  m_dGlobalLatitudeAprioriSigma > 0.0 )
//        dAprioriSigmaLat = m_dGlobalLatitudeAprioriSigma;
//      else
//        dAprioriSigmaLat = (double) point->GetAprioriSurfacePoint().GetLatSigma();

        dAprioriSigmaLat = point->GetAprioriSurfacePoint().GetLatSigmaDistance().GetMeters();

        if (dAprioriSigmaLat <= 0.0 || dAprioriSigmaLat >= 1000.0) {
          if (m_dGlobalLatitudeAprioriSigma > 0.0)
            dAprioriSigmaLat = m_dGlobalLatitudeAprioriSigma;
        }
        apriorisigmas[0] = dAprioriSigmaLat;

        if (dAprioriSigmaLat > 0.0  &&  dAprioriSigmaLat < 1000.0) {
          d = dAprioriSigmaLat * m_dMTR;
          weights[0] = 1.0 / (d * d);
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

        dAprioriSigmaLon = point->GetAprioriSurfacePoint().GetLonSigmaDistance().GetMeters();
        if (dAprioriSigmaLon <= 0.0 || dAprioriSigmaLon >= 1000.0) {
          if (m_dGlobalLongitudeAprioriSigma > 0.0)
            dAprioriSigmaLon = m_dGlobalLongitudeAprioriSigma;
        }
        apriorisigmas[1] = dAprioriSigmaLon;

        if (dAprioriSigmaLon > 0.0  &&  dAprioriSigmaLon < 1000.0) {
          d = dAprioriSigmaLon * m_dMTR * cos(point->GetSurfacePoint().GetLatitude().GetRadians());
          weights[1] = 1.0 / (d * d);
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

        dAprioriSigmaRad = point->GetAprioriSurfacePoint().GetLocalRadiusSigma().GetMeters();
        if (dAprioriSigmaRad <= 0.0 || dAprioriSigmaRad >= 1000.0) {
          if (m_dGlobalRadiusAprioriSigma > 0.0)
            dAprioriSigmaRad = m_dGlobalRadiusAprioriSigma;
        }
        apriorisigmas[2] = dAprioriSigmaRad;

        if (!m_bSolveRadii) {
          weights[2] = 1.0e+25;
        }
        else if (dAprioriSigmaRad > 0.0  &&  dAprioriSigmaRad < 1000.0) {
          d = dAprioriSigmaRad * 0.001;
          weights[2] = 1.0 / (d * d);
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

      aprioriSurfacePoint.SetSphericalSigmasDistance(
          Distance(apriorisigmas[0], Distance::Meters),
          Distance(apriorisigmas[1], Distance::Meters),
          Distance(apriorisigmas[2], Distance::Meters));
      point->SetAprioriSurfacePoint(aprioriSurfacePoint);
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
      point->SetSurfacePoint(aprioriSurfacePoint);
    }

  }


  void BundleAdjust::AmultAdd_CNZRows(double alpha, matrix<double>& A, compressed_matrix<double>& B,
                                      symmetric_matrix<double, upper, column_major>& C)
//  void BundleAdjust::AmultAdd_CNZRows(double alpha, matrix<double>& A, compressed_matrix<double>& B,
//                                      symmetric_matrix<double, lower>& C)
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

//    std::cout << nz << std::endl;

    /*
          for( it_row itr = cmd.begin1(); itr != cmd.end1(); ++itr )
        }
        {
          for( it_col itc = itr.begin(); itc != itr.end(); ++itc )
          {
            std::cout << "(" << itc.index1() << "," << itc.index2()
                      << ":" << *itc << ")  ";
          }
          std::cout << endl;
        }
    */

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

  void BundleAdjust::transA_NZ_multAdd(double alpha, compressed_matrix<double>& A, vector<double>& B,
                                       vector<double>& C) {
    if (alpha == 0.0)
      return;

    register int i, j, ii;
    double d;

    int nRowsA = A.size1();

    // create array of non-zero indices of matrix A
    std::vector<int> nz(A.nnz() / A.size1());

    // iterators for A
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

  bool BundleAdjust::FormNormalEquations3(compressed_vector<double>& n1, vector<double>& nj) {
    // apply weighting for spacecraft position, velocity, acceleration
    // and camera angles, angular velocities, angular accelerations
    // if so stipulated (legalese)

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

  bool BundleAdjust::SolveSystem() {
    // decomposition (this is UTDU - need to test row vs column major
    // storage and access, and use of matrix iterators)
//    clock_t CholeskyUtDUclock1 = clock();
//    printf("Starting Cholesky\n");
//    cholesky_decompose(m_Normals);
    if (!CholeskyUT_NOSQR())
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
    /*
        for( i = 0; i < nRows; i++ )
        {
          for( j = i; j < nRows; j++ )
          {
            printf("%lf ",m_Normals(i,j));
          }
          printf("\n");
        }
    */
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

      // store solution in corresponding column of inverse (replacing column in m_Normals)
      for (j = 0; j <= i; j++)
        m_Normals(j, i) = s(j);
    }

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
    double dObservationWeight;

    // auxiliary variables
//    double NX_C,NY_C,D_C;
//    double NX,NY;
//    double a1,a2,a3;
//    double z1,z2,z3,z4;

//    double dTime = -1.0;

    // partials for ground point w/r lat, long, radius in Body-Fixed (?)
    // need to verify this
    // For now move entire point partial below.  Maybe later be more efficient  ----DC
    // and split the GetdXYdPoint method of CameraGroundMap into PointPartial part
    // and the rest like Ken has it.

//    d_lookB_WRT_LAT = PointPartial(point,WRT_Latitude);
//    d_lookB_WRT_LON = PointPartial(point,WRT_Longitude);
//    d_lookB_WRT_RAD = PointPartial(point,WRT_Radius);

    pCamera = measure.Camera();

    // Compute ground point in body-fixed coordinates

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
    if (!(pCamera->GroundMap()->GetXY(point.GetSurfacePoint(), &dComputedx, &dComputedy))) {
      std::string msg = "Unable to map apriori surface point for measure ";
      msg += measure.GetCubeSerialNumber() + " on point " + point.GetId() + " into focal plane";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // partials for ground point w/r lat, long, radius in Body-Fixed
    d_lookB_WRT_LAT = pCamera->GroundMap()->PointPartial(point.GetSurfacePoint(),
                      CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = pCamera->GroundMap()->PointPartial(point.GetSurfacePoint(),
                      CameraGroundMap::WRT_Longitude);
    d_lookB_WRT_RAD = pCamera->GroundMap()->PointPartial(point.GetSurfacePoint(),
                      CameraGroundMap::WRT_Radius);

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
    pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_RAD, &coeff_point3D(0, 2),
                                       &coeff_point3D(1, 2));

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
  double BundleAdjust::Solve(double tol) {
    double averageError;
    std::vector<int> observationInitialValueIndex;  // image index for observation inital values
    int iIndex = -1;                                // image index for initial spice for an observation
    int oIndex = -1;                                // Index of current observation

    ComputeNumberPartials();

    if (m_bObservationMode)
      observationInitialValueIndex.assign(m_pObsNumList->ObservationSize(), -1);

    for (int i = 0; i < Images(); i++) {
      if (m_nHeldImages > 0)
        if ((m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))) continue;

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
    m_nIteration = 0;

    clock_t t1 = clock();

    // Create the basis function and prep for a least squares solution
    m_nBasisColumns = BasisColumns();
    BasisFunction basis("Bundle", m_nBasisColumns, m_nBasisColumns);
    if (m_strSolutionMethod == "SPARSE") {
      m_pLsq = new Isis::LeastSquares(basis, true,
                                      m_pCnet->GetNumValidMeasures() * 2, m_nBasisColumns);
    }
    else
      m_pLsq = new Isis::LeastSquares(basis);

    // set size of partial derivative vectors
    m_dxKnowns.resize(m_nBasisColumns);
    m_dyKnowns.resize(m_nBasisColumns);

    double dprevious_Sigma0 = 0.0;

    Progress progress;

    while (m_nIteration < m_nMaxIterations) {
      m_nIteration++;

      m_pCnet->ComputeResiduals();
      m_dError = m_pCnet->GetMaximumResidual();
      averageError = m_pCnet->AverageResidual();

      // kle testing - print residual(?) statistics
      printf("Iteration #%d\n\tAverage Error: %lf\n\tSigmaXY: %lf\n\tSigmaHat: %lf\n\tSigmaX: %lf\n\tSigmaY: %lf\n",
             m_nIteration, averageError, sigmaXY, sigmaHat, sigmaX, sigmaY);

      if (m_bPrintSummary)
        IterationSummary(averageError, sigmaXY, sigmaHat, sigmaX, sigmaY);

      // these vectors hold statistics for right-hand sides (observed - computed)
      m_Statsx.Reset();
      m_Statsy.Reset();

      // these vectors hold statistics for true residuals
      m_Statsrx.Reset();
      m_Statsry.Reset();
      m_Statsrxy.Reset();

      if (m_nIteration == 1)
        m_dSigma0 = sigmaHat = 10.0;

      // we've converged
      if (fabs(dprevious_Sigma0 - m_dSigma0) <= m_dConvergenceThreshold) {
        clock_t t2 = clock();
        m_dElapsedTime = ((t2 - t1) / (double)CLOCKS_PER_SEC);

        // retrieve vector of parameter corrections
        m_dEpsilons = m_pLsq->GetEpsilons();

        if (m_bErrorPropagation) {
          progress.SetText("Performing Error Propagation...");

          printf("start error prop\n");
          if (m_pLsq->SparseErrorPropagation())
            SetPostBundleSigmas();
          printf("end error prop\n");
        }

        printf("start output\n");
        Output();
        OutputPointsCSV();
        OutputImagesCSV();
        printf("end output\n");

        return m_dError;
      }

      dprevious_Sigma0 = m_dSigma0;

      if (m_nIteration > 1  && m_strSolutionMethod == "SPARSE")
        m_pLsq->ResetSparse();

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
        else {
          // set parameter weights
          SetParameterWeights();
          ComputeConstrainedParameters();

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
        msg += Isis::iString(m_nMaxIterations) + ", Tolerance = ";
        msg += Isis::iString(tol);
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
      sigmaHat = (m_nObservations - m_nBasisColumns) ?
                 (sqrt((m_Statsx.SumSquare() + m_Statsy.SumSquare()) / (m_nObservations - m_nBasisColumns)))
                 : 0.;

      m_dSigma0 = m_pLsq->GetSigma0();
      m_nDegreesOfFreedom = m_pLsq->GetDegreesOfFreedom();

      printf("Observations: %d   Unknowns: %d\n", m_nObservations, m_nUnknownParameters);
      printf("SigmaHat: %20.10lf   Sigma0: %20.10lf\n", sigmaHat, m_dSigma0);

      sigmaX = m_Statsx.TotalPixels() ?
               sqrt(m_Statsx.SumSquare() / m_Statsx.TotalPixels()) : 0.;
      sigmaY = m_Statsy.TotalPixels() ?
               sqrt(m_Statsy.SumSquare() / m_Statsy.TotalPixels()) : 0.;
    }

    std::string msg = "Did not converge to tolerance [";
    msg += iString(tol) + "] in less than [";
    msg += iString(m_nMaxIterations) + "] iterations";
    throw iException::Message(iException::User, msg, _FILEINFO_);
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
    double pB[3];                        // Point on surface
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
    double fl;

    int nIndex;
//    double dMeasuredx,dComputedx,dMeasuredy,dComputedy;
    double dMeasuredx, dMeasuredy;
    double deltax, deltay;
    double dObservationSigma;
    double dObservationWeight;

    // auxiliary variables
    double NX_C, NY_C, D_C;
    double NX, NY;
    double a1, a2, a3;
    double z1, z2, z3, z4;

    double dTime = -1.0;

    // partials for ground point w/r lat, long, radius in Body-Fixed
    d_lookB_WRT_LAT = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetSurfacePoint(),
                        CameraGroundMap::WRT_Latitude);
    d_lookB_WRT_LON = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetSurfacePoint(),
                        CameraGroundMap::WRT_Longitude);
    d_lookB_WRT_RAD = point->GetMeasure(0)->Camera()->GroundMap()->PointPartial(
                        point->GetSurfacePoint(),
                        CameraGroundMap::WRT_Radius);

    // Compute ground point in body-fixed coordinates
    latrec_c((double) point->GetSurfacePoint().GetLocalRadius().GetKilometers(),
             (double) point->GetSurfacePoint().GetLongitude().GetRadians(),
             (double) point->GetSurfacePoint().GetLatitude().GetRadians(),
             pB);

    int nObservations = point->GetNumMeasures();
    for (int i = 0; i < nObservations; i++) {
      const ControlMeasure *measure = point->GetMeasure(i);
      if (measure->IsIgnored())
        continue;

      if (m_nHeldImages > 0) {
        if (m_pHeldSnList->HasSerialNumber(measure->GetCubeSerialNumber()))
          continue;
      }

      // zero partial derivative vectors
      memset(px, 0, m_nBasisColumns * sizeof(double));
      memset(py, 0, m_nBasisColumns * sizeof(double));

      pCamera = measure->Camera();

      // Get focal length with direction
      fl = pCamera->DistortionMap()->UndistortedFocalPlaneZ();

      // no need to call SetImage for framing camera ( CameraType  = 0 )
      if (pCamera->GetCameraType() != 0) {
        // Set the Spice to the measured point
        // but, can this be simplified???
        if (!pCamera->SetImage(measure->GetSample(), measure->GetLine()))
          printf("\n***Call to Camera::SetImage failed - need to handle this***\n");
      }

      //Compute the look vector in instrument coordinates based on time of observation and apriori lat/lon/radius
      double dComputedx, dComputedy;
      if (!(pCamera->GroundMap()->GetXY(point->GetSurfacePoint(), &dComputedx, &dComputedy))) {
        std::string msg = "Unable to map apriori surface point for measure ";
        msg += measure->GetCubeSerialNumber() + " on point " + point->GetId() + " into focal plane";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      // May need to do back of planet test here TODO
      SpiceRotation *pBodyRot = pCamera->BodyRotation();

      // "InstumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
      // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
      sB = pBodyRot->ReferenceVector(pCamera->InstrumentPosition()->Coordinate());

      lookB[0] = pB[0] - sB[0];
      lookB[1] = pB[1] - sB[1];
      lookB[2] = pB[2] - sB[2];

      // get look vector in the camera frame
      lookJ = pBodyRot->J2000Vector(lookB);
      SpiceRotation *pInstRot = pCamera->InstrumentRotation();
      lookC = pInstRot->ReferenceVector(lookJ);

      // get J2000 to camera rotation matrix
      CJ = pCamera->InstrumentRotation()->Matrix();

      // collinearity auxiliaries
      NX_C = CJ[0] * lookJ[0] + CJ[1] * lookJ[1] + CJ[2] * lookJ[2];
      NY_C = CJ[3] * lookJ[0] + CJ[4] * lookJ[1] + CJ[5] * lookJ[2];
      D_C = CJ[6] * lookJ[0] + CJ[7] * lookJ[1] + CJ[8] * lookJ[2];
      a1 = fl / D_C;
      a2 = NX_C / D_C;
      a3 = NY_C / D_C;

      // Determine the image index
      nIndex = m_pSnList->SerialNumberIndex(measure->GetCubeSerialNumber());
      nIndex = ImageIndex(nIndex);

      if (m_spacecraftPositionSolveType != Nothing) {
        SpicePosition *pInstPos = pCamera->InstrumentPosition();

        // Add the partial for the x coordinate of the position (differentiating
        // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
        // ***TODO*** check derivative with scale added to dTime
        px[nIndex] = a1 * (CJ[6] * a2 - CJ[0]);
        py[nIndex] = a1 * (CJ[6] * a3 - CJ[3]);
        nIndex++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          dTime = pInstPos->EphemerisTime() - pInstPos->GetBaseTime();
          dTime = dTime / pInstPos->GetTimeScale();

          px[nIndex] = px[nIndex-1] * dTime;
          py[nIndex] = py[nIndex-1] * dTime;
          nIndex++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            px[nIndex] = px[nIndex-1] * dTime;
            py[nIndex] = py[nIndex-1] * dTime;
            nIndex++;
          }
        }

        // Add the partial for the y coordinate of the position
        px[nIndex] = a1 * (CJ[7] * a2 - CJ[1]);
        py[nIndex] = a1 * (CJ[7] * a3 - CJ[4]);
        nIndex++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          px[nIndex] = px[nIndex-1] * dTime;
          py[nIndex] = py[nIndex-1] * dTime;
          nIndex++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            px[nIndex] = px[nIndex-1] * dTime;
            py[nIndex] = py[nIndex-1] * dTime;
            nIndex++;
          }
        }

        // Add the partial for the z coordinate of the position
        px[nIndex] = a1 * (CJ[8] * a2 - CJ[2]);
        py[nIndex] = a1 * (CJ[8] * a3 - CJ[5]);
        nIndex++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          px[nIndex] = px[nIndex-1] * dTime;
          py[nIndex] = py[nIndex-1] * dTime;

          nIndex++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            px[nIndex] = px[nIndex-1] * dTime;
            py[nIndex] = py[nIndex-1] * dTime;
            nIndex++;
          }
        }
      }

      if (m_cmatrixSolveType != None) {
        TC = pInstRot->ConstantMatrix();
        TB = pInstRot->TimeBasedMatrix();

        dTime = pInstRot->EphemerisTime() - pInstRot->GetBaseTime();
        dTime = dTime / pInstRot->GetTimeScale();

        // additional collinearity auxiliaries
        NX = TB[0] * lookJ[0] + TB[1] * lookJ[1] + TB[2] * lookJ[2];
        NY = TB[3] * lookJ[0] + TB[4] * lookJ[1] + TB[5] * lookJ[2];

        // Add the partials for ra
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          if (icoef == 0) {
            z1 = -TB[1] * lookJ[0] + TB[0] * lookJ[1];
            z2 = -TB[4] * lookJ[0] + TB[3] * lookJ[1];
            z3 = -TB[7] * lookJ[0] + TB[6] * lookJ[1];
            z4 =  TC[6] * z1 + TC[7] * z2 + TC[8] * z3;

            px[nIndex] = a1 * (TC[0] * z1 + TC[1] * z2 + TC[2] * z3 - z4 * a2);
            py[nIndex] = a1 * (TC[3] * z1 + TC[4] * z2 + TC[5] * z3 - z4 * a3);
          }
          else {
            px[nIndex] = px[nIndex-1] * dTime;
            py[nIndex] = py[nIndex-1] * dTime;
          }
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
          z1 = TC[6] * NY - TC[7] * NX;

          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            if (icoef == 0) {
              px[nIndex] = a1 * (((TC[0] * NY - TC[1] * NX) - z1 * a2));
              py[nIndex] = a1 * (((TC[3] * NY - TC[4] * NX) - z1 * a3));
            }
            else {
              px[nIndex] = px[nIndex-1] * dTime;
              py[nIndex] = py[nIndex-1] * dTime;
            }
            nIndex++;
          }
        }
      }

      // partials for 3D point
      nIndex = PointIndex(nPointIndex);

      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LAT, &px[nIndex],
                                         &py[nIndex]);
      nIndex++;
      pCamera->GroundMap()->GetdXYdPoint(d_lookB_WRT_LON, &px[nIndex],
                                         &py[nIndex]);
      nIndex++;
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
      // right-hand side (measured - computed)
      dMeasuredx = measure->GetFocalPlaneMeasuredX();
      dComputedx = lookC[0] * fl / lookC[2];

      dMeasuredy = measure->GetFocalPlaneMeasuredY();
      dComputedy = lookC[1] * fl / lookC[2];

      deltax = dMeasuredx - dComputedx;
      deltay = dMeasuredy - dComputedy;

      dObservationSigma = 1.4 * pCamera->PixelPitch();
      dObservationWeight = 1.0 / (dObservationSigma * dObservationSigma);

      m_pLsq->AddKnown(m_dxKnowns, deltax, dObservationWeight);
      m_pLsq->AddKnown(m_dyKnowns, deltay, dObservationWeight);

      m_Statsx.AddData(deltax);
      m_Statsy.AddData(deltay);
    }
  }

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
    m_pCnet->GetPoint(nIndex)->SetSurfacePoint(
        SurfacePoint(
          Latitude(lat, Angle::Radians),
          Longitude(lon, Angle::Radians),
          Distance(rad, Distance::Kilometers)));

    // Compute ground point in body-fixed coordinates
    double pB[3];
    latrec_c((double) rPoint.GetSurfacePoint().GetLocalRadius().GetKilometers(),
             (double) rPoint.GetSurfacePoint().GetLongitude().GetRadians(),
             (double) rPoint.GetSurfacePoint().GetLatitude().GetRadians(),
             pB);

//    printf("%s: %lf   %lf   %lf\n",rPoint.Id().c_str(), AveragePoint[0],AveragePoint[1],AveragePoint[2]);
//    printf("    %lf   %lf   %lf\n", avglat,avglon,avgrad);
//    printf("    %lf   %lf   %lf\n", lat,lon,rad);
//    printf("    %lf   %lf   %lf\n",pB[0],pB[1],pB[2]);

    return true;
  }

  /**
   * Convenience method for quotient rule
   */
//double BundleAdjust::LowDHigh(std::vector<double> &lookC,
//                            std::vector<double> &dlookC,
//                            int index) {
//  return (lookC[2] * dlookC[index] - lookC[index] * dlookC[2]) /
//         (lookC[2] * lookC[2]);
//}
//
  /**
   * apply parameter corrections
   */
  void BundleAdjust::ApplyParameterCorrections() {
//    std::cout << "image corrections: " << m_Image_Corrections <<\ std::endl;
//    std::cout << "   image solution: " << m_Image_Solution << std::endl;

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

        // Update the X coordinate coefficient(s) and sum parameter correction
        abcX[0] += m_Image_Solution(index);
        m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcX[1] += m_Image_Solution(index);
          m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcX[2] += m_Image_Solution(index);
            m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        // Update the Y coordinate coefficient(s)
        abcY[0] += m_Image_Solution(index);
        m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcY[1] += m_Image_Solution(index);
          m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcY[2] += m_Image_Solution(index);
            m_Image_Corrections(index) += m_Image_Solution(index);
            index++;
          }
        }

        // Update the Z coordinate coefficient(s)
        abcZ[0] += m_Image_Solution(index);
        m_Image_Corrections(index) += m_Image_Solution(index);
        index++;

        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcZ[1] += m_Image_Solution(index);
          m_Image_Corrections(index) += m_Image_Solution(index);
          index++;

          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcZ[2] += m_Image_Solution(index);
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
          m_Image_Corrections(index) += m_Image_Solution(index);
          index++;
        }

        // Update declination coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
          coefDEC[icoef] += m_Image_Solution(index);
          m_Image_Corrections(index) += m_Image_Solution(index);
          index++;
        }

        if (m_bSolveTwist) {
          // Update twist coefficient(s)
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++) {
            coefTWI[icoef] += m_Image_Solution(index);
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

//      if( point->Status() == ControlPoint::JigsawHighSigma )
//        continue;

      // no update to points that are being held absolutely
      // ***NOTE: think about this
//      if( point->Held() || point->Type() == ControlPoint::Ground )
//        continue;

      // get NIC, Q, and correction vector for this point
      bounded_vector<double, 3>& NIC = m_NICs[nPointIndex];
      compressed_matrix<double>& Q = m_Qs[nPointIndex];
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

//      printf("Point %s Corrections\n Latitude: %20.10lf\nLongitude: %20.10lf\n   Radius: %20.10lf\n",point->Id().c_str(),dLatCorr, dLongCorr, dRadCorr);

      double dLat = point->GetSurfacePoint().GetLatitude().GetDegrees();
      double dLon = point->GetSurfacePoint().GetLongitude().GetDegrees();
      double dRad = point->GetSurfacePoint().GetLocalRadius().GetMeters();

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

      point->SetSurfacePoint(SurfacePoint(Latitude(dLat, Angle::Degrees),
                                          Longitude(dLon, Angle::Degrees),
                                          Distance(dRad, Distance::Meters)));
      nPointIndex++;

      // testing
      // Compute ground point in body-fixed coordinates
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
    double v, vx, vy, vz;

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
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

// Next line appears to do nothing so skip it.
//      point->ComputeResiduals_Millimeters();

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

    // add vtpv from constrained 3D points
    int nPointIndex = 0;
    for (int i = 0; i < nObjectPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored() || point->IsInvalid())
        continue;

      // get weight and correction vector for this point
      bounded_vector<double, 3>& weights = m_Point_Weights[nPointIndex];
      bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];

//    std::cout << weights << std::endl;
//    std::cout << corrections << std::endl;

      vx = corrections[0];
      vy = corrections[1];
      vz = corrections[2];

      vtpv_control += vx * vx * weights[0] + vy * vy * weights[1] + vz * vz * weights[2];
      nPointIndex++;
    }

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


    vtpv = vtpv + vtpv_control + vtpv_image;

    // Compute stats for image coordinate residuals
    m_drms_rx  = sqrt(m_Statsrx.SumSquare() / (m_nObservations / 2));
    m_drms_ry  = sqrt(m_Statsry.SumSquare() / (m_nObservations / 2));
    m_drms_rxy = sqrt(m_Statsrxy.SumSquare() / m_nObservations);

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
      /*
            int nMeasures = point->Size();
            for( int j = 0; j < nMeasures; j++ )
            {
              ControlMeasure& measure = point[j];
              if ( measure.Ignore() )
                continue;

              // framing camera
              if( measure.Camera()->GetCameraType() == 0 )
              {
                dpixpitch = (measure.Camera())->PixelPitch();

                vx = measure.SampleResidual();
                vy = measure.LineResidual();

                vx = vx/dpixpitch;
                vy = vy/dpixpitch;
              }

              measure.SetResidual(vx,vy);
            }
      */
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
      if (point->IsIgnored())
        continue;

//      if ( point->Status() == ControlPoint::JigsawHighSigma )
//        continue;

      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {
        const ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored())
          continue;

        if (measure->IsRejected())
          continue;

        vx = measure->GetSampleResidual();
        vy = measure->GetLineResidual();

        x_residuals[nObservation] = fabs(vx);
        y_residuals[nObservation] = fabs(vy);

        nObservation++;
      }
    }

//    std::cout << "x residuals\n" << x_residuals << std::endl;
//    std::cout << "y_residuals\n" << y_residuals << std::endl;

    // sort vectors
    std::sort(x_residuals.begin(), x_residuals.end());
    std::sort(y_residuals.begin(), y_residuals.end());

//    std::cout << "x residuals sorted\n" << x_residuals << std::endl;
//    std::cout << "y_residuals sorted\n" << y_residuals << std::endl;

    double xmedian, ymedian;
    double xmad, ymad;

    int nmidpoint = nResiduals / 2;

    if (nResiduals % 2) {
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

    if (nResiduals % 2) {
      xmad = 1.4826 * x_residuals[nmidpoint];
      ymad = 1.4826 * y_residuals[nmidpoint];
    }
    else {
      xmad = 1.4826 * (x_residuals[nmidpoint-1] + x_residuals[nmidpoint]) / 2;
      ymad = 1.4826 * (y_residuals[nmidpoint-1] + y_residuals[nmidpoint]) / 2;
    }

    m_dRejectionLimit = 5.0 * std::max(xmad, ymad);

    return true;
  }

  bool BundleAdjust::FlagOutliers() {
    double vx, vy;
    int nRejected;

    int nIndexMaxResidual = -1;
    double dMaxResidual;
    double dSumSquares;

    printf("Iteration %d Rejection Limit: %20.10lf\n", m_nIteration, m_dRejectionLimit);

//    m_nRejectedObservations = 0;

    int ntotalrejected = 0;

    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

// Do we need this method below...TODO
      //      point->ZeroNumberOfRejectedMeasures();

      nRejected = 0;
      dMaxResidual = -1.0;

      int nMeasures = point->GetNumMeasures();
      for (int j = 0; j < nMeasures; j++) {
        ControlMeasure *measure = point->GetMeasure(j);
        if (measure->IsIgnored())
          continue;

        vx = measure->GetSampleResidual();
        vy = measure->GetLineResidual();

        if (fabs(vx) < m_dRejectionLimit && fabs(vy) < m_dRejectionLimit) {
          measure->SetRejected(false);
          continue;
        }

        // if it's still rejected, skip it
        if (measure->IsRejected()) {
          nRejected++;
          continue;
        }

        dSumSquares = vx * vx + vy * vy;

        if (dSumSquares > dMaxResidual) {
          dMaxResidual = dSumSquares;
          nIndexMaxResidual = j;
        }
      }

      // no observations above the current
      // rejection limit for this 3D point
      if (dMaxResidual == -1.0) {
        point->SetNumberOfRejectedMeasures(nRejected);
        continue;
      }

      // this is another kluge - if we only have two observations
      // we won't reject (for now)
      if ((nMeasures - (nRejected + 1)) < 2) {
        point->SetNumberOfRejectedMeasures(nRejected);
        continue;
      }

      // otherwise, we have at least one observation
      // for this point whose residual is above the
      // current rejection limit - we'll flag the
      // worst of these as rejected
      ControlMeasure *rejected = point->GetMeasure(nIndexMaxResidual);
      rejected->SetRejected(true);
      nRejected++;
      point->SetNumberOfRejectedMeasures(nRejected);
      ntotalrejected++;

      // do we still have sufficient remaining observations for this 3D point?
//      if( (nMeasures-nRejected) < 2 )
//        point->SetStatus(ControlPoint::JigsawHighSigma); // ATTENTION: this is a kluge, need to check number of valid
//      else                                              // measures that the 3D point has
//        point->SetStatus(ControlPoint::Valid);

      int ndummy = point->GetNumberOfRejectedMeasures();
      printf("Rejected for point %s = %d\n", point->GetId().c_str(), ndummy);
      printf("%s: %20.10lf  %20.10lf*\n",
             point->GetId().c_str(), rejected->GetSampleResidual(), rejected->GetLineResidual());
    }

    printf("Total Rejections: %d\n", ntotalrejected);

    return true;
  }

  bool BundleAdjust::ErrorPropagation() {
    // create inverse of normal equations matrix
    if (!CholeskyUT_NOSQR_Inverse())
      return false;

    matrix<double> T(3, 3);
    matrix<double> QS(3, m_nRank);
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    double t;

    double dSigma02 = m_dSigma0 * m_dSigma0;

    int nPointIndex = 0;
    int nObjectPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nObjectPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      T.clear();
      QS.clear();

      // get corresponding Q matrix
      compressed_matrix<double>& Q = m_Qs[nPointIndex];

      // form QS
      QS = prod(Q, m_Normals);

      // form T
      T = prod(QS, trans(Q));

      // Ask Ken what is happening here...Setting just the sigmas is not very accurate
      // Shouldn't we be updating and setting the matrix???  TODO
      point->GetSurfacePoint().SetRadii(Distance(m_BodyRadii[0]),
                                        Distance(m_BodyRadii[1]),
                                        Distance(m_BodyRadii[0]));
      dSigmaLat = point->GetSurfacePoint().GetLatSigmaDistance().GetMeters();
      dSigmaLong = point->GetSurfacePoint().GetLonSigmaDistance().GetMeters();
      dSigmaRadius = point->GetSurfacePoint().GetLocalRadiusSigma().GetMeters();

      t = dSigmaLat + T(0, 0);
      Distance tLatSig(sqrt(dSigma02 * t) * m_dRTM, Distance::Meters);

      t = dSigmaLong + T(1, 1);
      t = sqrt(dSigma02 * t) * m_dRTM;
      Distance tLonSig(
          t / cos(point->GetSurfacePoint().GetLatitude().GetRadians()),
          Distance::Meters);
//      point->SetSigmaLongitude(t);

      t = dSigmaRadius + T(2, 2);
      t = sqrt(dSigma02 * t) * 1000.0;
      point->GetSurfacePoint().SetSphericalSigmasDistance(tLatSig, tLonSig,
          Distance(t, Distance::Meters));

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
          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration)
            abcX[2] += basis.Coefficient(index);
          index++;
        }

        // Update the Y coordinate coefficient(s)
        abcY[0] += basis.Coefficient(index);
        index++;
        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcY[1] += basis.Coefficient(index);
          index++;
          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration)
            abcY[2] += basis.Coefficient(index);
          index++;
        }

        // Update the Z coordinate cgoefficient(s)
        abcZ[0] += basis.Coefficient(index);
        index++;
        if (m_spacecraftPositionSolveType > PositionOnly) {
          abcZ[1] += basis.Coefficient(index);
          index++;
          if (m_spacecraftPositionSolveType == PositionVelocityAcceleration)
            abcZ[2] += basis.Coefficient(index);
          index++;
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
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++)
          coefRA[icoef] += basis.Coefficient(index);
        index++;

        // Update declination coefficient(s)
        for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++)
          coefDEC[icoef] += basis.Coefficient(index);
        index++;

        if (m_bSolveTwist) {
          // Update twist coefficient(s)
          for (int icoef = 0; icoef < m_nNumberCameraCoefSolved; icoef++)
            coefTWI[icoef] += basis.Coefficient(index);
          index++;
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

      double dLat = point->GetSurfacePoint().GetLatitude().GetDegrees();
      double dLon = point->GetSurfacePoint().GetLongitude().GetDegrees();
      double dRad = point->GetSurfacePoint().GetLocalRadius().GetMeters();

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

//    if ( m_bSolveRadii )
      dRad += 1000.*basis.Coefficient(index);
      index++;

      // testing
      // Compute ground point in body-fixed coordinates
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
      point->SetSurfacePoint(SurfacePoint(Latitude(dLat, Angle::Degrees),
                                          Longitude(dLon, Angle::Degrees),
                                          Distance(dRad, Distance::Meters)));
    }

  }

  //! Return index to basis function for ith point
  int BundleAdjust::PointIndex(int i) const {
    int nIndex;

    if (!m_bObservationMode)
      nIndex = (Images() - m_nHeldImages) * m_nNumImagePartials;
    else
      nIndex = Observations() * m_nNumImagePartials;

    nIndex += m_nPointIndexMap[i] * m_nNumPointPartials;

    return nIndex;
  }

  //! Return index to basis function for ith image
  int BundleAdjust::ImageIndex(int i) const {
    if (!m_bObservationMode)
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
      return m_pSnList->Size() - m_nHeldImages;
    else
      return m_pObsNumList->ObservationSize();
  }

//std::vector<double> BundleAdjust::PointPartial(Isis::ControlPoint &point, PartialDerivative wrt)
//{
//  double lat = point.GetSurfacePoint().GetLatitude();  // Radians
//  double lon = point.GetSurfacePoint().GetLongitude();
//  double sinLon = sin(lon);
//  double cosLon = cos(lon);
//  double sinLat = sin(lat);
//  double cosLat = cos(lat);
//  double radius = point.GetSurfacePoint().GetLocalRadius() * 0.001;  /km
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
    * the BundleAdjust summary.
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
    gp += PvlKeyword("SigmaHat", sigmaHat, "mm");
    gp += PvlKeyword("SigmaX", sigmaX, "mm");
    gp += PvlKeyword("SigmaY", sigmaY, "mm");

    Application::Log(gp);
  }

  /**
   * set parameter weighting
   */
  bool BundleAdjust::SetParameterWeights() {
    double dAprioriSigmaLat;
    double dAprioriSigmaLon;
    double dAprioriSigmaRad;
    double dVarianceLat;
    double dVarianceLon;
    double dVarianceRad;

    m_dParameterWeights.resize(m_nBasisColumns);
    std::fill_n(m_dParameterWeights.begin(), m_nBasisColumns, 0.0);

    // loop over 3D object points
    int nWtIndex = m_nImageParameters;
    int nObjectPoints = m_pCnet->GetNumPoints();
    m_nConstrainedPointParameters = 0;
    for (int i = 0; i < nObjectPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;
      /*
             if(  m_dGlobalLatitudeAprioriSigma > 0.0 )
               dAprioriSigmaLat = m_dGlobalLatitudeAprioriSigma;
             else
               dAprioriSigmaLat = point->AprioriSigmaLatitude();

             if(  m_dGlobalLatitudeAprioriSigma > 0.0 )
               dAprioriSigmaLon = m_dGlobalLongitudeAprioriSigma;
             else
               dAprioriSigmaLon = point->AprioriSigmaLongitude();

             if(  m_dGlobalRadiusAprioriSigma > 0.0 )
               dAprioriSigmaRad = m_dGlobalRadiusAprioriSigma;
             else
               dAprioriSigmaRad = point->AprioriSigmaRadius();
      */

      dAprioriSigmaLat = point->GetAprioriSurfacePoint().GetLatSigmaDistance().GetMeters();
      dAprioriSigmaLon = point->GetAprioriSurfacePoint().GetLonSigmaDistance().GetMeters();
      dAprioriSigmaRad = point->GetAprioriSurfacePoint().GetLocalRadiusSigma().GetMeters();

      if (dAprioriSigmaLat > 1000.0 || dAprioriSigmaLat <= 0.0) {
        if (m_dGlobalLatitudeAprioriSigma > 0.0)
          dAprioriSigmaLat = m_dGlobalLatitudeAprioriSigma;
      }

      if (dAprioriSigmaLon > 1000.0 || dAprioriSigmaLon <= 0.0) {
        if (m_dGlobalLongitudeAprioriSigma > 0.0)
          dAprioriSigmaLon = m_dGlobalLongitudeAprioriSigma;
      }

      if (dAprioriSigmaRad > 1000.0 || dAprioriSigmaRad <= 0.0) {
        if (m_dGlobalRadiusAprioriSigma > 0.0)
          dAprioriSigmaRad = m_dGlobalRadiusAprioriSigma;
      }

      dVarianceLat = dAprioriSigmaLat * m_dMTR;
      dVarianceLat *= dVarianceLat;
      dVarianceLon = dAprioriSigmaLon * m_dMTR * cos(point->GetSurfacePoint().GetLatitude().GetRadians()); // Lat in radians
      dVarianceLon *= dVarianceLon;
      dVarianceRad = dAprioriSigmaRad * 0.001;
      dVarianceRad *= dVarianceRad;

      if (point->GetType() == ControlPoint::Ground) {
        m_dParameterWeights[nWtIndex] = 1.0e+25;
        m_dParameterWeights[nWtIndex+1] = 1.0e+25;
        m_dParameterWeights[nWtIndex+2] = 1.0e+25;
        m_nConstrainedPointParameters += 3;
      }
      else {
        if (dAprioriSigmaLat < 1000.0) {
          m_dParameterWeights[nWtIndex] = 1.0 / dVarianceLat;
          m_nConstrainedPointParameters++;
        }
        if (dAprioriSigmaLon < 1000.0) {
          m_dParameterWeights[nWtIndex+1] = 1.0 / dVarianceLon;
          m_nConstrainedPointParameters++;
        }

        if (!m_bSolveRadii) {
          m_dParameterWeights[nWtIndex+2] = 1.0e+10;
          m_nConstrainedPointParameters++;
        }
        else if (dAprioriSigmaRad < 1000.0) {
          m_dParameterWeights[nWtIndex+2] = 1.0 / dVarianceRad;
          m_nConstrainedPointParameters++;
        }
      }

      nWtIndex += 3;
    }

    m_pLsq->SetParameterWeights(m_dParameterWeights);

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
      dSigmaLong *= m_dRTM / cos(point->GetSurfacePoint().GetLatitude().GetRadians()); // Lat in radians
      nIndex++;

      dSigmaRadius = sqrt((double)(lsqCovMatrix(nIndex, nIndex)));
      nIndex++;

      point->GetSurfacePoint().SetSphericalSigmasDistance(
          Distance(dSigmaLat, Distance::Meters),
          Distance(dSigmaLong, Distance::Meters),
          Distance(dSigmaRadius, Distance::Kilometers));
    }
  }

  /**
   * output bundle results to file
   */
  bool BundleAdjust::Output() {
    if (m_bConverged && m_bErrorPropagation)
      OutputWithErrorPropagation();
    else
      OutputNoErrorPropagation();

    if (m_bOutputCSV) {
      OutputPointsCSV();
      OutputImagesCSV();
    }

    return true;
  }

  /**
   * output bundle results to file with error propagation
   */
  bool BundleAdjust::OutputWithErrorPropagation() {
    char buf[1056];

    bool bHeld = false;
    std::vector<double> PosX(3);
    std::vector<double> PosY(3);
    std::vector<double> PosZ(3);
    std::vector<double> coefRA(m_nNumberCameraCoefSolved);
    std::vector<double> coefDEC(m_nNumberCameraCoefSolved);
    std::vector<double> coefTWI(m_nNumberCameraCoefSolved);
    Camera *pCamera = NULL;
    SpicePosition *pSpicePosition = NULL;
    SpiceRotation *pSpiceRotation = NULL;

    std::ofstream fp_out("bundleout.txt", std::ios::out);
    if (!fp_out)
      return false;

    int nImages = Images();
    int nValidPoints = m_pCnet->GetNumValidPoints();
    int nInnerConstraints = 0;
    int nDistanceConstraints = 0;
    int nDegreesOfFreedom = m_nObservations + m_nConstrainedPointParameters + m_nConstrainedImageParameters - m_nUnknownParameters;
    int nConvergenceCriteria = 1;

    sprintf(buf, "\n\t\tJIGSAW: BUNDLE ADJUSTMENT\n\t\t=========================\n");
    fp_out << buf;
    sprintf(buf, "\n               Network Filename: %s", p_cnetFile.c_str());
    fp_out << buf;
    sprintf(buf, "\n                     Network Id: %s", m_pCnet->GetNetworkId().c_str());
    fp_out << buf;
    sprintf(buf, "\n            Network Description: %s", m_pCnet->Description().c_str());
    fp_out << buf;
    sprintf(buf, "\n                         Target: %s", m_pCnet->GetTarget().c_str());
    fp_out << buf;
    sprintf(buf, "\n                   Linear Units: kilometers");
    fp_out << buf;
    sprintf(buf, "\n                  Angular Units: decimal degrees");
    fp_out << buf;
    sprintf(buf, "\n\n                         Images: %6d", nImages);
    fp_out << buf;
    sprintf(buf, "\n                         Points: %6d", nValidPoints);
    fp_out << buf;
    sprintf(buf, "\n                   Observations: %6d", m_nObservations);
    fp_out << buf;
    if (m_nConstrainedPointParameters > 0) {
      sprintf(buf, "\n   Constrained Point Parameters: %6d", m_nConstrainedPointParameters);
      fp_out << buf;
    }
    if (m_nConstrainedImageParameters > 0) {
      sprintf(buf, "\n   Constrained Image Parameters: %6d", m_nConstrainedImageParameters);
      fp_out << buf;
    }
    sprintf(buf, "\n                       Unknowns: %6d", m_nUnknownParameters);
    fp_out << buf;
    if (nInnerConstraints > 0) {
      sprintf(buf, "\n      Inner Constraints: %6d", nInnerConstraints);
      fp_out << buf;
    }
    if (nDistanceConstraints > 0) {
      sprintf(buf, "\n   Distance Constraints: %d", nDistanceConstraints);
      fp_out << buf;
    }
    sprintf(buf, "\n             Degrees of Freedom: %6d", nDegreesOfFreedom);
    fp_out << buf;
    sprintf(buf, "\n           Convergence Criteria: %6.3g", m_dConvergenceThreshold);
    fp_out << buf;
    if (nConvergenceCriteria == 1) {
      sprintf(buf, "(Sigma0)");
      fp_out << buf;
    }
    sprintf(buf, "\n                     Iterations: %6d", m_nIteration);
    fp_out << buf;
    if (m_nIteration >= m_nMaxIterations) {
      sprintf(buf, "(Maximum reached)");
      fp_out << buf;
    }
    sprintf(buf, "\n                         Sigma0: %6.4lf\n", m_dSigma0);
    fp_out << buf;

    sprintf(buf, " Error Propagation Elapsed Time: %6.4lf (seconds)\n", m_dElapsedTimeErrorProp);
    fp_out << buf;

    sprintf(buf, "             Total Elapsed Time: %6.4lf (seconds)\n", m_dElapsedTime);
    fp_out << buf;

//     sprintf(buf, "\n  Meters to Radians Conversion: %6.4g\n",m_dMTR);
//     fp_out << buf;
//     sprintf(buf, "\n  Radians to Meters Conversion: %6.4g\n",m_dRTM);
//     fp_out << buf;

    double dSigma;
    int nIndex = 0;

    sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION\n==========================\n");
    fp_out << buf;

    for (int i = 0; i < nImages; i++) {
      if (m_nHeldImages > 0 && m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))
        bHeld = true;

      pCamera = m_pCnet->Camera(i);
      if (!pCamera)
        continue;

      pSpicePosition = pCamera->InstrumentPosition();
      if (!pSpicePosition)
        continue;

      pSpiceRotation = pCamera->InstrumentRotation();
      if (!pSpiceRotation)
        continue;

      if (m_spacecraftPositionSolveType > 0)
        pSpicePosition->GetPolynomial(PosX, PosY, PosZ);
      else {
        pSpicePosition->SetPolynomial();
        pSpicePosition->GetPolynomial(PosX, PosY, PosZ);
      }

      if (m_cmatrixSolveType > 0)
        pSpiceRotation->GetPolynomial(coefRA, coefDEC, coefTWI);
      else {
        pSpiceRotation->SetPolynomial();
        pSpiceRotation->GetPolynomial(coefRA, coefDEC, coefTWI);
      }

      sprintf(buf, "\nImage Full File Name: %s\n", m_pSnList->Filename(i).c_str());
      fp_out << buf;
      sprintf(buf, "\nImage Serial Number: %s\n", m_pSnList->SerialNumber(i).c_str());
      fp_out << buf;
      sprintf(buf, "\n    Image         Initial              Total               Final             Initial           Final\n"
              "Parameter         Value              Correction            Value             Accuracy          Accuracy\n");
      fp_out << buf;

      if (m_spacecraftPositionSolveType == 0) {
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", PosX[0], 0.0, PosX[0], 0.0, "N/A");
        fp_out << buf;
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", PosY[0], 0.0, PosY[0], 0.0, "N/A");
        fp_out << buf;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", PosZ[0], 0.0, PosZ[0], 0.0, "N/A");
        fp_out << buf;
      }
      else if (m_spacecraftPositionSolveType == 1) {
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
      }
      else if (m_spacecraftPositionSolveType == 2) {
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Xv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosX[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Yv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosY[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Zv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosZ[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
      }
      else if (m_spacecraftPositionSolveType == 3) {
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        X%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", PosX[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Xv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosX[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Xa%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosX[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosX[2], m_dGlobalSpacecraftAccelerationAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        Y%17.8f%21.8f%20.8f%18.8lf%18.8lf\n",
                PosY[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Yv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosY[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Ya%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosY[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosY[2], m_dGlobalSpacecraftAccelerationAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "        Z%17.8f%21.8f%20.8f%18.8lf%18.8lf\n",
                PosZ[0] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[0], m_dGlobalSpacecraftPositionAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Zv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosZ[1] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[1], m_dGlobalSpacecraftVelocityAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       Za%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                PosZ[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[2], m_dGlobalSpacecraftAccelerationAprioriSigma, dSigma);
        fp_out << buf;
        nIndex++;
      }

      if (m_cmatrixSolveType == 0) {
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefRA[0]*RAD2DEG, 0.0, coefRA[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefDEC[0]*RAD2DEG, 0.0, coefDEC[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
        sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
      }
      else if (m_cmatrixSolveType == 1) {
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                (coefRA[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                (coefDEC[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;

        if (!m_bSolveTwist) {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
        }
        else {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  (coefTWI[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
          fp_out << buf;
          nIndex++;
        }
      }
      else if (m_cmatrixSolveType == 2) {
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                (coefRA[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "      RAv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                (coefRA[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                (coefDEC[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "     DECv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                (coefDEC[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;

        if (!m_bSolveTwist) {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
        }
        else {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  (coefTWI[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
          fp_out << buf;
          nIndex++;
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          sprintf(buf, "   TWISTv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  (coefTWI[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, dSigma * RAD2DEG);
          fp_out << buf;
          nIndex++;
        }
      }
      else if (m_cmatrixSolveType == 3 || m_cmatrixSolveType == 4) {
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", (coefRA[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "      RAv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", (coefRA[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "      RAa%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", (coefRA[2] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[2]*RAD2DEG, m_dGlobalCameraAngularAccelerationAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", (coefDEC[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "     DECv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", (coefDEC[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;
        dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
        sprintf(buf, "     DECa%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n", (coefDEC[2] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[2]*RAD2DEG, m_dGlobalCameraAngularAccelerationAprioriSigma, dSigma * RAD2DEG);
        fp_out << buf;
        nIndex++;

        if (!m_bSolveTwist) {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
        }
        else {
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  (coefTWI[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, dSigma * RAD2DEG);
          fp_out << buf;
          nIndex++;
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          sprintf(buf, "   TWISTv%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  (coefTWI[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, dSigma * RAD2DEG);
          fp_out << buf;
          nIndex++;
          dSigma = sqrt((double)(m_Normals(nIndex, nIndex))) * m_dSigma0;
          sprintf(buf, "   TWISTa%17.8lf%21.8lf%20.8lf%18.8lf%18.8lf\n",
                  (coefTWI[2] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[2]*RAD2DEG, m_dGlobalCameraAngularAccelerationAprioriSigma, dSigma * RAD2DEG);
          fp_out << buf;
          nIndex++;
        }
      }
    }

    fp_out << "\n\n\n";

    // output point data
    sprintf(buf, "\nPOINTS SUMMARY\n==============\n%91sSigma           Sigma             Sigma\n"
            "           Label   Status     Rays        Latitude       Longitude          Radius"
            "        Latitude       Longitude          Radius\n", "");
    fp_out << buf;

    int nRays = 0;
    double dLat, dLon, dRadius;
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    double cor_lat_dd, cor_lon_dd, cor_rad_m;
    double cor_lat_m, cor_lon_m;
    double dLatInit, dLonInit, dRadiusInit;
    int nGoodRays;

    std::string strStatus;
    int nPointIndex = 0;

    int nPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      nRays = point->GetNumMeasures();
      dLat = point->GetSurfacePoint().GetLatitude().GetRadians();
      dLon = point->GetSurfacePoint().GetLongitude().GetRadians();
      dRadius = point->GetSurfacePoint().GetLocalRadius().GetMeters();
      dSigmaLat = point->GetSurfacePoint().GetLatSigmaDistance().GetMeters();
      dSigmaLong = point->GetSurfacePoint().GetLonSigmaDistance().GetMeters();
      dSigmaRadius = point->GetSurfacePoint().GetLocalRadiusSigma().GetMeters();
      nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

//     Held point type is obsolete
//     if( point->Held() )
//       strStatus = "HELD";
//     else if( point->Type() == ControlPoint::Ground)
      if (point->GetType() == ControlPoint::Ground)
        strStatus = "GROUND";
      else if (point->GetType() == ControlPoint::Tie)
        strStatus = "TIE";
      else
        strStatus = "UNKNOWN";

      sprintf(buf, "%16s%9s%5d of %d%16.8lf%16.8lf%16.8lf%16.8lf%16.8lf%16.8lf\n",
              point->GetId().c_str(), strStatus.c_str(), nGoodRays, nRays, dLat, dLon, dRadius * 0.001, dSigmaLat, dSigmaLong, dSigmaRadius);

      fp_out << buf;
      nPointIndex++;
    }

    // output point data
    sprintf(buf, "\n\nPOINTS DETAIL\n=============\n\n");
    fp_out << buf;

    nPointIndex = 0;
    for (int i = 0; i < nPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);
      if (point->IsIgnored())
        continue;

      nRays = point->GetNumMeasures();
      dLat = point->GetSurfacePoint().GetLatitude().GetRadians();
      dLon = point->GetSurfacePoint().GetLongitude().GetRadians();
      dRadius = point->GetSurfacePoint().GetLocalRadius().GetMeters();
      dSigmaLat = point->GetSurfacePoint().GetLatSigmaDistance().GetMeters();
      dSigmaLong = point->GetSurfacePoint().GetLonSigmaDistance().GetMeters();
      dSigmaRadius = point->GetSurfacePoint().GetLocalRadiusSigma().GetMeters();
      nGoodRays = nRays - point->GetNumberOfRejectedMeasures();

      // point corrections and initial sigmas
      bounded_vector<double, 3>& corrections = m_Point_Corrections[nPointIndex];
      bounded_vector<double, 3>& apriorisigmas = m_Point_AprioriSigmas[nPointIndex];

      cor_lat_dd = corrections[0] * Isis::RAD2DEG;
      cor_lon_dd = corrections[1] * Isis::RAD2DEG;
      cor_rad_m  = corrections[2] * 1000.0;

      cor_lat_m = corrections[0] * m_dRTM;
      cor_lon_m = corrections[1] * m_dRTM / cos(dLat);

      dLatInit = dLat - cor_lat_dd;
      dLonInit = dLon - cor_lon_dd;
      dRadiusInit = dRadius - (corrections[2] * 1000.0);

// Held method is obsolete
//     if( point->Held() )
//       strStatus = "HELD";
//     else if( point->Type() == ControlPoint::Ground)
      if (point->GetType() == ControlPoint::Ground)
        strStatus = "GROUND";
      else if (point->GetType() == ControlPoint::Tie)
        strStatus = "TIE";
      else
        strStatus = "UNKNOWN";

      sprintf(buf, " Label: %s\nStatus: %s\n  Rays: %d of %d\n", point->GetId().c_str(), strStatus.c_str(), nGoodRays, nRays);
      fp_out << buf;

//       sprintf(buf,"%16s%9s%9d\n",
//               point->Id().c_str(),strStatus.c_str(),nRays);
//       fp_out << buf;

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
              dRadiusInit * 0.001, corrections[2], cor_rad_m, dRadius * 0.001, apriorisigmas[2], dSigmaRadius);

      fp_out << buf;

      nPointIndex++;
    }

    fp_out.close();

    return true;
  }

  /**
   * output bundle results to file with no error propagation
   */
  bool BundleAdjust::OutputNoErrorPropagation() {
    char buf[1056];

    bool bHeld = false;
    std::vector<double> PosX(3);
    std::vector<double> PosY(3);
    std::vector<double> PosZ(3);
    std::vector<double> coefRA(m_nNumberCameraCoefSolved);
    std::vector<double> coefDEC(m_nNumberCameraCoefSolved);
    std::vector<double> coefTWI(m_nNumberCameraCoefSolved);
    Camera *pCamera = NULL;
    SpicePosition *pSpicePosition = NULL;
    SpiceRotation *pSpiceRotation = NULL;

    std::ofstream fp_out("bundleout.txt", std::ios::out);
    if (!fp_out)
      return false;

    int nImages = Images();
    int nValidPoints = m_pCnet->GetNumValidPoints();
    int nInnerConstraints = 0;
    int nDistanceConstraints = 0;
    int nDegreesOfFreedom = m_nObservations + m_nConstrainedPointParameters + m_nConstrainedImageParameters - m_nUnknownParameters;
    int nConvergenceCriteria = 1;

    sprintf(buf, "\n\t\tJIGSAW: BUNDLE ADJUSTMENT\n\t\t=========================\n");
    fp_out << buf;
    sprintf(buf, "\n               Network Filename: %s", p_cnetFile.c_str());
    fp_out << buf;
    sprintf(buf, "\n                     Network Id: %s", m_pCnet->GetNetworkId().c_str());
    fp_out << buf;
    sprintf(buf, "\n            Network Description: %s", m_pCnet->Description().c_str());
    fp_out << buf;
    sprintf(buf, "\n                         Target: %s", m_pCnet->GetTarget().c_str());
    fp_out << buf;
    sprintf(buf, "\n                   Linear Units: kilometers");
    fp_out << buf;
    sprintf(buf, "\n                  Angular Units: decimal degrees");
    fp_out << buf;
    sprintf(buf, "\n\n                         Images: %6d", nImages);
    fp_out << buf;
    sprintf(buf, "\n                         Points: %6d", nValidPoints);
    fp_out << buf;
    sprintf(buf, "\n                   Observations: %6d", m_nObservations);
    fp_out << buf;
    sprintf(buf, "\n                       Unknowns: %6d", m_nUnknownParameters);
    fp_out << buf;
    if (m_nConstrainedPointParameters > 0) {
      sprintf(buf, "\n   Constrained Point Parameters: %6d", m_nConstrainedPointParameters);
      fp_out << buf;
    }
    if (m_nConstrainedImageParameters > 0) {
      sprintf(buf, "\n   Constrained Image Parameters: %6d", m_nConstrainedImageParameters);
      fp_out << buf;
    }
    if (nInnerConstraints > 0) {
      sprintf(buf, "\n      Inner Constraints: %6d", nInnerConstraints);
      fp_out << buf;
    }
    if (nDistanceConstraints > 0) {
      sprintf(buf, "\n   Distance Constraints: %d", nDistanceConstraints);
      fp_out << buf;
    }
    sprintf(buf, "\n             Degrees of Freedom: %6d", nDegreesOfFreedom);
    fp_out << buf;
    sprintf(buf, "\n           Convergence Criteria: %6.3g", m_dConvergenceThreshold);
    fp_out << buf;
    if (nConvergenceCriteria == 1) {
      sprintf(buf, "(Sigma0)");
      fp_out << buf;
    }
    sprintf(buf, "\n                     Iterations: %6d", m_nIteration);
    fp_out << buf;
    if (m_nIteration >= m_nMaxIterations) {
      sprintf(buf, "(Maximum reached)");
      fp_out << buf;
    }
    sprintf(buf, "\n                         Sigma0: %6.4lf\n", m_dSigma0);
    fp_out << buf;
    sprintf(buf, "                   Elapsed Time: %6.4lf (seconds)\n", m_dElapsedTime);
    fp_out << buf;

    sprintf(buf, "\n  Meters to Radians Conversion: %6.4g\n", m_dMTR);
    fp_out << buf;
    sprintf(buf, "\n  Radians to Meters Conversion: %6.4g\n", m_dRTM);
    fp_out << buf;

    int nIndex = 0;

    sprintf(buf, "\nIMAGE EXTERIOR ORIENTATION ***J2000***\n==========================\n");
    fp_out << buf;

    for (int i = 0; i < nImages; i++) {
      if (m_nHeldImages > 0 && m_pHeldSnList->HasSerialNumber(m_pSnList->SerialNumber(i)))
        bHeld = true;

      pCamera = m_pCnet->Camera(i);
      if (!pCamera)
        continue;

      pSpicePosition = pCamera->InstrumentPosition();
      if (!pSpicePosition)
        continue;

      pSpiceRotation = pCamera->InstrumentRotation();
      if (!pSpiceRotation)
        continue;

//       if( m_spacecraftPositionSolveType > 0 )
      pSpicePosition->GetPolynomial(PosX, PosY, PosZ);
//       else
//         PosX = pSpicePosition->Coordinate();

//       if( m_cmatrixSolveType > 0 )
      pSpiceRotation->GetPolynomial(coefRA, coefDEC, coefTWI);
//       else
//         coefRA = pSpiceRotation->Angles();

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
        sprintf(buf, "        Y%17.8lf%21.8lf%20.8lf%18s%18s\n", PosY[0], 0.0, PosX[1], "N/A", "N/A");
        fp_out << buf;
        sprintf(buf, "        Z%17.8lf%21.8lf%20.8lf%18s%18s\n", PosZ[0], 0.0, PosX[2], "N/A", "N/A");
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
        sprintf(buf, "       Y1%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
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
        sprintf(buf, "       Z1%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                PosZ[2] - m_Image_Corrections(nIndex), m_Image_Corrections(nIndex), PosZ[2], m_dGlobalSpacecraftAccelerationAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
      }

      if (m_cmatrixSolveType == 0) {
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefRA[0]*RAD2DEG, 0.0, coefRA[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefDEC[0]*RAD2DEG, 0.0, coefDEC[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
        sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
        fp_out << buf;
      }
      else if (m_cmatrixSolveType == 1) {
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefRA[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefDEC[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;

        if (!m_bSolveTwist) {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
        }
        else {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  (coefTWI[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
          fp_out << buf;
          nIndex++;
        }
      }
      else if (m_cmatrixSolveType == 2) {
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefRA[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "      RAv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefRA[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefDEC[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "     DECv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefDEC[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;

        if (!m_bSolveTwist) {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
        }
        else {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  (coefTWI[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
          fp_out << buf;
          nIndex++;
          sprintf(buf, "   TWISTv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  (coefTWI[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, "N/A");
          fp_out << buf;
          nIndex++;
        }
      }
      else if (m_cmatrixSolveType == 3 || m_cmatrixSolveType == 4) {
        sprintf(buf, "       RA%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefRA[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "      RAv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefRA[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "      RAa%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefRA[2] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefRA[2]*RAD2DEG, m_dGlobalCameraAngularAccelerationAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "      DEC%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefDEC[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "     DECv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefDEC[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;
        sprintf(buf, "     DECa%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                (coefDEC[2] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefDEC[2]*RAD2DEG, m_dGlobalCameraAngularAccelerationAprioriSigma, "N/A");
        fp_out << buf;
        nIndex++;

        if (!m_bSolveTwist) {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n", coefTWI[0]*RAD2DEG, 0.0, coefTWI[0]*RAD2DEG, 0.0, "N/A");
          fp_out << buf;
        }
        else {
          sprintf(buf, "    TWIST%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  (coefTWI[0] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[0]*RAD2DEG, m_dGlobalCameraAnglesAprioriSigma, "N/A");
          fp_out << buf;
          nIndex++;
          sprintf(buf, "   TWISTv%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  (coefTWI[1] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[1]*RAD2DEG, m_dGlobalCameraAngularVelocityAprioriSigma, "N/A");
          fp_out << buf;
          nIndex++;
          sprintf(buf, "   TWISTa%17.8lf%21.8lf%20.8lf%18.8lf%18s\n",
                  (coefTWI[2] - m_Image_Corrections(nIndex))*RAD2DEG, m_Image_Corrections(nIndex)*RAD2DEG, coefTWI[2]*RAD2DEG, m_dGlobalCameraAngularAccelerationAprioriSigma, "N/A");
          fp_out << buf;
          nIndex++;
        }
      }
    }

    fp_out << "\n\n\n";

    // output point data
    sprintf(buf, "\nPOINTS\n======\n%91sSigma           Sigma             Sigma\n"
            "           Label   Status     Rays        Latitude       Longitude          Radius"
            "        Latitude       Longitude          Radius\n", "");
    fp_out << buf;

    int nRays = 0;
    double dLat, dLon, dRadius;
    std::string strStatus;

    int nPoints = m_pCnet->GetNumPoints();
    for (int i = 0; i < nPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if (point->IsIgnored())
        continue;

      nRays = point->GetNumMeasures();
      dLat = point->GetSurfacePoint().GetLatitude().GetRadians();
      dLon = point->GetSurfacePoint().GetLongitude().GetRadians();
      dRadius = point->GetSurfacePoint().GetLocalRadius().GetMeters();

//     if( point->Held() )
//       strStatus = "HELD";
//     else if( point->Type() == ControlPoint::Ground)
      if (point->GetType() == ControlPoint::Ground)
        strStatus = "GROUND";
      else if (point->GetType() == ControlPoint::Tie)
        strStatus = "TIE";
      else
        strStatus = "UNKNOWN";

      sprintf(buf, "%16s%9s%9d%16.8lf%16.8lf%16.8lf%16s%16s%16s\n",
              point->GetId().c_str(), strStatus.c_str(), nRays, dLat, dLon, dRadius * 0.001, "N/A", "N/A", "N/A");

      fp_out << buf;
    }

    fp_out.close();

    return true;
  }

  /**
   * output point data to csv file
   */
  bool BundleAdjust::OutputPointsCSV() {
    char buf[1056];

    std::ofstream fp_out("bundleout_points.csv", std::ios::out);
    if (!fp_out)
      return false;

    int nPoints = m_pCnet->GetNumPoints();

    double dLat, dLon, dRadius;
    double dSigmaLat, dSigmaLong, dSigmaRadius;
    std::string strStatus;

    for (int i = 0; i < nPoints; i++) {
      const ControlPoint *point = m_pCnet->GetPoint(i);

      if (point->IsIgnored())
        continue;

      dLat = point->GetSurfacePoint().GetLatitude().GetRadians();
      dLon = point->GetSurfacePoint().GetLongitude().GetRadians();
      dRadius = point->GetSurfacePoint().GetLocalRadius().GetMeters();

//     if( point->Held() )
//       strStatus = "HELD";
//     else if( point->Type() == ControlPoint::Ground)
      if (point->GetType() == ControlPoint::Ground)
        strStatus = "GROUND";
      else if (point->GetType() == ControlPoint::Tie)
        strStatus = "TIE";
      else
        strStatus = "UNKNOWN";

      if (m_bErrorPropagation) {
        dSigmaLat = point->GetSurfacePoint().GetLatSigmaDistance().GetMeters();
        dSigmaLong = point->GetSurfacePoint().GetLonSigmaDistance().GetMeters();
        dSigmaRadius = point->GetSurfacePoint().GetLocalRadiusSigma().GetMeters();

        sprintf(buf, "%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                point->GetId().c_str(), strStatus.c_str(), dLat, dLon, dRadius * 0.001,
                dSigmaLat, dSigmaLong, dSigmaRadius);
      }
      else
        sprintf(buf, "%s,%16.8lf,%16.8lf,%16.8lf\n", point->GetId().c_str(), dLat, dLon, dRadius * 0.001);

      fp_out << buf;
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

    std::ofstream fp_out("residuals.csv", std::ios::out);
    if (!fp_out)
      return false;

    // output column headers
    sprintf(buf, "Point,Image,Image,x image,y image,sample,line,Residual Vector\n");
    fp_out << buf;
    sprintf(buf, "Label,Filename,Serial Number,coordinate,coordinate,residual,residual,Magnitude,Rejected\n");
    fp_out << buf;

    int nImageIndex;

    printf("output residuals!!!\n");

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
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf,*\n",
                  point->GetId().c_str(), m_pSnList->Filename(nImageIndex).c_str(), m_pSnList->SerialNumber(nImageIndex).c_str(),
                  measure->GetFocalPlaneMeasuredX(), measure->GetFocalPlaneMeasuredY(), measure->GetSampleResidual(), measure->GetLineResidual(), measure->GetResidualMagnitude());
        else
          sprintf(buf, "%s,%s,%s,%16.8lf,%16.8lf,%16.8lf,%16.8lf,%16.8lf\n",
                  point->GetId().c_str(), m_pSnList->Filename(nImageIndex).c_str(), m_pSnList->SerialNumber(nImageIndex).c_str(),
                  measure->GetFocalPlaneMeasuredX(), measure->GetFocalPlaneMeasuredY(), measure->GetSampleResidual(), measure->GetLineResidual(), measure->GetResidualMagnitude());
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

    std::ofstream fp_out("bundleout_images.csv", std::ios::out);
    if (!fp_out)
      return false;

    Camera *pCamera = NULL;
    SpicePosition *pSpicePosition = NULL;
    SpiceRotation *pSpiceRotation = NULL;

    std::vector<double> BFP(3);
    std::vector<double> PosX(3);
    std::vector<double> PosY(3);
    std::vector<double> PosZ(3);
    std::vector<double> coefRA(m_nNumberCameraCoefSolved);
    std::vector<double> coefDEC(m_nNumberCameraCoefSolved);
    std::vector<double> coefTWI(m_nNumberCameraCoefSolved);

    int nImages = Images();
    for (int i = 0; i < nImages; i++) {
      pCamera = m_pCnet->Camera(i);
      if (!pCamera)
        continue;

      pSpicePosition = pCamera->InstrumentPosition();
      if (!pSpicePosition)
        continue;

      pSpiceRotation = pCamera->InstrumentRotation();
      if (!pSpiceRotation)
        continue;

      // "InstumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
      // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
      BFP = pCamera->BodyRotation()->ReferenceVector(pSpicePosition->Coordinate());

//       if( m_spacecraftPositionSolveType > 0 )
//         pSpicePosition->GetPolynomial(PosX,PosY,PosZ);
//       else
//         PosX = pSpicePosition->Coordinate();

      pSpiceRotation->GetPolynomial(coefRA, coefDEC, coefTWI);

//       if( m_spacecraftPositionSolveType == 0 )
      sprintf(buf, "%s,%lf,%lf,%lf,%lf,%lf,%lf\n", m_pSnList->Filename(i).c_str(),
              BFP[0], BFP[1], BFP[2], coefRA[0]*RAD2DEG, coefDEC[0]*RAD2DEG, coefTWI[0]*RAD2DEG);
//       else
//         sprintf(buf,"%s,%lf,%lf,%lf,%lf,%lf,%lf\n", m_pSnList->Filename(i).c_str(),
//                 PosX[0],PosY[0],PosZ[0],coefRA[0]*RAD2DEG,coefDEC[0]*RAD2DEG,coefTWI[0]*RAD2DEG);

      fp_out << buf;
    }

    fp_out.close();

    return true;
  }
}

