/**
 * @file
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
#include "OsirisRexTagcamsDistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Distortion Map object for the OSIRIS-REx TagCams Camera.
   *  
   * @param parent Pointer to parent Camera object
   * @param k1 First coefficient of radial distortion
   * @param k2 Second coefficient of radial distortion
   * @param k3 Third coefficient of radial distortion
   * @param p1 x tangential distortion component
   * @param p2 y tangential distortion compoment
   * @param cx X distortion center pixel location (pixels)
   * @param cy Y distortion center pixel location (pixels)
   * 
   * @internal 
   * @history 2019-01-22 Kris Becker  Renamed model parameter names for 
   *                       clarification of the OpenCV model
   *                       documentation/description. Updated to add the k4-k6
   *                       parameters.
   */
  OsirisRexTagcamsDistortionMap::OsirisRexTagcamsDistortionMap(Camera *parent,
                                                               int naifIkCode,
                                                               const double zdir)
                                                 : CameraDistortionMap(parent, zdir) {

      // Define kernel keywords to fetch
    QString ikCode = toString(naifIkCode);
    QString odkKey = "INS" + ikCode + "_OPENCV_OD_K";
    QString ppKey  = "INS" + ikCode + "_OPENCV_OD_P";
    QString flKey  = "INS" + ikCode + "_OPENCV_OD_F";
    QString aoKey  = "INS" + ikCode + "_OPENCV_OD_C";
    QString tdKey  = "INS" + ikCode + "_OPENCV_OD_A";
    QString tolKey = "INS" + ikCode + "_TOLERANCE";
    QString dbKey = "INS" + ikCode + "_DEBUG_MODEL";

    // Fetch the values for the distortion model
    p_k1 = parent->getDouble(odkKey, 0);
    p_k2 = parent->getDouble(odkKey, 1);
    p_k3 = parent->getDouble(odkKey, 2);
    p_k4 = parent->getDouble(odkKey, 3);
    p_k5 = parent->getDouble(odkKey, 4);
    p_k6 = parent->getDouble(odkKey, 5);
    p_p1 = parent->getDouble(ppKey, 0);
    p_p2 = parent->getDouble(ppKey, 1);
    p_fx = parent->getDouble(flKey, 0);
    p_fy = parent->getDouble(flKey, 1);
    p_cx = parent->getDouble(aoKey, 0);
    p_cy = parent->getDouble(aoKey, 1);
    p_td = parent->getDouble(tdKey, 0);
    p_camTemp = 0.0;
    p_tolerance = parent->getDouble(tolKey, 0);
    p_debug     = toBool(parent->getString(dbKey, 0));

    // Set up our own focal plane map from the camera model. NOTE!!!
    // The CameraFocalPlaneMap must be set in the Camera obhect 
    // prior to calling distortion model!!!
    if ( !parent->FocalPlaneMap() ) {
        QString mess = "FocalPlaneMap must be set in the Camera object prior to"
                       " initiating this distortion model!";
        throw IException(IException::Programmer, mess, _FILEINFO_);
    }

    // Replicate focal plane map
    m_focalMap.reset(new CameraFocalPlaneMap(*parent->FocalPlaneMap()));

    return;
  }

  /**
   * Set camera head temperature for the model 
   * 
   * @param temp Temperature of the camera (Celsius)
   */
  void OsirisRexTagcamsDistortionMap::SetCameraTemperature(const double temp) {
      p_camTemp = temp;
      return;
  }

  /**
   * Compute undistorted focal plane x/y
   *
   * Compute undistorted focal plane x/y given a distorted focal plane dx/dy.
   * 
   * After calling this method, you can obtain the undistorted x/y via the
   * UndistortedFocalPlaneX and UndistortedFocalPlaneY methods
   *
   * @param dx Distorted focal plane x, in millimeters
   * @param dy Distorted focal plane y, in millimeters
   *
   * @return whether the conversion was successful
   */
  bool OsirisRexTagcamsDistortionMap::SetFocalPlane(const double dx, const double dy) {
    if ( p_debug ) cout << "\nUndistorting FP at " << dx << ", " << dy << "\n";
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    double u, v, xpp, ypp;
    invert_model_point(dx, dy, &u, &v, &xpp, &ypp);
    if ( p_debug ) cout << "xpp=" << xpp << ", ypp=" << ypp << "\n";
    
    // With no distortion, xp == xpp and yp == ypp.    
    double xp = xpp;
    double yp = ypp;

    // Provided by the user/model
    double tolerance = p_tolerance;
    bool bConverged = false;

    // iterating to introduce distortion...
    // we stop when the difference between distorted coordinates
    // in successive iterations is at or below the given tolerance
    double xdistortion, ydistortion;
    double xdistorted, ydistorted;
    int nrevs(0);
    for(int i = 0; i < 50; i++) {

      // Apply the model...
      apply_model(xp, yp, &xdistorted, &ydistorted);

      // updated image coordinates
      xdistortion = xpp - xdistorted;
      ydistortion = ypp - ydistorted;

      // check for convergence
      if((fabs(xdistortion) <= tolerance) && (fabs(ydistortion) <= tolerance)) {
        bConverged = true;
        break;
      }

      nrevs++;
      if ( p_debug ) {
          cout << "i=" << i << ", xp=" << xp << ", yp=" << yp 
               << ", xdist=" << xdistortion << ", ydist=" << ydistortion << "\n";
      }

      // Update for next loop toward direction of convergence
      // Ok this is dependent upon the direction of distortion. In this
      // context, the fisheye model, the distortion is toward the corners,
      // so add the difference in the distortion.
      xp = xp + xdistortion;
      yp = yp + ydistortion;
    }

    if ( p_debug ) {
        std::cout << "Loop terminated after " << nrevs << " iterations! - converged? "
                  << ( ( bConverged ) ? "Yes!" : "No-(") << "\n";
    }

    if( bConverged ) {
      double x, y;
      model_to_isis(xp, yp, &x, &y, 
                    &p_undistortedFocalPlaneX, 
                    &p_undistortedFocalPlaneY);
    }

    return bConverged;
  }

  /**
   * @brief Compute distorted focal plane x/y
   *
   * Compute distorted focal plane x/y given an undistorted focal plane x/y.
   * After calling this method, you can obtain the distorted x/y via the
   * FocalPlaneX and FocalPlaneY methods
   *
   * @param ux Undistorted focal plane x, in millimeters
   * @param uy Undistorted focal plane y, in millimeters
   *
   * @return whether the conversion was successful
   */
  bool OsirisRexTagcamsDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    if ( p_debug )  cout << "\nDistorting FP at " << ux << ", " << uy << "\n";
    // image coordinates prior to introducing distortion
    p_focalPlaneX = p_undistortedFocalPlaneX = ux;
    p_focalPlaneY = p_undistortedFocalPlaneY = uy;

    double x, y, xp, yp;
    normalize_detector_point(ux, uy, &x, &y, &xp, &yp);
    if ( p_debug ) cout << "xp=" << xp << ", yp=" << yp << "\n";

    double xpp, ypp;
    apply_model(xp, yp, &xpp, &ypp);
    if ( p_debug ) cout << "xpp=" << xpp << ", ypp=" << ypp << "\n";

    double u,v;
    scale_model_point(xpp, ypp, &u, &v,
                      &p_focalPlaneX, 
                      &p_focalPlaneY);
    if ( p_debug ) cout << "Final FP ux=" << p_focalPlaneX << ", uy=" << p_focalPlaneY << "\n\n";

    return true;
  }


/**
 *  @brief Normalize ISIS3 focal plane coordinate to model point
 *  
 *  This method abtracts the conversion from the ISIS3 focal plane point (x,y)
 *  to the model point (x',y').
 *  
 *  The OpenCV model is derived from (sample,line) image coordinates whereas
 *  ISIS3 distortions are typically generated from focal plane (dx, dy)
 *  coordinaetes.
 *  
 *  The unitless (x',y') coordinates are computed from the focal plane (dx,dy)
 *  by converting to image coordinates, divide by focal length and multiply by
 *  the pixel pitch (z). Both these values are in millimeters.
 * 
 * @author 2019-01-11 Kris Becker Original Version
 * 
 * @param dx ISIS3 distorted focal plane x coordinate
 * @param dy ISIS3 distorted focal plane y coordinate
 * @param x  Returns ISIS3 image sample coordinate
 * @param y  Returns ISIS3 image line coordinate
 * @param xp Returns distortion model x' coordinate
 * @param yp Returns distortion model y' coordinate
 */
  void OsirisRexTagcamsDistortionMap::normalize_detector_point(const double dx, 
                                                               const double dy,
                                                               double *x,
                                                               double *y,
                                                               double *xp, 
                                                               double *yp) const {

    // For normalizing the focal plane coordinates
    double pixel_pitch  = p_camera->PixelPitch();
    double focal_length = p_camera->FocalLength();

    // Get the current sample/line as this is the basis of the distortion 
    // model. We must get original sample/line for this so the z factor
    // is a bit different.
    m_focalMap->SetFocalPlane(dx, dy);
    *x = m_focalMap->DetectorSample();
    *y = m_focalMap->DetectorLine();
    if ( p_debug ) cout << "Detector sample=" << *x << ", line=" << *y << "\n";

    // Normalized input into OpenCV model
    *xp = (*x - p_cx) / focal_length * pixel_pitch;
    *yp = (*y - p_cy) / focal_length * pixel_pitch;

    return;
  }


/**
 * @brief Converts a distortion model coordinate to ISIS3 focal plane coordinate
 *  
 * This method computes ISIS3 (u,v) and (dx,dy) coordinates from the result of 
 * the distortion model that is applied to (x',y'). 
 *  
 * The (x'',y'') point is scaled by the axis focal length temperature dependent 
 * correction (fx, fy) and then decentered from the optics center (cx, cy) to 
 * produce (u,v). 
 *  
 * As a convenience, the ISIS3 focal plane coordinates are also computed. 
 * 
 * @author 2019-01-11 Kris Becker Original Version
 * 
 * @param xpp Distortion model x'' coordinate
 * @param ypp Distortion model y'' coordinate
 * @param u   Returns ISIS3 image sample cordinate
 * @param v   Returns ISIS3 image line cordinate
 * @param dx  Returns ISIS3 x-axis distotred focal plane coordinate
 * @param dy  Returns ISIS3 y-axis distorted focal plane coordinate
 */
  void OsirisRexTagcamsDistortionMap::scale_model_point(const double xpp, 
                                                        const double ypp,
                                                        double *u, 
                                                        double *v,
                                                        double *dx,
                                                        double *dy) const {
    double deltaT = p_td * p_camTemp;
    *u = p_fx * ( 1.0 + deltaT ) * xpp + p_cx;
    *v = p_fy * ( 1.0 + deltaT ) * ypp + p_cy;
    if ( p_debug ) cout << "u=" << *u << ", v=" << *v << "\n";

    // Use the new image pixel coordinate to compute the new focal plane coordinate
    m_focalMap->SetDetector(*u, *v);

    *dx = m_focalMap->FocalPlaneX();
    *dy = m_focalMap->FocalPlaneY();

    return;

  }

/**
 * Compute model point from undistorted focal plane coordinates 
 *  
 * This method computes (x'',y'') from ISIS3 undistorted focal plane coordinates
 * (ux,uy). 
 *  
 * The (ux,uy) focal plane points are used to compute the image pixel 
 * coordinates that the OpenCV model is based upon. The pixel coordinate is  
 * 
 * @author 2019-01-11 Kris Becker Original Version
 * 
 * @param ux  ISIS3 undistorted focal plane x coordinate
 * @param uy  ISIS3 undistorted focal plane y coordinate
 * @param u   Returns ISIS3 image sample cordinate
 * @param v   Returns ISIS3 image line cordinate
 * @param xpp Distortion model x'' coordinate
 * @param ypp Distortion model y'' coordinate
 */
  void OsirisRexTagcamsDistortionMap::invert_model_point(const double ux, 
                                                         const double uy,
                                                         double *u, 
                                                         double *v,
                                                         double *xpp, 
                                                         double *ypp) const {

    // For normalizing the focal plane coordinates
    double pixel_pitch  = p_camera->PixelPitch();
    double focal_length = p_camera->FocalLength();

    // Get the current undistorted pixel coordinate
    m_focalMap->SetFocalPlane(ux, uy);
    *u = m_focalMap->DetectorSample();
    *v = m_focalMap->DetectorLine();
    if ( p_debug ) cout << "Detector sample=" << *u << ", line=" << *v << "\n";

    // Normalize to get the xpp/ypp parameters. This is the target
    // of the undistorted pixel location and what our loop will
    // converge to.
    double deltaT = p_td * p_camTemp;
    *xpp = ( *u - p_cx ) / ( p_fx * (1.0 + deltaT) );
    *ypp = ( *v - p_cy ) / ( p_fy * (1.0 + deltaT) );

    return;
  }

/**
 * Compute ISIS3 distorted focal plane coordinates from model 
 *  
 * This method computes the distorted ISIS3 focal plane coordinates (dx,dy) 
 * resulting from the inverse of the OpenCV distortion model unitless (x',y') 
 * coordinates. 
 *  
 * Essentially, the conversion to focal plane coordinates need to be scaled to 
 * image coordinates by dividing by the pixel pitch and multiplying by the focal 
 * length (z). Finally the image coordinate is adjusted by decentering from the 
 * optical axis center providing (x,y). 
 *  
 * Then, (x,y) is used to compute the ISIS3 distorted focal plane coordinates 
 * (dx, dy). 
 * 
 * @author 2019-01-11 Kris Becker Original Version
 * 
 * @param xp Distortion model x' coordinate
 * @param yp Distortion model y' coordinate
 * @param x  Returns ISIS3 distorted image sample coordinate
 * @param y  Returns ISIS3 distorted image line coordinate
 * @param dx Returns ISIS3 x-axis distorted focal plane coordinate
 * @param dy Returns ISIS3 y-axis distorted focal plane coordinate
 */
  void OsirisRexTagcamsDistortionMap::model_to_isis(const double xp, 
                                                    const double yp,
                                                    double *x, 
                                                    double *y,
                                                    double *dx,
                                                    double *dy) const {

    double pixel_pitch  = p_camera->PixelPitch();
    double focal_length = p_camera->FocalLength();

    *x = ( xp / pixel_pitch * focal_length ) + p_cx;
    *y = ( yp / pixel_pitch * focal_length ) + p_cy;

    m_focalMap->SetDetector(*x, *y);
    *dx = m_focalMap->FocalPlaneX();
    *dy = m_focalMap->FocalPlaneY();

    if ( p_debug ) {
        std::cout << "ux=" << *dx<< ", uy=" << *dy << "\n";
        std::cout << "Detector sample=" << m_focalMap->DetectorSample() 
                  << ", line=" << m_focalMap->DetectorLine()<< "\n";
    }

    return;
  }


/**
 * @brief Apply OpenCV model equations to normalized parameters 
 *  
 * This method abstracts the OpenCV distortion model equations that are 
 * appled to the normalized focal plane coordinates (x',y'). The normalized 
 * focal plane coordinate system is centered at the optics axis origin and are 
 * unitless. To convert from ISIS3 focal plane coordinates, one only needs to 
 * multiply the coordinates by the pixel pitch and divide by the focal length. 
 *  
 * The result of the model is the distortion coordinates (x'',y''). 
 * 
 * @author 2019-01-15 Kris Becker
 * 
 * @param xp   Normalized x' coordinate
 * @param yp   Normalized y' coordinate
 * @param xpp  Returns the x'' parameter after applying distortion model 
 * @param ypp  Returns the y'' parameter after applying distortion model
 */
  void OsirisRexTagcamsDistortionMap::apply_model(const double xp, 
                                                  const double yp,
                                                  double *xpp, 
                                                  double *ypp) const {

    // Apply the OpenCV model to the normalized focal plane
    // coordinates
    double r2 = xp * xp + yp * yp;
    double r4 = r2 * r2;
    double r6 = r2 * r4;

    //  dr is the radial distortion contribution
    double dr = ( 1.0 + (p_k1 * r2) + (p_k2 * r4) + (p_k3 * r6) ) /
                ( 1.0 + (p_k4 * r2) + (p_k5 * r4) + (p_k6 * r6) );

    // Tangential contributions
    double dt_x = ( 2.0 * p_p1 * xp * yp ) + ( ( p_p2 * r2 ) + ( p_p2 * 2.0 * xp * xp ) );
    double dt_y = ( ( ( p_p1 * r2 ) + ( p_p1 * 2.0 * yp * yp ) ) + ( 2.0 * p_p2 * xp * yp ) );

    // Image coordinates corrected for principal point, radial and decentering distortion
    *xpp = ( dr * xp ) + dt_x;
    *ypp = ( dr * yp ) + dt_y;

    return;
  }
}
