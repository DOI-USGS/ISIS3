/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/06/17 18:59:12 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#ifndef SampleScanCameraGroundMap_h
#define SampleScanCameraGroundMap_h

#include "CameraGroundMap.h"

namespace Isis {
  /** Convert between undistorted focal plane and ground coordinates
   *
   * This class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and ground coordinates lat/lon
   * for sample scan cameras.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2012-09-12 Ken Edmundson
   *
   * @internal
   *
   *   @history 2012-09-12 Ken Edmundson - Original version
   *   @history 2018-07-14 Makayla Shepherd - Updated documentation
   *
   */
  class SampleScanCameraGroundMap : public CameraGroundMap {
    public:

      //! Constructor
      SampleScanCameraGroundMap(Camera *cam);

      //! Destructor
      virtual ~SampleScanCameraGroundMap();

      virtual bool SetGround(const Latitude &lat, const Longitude &lon);
      virtual bool SetGround(const SurfacePoint &surfacePoint);
      virtual bool SetGround(const SurfacePoint &surfacePoint, const int &approxSample);

      virtual bool SetFocalPlane(const double ux, const double uy, const double uz);


    protected:
      enum FindFocalPlaneStatus {
        Success,
        BoundingProblem,
        Failure
      };

      FindFocalPlaneStatus FindFocalPlane(const int &approxSample,
                                          const SurfacePoint &surfacePoint);
      double FindSpacecraftDistance(int sample, const SurfacePoint &surfacePoint);
      
    private:
      /**
      * @author 2012-05-09 Orrin Thomas
      *
      * @internal
      */
      class SampleOffsetFunctor : public std::unary_function<double, double > {
      public:
        
        SampleOffsetFunctor(Isis::Camera *camera, const Isis::SurfacePoint &surPt);
        ~SampleOffsetFunctor();
        
        double operator()(double et);
        
      private:
        SurfacePoint m_surfacePoint;
        Camera *m_camera;
      };
      
      
      /**
      * @author 2012-05-09 Orrin Thomas
      *
      * @internal
      */
      class SensorSurfacePointDistanceFunctor : public std::unary_function<double, double > {
        public:
          
          SensorSurfacePointDistanceFunctor(Isis::Camera *camera, const Isis::SurfacePoint &surPt);
          ~SensorSurfacePointDistanceFunctor();
          
          double operator()(double et);
          
        private:
          SurfacePoint m_surfacePoint;
          Camera* m_camera;
      };
  };
  
  
};
#endif
