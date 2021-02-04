#ifndef CSMCamera_h
#define CSMCamera_h
/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "Target.h"

#include <vector>

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

      virtual bool SetImage(const double sample, const double line);

      virtual bool SetGround(Latitude latitude, Longitude longitude);
      virtual bool SetGround(const SurfacePoint &surfacePt);
      virtual bool SetUniversalGround(const double latitude, const double longitude);
      virtual bool SetUniversalGround(const double latitude, const double longitude, double radius);

      virtual double LineResolution();
      virtual double SampleResolution();
      virtual double DetectorResolution();
      virtual double ObliqueLineResolution();
      virtual double ObliqueSampleResolution();
      virtual double ObliqueDetectorResolution();

      virtual double parentLine() const;
      virtual double parentSample() const;

      void subSpacecraftPoint(double &lat, double &lon);
      void subSpacecraftPoint(double &lat, double &lon, double line, double sample);

      virtual double PhaseAngle() const;
      virtual double EmissionAngle() const;
      virtual double IncidenceAngle() const;

      virtual SpicePosition *sunPosition() const;
      virtual SpicePosition *instrumentPosition() const;
      virtual SpiceRotation *bodyRotation() const;
      virtual SpiceRotation *instrumentRotation() const;

      virtual void instrumentBodyFixedPosition(double p[3]) const;

    protected:
      void setTarget(Pvl label);

      std::vector<double> sensorPositionBodyFixed() const;
      std::vector<double> sensorPositionBodyFixed(double line, double sample) const;

    private:
      csm::RasterGM *m_model; //! CSM sensor model

      void isisToCsmPixel(double line, double sample, csm::ImageCoord &csmPixel) const;
      void csmToIsisPixel(csm::ImageCoord csmPixel, double &line, double &sample) const;
      csm::EcefCoord isisToCsmGround(const SurfacePoint &groundPt) const;
      SurfacePoint csmToIsisGround(const csm::EcefCoord &groundPt) const;

      virtual std::vector<double> ImagePartials(SurfacePoint groundPoint);
      virtual std::vector<double> ImagePartials();

  };
};
#endif
