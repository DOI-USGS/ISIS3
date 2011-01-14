#ifndef MarciCamera_h
#define MarciCamera_h

#include "PushFrameCamera.h"

namespace Isis {
  namespace Mro {
    /**
     * @brief Marci Camera Model
     *
     * This is the camera model for the MARCI Instrument
     *
     * @ingroup SpiceInstrumentsAndCameras
     * @ingroup MarsReconnaissanceOrbiter
     *
     * @author 2008-10-23 Steven Lambright
     *
     * @internal
     *   @history 2008-11-19 Steven Lambright - Added distortion model, made VIS
     *                      bands other than orange work
     *   @history 2008-11-25 Steven Lambright - The coloroffset now works properly;
     *            if an offset is supplied in marci2isis the cube will still project
     *            properly.
     *   @history 2009-03-17 Steven Lambright - Fixed UV to work with the distortion
     *            model correctly
     *   @history 2009-05-21 Steven Lambright - Fixed GeometricTilingHint for summed
     *            images
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class MarciCamera : public Isis::PushFrameCamera {
      public:
        // constructor
        MarciCamera(Isis::Pvl &lab);

        //! Destroys the Themis Vis Camera object
        ~MarciCamera() {};

        // Sets the band to the band number given
        void SetBand(const int band);

        /**
         * The camera model is band dependent, so this method returns false
         *
         * @return bool False
         */
        bool IsBandIndependent() {
          return false;
        };

        /** CK Frame ID - Instrument Code from spacit run on CK */
        virtual int CkFrameId() const { return (-74000); }

        /** CK Reference ID - MRO_MME_OF_DATE */
        virtual int CkReferenceId() const { return (-74900); }

        /** SPK Reference ID - J2000 */
        virtual int SpkReferenceId() const { return (1); }

      private:
        void StoreCoefficients(int naifIkCode);
        void RestoreCoefficients(int vband);

        double p_etStart;              //!<Ephemeris Start iTime
        double p_bandTimeOffset;       //!<Offset iTime for Band
        double p_exposureDur;          //!<Exposure Duration value from labels
        double p_interframeDelay;      //!<Interframe Delay value from labels
        int p_nframelets;                 //!<Number of framelets in whole image
        std::vector<int> p_detectorStartLines;
        std::vector<int> p_filterNumbers;
        std::vector<int> p_frameletOffsets;
    };
  };
};
#endif

