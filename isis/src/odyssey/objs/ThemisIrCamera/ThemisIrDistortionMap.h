#ifndef ThemisIrDistortionMap_h
#define ThemisIrDistortionMap_h
/**
 * @file
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

#include "CameraDistortionMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Themis IR camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @see ThemisIrCamera
   *  
   *  @author 2005-02-01 Jeff Anderson 
   *  
   *  @internal
   *   @history 2005-02-01 Jeff Anderson - Original version
   *   @history 2009-03-27 Jeff Anderson - Modified to use Duxbury's distortion 
   *                         model from Feb 2009 email with attached PDF
   *                         document
   *   @history 2011-05-03 Jeannie Walldren - Removed Odyssey namespace wrap.
   *
   */
  class ThemisIrDistortionMap : public CameraDistortionMap {
    public:
      ThemisIrDistortionMap(Camera *parent);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      void SetBand(int band);

    private:
      double p_k;
      double p_alpha1;
      double p_alpha2;
  };
};
#endif
