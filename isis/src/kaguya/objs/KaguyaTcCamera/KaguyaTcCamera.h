#ifndef KaguyaTcCamera_h
#define KaguyaTcCamera_h
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * This is the camera model for the Kaguya Terrain Cameras TC1 and TC2 
   *
   * @internal
   *   @history 2018-10-01 Adam Goins and Jeannie Backer - Original Version
   *  
   *   @history 2019-04-26 Stuart Sides and Kristin Berry - Updates to Kaguya TC camera model
   *                        including updating to use LineScanCamera detector and ground maps, adding
   *                        detector offsets for swath modes, setting the focal plane map center to
   *                        the center of the detector, regardless of swath mode, and using the
   *                        spacecraft clock start count, rather than the StartTime for image timing.
   *                        See Git issue #3215 for more information.
   *                             
   */
  class KaguyaTcCamera : public LineScanCamera {
    public:
      KaguyaTcCamera(Cube &cube);
      ~KaguyaTcCamera();
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
