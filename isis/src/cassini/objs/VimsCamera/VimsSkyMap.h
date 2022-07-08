#ifndef VimsSkyMap_h
#define VimsSkyMap_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2009/04/06 15:23:27 $
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

#include "CameraSkyMap.h"


namespace Isis {
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
   * @author 2006-04-05 Tracie Sucharski
   *
   * @internal
   *
   *   @history 2006-04-05 Tracie Sucharski - Original version
   *   @history 2009-04-06 Steven Lambright - Fixed problem that caused double
   *                          deletion of sky map / ground map.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed
   *                          Cassini namespace wrap inside Isis namespace.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *              coding standards. References #972.
   *   @history 2013-11-18 Tracie Sucharski - Added LookDirection method to calculate unit vectors
   *                          so that old unit vector files are no longer needed.
   *   @history 2016-08-28 Kelvin Rodriguez - Removed unused member variables to squash warnings
   *                              in clang. Part of porting to OS X 10.11 
   */
  class VimsSkyMap : public CameraSkyMap {
    public:
      VimsSkyMap(Camera *parent, Pvl &lab);

      //! Destroys the VimsSkyMap object.
      virtual ~VimsSkyMap() {};

      virtual bool SetFocalPlane(NaifContextPtr naif, const double ux, const double uy,
                                 const double uz) override;

      virtual bool SetSky(NaifContextPtr naif, const double ra, const double dec) override;

      void Init(NaifContextPtr naif, Pvl &lab);

    protected:

    private:
      void LookDirection(double v[3]);

      SpiceDouble p_etStart;           //!< Start ephemeris time

      double p_interlineDelay;         /**< InterlineDelayDuration keyword value from
                                            the instrument group of the labels, divided by 1000 */
      double p_ux;               //!< Distorted focal plane x, in millimeters
      double p_uy;               //!< Distorted focal plane y, in millimeters
      double p_uz;               //!< Distorted focal plane z, in millimeters

      double p_xPixSize;         //!< X pixel size
      double p_yPixSize;         //!< Y pixel size
      double p_xBore;            //!< X boresight
      double p_yBore;            //!< Y boresight

      QString p_channel;           /**< Channel keyword value from the instrument group of the labels.
                                            Possible values are IR or VIS */
      double p_visExp;                 //!< VIS exposure duration, divided by 1000
      double p_irExp;                  //!< IR exposure duration, divided by 1000
      int    p_swathWidth;             /**< SwathWidth keyword value from the instrument group of the labels.
                                            This will be image size unless occultation image */
      int    p_swathLength;            /**< SwathLength keyword value from the instrument group of the labels.
                                            This will be image size unless occultation image */
      int    p_camSampOffset;          //!< Sample offset
      int    p_camLineOffset;          //!< Line offset

      double p_minRa;                  //!< Minimum right ascension
      double p_maxRa;                  //!< Maximum right ascension
      double p_minDec;                 //!< Minimum declination
      double p_maxDec;                 //!< Maximum declination
      double p_raMap[64][64];          //!< Right ascension map
      double p_decMap[64][64];         //!< Declination map
  };
};
#endif
