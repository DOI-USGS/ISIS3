#ifndef CSMCamera_h
#define CSMCamera_h
/**
 * New licensing blurb TODO
 */

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
      virtual double PixelResolution();
      virtual double ObliqueLineResolution();
      virtual double ObliqueSampleResolution();
      virtual double ObliquePixelResolution();

      virtual double parentLine();
      virtual double parentSample();

      void subSpacecraftPoint(double &lat, double &lon);
      void subSpacecraftPoint(double &lat, double &lon, double line, double sample);

    protected:
      virtual Target *setTarget(Pvl label);

      std::vector<double> sensorPositionBodyFixed();
      std::vector<double> sensorPositionBodyFixed(double line, double sample);

    private:
      csm::RasterGM *m_model; //! CSM sensor model

      void isisToCsmPixel(double line, double sample, csm::ImageCoord &csmPixel);
      void csmToIsisPixel(csm::ImageCoord csmPixel, double &line, double &sample);

      virtual std::vector<double> ImagePartials(SurfacePoint groundPoint);
      virtual std::vector<double> ImagePartials();

  };
};
#endif
