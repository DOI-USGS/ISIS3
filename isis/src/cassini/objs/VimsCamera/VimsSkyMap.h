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
   */
  class VimsSkyMap : public CameraSkyMap {
    public:
      VimsSkyMap(Camera *parent, Pvl &lab);

      //! Destroys the VimsSkyMap object.
      virtual ~VimsSkyMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);

      virtual bool SetSky(const double ra, const double dec);

      void Init(Pvl &lab);

    protected:

    private:
      SpiceDouble p_etStart;           //!< Start ephemeris time

      double p_exposureDuration;       //!< Exposure duration
      double p_interlineDelay;         /**< InterlineDelayDuration keyword value from
                                            the instrument group of the labels, divided by 1000 */

      std::string p_channel;           /**< Channel keyword value from the instrument group of the labels.
                                            Possible values are IR or VIS */
      double p_visExp;                 //!< VIS exposure duration, divided by 1000
      double p_irExp;                  //!< IR exposure duration, divided by 1000
      int    p_nsUv;                   //!< Normal or high resolution sample uv
      int    p_nlUv;                   //!< Normal or high resolution line uv
      int    p_swathWidth;             /**< SwathWidth keyword value from the instrument group of the labels.
                                            This will be image size unless occultation image */
      int    p_swathLength;            /**< SwathLength keyword value from the instrument group of the labels.
                                            This will be image size unless occultation image */
      int    p_camSampOffset;          //!< Sample offset
      int    p_camLineOffset;          //!< Line offset

      double p_unitVector[192][192][3];//!< Unit vector

      double p_minRa;                  //!< Minimum right ascension
      double p_maxRa;                  //!< Maximum right ascension
      double p_minDec;                 //!< Minimum declination
      double p_maxDec;                 //!< Maximum declination
      double p_raMap[64][64];          //!< Right ascension map
      double p_decMap[64][64];         //!< Declination map
  };
};
#endif
