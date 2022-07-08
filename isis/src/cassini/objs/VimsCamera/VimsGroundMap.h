#ifndef VimsGroundMap_h
#define VimsGroundMap_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/08/07 22:08:33 $
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

#include "CameraGroundMap.h"

#include <QVector3D>

namespace Isis {
  class Distance;
  class Latitude;
  class Longitude;

  /**
   *  Convert between undistorted focal plane and ground coordinates
   *
   * This base class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and ground coordinates lat/lon.
   * This class handles the case of framing cameras.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Cassini-Huygens
   *
   * @see Camera
   * @see VimsCamera
   *
   * @author 2006-03-16 Tracie Sucharski
   *
   * @internal
   *
   *   @history 2006-03-16 Tracie Sucharski Original version
   *   @history 2008-02-05 Tracie Sucharski, Replaced unitVector files with
   *                            Rick McCloskey's code to calculate look direction.
   *   @history 2008-06-18 Steven Lambright Fixed documentation
   *   @history 2009-04-06 Steven Lambright Fixed problem that caused double
   *                          deletion of sky map / ground map.
   *   @history 2009-08-06 Tracie Sucharski, Bug in unit vector change made
   *                           on 2008-02-05, had the incorrect boresight for
   *                           VIS Hires.
   *   @history 2011-02-08 Steven Lambright & Debbie Cook, Added
   *                           WrapWorldToBeClose and refactored to use the
   *                           Latitude and Longitude classes.
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed
   *                          Cassini namespace wrap inside Isis namespace.
   *   @history 2011-10-19 Steven Lambright - Added common sense check to
   *                           constructor to at least provide a string
   *                           explanation for why their program is going to
   *                           crash when the original cube makes no sense.
   *                           Since the exception is in the constructor the
   *                           error will probably lead to an alternate seg
   *                           fault.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *   @history 2012-09-10 Ken Edmundson,  Added new methods, SetGroundwithLatitudeLongitude and
   *                          SetGroundwithRadiusLongitude to support rings data.
   *   @history 2012-12-03 Tracie Sucharski - Added error check to SetGround method.  Returns false
   *                          if none of the pixel centers in the cube intersect with the planet.
   *                          Fixes #1306.
   *   @history 2013-07-24 Tracie Sucharski - Fix bug in camera model which causes a problems
   *                          finding either north or south poles in images.  References #1289.
   *   @history 2013-08-12 Tracie Sucharski - Change all computations base on latitude and
   *                          longitude to x, y and z.  This takes care of pole problems and
   *                          improves accuracy.  Fixes #1289.
   *   @history 2014-04-09 Tracie Sucharski - When changing the camera model from lat/lon
   *                          to x/y/z calculations the range checking was removed.  This caused
   *                          extra pixels to be projected into incorrect positions when doing
   *                          global projections. Range checking was put back in using x/y/z values.
   *                          References #1289.
   */
  class VimsGroundMap : public CameraGroundMap {
    public:
      VimsGroundMap(Camera *parent, Pvl &lab);

      virtual ~VimsGroundMap();

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz, NaifContextPtr naif) override;

      virtual bool SetGround(NaifContextPtr naif, const Latitude &lat, const Longitude &lon) override;
//    bool SetGroundwithLatitudeLongitude(const Latitude &lat, const Longitude &lon);
//    bool SetGroundwithRadiusLongitude(const double &radius, const Longitude &lon);
      virtual bool SetGround(NaifContextPtr naif, const SurfacePoint &surfacePoint) override;

      void Init(NaifContextPtr naif, Pvl &lab);

    private:
      void LookDirection(double v[3]);

      SpiceDouble p_etStart;     //!< Start ephemeris time

      double p_interlineDelay;   /**< InterlineDelayDuration keyword value from
                                      the instrument group of the labels, divided by 1000 */

      double p_ux;               //!< Distorted focal plane x, in millimeters
      double p_uy;               //!< Distorted focal plane y, in millimeters
      double p_uz;               //!< Distorted focal plane z, in millimeters

      double p_xPixSize;         //!< X pixel size
      double p_yPixSize;         //!< Y pixel size
      double p_xBore;            //!< X boresight
      double p_yBore;            //!< Y boresight

      QString p_channel;     /**< Channel keyword value from the instrument group of the labels.
                                      Possible values are IR or VIS */
      double p_visExp;           //!< VIS exposure duration, divided by 1000
      double p_irExp;            //!< IR exposure duration, divided by 1000
      int    p_swathWidth;       /**< SwathWidth keyword value from the instrument group of the labels. 
                                      This will be image size unless occultation image */
      int    p_swathLength;      /**< SwathLength keyword value from the instrument group of the labels.
                                      This will be image size unless occultation image */
      int    p_camSampOffset;    //!< Sample offset
      int    p_camLineOffset;    //!< Line offset

      double p_minX,p_maxX;
      double p_minY,p_maxY;
      double p_minZ,p_maxZ;

      QVector3D p_xyzMap[64][64];

  };
};
#endif
