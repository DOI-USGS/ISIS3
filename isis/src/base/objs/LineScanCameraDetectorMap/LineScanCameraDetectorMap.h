/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/03/07 17:57:27 $
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

#ifndef LineScanCameraDetectorMap_h
#define LineScanCameraDetectorMap_h

#include "CameraDetectorMap.h"

namespace Isis {
  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a line scan camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @internal
   *
   * @history 2005-02-09 Jeff Anderson
   * Original version
   * @history 2009-03-07 Debbie A. Cook Removed reference to obsolute CameraDetectorMap methods
   *
   */
  class LineScanCameraDetectorMap : public CameraDetectorMap {
    public:
      /** Construct a detector map for line scan cameras
       * 
       * @param parent    The parent camera model for the detector map
       * @param etStart   starting ephemeris time in seconds
       *                  at the top of the first line
       * @param lineRate  the time in seconds between lines
       *
       */
      LineScanCameraDetectorMap(Camera *parent, const double etStart,
                                const double lineRate) :
        CameraDetectorMap(parent) {
        p_etStart = etStart;
        p_lineRate = lineRate;
      }

      //! Destructor
      virtual ~LineScanCameraDetectorMap() {};

      /** Reset the starting ephemeris time
       *
       * Use this method to reset the starting time of the top edge of
       * the first line in the parent image.  That is the time, prior
       * to cropping, scaling, or padding.  Usually this will not need
       * to be done unless the time changes between bands.
       *
       * @param etStart starting ephemeris time in seconds
       *
       */
      void SetStartTime (const double etStart) { p_etStart = etStart; };

      /** Reset the line rate
       *
       * Use this method to reset the time between lines.  Usually this
       * will not need to be done unless the rate changes between bands.
       *
       * @param lineRate the time in seconds between lines
       *
       */
      void SetLineRate (const double lineRate) { p_lineRate = lineRate; };

      //! Return the time in seconds between scan lines
      double LineRate () const { return p_lineRate; };

      virtual bool SetParent(const double sample, const double line);

      virtual bool SetDetector(const double sample, const double line);

    private:
      double p_etStart;     //!< Starting time at the top of the 1st parent line
      double p_lineRate;    //!< iTime between lines in parent cube
  };
};
#endif
