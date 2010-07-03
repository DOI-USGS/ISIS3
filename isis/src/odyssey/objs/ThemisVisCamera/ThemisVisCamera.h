#ifndef ThemisVisCamera_h
#define ThemisVisCamera_h

#include "PushFrameCamera.h"

namespace Isis {
  namespace Odyssey {
    /**
     * @brief Themis VIS Camera Model
     *
     * This is the camera model for the Themis Vis Framing camera
     *
     * @ingroup SpiceInstrumentsAndCameras
     * @ingroup MarsOdyssey
     *
     * @author 2006-01-06 Elizabeth Ribelin
     *
     * @internal
     *   @history 2006-05-17 Elizabeth Miller - Depricated CameraManager to
     *                                          CameraFactory in unitTest
     *
     *  @history 2006-09-12 Elizabeth Miller - Fixed bug in detector map
     *                                        caused by the deletion of the
     *                                        alphacube group in the labels
     *  @history 2008-06-16 Steven Lambright - Made camera work with
     *                                         new push frame
     *                                         classes
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class ThemisVisCamera : public Isis::PushFrameCamera {
      public:
        // constructor
        ThemisVisCamera (Isis::Pvl &lab);

        //! Destroys the Themis Vis Camera object
        ~ThemisVisCamera () {};

        // Sets the band to the band number given
        void SetBand (const int band);

        double BandEphemerisTimeOffset(int vband);

       /**
        * The camera model is band dependent, so this method returns false
        *
        * @return bool False
        */
        bool IsBandIndependent () { return false; };

      private:
        double p_etStart;              //!<Ephemeris Start iTime
        double p_bandTimeOffset;       //!<Offset iTime for Band
        double p_exposureDur;          //!<Exposure Duration value from labels
        double p_interframeDelay;      //!<Interframe Delay value from labels
        int p_nframes;                 //!<Number of frames in whole image
        std::vector<int> p_originalBand;  //!<Vector of order for original band numbers
    };
  };
};
#endif

