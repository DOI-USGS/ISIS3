#ifndef ApolloMetricCamera_h
#define ApolloMetricCamera_h

#include "FramingCamera.h"
namespace Isis {
  namespace Apollo {
 /**
  * @brief Apollo Metric Camera Model
  *
  * This is the camera model for the Apollo metric camera.
  *
  * @ingroup Camera
  *
  * @author 2006-11-14 Jacob Danton
  *
  * @internal
  *   @history 2006-11-14 Jacob Danton - Original Version
  *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
  *            inherit directly from Camera
  *   @history 2010-07-20 Sharmila Prasad - Modified documentation to remove Doxygen Warning
  */
    class ApolloMetricCamera : public FramingCamera {
      public:
        ApolloMetricCamera(Isis::Pvl &lab);

        ~ApolloMetricCamera() {};

        /**
         * CK frame ID -
         * Apollo 15 instrument code (A15_METRIC) = -915240
         * Apollo 16 instrument code (A16_METRIC) = -916240
         * Apollo 17 instrument code (A17_METRIC) = -917240
         */
        virtual int CkFrameId() const { return p_ckFrameId; }

        /**
         * CK Reference ID -
         * APOLLO_15_NADIR = 1400015
         * APOLLO_16_NADIR = 1400016
         * APOLLO_17_NADIR = 1400017
         */
        virtual int CkReferenceId() const { return p_ckReferenceId; }

        /**
         * SPK Target Body ID -
         * Apollo 15 = -915
         * Apollo 16 = -916
         * Apollo 17 = -917
         */
        virtual int SpkTargetId() const { return p_spkTargetId; }

        /** SPK Reference ID - B1950 */
        virtual int SpkReferenceId() const { return (2); }

      private:
        int p_ckFrameId;
        int p_ckReferenceId;
        int p_spkTargetId;
    };
  };
};

#endif
