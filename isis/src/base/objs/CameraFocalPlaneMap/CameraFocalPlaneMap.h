#ifndef CameraFocalPlaneMap_h
#define CameraFocalPlaneMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

template<class T> class QVector;

namespace Isis {
 
  class Affine;
  class Camera;
  /** Convert between distorted focal plane and detector coordinates
   *
   * This base class is used to convert between distorted focal plane
   * coordinates (x/y) in millimeters and detector coordinates in
   * pixels.  The class expects to find a set of coefficients in the
   * naif instrument (or instrument addendum) kernel that describe the
   * transform from detector to focal plane and vice versa.  The transform
   * from detector to focal plane is:
   *
   * @code
   * x = transx[0] + sample * transx[1] + line * transx[2];
   * y = transy[0] + sample * transy[1] + line * transy[2];
   *
   * where, transx and transy are the coefficients from the naif kernel.
   * The should be in the form of:
   *
   * INSxxxxxx_TRANSX = ( a, b, c)
   * INSxxxxxx_TRANSY = ( d, e, f)
   *
   * where, xxxxxx is the NAIF instrument id code.
   * @endcode
   *
   * Likewise, the inverse transform is:
   *
   * @code
   * samp = itranss[0] + x * itranss[1] + y * itranss[2];
   * line = itransl[0] + x * itransl[1] + y * itransl[2];
   *
   * where, itranss and itranss are the coefficients from the naif kernel.
   * The should be in the form of:
   *
   * INSxxxxxx_ITRANSS = ( a, b, c)
   * INSxxxxxx_ITRANSL = ( d, e, f)
   *
   * where, xxxxxx is the NAIF instrument id code.
   * @endcode
   *
   * @ingroup Camera
   *
   * @author 2005-02-05 Jeff Anderson
   *
   * @see Camera
   *
   * @internal
   *   @history 2007-02-13 Debbie A. Cook - Added methods SignMostSigX() and
   *                           SignMostSigY()
   *   @history 2011-05-25 Janet Barrett and Steven Lambright - Spice::GetDouble
   *                           is no longer static. 
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more
   *                           compliant with Isis coding standards.
   *                           References #972.
   *   @history 2012-11-21 Jeannie Backer - Added accesssor methods:
   *                           TransX(), TransY(), TransS(), and TransL().
   *                           These are tested by application hideal2pds
   *                           since no unitTest exists. Fixed indentation
   *                           of history entries, order of includes,
   *                           moved method implementations to cpp, and
   *                           fixed control statement padding to be more
   *                           compliant with Isis standards. Added
   *                           documentation to member
   *                           variables.References #678.
   *   @history 2014-03-27 Jeff Anderson - Added constructor which takes an
   *                           Affine transform instead of only reading from 
   *                           NAIF kernels to obtain the transform.  This is
   *                           required for non-NAIF instruments such as
   *                           Aerial photos.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *  
   */
  class CameraFocalPlaneMap {
    public:
      CameraFocalPlaneMap(Camera *parent, const int naifIkCode);
      CameraFocalPlaneMap(const int naifIkCode);
      CameraFocalPlaneMap(Camera *parent, Affine &affine);
      virtual ~CameraFocalPlaneMap();

      virtual bool SetDetector(const double sample, const double line);
      virtual bool SetFocalPlane(const double dx, const double dy);

      double FocalPlaneX() const;
      double FocalPlaneY() const;
      double DetectorSample() const;
      double DetectorLine() const;
      double CenteredDetectorSample() const;
      double CenteredDetectorLine() const;
      void SetDetectorOrigin(const double sample, const double line);
      double DetectorLineOrigin() const;
      double DetectorSampleOrigin() const;
      void SetDetectorOffset(const double sampleOffset,
                                    const double lineOffset);
      double DetectorLineOffset() const;
      double DetectorSampleOffset() const;

      void SetTransL(const QVector<double> transL);
      void SetTransS(const QVector<double> transS);
      void SetTransX(const QVector<double> transX);
      void SetTransY(const QVector<double> transY);

      const double *TransL() const;
      const double *TransS() const;
      const double *TransX() const;
      const double *TransY() const;

      /**
       * Focal plane x dependency indicates whether the x value of the focal 
       * plane coordinate maps to a sample or a line. 
       */
      enum FocalPlaneXDependencyType {
        Sample = 1, //!< The x value of the focal plane maps to a sample
        Line = 2    //!< The x value of the focal plane maps to a line
      };

//      FocalPlaneXDependencyType FocalPlaneXDependency();
      int FocalPlaneXDependency();
      double SignMostSigX();
      double SignMostSigY();

    protected:
      void ComputeCentered();
      void ComputeUncentered();

      Camera *p_camera;                 //!< Camera of the image

      double p_detectorLineOrigin;      //!< The origin line of the detector
      double p_detectorSampleOrigin;    //!< The origin sample of the detector

      double p_focalPlaneX;             //!< x value of focal plane coordinate
      double p_focalPlaneY;             //!< y value of focal plane coordinate
      double p_detectorLine;            //!< line value of the detector
      double p_detectorSample;          //!< sample value of the detector
      double p_centeredDetectorSample;  //!< Detector sample position
      double p_centeredDetectorLine;    //!< Detector line position

      double p_detectorLineOffset;      //!< Offset of the detector origin line from the average
      double p_detectorSampleOffset;    //!< offset of the detector origin sample from the average

      double p_transx[3];               //!< The x transition from detector to distorted
      double p_transy[3];               //!< The y transition from detector to distorted
      double p_itranss[3];              //!< The x transition from distorted to detector
      double p_itransl[3];              //!< The y transition from distorted to detector

    private:
      void Init(Camera *parent, const int naifIkCode);
  };
};
#endif
