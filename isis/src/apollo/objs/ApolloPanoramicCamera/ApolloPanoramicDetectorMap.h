#ifndef ApolloPanoramicDetectorMap_h
#define ApolloPanoramicDetectorMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                   

#include "ApolloPanIO.h"
#include "LineScanCameraDetectorMap.h"
#include "Pvl.h"
#include "PvlGroup.h"

namespace Isis {
    /**
     * @brief Convert between parent image (aka encoder aka machine)  coordinates and detector
     *         coordinates
     *
     * This class is used to convert between parent image (aka encoder aka machine) coordinates
     * (sample/line) and detector coordinates for a the Apollo Panoramic Image.
     *
     * @author 2011-11-21 Orrin Thomas
     *
     * @internal
     *   @history 2011-11-21 Orrin Thomas - Original version
     *   @history 2012-07-10 Orrin Thomas - Updated to current coding standards
     *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
     */
    class ApolloPanoramicDetectorMap : public CameraDetectorMap {
    public:
      /**
       * Construct a detector map for line scan cameras
       *
       * @param parent    The parent Camera Model
       * @param etMiddle  Time of the center line, line 0 after interior orientation
       * @param lineRate  the time in seconds between lines (msec)
       * @param lab The labels to use for the camera creation
       *
       */
      ApolloPanoramicDetectorMap(Camera *parent, double etMiddle,
                                double lineRate, Pvl *lab) : CameraDetectorMap(parent) {
        m_lineRate = lineRate;
        m_etMiddle = etMiddle;
        m_lab = lab;

        this->initializeInteriorOrientation();
      }

      //! Destroys ApolloPanoramicDetectorMap object
      virtual ~ApolloPanoramicDetectorMap() {};

      virtual bool SetParent(const double sample, const double line);

      virtual bool SetDetector(const double sample, const double line);

      /** Reset the line rate
       *
       * Use this method to reset the time between lines.  Usually this
       * will not need to be done unless the rate changes between bands.
       *
       * @param lineRate the time in seconds between lines
       *
       * @return void
       */
      void SetLineRate(const double lineRate) {
        m_lineRate = lineRate;
      };

      /**
       * Return the time in seconds between scan lines
       *
       * @return double Time in seconds between scan lines
       */
      double LineRate() const {
        return m_lineRate;
      };

      /**
       * Mean (average) of interior orientation residual vector lengths, accesor
       *
       * @return double Mean of interior orientation residual vector lengths
       */
      double  meanResidual() { return p_intOri.meanResiduals(); };

      /**
       * Max interior orientation residual vector length, accesor
       *
       * @return double Max interior orientation residual vector length
       */
      double   maxResidual() { return p_intOri.maxResiduals(); };

      /**
       * Standard deviation of interior orientation residual vector lengths, accesor
       *
       * @return double Standard deviation of interior orientation residual vector lengths
       */
      double stdevResidual() { return p_intOri.stdevResiduals(); };

      private:

        double m_etMiddle; //!< Ephemeris time at the middle line
        double m_lineRate; //!< line exposure duration
        Pvl *m_lab;        //!< Image label used to construct camera object

        /**
         * Class to preform transformations from cube coordiantes to image coordinates
         */
        ApolloPanIO p_intOri;

        int initializeInteriorOrientation();
    };
};
#endif
