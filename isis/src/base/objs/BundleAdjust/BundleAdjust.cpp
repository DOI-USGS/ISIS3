#include "BundleAdjust.h"

#include <iomanip>

#include "SpecialPixel.h"
#include "BasisFunction.h"
#include "LeastSquares.h"
#include "CameraGroundMap.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "ControlPoint.h"
#include "SpicePosition.h"
#include "Application.h"

namespace Isis {
  BundleAdjust::BundleAdjust(const std::string &cnetFile,
                             const std::string &cubeList,
                             bool printSummary) {
    // Get control net and serial number list
    p_cleanUp = true;
    Progress progress;
    p_cnet = new Isis::ControlNet(cnetFile, &progress);
    p_snlist = new Isis::SerialNumberList(cubeList);
    p_printSummary = printSummary;
    p_heldsnlist = NULL;
    p_observationMode = false;
    p_solutionMethod = "SVD";
    p_onlist = NULL;

    Init(&progress);
  }

  BundleAdjust::BundleAdjust(const std::string &cnetFile,
                             const std::string &cubeList,
                             const std::string &heldList,
                             bool printSummary) {
    // Get control net, serial number list, and held serial number list
    p_cleanUp = true;
    Progress progress;
    p_cnet = new Isis::ControlNet(cnetFile, &progress);
    p_snlist = new Isis::SerialNumberList(cubeList);
    p_heldsnlist = new Isis::SerialNumberList(heldList);
    p_printSummary = printSummary;
    p_observationMode = false;
    p_solutionMethod = "SVD";
    p_onlist = NULL;

    Init(&progress);
  }

  BundleAdjust::BundleAdjust(Isis::ControlNet &cnet,
                             Isis::SerialNumberList &snlist,
                             bool printSummary) {
    // Get control net and serial number list
    p_cleanUp = false;
    p_cnet = &cnet;
    p_snlist = &snlist;
    p_printSummary = printSummary;
    p_heldsnlist = NULL;
    p_observationMode = false;
    p_solutionMethod = "SVD";
    p_onlist = NULL;

    Init();
  }

  BundleAdjust::BundleAdjust(Isis::ControlNet &cnet,
                             Isis::SerialNumberList &snlist,
                             Isis::SerialNumberList &heldsnlist,
                             bool printSummary) {
    // Get control net, image serial number list and hold image serial number list
    p_cleanUp = false;
    p_cnet = &cnet;
    p_snlist = &snlist;
    p_heldsnlist = &heldsnlist;
    p_printSummary = printSummary;
    p_observationMode = false;
    p_solutionMethod = "SVD";
    p_onlist = NULL;

    Init();
  }

  BundleAdjust::~BundleAdjust() {
    if(p_cleanUp) {
      delete p_cnet;
      delete p_snlist;
      if(p_heldImages > 0) delete p_heldsnlist;
      if(p_observationMode) delete p_onlist;
    }
  }


  void BundleAdjust::Init(Progress *progress) {
    // Get the cameras set up for all images
    p_cnet->SetImages(*p_snlist, progress);

    p_heldImages = 0;
    int count;

    if(p_heldsnlist != NULL) {
      //Check to make sure held images are in the control net
      CheckHeldList();
      // Set all points on held images to held, using measurement on held image
      // to get lat/lon/radius of point
      ApplyHeldList();

      // Create a lookup table of held images
      count = 0;
      for(int i = 0; i < p_snlist->Size(); i++) {
        if(p_heldsnlist->HasSerialNumber(p_snlist->SerialNumber(i))) {
          p_imageIndexMap.push_back(-1);
          p_heldImages++;
        }
        else {
          p_imageIndexMap.push_back(count);
          count++;
        }
      }
    }
    else {
      for(int i = 0; i < p_snlist->Size(); i++) p_imageIndexMap.push_back(i);
    }


    // Create a lookup table of ignored, held, and ground points
    p_heldPoints = p_groundPoints = p_ignoredPoints = 0;
    count = 0;
    for(int i = 0; i < p_cnet->Size(); i++) {
      if((*p_cnet)[i].Held()) {
        p_pointIndexMap.push_back(-1);
        p_heldPoints++;
      }
      else if((*p_cnet)[i].Ignore()) {
        p_pointIndexMap.push_back(-1);
        p_ignoredPoints++;
      }
      else if((*p_cnet)[i].Type() == ControlPoint::Ground) {
        p_pointIndexMap.push_back(-1);
        p_groundPoints++;
      }
      else {
        p_pointIndexMap.push_back(count);
        count++;
      }
    }

    // Set default variables to solve for
    p_solveTwist = true;
    p_solveRadii = false;
    p_cmatrixSolveType = AnglesOnly;
    p_spacecraftPositionSolveType = Nothing;
    p_ckDegree = 2;
    p_solveCamDegree = p_ckDegree;
    p_numberCameraCoefSolved = 1;

    ComputeNumberPartials();

    // TODO:  Need to have some validation code to make sure everything is
    // on the up-and-up with the control network.  Add checks for multiple
    // networks, images without any points, and points on images removed from
    // the control net (when we start adding software to remove points with high
    // residuals) and ?.
  }

  /**
   * This method finds all the measurements on held images and holds the control
   * point they are on.  The lat/lon/radius are determined by mapping the line/sample
   * measurement on the held image to the surface.
   */
  void BundleAdjust::CheckHeldList() {
    for(int ih = 0; ih < p_heldsnlist->Size(); ih++) {
      if(!(p_snlist->HasSerialNumber(p_heldsnlist->SerialNumber(ih)))) {
        std::string msg = "Held image [" + p_heldsnlist->SerialNumber(ih) +
            " not in FROMLIST";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }


  /**
   * This method finds all the measurements on held images, finds their
   * lat/lon/radius(m), sets the control point they are on to that lat/lon/radius,
   * and makes the control point a held point.
   */
  void BundleAdjust::ApplyHeldList() {
    double lat, lon, rad;
    // TODO  Check for points already held ie) case where user has a point on 2
    // or more held images

    for(int i = 0; i < p_cnet->Size(); i++) {
      ControlPoint &pt = (*p_cnet)[i];
      if(pt.Ignore()) continue;

      for(int j = 0; j < pt.Size(); j++) {
        ControlMeasure &m = pt[j];
        if(m.Ignore()) continue;

        if(p_heldsnlist->HasSerialNumber(m.CubeSerialNumber())) {
          Camera *cam = m.Camera();
          if(cam->SetImage(m.Sample(), m.Line())) {
            lat = cam->UniversalLatitude();
            lon = cam->UniversalLongitude();
            rad = cam->LocalRadius(); //meters
          }
          else {
            QString msg = "Cannot compute lat/lon for control point [" +
                pt.Id() + "], measure [" + m.CubeSerialNumber() + "]";
            throw iException::Message(iException::User, msg.toStdString(),
                _FILEINFO_);
          }
          pt.SetUniversalGround(lat, lon, rad);
          pt.SetHeld(true);
        }
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
    p_numImagePartials = 0;

    if(p_cmatrixSolveType != None) {
      // Solve for ra/dec always
      p_numImagePartials = 2;

      // Do we solve for twist
      if(p_solveTwist) {
        p_numImagePartials++;
      }

      // Do we solve for angles only, +velocity, or +velocity and acceleration, or all coefficients
      p_numImagePartials *= p_numberCameraCoefSolved;
      /*      if (p_cmatrixSolveType == AnglesVelocity) {
              p_numImagePartials *= 2;
            }
            else if (p_cmatrixSolveType == AnglesVelocityAcceleration) {
              p_numImagePartials *= 3;
            }*/
    }

    if(p_spacecraftPositionSolveType != Nothing) {
      // Solve for position always.
      p_numImagePartials += 3;

      // Do we solve for position and velocity, position, velocity and acceleration, or position only
      if(p_spacecraftPositionSolveType == PositionVelocity) {
        p_numImagePartials += 3;
      }
      else if(p_spacecraftPositionSolveType == PositionVelocityAcceleration) {
        p_numImagePartials += 6;
      }
    }

    // Solve for lat/lon always
    p_numPointPartials = 2;

    // Do we solve for radii
    if(p_solveRadii) {
      p_numPointPartials++;
    }
  }

  /**
   * This method turns on observation mode and creates the observation number list.
   * It also checks to make sure the held image list is consistent for all images in
   * an observation
   */
  void BundleAdjust::SetObservationMode(bool observationMode) {
    p_observationMode = observationMode;

    if(p_observationMode) {
      // Create the observation number list
      p_onlist = new Isis::ObservationNumberList(p_snlist);
      if(p_heldImages != 0) {
        p_onlist->Remove(p_heldsnlist);
      }

      if(p_heldsnlist != NULL) {
        //Make sure ALL images in an observation are held if any are
        for(int ih = 0; ih < p_heldsnlist->Size(); ih++) {
          for(int isn = 0; isn < p_snlist->Size(); isn++) {
            if(p_heldsnlist->ObservationNumber(ih) == p_snlist->ObservationNumber(isn)) {
              if(!(p_heldsnlist->HasSerialNumber(p_snlist->SerialNumber(isn)))) {
                std::string msg = "Cube file " + p_snlist->Filename(isn)
                                  + " must be held since it is on the same observation as held cube "
                                  + p_heldsnlist->Filename(ih);
                throw iException::Message(iException::User, msg, _FILEINFO_);
              }
            }
          }
        }
      }
    }
  }




  /**
   * Should we solve for the twist in each image?
   */
  void BundleAdjust::SetSolveTwist(bool solve) {
    p_solveTwist = solve;
    ComputeNumberPartials();
  }

  /**
   * Should we solve for the radii at each point?
   */
  void BundleAdjust::SetSolveRadii(bool solve) {
    p_solveRadii = solve;
    ComputeNumberPartials();
  }

  /**
   * For which camera angle coefficients do we solve?
   */
  void BundleAdjust::SetSolveCmatrix(CmatrixSolveType type) {
    p_cmatrixSolveType = type;

    switch(type) {
      case BundleAdjust::AnglesOnly:
        p_numberCameraCoefSolved = 1;
        break;
      case BundleAdjust::AnglesVelocity:
        p_numberCameraCoefSolved = 2;
        break;
      case BundleAdjust::AnglesVelocityAcceleration:
        p_numberCameraCoefSolved = 3;
        break;
      case BundleAdjust::All:
        p_numberCameraCoefSolved = p_solveCamDegree + 1;
        break;
      default:
        p_numberCameraCoefSolved = 0;
        break;
    }

    // Make sure the degree of the polynomial the user selected for
    // the camera angles fit is sufficient for the selected CAMSOLVE
    if(p_numberCameraCoefSolved > p_solveCamDegree + 1) {
      std::string msg = "Selected SolveCameraDegree " + iString(p_solveCamDegree)
                        + " is not sufficient for the CAMSOLVE";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    ComputeNumberPartials();
  }

  /**
   * Should we solve for the spacecraft position, velocity, and acceleration
   */
  void BundleAdjust::SetSolveSpacecraftPosition(SpacecraftPositionSolveType type) {
    p_spacecraftPositionSolveType = type;
    ComputeNumberPartials();
  }

  /**
   * Determine the number of columns we will need for the least
   * squares. When we create a row of data we will store all the
   * image partials first and then the point partials
   */
  int BundleAdjust::BasisColumns() const {
    int imageColumns = Observations() * p_numImagePartials;

//    imageColumns -= p_heldImages * p_numImagePartials;

    int pointColumns = p_cnet->Size() * p_numPointPartials;

    pointColumns -= p_groundPoints * p_numPointPartials;
    pointColumns -= p_heldPoints * p_numPointPartials;
    pointColumns -= p_ignoredPoints * p_numPointPartials;

    return imageColumns + pointColumns;
  }

  /**
   * The solve method is an least squares solution for updating the camera
   * pointing.  It is iterative as the equations are non-linear.  If it does
   * not iterate to a solution in maxIterations it will throw an error.  During
   * each iteration it is updating portions of the control net, as well as the
   * instrument pointing in the camera.  An error is thrown if it does not
   * converge in the maximum iterations.  However, even if an error is thrown
   * the control network will contain the errors at each control measure.
   *
   * @param tol             Maximum pixel error for any control network
   *                        measurement
   * @param maxIterations   Maximum iterations, if tolerance is never
   *                        met an iException will be thrown.
   */
  double BundleAdjust::Solve(double tol, int maxIterations) {
    double mmPerPixel = DBL_MAX;
    double averageError;
    std::vector<int> observationInitialValueIndex;  // Index of image to use for observation inital values
    int iIndex = -1; //Index of the image to use for initial spice for an observation
    int oIndex = -1;      // Index of current observation

    if(p_observationMode) {
      observationInitialValueIndex.assign(p_onlist->ObservationSize(), -1);
    }

    for(int i = 0; i < Images(); i++) {
      if(p_heldImages > 0) {
        if((p_heldsnlist->HasSerialNumber(p_snlist->SerialNumber(i)))) continue;
      }
      Camera *cam = p_cnet->Camera(i);

      if(p_observationMode) {
        oIndex = i;
        oIndex = p_onlist->ObservationNumberMapIndex(oIndex);  // Get the observation index for this image
        iIndex = observationInitialValueIndex[oIndex]; // Get the index of the image to use for initial values
        // being used for the observation
      }
      if(p_cmatrixSolveType != None) {
        // For observations, find the index of the first image and use its polynomial for the observation
        // initial coefficient values.  Initialize indeces to -1

        // Fit the camera pointing to an equation
        SpiceRotation *rot = cam->InstrumentRotation();

        if(!p_observationMode) {
          rot->SetPolynomialDegree(p_ckDegree);   // Set the ck polynomial fit degree
          rot->SetPolynomial();
          rot->SetPolynomialDegree(p_solveCamDegree);   // Update to the solve polynomial fit degree
        }
        else {
          // Index of image to use for initial values is set already so set polynomial to initial values
          if(iIndex >= 0) {
            SpiceRotation *orot = p_cnet->Camera(iIndex)->InstrumentRotation(); //Observation rotation
            std::vector<double> anglePoly1, anglePoly2, anglePoly3;
            orot->GetPolynomial(anglePoly1, anglePoly2, anglePoly3);
            double baseTime = orot->GetBaseTime();
            double timeScale = orot->GetTimeScale();
            rot->SetPolynomialDegree(p_solveCamDegree);   // Update to the solve polynomial fit degree
            rot->SetOverrideBaseTime(baseTime, timeScale);
            rot->SetPolynomial(anglePoly1, anglePoly2, anglePoly3);
          }
          else {
            // Index of image to use for inital observation values has not been assigned yet so use this image
            rot->SetPolynomialDegree(p_ckDegree);
            rot->SetPolynomial();
            rot->SetPolynomialDegree(p_solveCamDegree);   // Update to the solve polynomial fit degree
            observationInitialValueIndex[oIndex] = i;
          }
        }
      }
      if(p_spacecraftPositionSolveType != Nothing) {
        // Set the spacecraft position to an equation
        SpicePosition *pos = cam->InstrumentPosition();

        if(!p_observationMode) {
          pos->SetPolynomial();
        }
        else {
          // Index of image to use for initial values is set already so set polynomial to initial values
          if(iIndex >= 0) {
            SpicePosition *opos = p_cnet->Camera(iIndex)->InstrumentPosition(); //Observation position
            std::vector<double> posPoly1, posPoly2, posPoly3;
            opos->GetPolynomial(posPoly1, posPoly2, posPoly3);
            double baseTime = opos->GetBaseTime();
            pos->SetOverrideBaseTime(baseTime);
            pos->SetPolynomial(posPoly1, posPoly2, posPoly3);
          }
          else {
            // Index of image to use for inital observation values has not been assigned yet so use this image
            pos->SetPolynomial();
            observationInitialValueIndex[oIndex] = i;
          }
        }
      }
      if(cam->PixelPitch() < mmPerPixel) {
        mmPerPixel = cam->PixelPitch();
      }
    }

    // Compute the apriori lat/lons for each nonheld point
    p_error = DBL_MAX;
    p_cnet->ComputeApriori();

    // Initialize solution parameters
    double sigmaXY, sigmaHat, sigmaX, sigmaY;
    sigmaXY = sigmaHat = sigmaX = sigmaY = 0.;
    p_iteration = 0;

    while(p_iteration < maxIterations) {
      p_iteration++;
      p_cnet->ComputeErrors();
      p_error = p_cnet->MaximumError();
      averageError = p_cnet->AverageError();
      if(p_printSummary) {
        IterationSummary(averageError, sigmaXY, sigmaHat, sigmaX, sigmaY);
      }
      p_statx.Reset();
      p_staty.Reset();

      if(p_error <= tol) return p_error;

      // Create the basis function and prep for a least squares solution
      BasisFunction basis("Bundle", BasisColumns(), BasisColumns());
      LeastSquares *lsq;
      if(p_solutionMethod == "SPARSE") {
        lsq = new LeastSquares(basis, Isis::LeastSquares::SPARSE,
                               p_cnet->NumValidMeasures() * 2, BasisColumns());
      }
      else {
        lsq = new LeastSquares(basis);
      }

      // Loop through the control net and add the partials for each point
      for(int i = 0; i < p_cnet->Size(); i++) {
        AddPartials(*lsq, i);
      }
      // Try to solve the iteration
      try {
        if(p_solutionMethod == "SVD") {
          lsq->Solve(Isis::LeastSquares::SVD);

        }
        else if(p_solutionMethod == "QRD") {
          lsq->Solve(Isis::LeastSquares::QRD);
        }
        else {
          int zeroColumn = lsq->Solve(Isis::LeastSquares::SPARSE);
          if(zeroColumn != 0) {
            std::string msg;
            int imageColumns = Observations() * p_numImagePartials;
            if(zeroColumn <= imageColumns) {
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
      catch(iException &e) {
        std::string msg = "Unable to solve in BundleAdjust, ";
        msg += "Iteration " + Isis::iString(p_iteration) + " of ";
        msg += Isis::iString(maxIterations) + ", Tolerance = ";
        msg += Isis::iString(tol);
        throw Isis::iException::Message(iException::Math, msg, _FILEINFO_);
      }

      // Ok take the results and put them back into the camera blobs
      Update(basis);
      //      return p_error;

      //Compute sigmas
      sigmaXY = sqrt((p_statx.SumSquare() + p_staty.SumSquare()) / lsq->Knowns());
      sigmaHat = (lsq->Knowns() - BasisColumns()) ?
                 (sqrt((p_statx.SumSquare() + p_staty.SumSquare()) / (lsq->Knowns() - BasisColumns())))
                 : 0.;
      sigmaX = p_statx.TotalPixels() ?
               sqrt(p_statx.SumSquare() / p_statx.TotalPixels()) : 0.;
      sigmaY = p_staty.TotalPixels() ?
               sqrt(p_staty.SumSquare() / p_staty.TotalPixels()) : 0.;
    }

    std::string msg = "Did not converge to tolerance [";
    msg += iString(tol) + "] in less than [";
    msg += iString(maxIterations) + "] iterations";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  /**
   * Populate the least squares matrix with measures for a point
   */
  void BundleAdjust::AddPartials(LeastSquares &lsq,
                                 int pointIndex) {
    ControlPoint &point = (*p_cnet)[pointIndex];
    if(point.Ignore()) return;   // Ignore entire point
    double cudx, cudy;

    for(int i = 0; i < point.Size(); i++) {
      if(point[i].Ignore()) continue;   // Ignore this measure

      if(p_heldImages) {
        if(p_heldsnlist->HasSerialNumber(point[i].CubeSerialNumber())) continue;
      }

      Camera *cam = point[i].Camera();
      // Get focal length with direction
      // Map the control point lat/lon/radius into the camera through the Spice
      // at the measured point to correctly compute the partials for line scan
      // cameras.  The camera SetUniversalGround method computes a time based
      // on the lat/lon/radius and uses the Spice for that time instead of the
      // measured point's time.
      cam->SetImage(point[i].Sample(), point[i].Line()); // Set the Spice to the measured point

      // Compute the look vector in instrument coordinates based on time of observation and apriori lat/lon/radius
      if(!(cam->GroundMap()->GetXY(point.UniversalLatitude(), point.UniversalLongitude(),
                                   point.Radius(), &cudx, &cudy))) {
        QString msg = "Unable to map apriori surface point for measure " +
            QString::number(i) + " on point " + point.Id() + " into focal plane";
        throw iException::Message(iException::User, msg.toStdString(),
            _FILEINFO_);
      }

      // Create the known array to put in the least squares
      std::vector<double> xKnowns(BasisColumns(), 0.0);
      std::vector<double> yKnowns(BasisColumns(), 0.0);

      // Determine the image index for nonheld images
      bool useImage = false;

      if(p_heldImages == 0) {
        useImage = true;
      }
      else if(p_heldImages > 0) {
        if((!(p_heldsnlist->HasSerialNumber(point[i].CubeSerialNumber()))))
          useImage = true;
      }
      if(useImage) {
        int index = p_snlist->SerialNumberIndex(point[i].CubeSerialNumber().toStdString());
        index = ImageIndex(index);

        if(p_spacecraftPositionSolveType != Nothing) {
          // Add the partial for the x coordinate of the position (differentiating
          // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
          for(int icoef = 0; icoef < p_spacecraftPositionSolveType; icoef++) {
            cam->GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, icoef, &xKnowns[index], &yKnowns[index]);
            index++;
          }

          // Add the partial for the y coordinate of the position
          for(int icoef = 0; icoef < p_spacecraftPositionSolveType; icoef++) {
            cam->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, icoef, &xKnowns[index], &yKnowns[index]);
            index++;
          }

          // Add the partial for the z coordinate of the position
          for(int icoef = 0; icoef < p_spacecraftPositionSolveType; icoef++) {
            cam->GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, icoef, &xKnowns[index], &yKnowns[index]);
            index++;
          }
        }
        if(p_cmatrixSolveType != None) {
          std::vector<double> d_lookC;

          // Add the partials for ra
          for(int icoef = 0; icoef < p_numberCameraCoefSolved; icoef++) {
            cam->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension, icoef, &xKnowns[index], &yKnowns[index]);
            index++;
          }

          // Add the partials for dec
          for(int icoef = 0; icoef < p_numberCameraCoefSolved; icoef++) {
            cam->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination, icoef, &xKnowns[index], &yKnowns[index]);
            index++;
          }

          // Add the partial for twist if necessary
          if(p_solveTwist) {
            for(int icoef = 0; icoef < p_numberCameraCoefSolved; icoef++) {
              cam->GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist, icoef, &xKnowns[index], &yKnowns[index]);
              index++;
            }
          }
        }
      }
      if((!point.Held()) && (point.Type() != ControlPoint::Ground)) {
        int index = PointIndex(pointIndex);
        cam->GroundMap()->GetdXYdPoint(point.UniversalLatitude(), point.UniversalLongitude(), point.Radius(),
                                       CameraGroundMap::WRT_Latitude, &xKnowns[index], &yKnowns[index]);
        index++;
        cam->GroundMap()->GetdXYdPoint(point.UniversalLatitude(), point.UniversalLongitude(), point.Radius(),                                       CameraGroundMap::WRT_Longitude, &xKnowns[index], &yKnowns[index]);
        index++;
        if(p_solveRadii) {
          cam->GroundMap()->GetdXYdPoint(point.UniversalLatitude(), point.UniversalLongitude(), point.Radius(),
                                         CameraGroundMap::WRT_Radius, &xKnowns[index], &yKnowns[index]);
          index++;
        }
      }

      double mudx = point[i].FocalPlaneMeasuredX();
      double mudy = point[i].FocalPlaneMeasuredY();

      double deltax = mudx - cudx;
      double deltay = mudy - cudy;

//      std::cout<<"mudx mudy="<<mudx<<" "<<mudy<<std::endl;
//      std::cout<<"cudx cudy="<<cudx<<" "<<cudy<<std::endl<<std::endl;

      lsq.AddKnown(xKnowns, deltax);
      lsq.AddKnown(yKnowns, deltay);
      p_statx.AddData(deltax);
      p_staty.AddData(deltay);
    }
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
    for(int i = 0; i < Images(); i++) {
      if(p_heldImages > 0) {
        if((p_heldsnlist->HasSerialNumber(p_snlist->SerialNumber(i)))) continue;
      }

      Camera *cam = p_cnet->Camera(i);
      int index = i;
      index = ImageIndex(index);

//      std::cout<<std::setprecision(16);
//      std::cout << "Spacecraft Position deltas for image "<< p_snlist->SerialNumber(i) << " " <<
//        basis.Coefficient(index)<<","<<basis.Coefficient(index+1)<<","<<basis.Coefficient(index+2)<<std::endl;

      if(p_spacecraftPositionSolveType != Nothing) {
        SpicePosition *pos = cam->InstrumentPosition();
        std::vector<double> abcX(3), abcY(3), abcZ(3);
        pos->GetPolynomial(abcX, abcY, abcZ);

        // Update the X coordinate coefficient(s)
        abcX[0] += basis.Coefficient(index);
        index++;
        if(p_spacecraftPositionSolveType > PositionOnly) {
          abcX[1] += basis.Coefficient(index);
          index++;
          if(p_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcX[2] += basis.Coefficient(index);
            index++;
          }
        }

        // Update the Y coordinate coefficient(s)
        abcY[0] += basis.Coefficient(index);
        index++;
        if(p_spacecraftPositionSolveType > PositionOnly) {
          abcY[1] += basis.Coefficient(index);
          index++;
          if(p_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcY[2] += basis.Coefficient(index);
            index++;
          }
        }

        // Update the Z coordinate coefficient(s)
        abcZ[0] += basis.Coefficient(index);
        index++;
        if(p_spacecraftPositionSolveType > PositionOnly) {
          abcZ[1] += basis.Coefficient(index);
          index++;
          if(p_spacecraftPositionSolveType == PositionVelocityAcceleration) {
            abcZ[2] += basis.Coefficient(index);
            index++;
          }
        }
        pos->SetPolynomial(abcX, abcY, abcZ);
      }

      if(p_cmatrixSolveType != None) {
        SpiceRotation *rot = cam->InstrumentRotation();
        std::vector<double> coefRA(p_numberCameraCoefSolved),
            coefDEC(p_numberCameraCoefSolved),
            coefTWI(p_numberCameraCoefSolved);
        rot->GetPolynomial(coefRA, coefDEC, coefTWI);

        // Update right ascension coefficient(s)
        for(int icoef = 0; icoef < p_numberCameraCoefSolved; icoef++) {
          coefRA[icoef] += basis.Coefficient(index);
          index++;
        }

        // Update declination coefficient(s)
        for(int icoef = 0; icoef < p_numberCameraCoefSolved; icoef++) {
          coefDEC[icoef] += basis.Coefficient(index);
          index++;
        }

        if(p_solveTwist) {
          // Update twist coefficient(s)
          for(int icoef = 0; icoef < p_numberCameraCoefSolved; icoef++) {
            coefTWI[icoef] += basis.Coefficient(index);
            index++;
          }
        }

        rot->SetPolynomial(coefRA, coefDEC, coefTWI);
      }
    }

    // Update lat/lon for each control point
    for(int i = 0; i < p_cnet->Size(); i++) {
      if((*p_cnet)[i].Held()) continue;
      if((*p_cnet)[i].Ignore()) continue;
      if((*p_cnet)[i].Type() == ControlPoint::Ground) continue;

      double lat = (*p_cnet)[i].UniversalLatitude();
      double lon = (*p_cnet)[i].UniversalLongitude();
      double rad = (*p_cnet)[i].Radius();
      int index = PointIndex(i);

//      std::cout<<"For point "<<i<<" lat/lon deltas = "<<basis.Coefficient(index)<<"/"<<basis.Coefficient(index+1)<<std::endl;

      lat += (180.0 / Isis::PI) * (basis.Coefficient(index));
      index++;
      lon += (180.0 / Isis::PI) * (basis.Coefficient(index));
      index++;
      // Make sure updated values are still in valid range.
      // TODO What is the valid lon range?
      if(lat < -90.) {
        lat = -180. - lat;
        lon = lon + 180.;
      }
      if(lat > 90.) {
        lat = 180. - lat;
        lon = lon + 180.;
      }
      while(lon > 360.) lon = lon - 360.;
      while(lon < 0) lon = lon + 360.;

      if(p_solveRadii) {
        rad += 1000.*basis.Coefficient(index);
        index++;
      }
      /*      else {  // Recompute radius to match updated lat/lon... Should this be removed?
              ControlMeasure &m = ((*p_cnet)[i])[0];
              Camera *cam = m.Camera();
              cam->SetUniversalGround(lat, lon);
              rad = cam->LocalRadius(); //meters
           }*/
      (*p_cnet)[i].SetUniversalGround(lat, lon, rad);
    }
  }

  //! Return index to basis function for ith point
  int BundleAdjust::PointIndex(int i) const {
    int index;

    if(!p_observationMode) {
      index = (Images() - p_heldImages) * p_numImagePartials;
    }
    else {
      index = Observations() * p_numImagePartials;
    }

    index += p_pointIndexMap[i] * p_numPointPartials;
    return index;
  }

  //! Return index to basis function for ith image
  int BundleAdjust::ImageIndex(int i) const {
    if(!p_observationMode) {
      return p_imageIndexMap[i] * p_numImagePartials;
    }
    else {
      return p_onlist->ObservationNumberMapIndex(i) * p_numImagePartials;
    }
  }


  //! Return the ith filename in the cube list file given to constructor
  std::string BundleAdjust::Filename(int i) {
//    std::string serialNumber = (*p_snlist)[i];
//    return p_snlist->Filename(serialNumber);
    return p_snlist->Filename(i);
  }

  //! Return a table cmatrix for the ith cube in the cube list given to the
  //! constructor
  Table BundleAdjust::Cmatrix(int i) {
    return p_cnet->Camera(i)->InstrumentRotation()->Cache("InstrumentPointing");
  }

  //! Return a table spacecraft vector for the ith cube in the cube list given to the
  //! constructor
  Table BundleAdjust::SpVector(int i) {
    return p_cnet->Camera(i)->InstrumentPosition()->Cache("InstrumentPosition");
  }

  //! Return the number of cubes in list given to the constructor
  int BundleAdjust::Images() const {
    return p_snlist->Size();
  }

  //! Return the number of observations in list given to the constructor
  int BundleAdjust::Observations() const {
    if(!p_observationMode) {
      return p_snlist->Size() - p_heldImages;
    }
    else {
      return p_onlist->ObservationSize();
    }
  }


  /**
    * This method creates an iteration summary and creates an iteration group for
    * the BundleAdjust summary.
    *
    * @param it              Iteration number
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
    std::string itlog = "Iteration" + iString(p_iteration);
    PvlGroup gp(itlog);
    gp += PvlKeyword("MaximumError", p_error, "pixels");
    gp += PvlKeyword("AverageError", avErr, "pixels");
    gp += PvlKeyword("SigmaXY", sigmaXY, "mm");
    gp += PvlKeyword("SigmaHat", sigmaHat, "mm");
    gp += PvlKeyword("SigmaX", sigmaX, "mm");
    gp += PvlKeyword("SigmaY", sigmaY, "mm");

    Application::Log(gp);
  }

}
