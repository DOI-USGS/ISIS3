#include "LidarControlPoint.h"

#include <QString>
#include <QVector>

#include "Camera.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "IException.h"
#include "iTime.h"

using namespace std;

namespace Isis {
  

  /**
   * Constructs a LidarControlPoint with the given time, range, and sigma range.
   * 
   */
  LidarControlPoint::LidarControlPoint() {
    m_time = iTime();
    m_range = -1.0;
    m_sigmaRange = -1.0;
    m_snSimultaneous = NULL;

    m_snSimultaneous = new QStringList;
  }
  
  
  /**
   * Destructor
   */
  LidarControlPoint::~LidarControlPoint() {
    if (m_snSimultaneous) {
      delete m_snSimultaneous;
      m_snSimultaneous = NULL;
    }
    
  }
  
  
  /**
   * Set the time of the LidarControlPoint
   * 
   * @param time The time to set
   */
  ControlPoint::Status LidarControlPoint::setTime(iTime time) {
    if (IsEditLocked()) {
      return ControlPoint::Status::PointLocked;
    }
    m_time = time;
    return ControlPoint::Status::Success;
  }

  
  /**
   * Set the range of the LidarControlPoint
   * 
   * @param range The range to set
   */
  ControlPoint::Status LidarControlPoint::setRange(double range) {
    if (IsEditLocked()) {
      return ControlPoint::Status::PointLocked;
    }
    m_range = range;
    return ControlPoint::Status::Success;
  }
  
  
  /**
   * Sets the sigma range
   * 
   * @param sigmaRange The sigma range to set
   */
  ControlPoint::Status LidarControlPoint::setSigmaRange(double sigmaRange) {
    if (IsEditLocked()) {
      return ControlPoint::Status::PointLocked;
    }
    m_sigmaRange = sigmaRange;
    return ControlPoint::Status::Success;
  }
  
  
  /**
   * Add a measure to the list of simultaneous images of a  LidarControlPoint
   * 
   * @param time The serial number of the simultaneous image to add
   */
  ControlPoint::Status LidarControlPoint::addSimultaneous(QString newSerial) {
    m_snSimultaneous->append(newSerial);
    return ControlPoint::Status::Success;
  }
  
  
  /**
   * Returns the range of the point
   * 
   * @return double The range
   */
  double LidarControlPoint::range() {
    return m_range;
  }
  
  
  /**
   * Returns the time of the point
   * 
   * @return double The time
   */
  iTime LidarControlPoint::time() {
    return m_time;
  }
  
  
  /**
   * Returns the sigma range of the point
   * 
   * @return double The sigma range
   */
  double LidarControlPoint::sigmaRange() {
    return m_sigmaRange;
  }
  
  
  /**
   * Returns the list of serial numbers of simultaneous images of the Lidar point
   * 
   * @return QList The list of serial numbers
   */
//QList < QString > LidarControlPoint::snSimultaneous() const{
  QStringList LidarControlPoint::snSimultaneous() const{
    return *m_snSimultaneous;
  }


  /**
   * Determines if input serial number is in list of simultaneous measure serial numbers.
   *
   * @param serialNumber Serial number to check.
   *
   * @return bool true if serialNumber is contained in m_snSimultaneous.
   */
  bool LidarControlPoint::isSimultaneous(QString serialNumber) {

    if (m_snSimultaneous->contains(serialNumber)) {
      return true;
    }

    return false;
  }


  /**
   * This method computes the BundleAdjust residuals for a point.
   *     *** Warning:  Only BundleAdjust and its applications should be
   *                   using this method.
   * correcting measure version
   *
   * @history 2019-02-26 Ken Edmundson, initial version.
   */
/*
  ControlPoint::Status LidarControlPoint::ComputeResiduals() {
    if (IsIgnored()) {
      return Failure;
    }

//    ControlPoint::PointModified();

//    double cuSamp, cuLine;
//    double muSamp, muLine;
    double cudx = 0.0;
    double cudy = 0.0;
    double cuSamp = 0.0;
    double cuLine = 0.0;
    double muSamp = 0.0;
    double muLine = 0.0;
    double newsamp = 0.0;
    double newline = 0.0;

    // Loop for each measure to compute the error
    QList<QString> keys = measures->keys();

    for (int j = 0; j < keys.size(); j++) {
      ControlMeasure *measure = (*measures)[keys[j]];
      if (measure->IsIgnored()) {
        continue;
      }

      Camera* camera = measure->Camera();

      // back project the adjusted surface point with SPICE updated from current bundle iteration
      // newsamp and newline are distorted
      if (camera->SetGround(GetAdjustedSurfacePoint())) {
        newsamp = camera->Sample();
        newline = camera->Line();
      }

      // set the measures sample, line to the back-projected location
      measure->SetCoordinate(newsamp, newline);

      // set the camera model to the new back-projected location (this is the statefulness thing)
      camera->SetImage(newsamp,newline);

      // get the "computed" and "undistorted" focal plane x,y coordinates
      camera->GroundMap()->GetXY(GetAdjustedSurfacePoint(), &cudx, &cudy);

      // set the measure's computed focal plane coordinates to these values
      measure->SetFocalPlaneComputed(cudx, cudy);

      double newx = camera->DistortionMap()->UndistortedFocalPlaneX();
      double newy = camera->DistortionMap()->UndistortedFocalPlaneY();
      measure->SetFocalPlaneMeasured(newx,newy);

      // Now things get tricky.  We want to produce errors in pixels not mm but some of the camera
      // maps could fail. One that won't is the FocalPlaneMap which takes x/y to detector s/l. We
      // will bypass the distortion map and have residuals in undistorted pixels.
      if (!camera->FocalPlaneMap()->SetFocalPlane(cudx, cudy)) {
        QString msg = "Sanity check #1 for ControlPoint [" + GetId() +
            "], ControlMeasure [" + measure->GetCubeSerialNumber() + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
        // This error shouldn't happen but check anyways
      }

      cuSamp = camera->FocalPlaneMap()->DetectorSample();
      cuLine = camera->FocalPlaneMap()->DetectorLine();

      muSamp = camera->FocalPlaneMap()->DetectorSample();
      muLine = camera->FocalPlaneMap()->DetectorLine();

      double sampResidual = muSamp - cuSamp;
      double lineResidual = muLine - cuLine;

      measure->SetResidual(sampResidual, lineResidual);

//      camera->GroundMap()->GetXY(GetAdjustedSurfacePoint(), &cudx, &cudy);

//      measure->SetCoordinate(newsamp, newline);
//      camera->SetImage(newsamp,newline);
//      double newundx = camera->DistortionMap()->UndistortedFocalPlaneX();
//      double newundy = camera->DistortionMap()->UndistortedFocalPlaneY();
//      double newx = camera->DistortionMap()->FocalPlaneX();
//      double newy = camera->DistortionMap()->FocalPlaneY();
//      measure->SetFocalPlaneMeasured(newx,newy);
//      measure->SetFocalPlaneComputed(newundx,newundy);
    }
/asterisk



      // The following lines actually check for Candidate measures
      // Commented out on 2011-03-24 by DAC
//       if (!m->IsMeasured()) {
//         continue;

      Camera *cam = m->Camera();
      CameraFocalPlaneMap *fpmap = cam->FocalPlaneMap();

      // Map the lat/lon/radius of the control point through the Spice of the
      // measurement sample/line to get the computed undistorted focal plane
      // coordinates (mm if not radar).  This works for radar too because in
      // the undistorted focal plane, y has not been set to 0 (set to 0 when
      // going to distorted focal plane or ground range in this case), so we
      // can hold the Spice to calculate residuals in undistorted focal plane
      // coordinates.
      if (cam->GetCameraType() != 0) {  // no need to call setimage for framing camera
        cam->SetImage(m->GetSample(), m->GetLine());
      }

//      cam->GroundMap()->GetXY(GetAdjustedSurfacePoint(), &cudx, &cudy);
      cam->GroundMap()->GetXY(adjustedSurfacePoint, &cudx, &cudy);
      m->SetFocalPlaneComputed(cudx, cudy);



      // TODO:TESTING
//      cam->DistortionMap()->SetUndistortedFocalPlane(cudx,cudy);
//      double distortedx = cam->DistortionMap()->FocalPlaneX();
//      double distortedy = cam->DistortionMap()->FocalPlaneY();
//      fpmap->SetFocalPlane(distortedx, distortedy);
//      double distortedsample = fpmap->DetectorSample();
//      double distortedline = fpmap->DetectorLine();
      // TODO:TESTING

      if (cam->GetCameraType()  !=  Isis::Camera::Radar) {

        // Now things get tricky.  We want to produce errors in pixels not mm
        // but some of the camera maps could fail.  One that won't is the
        // FocalPlaneMap which takes x/y to detector s/l.  We will bypass the
        // distortion map and have residuals in undistorted pixels.
        if (!fpmap->SetFocalPlane(cudx, cudy)) {
          QString msg = "Sanity check #1 for ControlPoint [" + GetId() +
              "], ControlMeasure [" + m->GetCubeSerialNumber() + "]";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }

        cuSamp = fpmap->DetectorSample();
        cuLine = fpmap->DetectorLine();
      }
      else {
        // For radar line is calculated from time in the camera.  Use the
        // closest line to scale the focal plane y (doppler shift) to image line
        // for computing the line residual.  Get a local ratio
        //     measureLine    =   adjacentLine
        //     ------------       --------------  in both cases, doppler shift
        //     dopplerMLine       dopplerAdjLine  is calculated using SPICE
        //                                        at the time of the measurement
        //
        // 1.  Get the surface point mapped to by an adjacent pixel above (if
        //     doppler is < 0) or below (if doppler is > 0) the measured pixel
        // 2.  Set image to the measured sample/line to load the SPICE for the
        //     time of the measurement.
        // 3.  Map the surface point from the adjacent pixel through the SPICE
        //     into the image plane to get a scale for mapping from doppler
        //     shift to line.  Apply the scale to get the line residual
        double sample = m->GetSample();
        double computedY = m->GetFocalPlaneComputedY();
        double computedX = m->GetFocalPlaneComputedX();
        double adjLine;

        // Step 1. What happens if measured line is 1???  TODO
        if (computedY < 0) {
          adjLine = m->GetLine() - 1.;
        }
        else {
          adjLine = m->GetLine() + 1.;
        }

        cam->SetImage(sample, adjLine);
        SurfacePoint sp = cam->GetSurfacePoint();

        // Step 2.
        cam->SetImage(sample, m->GetLine());
        double focalplaneX;
        double scalingY;

        // Step 3.
        cam->GroundMap()->GetXY(sp, &focalplaneX, &scalingY);
        double deltaLine;

        if (computedY < 0) {
          deltaLine = -computedY/scalingY;
        }
        else {
          deltaLine = computedY/scalingY;
        }

        // Now map through the camera steps to take X from slant range to ground
        // range to pixels.  Y just tracks through as 0.
        if (cam->DistortionMap()->SetUndistortedFocalPlane(computedX,
                                                           computedY)){
          double focalPlaneX = cam->DistortionMap()->FocalPlaneX();
          double focalPlaneY = cam->DistortionMap()->FocalPlaneY();
          fpmap->SetFocalPlane(focalPlaneX,focalPlaneY);
        }
        cuSamp = fpmap->DetectorSample();
        cuLine = m->GetLine() + deltaLine;
      }

      if (cam->GetCameraType()  !=  Isis::Camera::Radar) {
        // Again we will bypass the distortion map and have residuals in undistorted pixels.
        if (!fpmap->SetFocalPlane(m->GetFocalPlaneMeasuredX(), m->GetFocalPlaneMeasuredY())) {
          QString msg = "Sanity check #2 for ControlPoint [" + GetId() +
              "], ControlMeasure [" + m->GetCubeSerialNumber() + "]";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }
        muSamp = fpmap->DetectorSample();
        muLine = fpmap->DetectorLine();
      }
      else {
        muSamp = m->GetSample();
        muLine = m->GetLine();
      }

      // The units are in detector sample/lines.  We will apply the instrument
      // summing mode to get close to real pixels.  Note however we are in
      // undistorted pixels except for radar instruments.
      double sampResidual = muSamp - cuSamp;
      double lineResidual = muLine - cuLine;

      // TODO: TESTING
//      double x = cam->DistortionMap()->FocalPlaneX();
//      double y = cam->DistortionMap()->FocalPlaneY();
//      m->SetFocalPlaneMeasured(x, y);
//      double mdissamp = fpmap->DetectorSample();
//      double mdisline = fpmap->DetectorLine();
//      double distortedSampleResidual = fpmap->DetectorSample() - distortedsample;
//      double distortedLineResidual = fpmap->DetectorLine() - distortedline;
      // TODO: TESTING

      m->SetResidual(sampResidual, lineResidual);
    }
asterisk/
    return Success;
  }
*/


  /**
   * This method computes the BundleAdjust residuals for a point.
   *     *** Warning:  Only BundleAdjust and its applications should be
   *                   using this method.
   *
   * @history 2008-07-17 Tracie Sucharski,  Added ptid and measure serial
   *                            number to the unable to map to surface error.
   * @history 2009-12-06 Tracie Sucharski, Renamed from ComputeErrors
   * @history 2010-08-05 Tracie Sucharski, Changed lat/lon/radius to x/y/z
   * @history 2010-12-10 Debbie A. Cook, Revised error calculation for radar
   *                            because it was always reporting line errors=0.
   * @history 2011-03-17 Debbie A. Cook, Fixed typo in radar call to get
   *                            longitude
   * @history 2011-03-24 Debbie A. Cook, Removed IsMeasured check since it
   *                            was really checking for Candidate measures.
   * @history 2011-07-01 Debbie A. Cook, Removed editLock check to allow
   *                            BundleAdjust to compute residuals for
   *                            editLocked points
   * @history 2012-01-18 Debbie A. Cook, Revised to call
   *                            ComputeResidualsMillimeters() to avoid
   *                            duplication of code
   * @history 2018-06-13 Debbie A. Cook, Ken Edmundson, Removed method ComputeResidualsMillimeters()
   *                            and the call to it that was in this method. Added computation of
   *                            focal plane computedx and computedy here.
   */
  ControlPoint::Status LidarControlPoint::ComputeResiduals() {
    if (IsIgnored()) {
      return Failure;
    }

//    PointModified();

    double cuSamp, cuLine;
    double muSamp, muLine;
    double cudx = 0.0;
    double cudy = 0.0;

    // Loop for each measure to compute the error
    QList<QString> keys = measures->keys();

    for (int j = 0; j < keys.size(); j++) {
      ControlMeasure *m = (*measures)[keys[j]];
      if (m->IsIgnored()) {
        continue;
      }
      // The following lines actually check for Candidate measures
      // Commented out on 2011-03-24 by DAC
//       if (!m->IsMeasured()) {
//         continue;

      Camera *cam = m->Camera();
      CameraFocalPlaneMap *fpmap = cam->FocalPlaneMap();

      // Map the lat/lon/radius of the control point through the Spice of the
      // measurement sample/line to get the computed undistorted focal plane
      // coordinates (mm if not radar).  This works for radar too because in
      // the undistorted focal plane, y has not been set to 0 (set to 0 when
      // going to distorted focal plane or ground range in this case), so we
      // can hold the Spice to calculate residuals in undistorted focal plane
      // coordinates.
      if (cam->GetCameraType() != 0) {  // no need to call setimage for framing camera
        cam->SetImage(m->GetSample(), m->GetLine());
      }

      cam->GroundMap()->GetXY(GetAdjustedSurfacePoint(), &cudx, &cudy);
//      cam->GroundMap()->GetXY(adjustedSurfacePoint, &cudx, &cudy);
      m->SetFocalPlaneComputed(cudx, cudy);



      // TODO:TESTING
//      cam->DistortionMap()->SetUndistortedFocalPlane(cudx,cudy);
//      double distortedx = cam->DistortionMap()->FocalPlaneX();
//      double distortedy = cam->DistortionMap()->FocalPlaneY();
//      fpmap->SetFocalPlane(distortedx, distortedy);
//      double distortedsample = fpmap->DetectorSample();
//      double distortedline = fpmap->DetectorLine();
      // TODO:TESTING

      if (cam->GetCameraType()  !=  Isis::Camera::Radar) {

        // Now things get tricky.  We want to produce errors in pixels not mm
        // but some of the camera maps could fail.  One that won't is the
        // FocalPlaneMap which takes x/y to detector s/l.  We will bypass the
        // distortion map and have residuals in undistorted pixels.
        if (!fpmap->SetFocalPlane(cudx, cudy)) {
          QString msg = "Sanity check #1 for ControlPoint [" + GetId() +
              "], ControlMeasure [" + m->GetCubeSerialNumber() + "]";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }

        cuSamp = fpmap->DetectorSample();
        cuLine = fpmap->DetectorLine();
      }
      else {
        // For radar line is calculated from time in the camera.  Use the
        // closest line to scale the focal plane y (doppler shift) to image line
        // for computing the line residual.  Get a local ratio
        //     measureLine    =   adjacentLine
        //     ------------       --------------  in both cases, doppler shift
        //     dopplerMLine       dopplerAdjLine  is calculated using SPICE
        //                                        at the time of the measurement
        //
        // 1.  Get the surface point mapped to by an adjacent pixel above (if
        //     doppler is < 0) or below (if doppler is > 0) the measured pixel
        // 2.  Set image to the measured sample/line to load the SPICE for the
        //     time of the measurement.
        // 3.  Map the surface point from the adjacent pixel through the SPICE
        //     into the image plane to get a scale for mapping from doppler
        //     shift to line.  Apply the scale to get the line residual
        double sample = m->GetSample();
        double computedY = m->GetFocalPlaneComputedY();
        double computedX = m->GetFocalPlaneComputedX();
        double adjLine;

        // Step 1. What happens if measured line is 1???  TODO
        if (computedY < 0) {
          adjLine = m->GetLine() - 1.;
        }
        else {
          adjLine = m->GetLine() + 1.;
        }

        cam->SetImage(sample, adjLine);
        SurfacePoint sp = cam->GetSurfacePoint();

        // Step 2.
        cam->SetImage(sample, m->GetLine());
        double focalplaneX;
        double scalingY;

        // Step 3.
        cam->GroundMap()->GetXY(sp, &focalplaneX, &scalingY);
        double deltaLine;

        if (computedY < 0) {
          deltaLine = -computedY/scalingY;
        }
        else {
          deltaLine = computedY/scalingY;
        }

        // Now map through the camera steps to take X from slant range to ground
        // range to pixels.  Y just tracks through as 0.
        if (cam->DistortionMap()->SetUndistortedFocalPlane(computedX,
                                                           computedY)){
          double focalPlaneX = cam->DistortionMap()->FocalPlaneX();
          double focalPlaneY = cam->DistortionMap()->FocalPlaneY();
          fpmap->SetFocalPlane(focalPlaneX,focalPlaneY);
        }
        cuSamp = fpmap->DetectorSample();
        cuLine = m->GetLine() + deltaLine;
      }

      if (cam->GetCameraType()  !=  Isis::Camera::Radar) {
        // Again we will bypass the distortion map and have residuals in undistorted pixels.
        if (!fpmap->SetFocalPlane(m->GetFocalPlaneMeasuredX(), m->GetFocalPlaneMeasuredY())) {
          QString msg = "Sanity check #2 for ControlPoint [" + GetId() +
              "], ControlMeasure [" + m->GetCubeSerialNumber() + "]";
          throw IException(IException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }
        muSamp = fpmap->DetectorSample();
        muLine = fpmap->DetectorLine();
      }
      else {
        muSamp = m->GetSample();
        muLine = m->GetLine();
      }

      // The units are in detector sample/lines.  We will apply the instrument
      // summing mode to get close to real pixels.  Note however we are in
      // undistorted pixels except for radar instruments.
      double sampResidual = muSamp - cuSamp;
      double lineResidual = muLine - cuLine;

      // TODO: TESTING
//      double x = cam->DistortionMap()->FocalPlaneX();
//      double y = cam->DistortionMap()->FocalPlaneY();
//      m->SetFocalPlaneMeasured(x, y);
//      double mdissamp = fpmap->DetectorSample();
//      double mdisline = fpmap->DetectorLine();
//      double distortedSampleResidual = fpmap->DetectorSample() - distortedsample;
//      double distortedLineResidual = fpmap->DetectorLine() - distortedline;
      // TODO: TESTING

      m->SetResidual(sampResidual, lineResidual);
    }

    return Success;
  }
}
