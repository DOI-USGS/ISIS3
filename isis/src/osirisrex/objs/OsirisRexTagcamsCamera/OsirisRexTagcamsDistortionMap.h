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

#include <QSharedPointer>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Camera.h"

namespace Isis {
  /**
   * OsirisRexTagcamsDistortionMap camera distortion model for TAGCAMS
   * 
   * This model implements a robust camera distortion model based upon the OpenCV 
   * stereo camera calibration model produced by the Kinetx NAV team for the 
   * TAGCAMS camera system. This model is described in the (white?) paper 
   * "IN-FLIGHT CALIBRATION OF THE OSIRIS-REX OPTICAL NAVIGATION IMAGERS". This 
   * model derives 10 parameters that make up the distortion model acquired during 
   * inflight and approach operations toward Bennu. 
   *  
   * This model is an image line/sample coordinate-based model that is computed 
   * from star positions in several images. The actual positions of the stars are 
   * determined where they should be seen in the undistorted image plane and the 
   * model produces the distorted location of the star inthe image. Therefore, the 
   * model is implemented such that the computation of distorted pixel location is 
   * a direct computation of the model and the undistorted location is iterative. 
   *  
   * All ten of the parameters (k1, k2, k3, p1, p2, fx, fy, cx, cy, td) are read 
   * from the instrument (addendum) kernel. The temperature of the camera head 
   * (ct) must come from the camera model (assumed to be provided in the cube 
   * header) in units of celcius. The temperature is initially set to 0 so if its 
   * not available, the variable focal length component (which is pretty small) 
   * will be excluded from the distortion. 
   *  
   * Other parameters that can be adjusted in the kernel are the convergence 
   * tolerance limit and invoking debugging output to help evaluate behavior. See 
   * $ISIS3DATA/osirisrex/kernels/iak/orex_tagcams_addendum_v02.ti. 
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
       * @param p2 y tangential distortion compomen 
       * @param fx x focal length
       * @param fy y focal length
       * @param cx X distortion axis center pixel location (pixels)
       * @param cy Y distortion axis center pixel location (pixels) 
       * @param td Temperature dependent focal length adjustment 
       * @param ct Camera head temperature (C) 
       * 
       * 
       * @internal
       */
      OsirisRexTagcamsDistortionMap(Camera *parent, int naifIkCode,
                                    const double zdir = 1.0);
 
      //! Destroys OsirisRexTagcamsDistortionMap object.
      ~OsirisRexTagcamsDistortionMap() {};
      
    /**
     * Set camera head temperature for the model 
     * 
     * 
     * @param temp Temperature of the camera (Celsius)
     */
      void SetCameraTemperature(const double temp);

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

    protected:
        void normalize_detector_point(const double dx, const double dy,
                                      double *x, double *y,
                                      double *xp, double *yp) const;
        void scale_model_point(const double xpp, const double ypp,
                               double *u, double *v,
                               double *dx, double *dy) const;
        void invert_model_point(const double ux, const double uy,
                               double *u, double *v,
                               double *xpp, double *ypp) const;
        void model_to_isis(const double xp, const double yp,
                           double *x, double *y,
                           double *dx, double *dy) const;

        void apply_model(const double xp, const double yp,
                         double *xpp, double *ypp) const;

      
    private:  
      // parameters below are from camera calibration report

      double p_k1;       //!< First coefficient of radial distortion.
      double p_k2;       //!< Second coefficient of radial distortion.
      double p_k3;       //!< Third coefficient of radial distortion.
      double p_p1;       //!< Tangential x-coordinate.
      double p_p2;       //!< Tangential y-coordinate.
      double p_fx;       //!< X focal plane length
      double p_fy;       //!< Y focal plane length
      double p_cx;       //!< x optical axis center.
      double p_cy;       //!< y optical axis center.
      double p_td ;      //!< Temperature dependent parameter
      double p_camTemp;  //!< Camera head temperature
      double p_tolerance; //!< Convergence tolerance
      bool   p_debug;     //!< Debug the model

      QSharedPointer<CameraFocalPlaneMap> m_focalMap;  // Local focal plane map

  };
};
#endif
