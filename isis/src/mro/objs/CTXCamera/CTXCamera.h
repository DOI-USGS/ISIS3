#ifndef CTXCamera_h
#define CTXCamera_h

#include "LineScanCamera.h"

namespace Isis {
  namespace Mro {
    /**
     * @internal
     *   @todo Allow the programmer to apply scale and shear.
     *   @todo Write multiplaction method (operator*) for Affine * Affine.
     *
     *   @history 2006-08-03  Tracie Sucharski, Added Scale method
     *   @history 2007-07-12  Debbie A. Cook, Added methods Coefficients and
     *            InverseCoefficients
     *   @history 2008-02-21 Steven Lambright Boresight, focal length, pixel pitch
     *            keywords now loaded from kernels instead of being hard-coded.
     *            The distortion map is now being used.
     *   @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
     *            change. Also, now using the new LoadCache(...) method instead of
     *            CreateCache(...).
     *   @history 2009-03-07 Debbie A. Cook Removed obsolute CameraDetectorMap methods
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class CTXCamera : public LineScanCamera {
      public:
        CTXCamera(Isis::Pvl &lab);

        ~CTXCamera() {};

        /** CK Frame ID - Instrument Code from spacit run on CK */
        virtual int CkFrameId() const { return (-74000); }

        /** CK Reference ID - MRO_MME_OF_DATE */
        virtual int CkReferenceId() const { return (-74900); }

        /** SPK Reference ID - J2000 */
        virtual int SpkReferenceId() const { return (1); }
    };
  };
};

#endif
