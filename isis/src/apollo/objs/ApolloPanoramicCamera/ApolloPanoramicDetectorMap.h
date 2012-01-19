#ifndef ApolloPanoramicDetectorMap_h
#define ApolloPanoramicDetectorMap_h

#include "apollo_pan_IO.h"
#include "LineScanCameraDetectorMap.h"
#include "Pvl.h"

/**
 *
 * @internal
 *    @history 2011-11-21 Orrin Thomas - Original Version
 */

namespace Isis {
  namespace Apollo {
    /** Convert between parent image (aka encoder aka machine)  coordinates and detector coordinates
     *
     * This class is used to convert between parent image (aka encoder aka machine) coordinates
     * (sample/line) and detector coordinates for a the Apollo Panoramic Image. 
     *
     */
    class ApolloPanoramicDetectorMap : public CameraDetectorMap {
    public:
      /**
       * Construct a detector map for line scan cameras
       *
       * @param parent The parent Camera Model
       * @param etStart   starting ephemeris time in seconds
       *                  the time at the top of the first line (mm)
       * @param lineRate  the time in seconds between lines (msec)
       * @param lab The labels to use for the camera creation
       *
       */
      ApolloPanoramicDetectorMap(Camera *parent, double etMiddle,
                                double lineRate, Pvl *lab) :
      CameraDetectorMap(parent) {
  p_lineRate = lineRate;
  p_etMiddle = etMiddle;
        p_lab = lab;
  this->Initialize_Interior_Orientation();
      }

      //! Destructor
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
       */
      void SetLineRate(const double lineRate) {
        p_lineRate = lineRate;
      };

      //! Return the time in seconds between scan lines
      double LineRate() const {
        return p_lineRate;
      };

      private:
  double p_etMiddle;    //time of the middle line
  double p_lineRate;    //line exposrue duration
        Pvl *p_lab;
  Apollo_Pan_IO io;
  int Initialize_Interior_Orientation();
    };
  };
};
#endif

