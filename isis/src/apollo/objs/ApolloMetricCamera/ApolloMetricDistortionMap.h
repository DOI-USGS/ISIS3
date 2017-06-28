#ifndef ApolloMetricDistortionMap_h
#define ApolloMetricDistortionMap_h
/**
 * @file
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
#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  /**
   * Apollo Metric Distortion Map
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed
   *                          Apollo namespace wrap inside Isis namespace. Added
   *                          Isis Disclaimer to files.
   *   @history 2013-03-18 Debbie A. Cook - Added flag to flip focal plane z axis
   *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
   */
  class ApolloMetricDistortionMap : public CameraDistortionMap {
    public:
      /**
       *  
       * Constructs a Distortion Map object for the Apollo Metric Camera. 
       *  
       * @param parent Pointer to parent Camera object
       * @param xp Pricipal point x-coordinate
       * @param yp Pricipal point y-coordinate
       * @param k1 First coefficient of radial distortion
       * @param k2 Second coefficient of radial distortion
       * @param k3 Third coefficient of radial distortion
       * @param j1 First coefficient of decentering distortion
       * @param j2 Second coefficient of decentering distortion
       * @param t0 Angle between positive x-axis of image and vector to imaged point
       * 
       * @internal
       *   @history 2011-05-03 Jeannie Walldren - Added documentation.
       *   @history 2013-03-18 Debbie A. Cook - Added flag to flip focal plane z axis
       */
      ApolloMetricDistortionMap(Camera *parent, double xp, double yp, 
                                double k1, double k2, double k3, double j1, 
                                double j2, double t0);
      
      
      //! Destroys ApolloMetricDistortionMap object.
      ~ApolloMetricDistortionMap() {};
      
      
      /**
       * Compute undistorted focal plane x/y
       *
       * Compute undistorted focal plane x/y given a distorted focal plane x/y.
       * fter calling this method, you can obtain the undistorted
       * x/y via the UndistortedFocalPlaneX and UndistortedFocalPlaneY methods
       *
       * @param dx Distorted focal plane x, in millimeters
       * @param dy Distorted focal plane y, in millimeters
       *
       * @return whether the conversion was successful
       */
      bool SetFocalPlane(const double dx, const double dy);
      
      
      /**
       * Compute distorted focal plane x/y
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
      bool SetUndistortedFocalPlane(const double ux, const double uy);
      
    private:  
      // parameters below are from camera calibration report
      double p_xp;  //!< Principal point x-coordinate.
      double p_yp;  //!< Principal point y-coordinate.
      double p_k1;  //!< First coefficient of radial distortion.
      double p_k2;  //!< Second coefficient of radial distortion.
      double p_k3;  //!< Third coefficient of radial distortion.
      double p_j1;  //!< First coefficient of decentering distortion.
      double p_j2;  //!< Second coefficient of decentering distortion.
      double p_t0;  /**< Angle between positive x-axis of image and vector to
                          imaged point. Used in computation of decentering 
                          distortion.*/
  };
};
#endif
