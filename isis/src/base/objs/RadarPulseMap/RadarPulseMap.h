/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/07/09 16:40:59 $
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
#ifndef RadarPulseMap_h
#define RadarPulseMap_h

#include "RadarPulseMap.h"
#include "CameraDetectorMap.h"

namespace Isis {
  /** Convert between alpha image coordinates and radar sample,
   *  time coordinates
   *
   * This class is used to convert between alpha coordinates
   * (sample/line) and radar pulse coordinates (sample,time) for a
   * radar instrument.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @internal
   *
   * @history 2008-06-16 Jeff Anderson
   * Original version
   *
   *  @history 2009-07-01 Janet Barrett - Corrected the calculations
   *                      used to convert from line to time and back
   *
   */
  class RadarPulseMap : public CameraDetectorMap {
    public:
      /** Construct a detector map for line scan cameras
       *
       * @param parent    The parent camera model for the detector map
       * @param etStart   starting ephemeris time in seconds
       *                  at the top of the first line
       * @param lineRate  the time in seconds between lines
       *
       */
      RadarPulseMap(Camera *parent, const double etStart,
                                const double lineRate) :
        CameraDetectorMap(parent) {
        p_etStart = etStart;
        p_lineRate = lineRate;
        p_yAxisTimeDependent = true;
      }

      //! Destructor
      virtual ~RadarPulseMap() {};

      /** Reset the starting ephemeris time
       *
       * Use this method to reset the starting time of the top edge of
       * the first line in the alpha image.  That is the time, prior
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

      /**
       * Set the time dependent axis, if never called y is the time dependent
       * axis
       */
      void SetXAxisTimeDependent(bool on) {
        p_xAxisTimeDependent = on;
        p_yAxisTimeDependent = !on;
      };

    private:
      bool p_xAxisTimeDependent;
      bool p_yAxisTimeDependent;
      double p_etStart;     //!< Starting time at the top of the 1st alpha line
      double p_lineRate;    //!< iTime between lines in parent cube
  };
};
#endif
