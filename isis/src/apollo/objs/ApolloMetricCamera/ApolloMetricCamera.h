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
    };
  };
};

#endif
