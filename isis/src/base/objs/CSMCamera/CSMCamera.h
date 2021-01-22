#ifndef CSMCamera_h
#define CSMCamera_h
/**
 * New licensing blurb TODO
 */

#include "Camera.h"

#include <QList>
#include <QPointF>

#include "csm/csm.h"
#include "csm/Model.h"

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

    private:
      double m_pixelPitchX;
      double m_pixelPitchY;
      csm::Model *m_model; //! CSM sensor model
  };
};
#endif
