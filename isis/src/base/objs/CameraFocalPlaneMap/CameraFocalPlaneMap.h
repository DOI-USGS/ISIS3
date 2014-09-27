#ifndef CameraFocalPlaneMap_h
#define CameraFocalPlaneMap_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2007/02/13 23:03:54 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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

      Camera *p_camera; //!< 

      double p_detectorLineOrigin;   //!< 
      double p_detectorSampleOrigin; //!< 

      double p_focalPlaneX;            //!< x value of focal plane coordinate
      double p_focalPlaneY;            //!< y value of focal plane coordinate
      double p_detectorLine;           //!< line value of the detector
      double p_detectorSample;         //!< sample value of the detector
      double p_centeredDetectorSample; //!< 
      double p_centeredDetectorLine;   //!< 

      double p_detectorLineOffset;   //!< 
      double p_detectorSampleOffset; //!< 

      double p_transx[3];  //!< 
      double p_transy[3];  //!< 
      double p_itranss[3]; //!< 
      double p_itransl[3]; //!< 

    private:
      void Init(Camera *parent, const int naifIkCode);
  };
};
#endif
