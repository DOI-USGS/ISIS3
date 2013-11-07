#ifndef MsiCamera_h
#define MsiCamera_h
/**
 * @file
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
#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief NEAR Shoemaker MSI Camera Model
   *
   * This is the camera model for the Near Earth Asteroid Rendezvous 
   * - Shoemaker Multi-Spectral Imager. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup NearEarthAsteroidRendezvousShoemaker
   *  
   * @see http://nssdc.gsfc.nasa.gov/nmc/masterCatalog.do?sc=1996-008A 
   * @see http://pdssbn.astro.umd.edu/data_sb/missions/near/index.shtml
   * @see http://near.jhuapl.edu/instruments/MSI/index.html 
   * @see http://near.jhuapl.edu/fact_sheets/MSI.pdf 
   *  
   * @author  2013-03-27 Jeannie Backer
   *
   * @internal
   *   @history 2013-03-27 Jeannie Backer - Original Version.
   */
  class MsiCamera : public FramingCamera {
    public:
      MsiCamera(Cube &cube);
      ~MsiCamera();
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
//      virtual int SpkCenterId() const;
      virtual int SpkReferenceId() const;
//      virtual int SpkTargetId() const;
  };
};
#endif
