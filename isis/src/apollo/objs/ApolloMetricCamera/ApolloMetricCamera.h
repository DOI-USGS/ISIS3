#ifndef ApolloMetricCamera_h
#define ApolloMetricCamera_h

#include "FramingCamera.h"
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
  */                                                                       
namespace Isis {
  namespace Apollo {
    class ApolloMetricCamera : public FramingCamera {
      public:
        ApolloMetricCamera (Isis::Pvl &lab);

        ~ApolloMetricCamera () {};
    };
  };
};

#endif
