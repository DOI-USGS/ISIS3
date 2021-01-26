#ifndef CSMCamera_h
#define CSMCamera_h
/**
 * New licensing blurb TODO
 */

#include "Camera.h"
#include "Target.h"

#include <QList>
#include <QPointF>

#include "csm/csm.h"
#include "csm/RasterGM.h"

namespace Isis {
  class CSMCamera : public Camera {
    public:
      // constructors
      CSMCamera(Cube &cube);

      //! Destroys the CSMCamera object.
      ~CSMCamera() {};

      /**
       * The CSM camera needs a bogus type for now.
       *
       * @return CameraType Camera::Point
       */
      virtual CameraType GetCameraType() const {
        return Point;
      }

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-1); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (-1); }

      /**
       *  SPK Center ID - 6 (Saturn)
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Center ID
       */
      virtual int SpkCenterId() const { return -1; }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (-1); }

      virtual QList<QPointF> PixelIfovOffsets();

      virtual Target *target() const;

      virtual bool SetImage(const double sample, const double line);

      private:
      double m_pixelPitchX;
      double m_pixelPitchY;
      csm::RasterGM *m_model; //! CSM sensor model
      Target *m_target; //! Target body (i.e. Mars, Earth) Overriding SPICE Target for CSM.
  };
};
#endif
