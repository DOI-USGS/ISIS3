#ifndef OsirisRexTagcamsDistortionMap_h
#define OsirisRexTagcamsDistortionMap_h
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
   * OsirisRexTagcamsDistortionMap 
   * 
   *  This model implements the OpenCV stereo camera model.
   *  
   * @author 2019-01-08 Kris Becker
   * 
   * @internal
   */
  class OsirisRexTagcamsDistortionMap : public CameraDistortionMap {
    public:
      /**
       *  
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
       * 
       * @internal
       */
      OsirisRexTagcamsDistortionMap(Camera *parent, int naifIkCode,
                                    const double zdir = 1.0);
 
      //! Destroys OsirisRexTagcamsDistortionMap object.
      ~OsirisRexTagcamsDistortionMap() {};
      
      
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

      double p_k1;  //!< First coefficient of radial distortion.
      double p_k2;  //!< Second coefficient of radial distortion.
      double p_k3;  //!< Third coefficient of radial distortion.
      double p_p1;  //!< Tangential x-coordinate.
      double p_p2;  //!< Tangential y-coordinate.
      double p_cx;  //!< x optical axis center.
      double p_cy;  //!< y optical axis center.
      double p_pixel_pitch; //!< Pixel pitch
      double p_focal_length; //!< Pixel pitch
  };
};
#endif
