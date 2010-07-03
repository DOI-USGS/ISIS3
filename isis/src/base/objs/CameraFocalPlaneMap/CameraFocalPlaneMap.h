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
#ifndef CameraFocalPlaneMap_h
#define CameraFocalPlaneMap_h

#include "Camera.h"

namespace Isis {
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
   * @author Jeff Anderson 2005-02-05
   * 
   * @see Camera
   * 
   * @internal 
   * @history 2007-02-13 Debbie A. Cook - Added methods SignMostSigX() and
   *                                      SignMostSigY()
   *     
   */
  class CameraFocalPlaneMap {
    public:
      CameraFocalPlaneMap(Camera *parent, const int naifIkCode);
      CameraFocalPlaneMap(const int naifIkCode);

      //! Destructor
      virtual ~CameraFocalPlaneMap() {};

      virtual bool SetDetector(const double sample, const double line);

      virtual bool SetFocalPlane(const double dx, const double dy);

      //! Return distorted focal plane x
      inline double FocalPlaneX() const { return p_focalPlaneX; };

      //! Return distorted focal plane y
      inline double FocalPlaneY() const { return p_focalPlaneY; };

      //! Return detector sample
      inline double DetectorSample() const { return p_detectorSample; };

      //! Return detector line 
      inline double DetectorLine() const { return p_detectorLine; };

      //! Return centered detector sample
      inline double CenteredDetectorSample() const { return p_centeredDetectorSample; };

      //! Return centered detector line 
      inline double CenteredDetectorLine() const { return p_centeredDetectorLine; };

      /** Set the detector origin
       *
       * This is used to set the origin of the detector.  Typically the middle
       * of the detector.  For example, a 512x512 dectector would have the
       * origin at (256.5,256.5).  If not set both are 0.
       * 
       * @param sample  detector sample at the origin
       * @param line    detector line at the origin
       */
      inline void SetDetectorOrigin (const double sample, const double line) { 
        p_detectorSampleOrigin = sample; 
        p_detectorLineOrigin = line;
      };

      //! Return detector line origin
      inline double DetectorLineOrigin () const { return p_detectorLineOrigin; };

      //! Return detector sample origin
      inline double DetectorSampleOrigin () const { return p_detectorSampleOrigin; };

      /** Set the detector offset
       * 
       * This is used to set the offset between the detector origin and
       * the average location in detector pixels where the image is being
       * viewed.  If not set the offset are both 0.0
       * 
       * @param sampleOffset sample offset in pixels
       * @param lineOffset sample offset in lines
       */
      inline void SetDetectorOffset (const double sampleOffset, 
                                     const double lineOffset) { 
        p_detectorSampleOffset = sampleOffset; 
        p_detectorLineOffset = lineOffset;
      };

      //! Return detector line offset
      inline double DetectorLineOffset () const { return p_detectorLineOffset; };

      //! Return detector sample offset
      inline double DetectorSampleOffset () const { return p_detectorSampleOffset; };

      enum FocalPlaneXDependencyType {
        Sample=1,
        Line=2
      };

//      FocalPlaneXDependencyType FocalPlaneXDependency();
      int FocalPlaneXDependency();
      double SignMostSigX();
      double SignMostSigY();

    protected:
      //! Convenience method to center detector origin (use when inheriting)
      inline void ComputeCentered() { 
        p_centeredDetectorSample = p_detectorSample - p_detectorSampleOrigin;
        p_centeredDetectorLine   = p_detectorLine   - p_detectorLineOrigin;
      }

      //! Convenience method to center detector origin (use when inheriting)
      inline void ComputeUncentered() {
        p_detectorSample = p_centeredDetectorSample + p_detectorSampleOrigin;
        p_detectorLine   = p_centeredDetectorLine   + p_detectorLineOrigin;
      }

      Camera *p_camera;

      double p_detectorLineOrigin;
      double p_detectorSampleOrigin;

      double p_focalPlaneX;             
      double p_focalPlaneY;
      double p_detectorLine;
      double p_detectorSample;
      double p_centeredDetectorSample;
      double p_centeredDetectorLine;

      double p_detectorLineOffset;
      double p_detectorSampleOffset;

      double p_transx[3],p_transy[3];
      double p_itranss[3],p_itransl[3];

    private:
      void Init (Camera *parent, const int naifIkCode);
  };
};
#endif
