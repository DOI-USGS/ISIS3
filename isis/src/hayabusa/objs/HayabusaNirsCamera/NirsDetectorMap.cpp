/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2009/04/08 02:32:55 $
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
#include "NirsDetectorMap.h"

namespace Isis {

  /**
   * Constructs a NirsDetectorMap object.
   * 
   * @param exposureDuration The time for the observations.
   * @param parent The parent camera that uses the detector map.
   */
  NirsDetectorMap::NirsDetectorMap(double exposureDuration, Camera *parent = 0):
                                   CameraDetectorMap(parent) {
    m_exposureDuration = exposureDuration;
  }


  /**
   * Destroys a NirsDetectorMap object.
   */
  NirsDetectorMap::~NirsDetectorMap() {
    
  }


  /**
   * Sets the exposure duration
   * 
   * @param exposureDuration The time for the observations.
   */
  void NirsDetectorMap::setExposureDuration(double exposureDuration) {
    m_exposureDuration = exposureDuration;
  }


  /**
   * Returns the exposure duration for a given pixel.
   * 
   * @param sample The sample location of the pixel.
   * @param line The line location of the pixel.
   * @param band The band location of the pixel.
   * 
   * @return @b double The exposure duration for the pixel in seconds.
   */
  double NirsDetectorMap::exposureDuration(const double sample,
                                           const double line,
                                           const int band) const {
    return m_exposureDuration;
  }
}