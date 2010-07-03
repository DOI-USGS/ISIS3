#ifndef MocWideAngleDetectorMap_h
#define MocWideAngleDetectorMap_h

#include "LineScanCameraDetectorMap.h"
#include "MocLabels.h"

namespace Isis {
  namespace Mgs {
    /** Convert between parent image coordinates and detector coordinates
     * 
     * This class is used to convert between parent dector coordinates
     * (sample/line) and detector coordinates for a the Moc wide angle
     * camera. It is needed to handle variable summing modes
     * 
     * @ingroup Camera
     * 
     * @see Camera
     * 
     * @internal
     * 
     * @history 2005-02-09 Jeff Anderson
     * Original version
     * 
     */
    class MocWideAngleDetectorMap : public LineScanCameraDetectorMap {
      public:
        /**
         * Construct a detector map for line scan cameras
         * 
         * @param parent The parent Camera Model
         * @param etStart   starting ephemeris time in seconds
         *                  at the top of the first line
         * @param lineRate  the time in seconds between lines
         * @param moclab The moc labels to use for the camera creation
         * 
         */
        MocWideAngleDetectorMap(Camera *parent, const double etStart,
                                  const double lineRate, MocLabels *moclab) :
          LineScanCameraDetectorMap(parent,etStart,lineRate) {
          p_moclab = moclab;
        }
    
        //! Destructor
        virtual ~MocWideAngleDetectorMap() {};
  
        virtual bool SetParent(const double sample, const double line);
  
        virtual bool SetDetector(const double sample, const double line);
  
      private:
        MocLabels *p_moclab;
    };
  };
};
#endif
